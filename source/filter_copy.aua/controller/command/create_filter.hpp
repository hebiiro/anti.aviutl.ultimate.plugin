#pragma once

namespace apn::filter_copy::controller::command
{
	//
	// このクラスはコントローラ層のフィルタ作成処理です。
	//
	inline struct create_filter_t : view::listener::create_filter_t, Magi::CommandID::SettingDialog
	{
		using unknown1_t = void (CDECL*)(int32_t object_index,int32_t filter_index);

		//
		// カレントフィルタの複製方法です。
		//
		uint32_t command_id = 0;

		//
		// カレントフィルタの複製を作成します。
		//
		virtual LRESULT on_create_filter(uint32_t command_id) override
		{
			// オブジェクトを取得します。
			ObjectHolder object(magi.exin.get_current_object_index());
			MY_TRACE_OBJECT_HOLDER(object);
			if (!object.is_valid()) return FALSE;

			// フィルタを取得します。
			FilterHolder filter(object, magi.exin.get_current_filter_index());
			MY_TRACE_FILTER_HOLDER(filter);
			if (!filter.is_valid()) return FALSE;
			MY_TRACE_STR(filter.get_filter()->name);
			MY_TRACE_HEX(filter.get_filter()->flag);

			// フィルタが複製できるものかどうかチェックします。
			if (!filter.is_moveable()) return FALSE;

			// フィルタIDを取得します。
			auto filter_id = object.get_object()->filter_param[filter.get_filter_index()].id;
			MY_TRACE_HEX(filter_id);
			if (filter_id < 0) return FALSE;

			// フラグを立ててmagi.exin.unknown1()をフックするようにします。
			this->command_id = command_id;

			// スコープ終了時にフラグを消去します。
			my::scope_exit scope_exit([this]() { this->command_id = 0; });

			// フィルタを作成するコマンドを発行します。
			// この中でmagi.exin.unknown1()が呼ばれます。
			return ::SendMessage(magi.exin.get_setting_dialog(), WM_COMMAND, 2000 + filter_id, 0);
		}

		//
		// カレントフィルタの複製を作成します。
		//
		void create_filter(int32_t orig_object_index, int32_t new_filter_index)
		{
			MY_TRACE_FUNC("{/}, {/}", orig_object_index, new_filter_index);

			// オブジェクトのインデックスを取得します。
			auto object_index = orig_object_index;

			// 中間点がある場合は中間点リーダーのインデックスを使用します。
			auto midpt_leader = magi.exin.get_object(object_index)->index_midpt_leader;
			if (midpt_leader >= 0) object_index = midpt_leader;

			// オブジェクトを走査します。
			while (object_index >= 0)
			{
				// オブジェクトのインデックスが無効の場合はループを終了します。
				if (object_index < 0) break;

				// オブジェクトを取得します。
				auto object = magi.exin.get_object(object_index);

				// 中間点リーダーが異なる場合はループを終了します。
				if (object->index_midpt_leader != midpt_leader) break;

				// コピー元フィルタのインデックスを取得します。
				auto src_filter_index = magi.exin.get_current_filter_index();
				if (src_filter_index < 0) break;

				// コピー先フィルタのインデックスを取得します。
				auto dst_filter_index = new_filter_index;
				if (dst_filter_index < 0) break;

				// コピー元フィルタを取得します。
				auto src_filter = magi.exin.get_filter(object, src_filter_index);
				if (!src_filter) break;

				// コピー先フィルタを取得します。
				auto dst_filter = magi.exin.get_filter(object, dst_filter_index);
				if (!dst_filter) break;

				// 完全な複製を作成する場合は
				if (command_id == c_create_clone)
				{
					// 拡張データをコピーします。
					auto src_filter_exdata = magi.exin.get_exdata(object, src_filter_index);
					auto dst_filter_exdata = magi.exin.get_exdata(object, dst_filter_index);
					memcpy(dst_filter_exdata, src_filter_exdata, src_filter->exdata_size);

					// トラックデータをコピーします。
					for (auto i = 0; i < src_filter->track_n; i++)
					{
						auto src_track_index = object->filter_param[src_filter_index].track_begin + i;
						auto dst_track_index = object->filter_param[dst_filter_index].track_begin + i;
						object->track_value_left[dst_track_index] = object->track_value_left[src_track_index];
						object->track_value_right[dst_track_index] = object->track_value_right[src_track_index];
						object->track_mode[dst_track_index] = object->track_mode[src_track_index];
						object->track_param[dst_track_index] = object->track_param[src_track_index];
					}

					// チェックデータをコピーします。
					for (auto i = 0; i < src_filter->check_n; i++)
					{
						auto src_check_index = object->filter_param[src_filter_index].check_begin + i;
						auto dst_check_index = object->filter_param[dst_filter_index].check_begin + i;
						object->check_value[dst_check_index] = object->check_value[src_check_index];
					}
				}

				// 中間点ではない場合はここでループを終了します。
				if (midpt_leader < 0) break;

				// 次の中間点を取得します。
				object_index = magi.exin.get_next_object_index(object_index);
			}

			// コピー元フィルタのインデックスを取得します。
			auto src_filter_index = magi.exin.get_current_filter_index();
			if (src_filter_index < 0) return;

			// コピー先フィルタのインデックスを取得します。
			auto dst_filter_index = new_filter_index;
			if (dst_filter_index < 0) return;

			switch (command_id)
			{
			case c_create_same_above:
				{
					// コピー元のすぐ上に移動します。
					auto c = dst_filter_index - src_filter_index;
					for (auto i = 0; i < c; i++)
						magi.exin.swap_filter(orig_object_index, dst_filter_index--, -1);

					break;
				}
			case c_create_clone:
			case c_create_same_below:
				{
					// コピー元のすぐ下に移動します。
					auto c = dst_filter_index - src_filter_index - 1;
					for (auto i = 0; i < c; i++)
						magi.exin.swap_filter(orig_object_index, dst_filter_index--, -1);

					break;
				}
			}
		}

		//
		// このクラスは拡張編集の内部関数unknown1()をフックします。
		//
		void on_unknown1(unknown1_t orig_proc, int32_t object_index, int32_t filter_index)
		{
			MY_TRACE_FUNC("{/}, {/}", object_index, filter_index);

			orig_proc(object_index, filter_index);

			if (command_id)
				create_filter(object_index, filter_index);
		}
	} create_filter;
}
