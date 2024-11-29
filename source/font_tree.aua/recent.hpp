#pragma once

namespace apn::font_tree
{
	//
	// このクラスはフォントの使用履歴を管理するコンボボックスです。
	//
	inline struct RecentManager : my::Window
	{
		//
		// すべての項目を消去します。
		//
		void clear()
		{
			ComboBox_ResetContent(*this);
		}

		//
		// 履歴にフォント名を追加します。
		//
		void insert(LPCWSTR font_name)
		{
			// 指定されたフォント名が既存かどうかチェックします。
			auto index = cb::find_string(*this, font_name);
			if (index >= 0)
			{
				// 既存の場合は一旦このフォント名を削除します。
				ComboBox_DeleteString(*this, index);
			}

			// リストの先頭にフォント名を追加します。
			ComboBox_InsertString(*this, 0, font_name);
			ComboBox_SetCurSel(*this, 0);
		}

		//
		// 履歴からフォント名を削除します。
		//
		void erase(LPCWSTR font_name)
		{
			// 指定されたフォント名が既存かどうかチェックします。
			auto index = cb::find_string(*this, font_name);
			if (index >= 0)
			{
				// 既存の場合はこのフォント名を削除します。
				ComboBox_DeleteString(*this, index);

				// 要素数が0になったとき再描画が発行されないので、ここで明示的に発行します。
				my::invalidate(*this);
			}
		}

		//
		// 初期化処理を実行します。
		//
		BOOL init(HWND parent)
		{
			MY_TRACE_FUNC("{:#010x}", parent);

			// コンボボックスを作成します。
			return create(
				0,
				WC_COMBOBOX,
				_T("recent"),
				WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
//				CBS_AUTOHSCROLL | CBS_DISABLENOSCROLL |
				CBS_DROPDOWNLIST | CBS_HASSTRINGS,
				0, 0, 100, 100,
				parent, nullptr, hive.instance, nullptr);
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return destroy();
		}

		//
		// コンテキストメニューを表示します。
		//
		BOOL show_context_menu(const POINT& point)
		{
			MY_TRACE_FUNC("");

			// テキストオブジェクトのフォントコンボボックスを取得します。
			auto font_combobox = magi.exin.get_font_combobox();

			// フォントコンボボックスのカレントテキストを取得します。
			auto font_name = cb::get_text(font_combobox, -1);

			// 履歴コンボボックスのカレントテキストを取得します。
			auto recent_font_name = cb::get_text(*this, -1);

			// ポップアップメニューのアイテムIDです。
			constexpr uint32_t c_insert_recent = 1;
			constexpr uint32_t c_erase_recent = 2;
			constexpr uint32_t c_show_config_dialog = 1000;
			constexpr uint32_t c_read_xml = 1001;

			// ポップアップメニューを作成します。
			my::menu::unique_ptr<> menu(::CreatePopupMenu());

			// フォントコンボボックスのフォント名が有効の場合は
			if (font_name.length())
			{
				// 履歴コンボボックスにfont_nameが存在しない場合は
				if (cb::find_string(*this, font_name.c_str()) < 0)
				{
					::AppendMenu(menu.get(), MF_STRING, c_insert_recent,
						my::format(_T("{}を追加"), font_name).c_str());
				}
			}

			// 履歴コンボボックスのフォント名が有効の場合は
			if (recent_font_name.length())
			{
				::AppendMenu(menu.get(), MF_STRING, c_erase_recent,
					my::format(_T("{}を削除"), recent_font_name).c_str());
			}

			if (::GetMenuItemCount(menu.get()))
				::AppendMenu(menu.get(), MF_SEPARATOR, 0, nullptr);
			::AppendMenu(menu.get(), MF_STRING, c_show_config_dialog, _T("フォントツリーの設定"));
			::AppendMenu(menu.get(), MF_STRING, c_read_xml, _T("xmlファイル(旧形式データ)を読み込む"));

			auto id = ::TrackPopupMenuEx(menu.get(),
				TPM_NONOTIFY | TPM_RETURNCMD, point.x, point.y, hwnd, nullptr);
			if (!id) return FALSE;

			switch (id)
			{
			case c_insert_recent:
				{
					insert(font_name.c_str());

					break;
				}
			case c_erase_recent:
				{
					erase(recent_font_name.c_str());

					break;
				}
			case c_show_config_dialog:
				{
					app->show_config_dialog();

					break;
				}
			case c_read_xml:
				{
					app->read_xml();

					break;
				}
			}

			return TRUE;
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_CREATE:
				{
					MY_TRACE_FUNC("WM_CREATE, {:#010x}, {:#010x}", wParam, lParam);

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {:#010x}, {:#010x}", wParam, lParam);

					break;
				}
			case WM_CONTEXTMENU:
				{
					MY_TRACE_FUNC("WM_CONTEXTMENU, {:#010x}, {:#010x}", wParam, lParam);

					show_context_menu(my::lp_to_pt(lParam));

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		//
		// 使用履歴を読み込みます。
		//
		void read(n_json& node)
		{
			MY_TRACE_FUNC("");

			// 使用履歴を読み込みます。
			get_child_nodes(node, "recent",
				[&](const n_json& recent_node, size_t i)
			{
				// フォント名を読み込みます。
				std::wstring font_name;
				get_string(recent_node, "font_name", font_name);
				MY_TRACE_STR(font_name);

				// フォント名が有効の場合は使用履歴に追加します。
				if (font_name.length())
					ComboBox_AddString(*this, font_name.c_str());

				return TRUE;
			});
		}

		//
		// 使用履歴を書き込みます。
		//
		void write(n_json& node)
		{
			MY_TRACE_FUNC("");

			// まず使用履歴のコレクションを作成します。
			std::vector<std::wstring> recents(ComboBox_GetCount(*this));
			for (auto& recent : recents)
			{
				recent = cb::get_text(*this,
					std::distance(recents.data(), &recent));
			}

			// 使用履歴を書き込みます。
			set_child_nodes(node, "recent", recents,
				[](n_json& recent_node, const auto& recent, size_t i)
			{
				// フォント名を書き込みます。
				set_string(recent_node, "font_name", recent);

				return TRUE;
			});
		}
	} recent_manager;
}
