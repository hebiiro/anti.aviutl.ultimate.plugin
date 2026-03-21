#pragma once

namespace apn::filter_copy::controller::command
{
	//
	// このクラスはコントローラ層のフィルタコピー処理です。
	//
	inline struct copy_filter_t : view::listener::copy_filter_t
	{
		//
		// 指定されたフィルタのコピーを発注します。
		//
		virtual LRESULT on_copy_filter(int32_t flag, BOOL cut) override
		{
			// カレントフィルタのインデックスを取得します。
			auto filter_index = magi.exin.get_current_filter_index();
			if (filter_index < 0) return FALSE;

			// コンテキストを作成します。
			model::context_t context;

			// コンテキストが無効の場合はFALSEを返します。
			if (!context.is_valid()) return FALSE;

			// オーダーの初期化に失敗した場合はFALSEを返します。
			if (!context.init_order()) return FALSE;

			// 戻り値です。
			auto ret_value = FALSE;

			switch (flag)
			{
			case 0:
				{
					// 指定されたフィルタのコピーを発注します。
					ret_value |= context.order(filter_index);

					break;
				}
			case -1:
				{
					// 指定されたフィルタより上にあるフィルタのコピーを発注します。
					for (int32_t i = 0; i <= filter_index; i++)
						ret_value |= context.order(i);

					break;
				}
			case 1:
				{
					// 指定されたフィルタより下にあるフィルタのコピーを発注します。
					for (int32_t i = filter_index; i < ExEdit::Object::MAX_FILTER; i++)
						ret_value |= context.order(i);

					break;
				}
			}

			if (cut) // カットする場合は、ここでフィルタを削除します。
			{
				// 対象オブジェクトのインデックスを取得します。
				auto object_index = context.object.get_object_index();

				// 中間点がある場合は先頭オブジェクトのインデックスを使用します。
				if (context.object.get_object()->index_midpt_leader >= 0)
					object_index = context.object.get_object()->index_midpt_leader;

				// 拡張編集のフィルタ削除処理と同じように実行します。
				magi.exin.push_undo();
				magi.exin.create_undo(object_index, 1);
				for (auto i = (int32_t)model::state.orders.size() - 1; i >= 0; i--)
					magi.exin.erase_filter(object_index, model::state.orders[i].filter_index);
				magi.exin.redraw_setting_dialog(object_index);
				magi.exin.hide_controls();
				magi.exin.show_controls(magi.exin.get_current_object_index());

				// これを実行しないとプレビューが更新されません。
				magi.redraw();
			}

			// 戻り値を返します。
			return ret_value;
		}
	} copy_filter;
}
