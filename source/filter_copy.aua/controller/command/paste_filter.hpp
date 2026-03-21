#pragma once

namespace apn::filter_copy::controller::command
{
	//
	// このクラスはコントローラ層のフィルタ貼り付け処理です。
	//
	inline struct paste_filter_t : view::listener::paste_filter_t
	{
		using add_alias_t = BOOL (CDECL*)(LPCSTR file_name, BOOL flag1, BOOL flag2, int32_t object_index);

		//
		// 独自のエイリアス読み込み処理を行うためのフラグです。
		//
		BOOL flag_hook_add_alias = FALSE;

		//
		// オブジェクトのタイプを返します。
		//
		inline static int32_t get_type(uint32_t flag)
		{
			if (flag & 0x00020000) {
				if (flag & 0x00050000)
					return 1; // 音声メディアオブジェクト
				else
					return 2; // 音声フィルタオブジェクト
			} else {
				if (flag & 0x00050000)
					return 3; // 映像メディアオブジェクト＆グループ制御
				else {
					if (flag & 0x00080000)
						return 4; // カメラ制御＆時間制御
					else
						return 5; // 映像フィルタオブジェクト
				}
			}
		}

		//
		// カレントオブジェクトにフィルタを貼り付けます。
		//
		LRESULT on_paste_filter()
		{
			MY_TRACE_FUNC("");

			// カレントオブジェクトのインデックスを取得します。
			auto object_index = magi.exin.get_current_object_index();
			MY_TRACE_INT(object_index);
			if (object_index < 0) return FALSE;

			// オブジェクトを取得します。
			auto object = magi.exin.get_object(object_index);
			MY_TRACE_HEX(object);
			if (!object) return FALSE;

			if (object->filter_param[0].id == 93) // 93 == 時間制御
				return FALSE; // 「時間制御」には貼り付けられるフィルタがないので何もしません。

			// オブジェクトのタイプを取得します。
			auto src_type = get_type((uint32_t)model::state.flag);
			auto dst_type = get_type((uint32_t)object->flag);
			MY_TRACE_INT(src_type);
			MY_TRACE_INT(dst_type);

			// オブジェクトのタイプが異なる場合は何もしません。
			if (src_type != dst_type) return FALSE;

			// フラグを立ててmagi.exin.add_alias()をフックするようにします。
			flag_hook_add_alias = TRUE;

			// スコープ終了時にフラグを消去します。
			my::scope_exit scope_exit([this]() { flag_hook_add_alias = FALSE; });

			// この中でmagi.exin.add_alias()が呼ばれます。
			// 1036はエイリアスを追加するコマンド、0はエイリアスのインデックスです。
			return ::SendMessage(magi.exin.get_exedit_window(), WM_COMMAND, 1036, 0);
		}

		//
		// 拡張編集の内部関数add_alias()をフックします。
		//
		BOOL on_add_alias(add_alias_t orig_proc, LPCSTR file_name, BOOL flag1, BOOL flag2, int32_t object_index)
		{
			MY_TRACE_FUNC("{/}, {/}, {/}, {/}", file_name, flag1, flag2, object_index);

			if (!flag_hook_add_alias) // フラグが立っていない場合はデフォルト処理を実行します。
				return orig_proc(file_name, flag1, flag2, object_index);

			// オブジェクトを取得します。
			auto object = magi.exin.get_object(object_index);
			MY_TRACE_HEX(object);
			if (!object) return FALSE;

			// カレントフィルタを取得します。
			auto filter_index = magi.exin.get_current_filter_index();
			MY_TRACE_INT(filter_index);
			if (filter_index < 0) return FALSE;

			// カメラ制御のフラグを取得しておきます。
			auto camera = object->flag & ExEdit::Object::Flag::Camera;

			// フィルタを挿入する位置です。
			auto insert_pos = filter_index;

			// オーダーを走査します。
			for (const auto& order : model::state.orders)
			{
				MY_TRACE_STR(order.file_name.c_str());

				// フィルタを末尾に追加します。
				auto result = orig_proc(order.file_name.c_str(), flag1, flag2, object_index);

				// 末尾に追加されたフィルタのインデックスを取得します。
				auto c = magi.exin.get_moveable_filter_count(object);

				// 末尾に追加されたフィルタを挿入位置まで移動します。
				for (int32_t i = c - 1; i > insert_pos + 1; i--)
					magi.exin.swap_filter(object_index, i, -1);

				// 次の挿入位置に移動します。
				insert_pos++;
			}

			// カメラ制御のフラグを復元します。
			object->flag |= camera;

			// 1個以上のフィルタが挿入された場合はTRUEを返します。
			return insert_pos > filter_index;
		}
	} paste_filter;
}
