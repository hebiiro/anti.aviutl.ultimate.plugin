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
		virtual LRESULT on_copy_filter(int32_t flag, BOOL flag_cut) override
		{
			// 名前付きミューテックスをロックします。
			my::Synchronizer synchronizer(model::property.mutex);

			// カレントフィルタのインデックスを取得します。
			auto filter_index = magi.exin.get_current_filter_index();
			if (filter_index < 0) return FALSE;

			// コンテキストを作成します。
			model::context_t context(magi.exin.get_current_object_index());

			// コンテキストが無効の場合はFALSEを返します。
			if (!context.is_valid()) return FALSE;

			// コピー済みフィルタを格納する配列です。
			std::vector<int32_t> copied_filters;

			// フラグで分岐します。
			switch (flag)
			{
			case 0:
				{
					// 指定されたフィルタをコピーします。
					if (context.write_alias(filter_index))
						copied_filters.emplace_back(filter_index);

					break;
				}
			case -1:
				{
					// 指定されたフィルタより上にあるフィルタをコピーします。
					for (auto i = 0; i <= filter_index; i++)
					{
						if (context.write_alias(i))
							copied_filters.emplace_back(filter_index);
					}

					break;
				}
			case 1:
				{
					// 指定されたフィルタより下にあるフィルタをコピーします。
					for (auto i = filter_index; i < ExEdit::Object::MAX_FILTER; i++)
					{
						if (context.write_alias(i))
							copied_filters.emplace_back(filter_index);
					}

					break;
				}
			}

			// コピー済みフィルタが存在しない場合はFALSEを返します。
			if (copied_filters.empty()) return FALSE;

			// カットする場合は
			if (flag_cut)
			{
				// コピー済みフィルタを削除します。

				// 対象オブジェクトのインデックスを取得します。
				auto object_index = context.object.get_object_index();

				// 中間点がある場合は先頭オブジェクトのインデックスを使用します。
				if (context.object.get_object()->index_midpt_leader >= 0)
					object_index = context.object.get_object()->index_midpt_leader;

				// 拡張編集のフィルタ削除処理と同じように実行します。
				magi.exin.push_undo();
				magi.exin.create_undo(object_index, 1);
				// フィルタのインデックスがずれないように、後ろのフィルタから削除します。
				for (auto it = copied_filters.crbegin(); it != copied_filters.crend(); it++)
					magi.exin.erase_filter(object_index, *it);
				magi.exin.redraw_setting_dialog(object_index);
				magi.exin.hide_controls();
				magi.exin.show_controls(magi.exin.get_current_object_index());

				// これを実行しないとプレビューが更新されません。
				magi.redraw();
			}

			// コピーデータをファイルに書き込みます。
			context.write_copy_data({ context.object.get_object()->flag, (int32_t)copied_filters.size() });

			return TRUE;
		}
	} copy_filter;
}
