#pragma once

namespace apn::item_list
{
	//
	// このクラスはアプリケーションです。
	//
	inline struct app_t : app_interface_t
	{
		//
		// アイテム一覧をリフレッシュするタイマーのIDです。
		//
		UINT_PTR refresh_timer_id = {};

		//
		// 拡張編集のメモリを参照して、
		// カレントシーンのアイテム一覧を構築します。
		//
		BOOL build_item_list_from_exedit_memory()
		{
			MY_TRACE_FUNC("");

			// aviutlの編集ハンドルを(拡張編集から)取得します。
			auto editp = magi.exin.get_editp();
			if (!editp) return FALSE;

			// aviutlのプロジェクトファイル情報を取得します。
			AviUtl::FileInfo fi = {};
			magi.fp->exfunc->get_file_info(editp, &fi);
			if (fi.video_rate <= 0 || fi.video_scale <= 0) return FALSE;

			// フレーム数を時間に変換するための倍率を算出します。
			auto frame_to_sec_scale = (double)fi.video_scale / fi.video_rate;

			//
			// この関数は、指定された値をデフォルトロケールで文字列化して返します。
			//
			const auto to_frame_string = [&](auto value) -> std::wstring
			{
				// 時間として表示する場合は
				if (hive.flag_as_time)
				{
					auto sec = value * frame_to_sec_scale;
					auto sec2 = (int)(sec * 100) % 100;
					auto sec1 = (int)sec % 60;
					auto min = (int)sec / 60 % 60;
					auto hour = (int)sec / 3600;

					// フレーム数を時間に変換して文字列化して返します。
					return my::format(L"{/02d}:{/02d}:{/02d}.{/02d}", hour, min, sec1, sec2);
				}
				else
				{
					// 文字列ストリームを作成します。
					std::wstringstream ss;

					// デフォルトロケールをセットします。
					ss.imbue(std::locale(""));

					// 数値をストリームに書き込みます。
					ss << value;

					// ストリームを文字列化して返します。
					return ss.str();
				}
			};

			//
			// この関数は、指定されたアイテムのレイヤーを文字列として返します。
			//
			constexpr auto to_layer_string = [](const auto& object) -> std::wstring
			{
				// レイヤー設定を取得します。
				auto layer_setting = magi.exin.get_layer_setting(object->layer_set);

				// 文字列化されたアイテムのレイヤーです。
				auto layer_string = my::format(L"{/}" L"," L"{/}", object->layer_set + 1, layer_setting->name ? layer_setting->name : "");

				// レイヤーが非表示の場合は
				if (has_flag(layer_setting->flag, decltype(layer_setting->flag)::UnDisp)) layer_string += L"," L"非表示";

				// レイヤーがロックされている場合は
				if (has_flag(layer_setting->flag, decltype(layer_setting->flag)::Locked)) layer_string += L"," L"ロック";

				// 「座標のリンク」が有効の場合は
				if (has_flag(layer_setting->flag, decltype(layer_setting->flag)::CoordLink)) layer_string += L"," L"座標のリンク";

				// 「上のオブジェクトでクリッピング」が有効の場合は
				if (has_flag(layer_setting->flag, decltype(layer_setting->flag)::Clip)) layer_string += L"," L"上のオブジェクトでクリッピング";

				// 文字列化されたアイテムのレイヤーを返します。
				return layer_string;
			};

			//
			// この関数は、指定されたアイテムのステータスを文字列として返します。
			//
			constexpr auto to_status_string = [](const auto& object) -> std::wstring
			{
				// 文字列化されたアイテムのステータスです。
				auto status_string = my::format(L"{/}", object->group_belong);

				// 中間点の場合は
				if (object->index_midpt_leader >= 0)
				{
					status_string += L"," L"中間点";
#if 0
					if (object->index_midpt_leader == magi.exin.get_object_index(object))
						status_string += L"リーダー";
#endif
				}

				// カーソルフレームを取得します。
				auto cursor_frame = magi.exin.get_exedit_current_frame();

				// カーソルフレームと交差している場合は
				if (object->frame_begin <= cursor_frame && cursor_frame <= object->frame_end) status_string += L"," L"カーソル";

				// フォーカスオブジェクトの場合は
				if (magi.exin.get_current_object_index() == magi.exin.get_object_index(object)) status_string += L"," L"フォーカス";

				// 文字列化されたアイテムのステータスを返します。
				return status_string;
			};

			//
			// この関数は、指定されたアイテムの名前を文字列として返します。
			//
			constexpr auto to_name_string = [](const auto& object) -> std::wstring
			{
				// アイテムの名前を文字列化して返します。
				return my::format(L"{/}", object->dispname);
			};

			//
			// この関数は、指定されたアイテムの情報を文字列として返します。
			//
			constexpr auto to_info_string = [](const auto& object) -> std::wstring
			{
				// フィルタの数です。
				auto c = 0;

				// すべてのフィルタを走査します。
				for (auto j = 0; j < object->MAX_FILTER; j++)
				{
					// フィルタが無効の場合はループを終了します。
					if (object->filter_param[j].id < 0) break;

					// フィルタの数をカウントします。
					c++;
				}

				// 文字列化されたアイテムの情報です。
				auto info = my::format(L"{/}", c);

				// フィルタを走査します。
				for (auto j = 0; j < c; j++)
				{
					// フィルタ名を文字列に追加します。
					info += my::format(L"," L"{/}", get_filter_name(object, j));
				}

				// 文字列化されたアイテムの情報を返します。
				return info;
			};

			// コレクションをリセットします。
			list.nodes.clear();
			finder.nodes.clear();

			// ソート済みオブジェクトを走査します。
			auto c = magi.exin.get_sorted_object_count();
			for (decltype(c) i = 0; i < c; i++)
			{
				// ソート済みオブジェクトを取得します。
				auto object = magi.exin.get_sorted_object(i);

				// ノードを新規作成します。
				auto node = list.create_node(object);

				auto start = object->frame_begin;
				auto range = object->frame_end - object->frame_begin + 1;

				// ノードの値をセットします。
				node->values[list.k_column.k_layer] = to_layer_string(object);
				node->values[list.k_column.k_status] = to_status_string(object);
				node->values[list.k_column.k_name] = to_name_string(object);
				node->values[list.k_column.k_info] = to_info_string(object);
				node->values[list.k_column.k_start] = to_frame_string(start);
				node->values[list.k_column.k_range] = to_frame_string(range);
			}

			return TRUE;
		}

		//
		// アイテム一覧を絞り込みます。
		//
		virtual BOOL apply_finder() override
		{
			MY_TRACE_FUNC("");

			// アイテム一覧を絞り込みます。
			finder.apply();

			// ダイアログのリストビューをリフレッシュします。
			listview.refresh();

			return TRUE;
		}

		//
		// アイテム一覧をリフレッシュします。
		//
		virtual BOOL refresh_list() override
		{
			MY_TRACE_FUNC("");

			// タイマーを開始します。
			refresh_timer_id = ::SetTimer(nullptr, refresh_timer_id, hive.refresh_delay,
				[](HWND hwnd, UINT message, UINT_PTR timer_id, DWORD time)
			{
				// アイテム一覧を構築します。
				app_impl.build_item_list_from_exedit_memory();

				// アイテム一覧をソートします。
				sort.apply();

				// アイテム一覧を絞り込みます。
				finder.apply();

				// ダイアログのリストビューをリフレッシュします。
				listview.refresh();

				// ダイアログのコンボボックスをリフレッシュします。
				combobox_manager.refresh();

				// タイマーを終了します。
				::KillTimer(nullptr, app_impl.refresh_timer_id), app_impl.refresh_timer_id = {};
			});

			return TRUE;
		}

		//
		// アイテム一覧をリセットします。
		//
		virtual BOOL reset_list() override
		{
			MY_TRACE_FUNC("");

			// コレクションをリセットします。
			list.nodes.clear();
			finder.nodes.clear();

			// ダイアログのリストビューをリフレッシュします。
			listview.refresh();

			// ダイアログのコンボボックスをリフレッシュします。
			combobox_manager.refresh();

			return TRUE;
		}

		//
		// アイテム一覧のソートをリセットします。
		//
		virtual BOOL reset_sort() override
		{
			MY_TRACE_FUNC("");

			// ソートをリセットします。
			sort.nodes.clear();
			addin_dialog.reset_columns();

			// リストをリフレッシュします。
			sort.apply();
			finder.apply();
			listview.refresh();

			return TRUE;
		}
	} app_impl;
}
