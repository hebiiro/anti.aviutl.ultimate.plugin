#pragma once

namespace apn::font_preview
{
	//
	// このクラスは設定ダイアログのサブクラスです。
	//
	inline struct SettingDialog : my::Window
	{
		//
		// 初期化処理を行います。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// 設定ダイアログをサブクラス化します。
			subclass(magi.exin.get_setting_dialog());

			return TRUE;
		}

		//
		// 後始末処理を行います。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			// 設定ダイアログのサブクラス化を解除します。
			unsubclass();

			return TRUE;
		}


		//
		// オーナードロー用の処理です。
		//
		BOOL on_draw_item(DRAWITEMSTRUCT* dis)
		{
//			MY_TRACE_FUNC("{}", dis->itemID);

			auto dc = dis->hDC;
			auto rc = dis->rcItem;
			auto font_name = my::get_text_from_combobox(dis->hwndItem, dis->itemID);

			MY_TRACE("{}, {}, {:#010x}, {:#010x}\n",
				font_name, dis->itemID, dis->itemAction, dis->itemState);

			if (dis->itemState & ODS_COMBOBOXEDIT)
			{
				// コンボボックスの本体部分を描画します。

				auto style = paint.c_style.c_normal;

				if (dis->itemState & ODS_SELECTED) style = paint.c_style.c_select;

				// 背景を描画します。
				paint.draw_background(dc, &rc, style);

				// フォント名を描画します。
				paint.draw_font_name(dc, font_name.c_str(), -1, &rc,
					DT_LEFT | DT_VCENTER | DT_SINGLELINE, style);
			}
			else
			{
				// ドロップダウンリストのアイテムを描画します。

				auto style = paint.c_style.c_normal;

				if (dis->itemState & ODS_FOCUS) return FALSE;
				if (dis->itemState & ODS_SELECTED) style = paint.c_style.c_select;

				// 背景を描画します。
				paint.draw_background(dc, &rc, style);

				if (hive.singleline)
				{
					// フォント名とサンプルをまとめて描画します。

					auto text = font_name + hive.sample;

					my::gdi::unique_ptr<HFONT> font(::CreateFont(
						hive.font_height, 0, 0, 0, 0, FALSE, FALSE, FALSE,
						DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
						DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name.c_str()));
					my::gdi::selector font_selector(dc, font.get());

					paint.draw_sample(dc, text.c_str(), -1, &rc, DT_LEFT | DT_TOP, style);
				}
				else
				{
					{
						// サンプルを描画します。

						auto text = hive.sample;

						my::gdi::unique_ptr<HFONT> font(::CreateFont(
							hive.font_height, 0, 0, 0, 0, FALSE, FALSE, FALSE,
							DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
							DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name.c_str()));
						my::gdi::selector font_selector(dc, font.get());

						auto size = RECT {};
						paint.draw_sample(dc, text.c_str(), -1, &size, DT_CALCRECT, style);

						auto text_rc = rc;
						text_rc.top = rc.bottom - size.bottom;
						paint.draw_sample(dc, text.c_str(), -1, &text_rc, DT_RIGHT | DT_TOP, style);
					}

					{
						// フォント名を描画します。

						auto text = font_name;

						paint.draw_font_name(dc, text.c_str(), -1, &rc, DT_LEFT | DT_TOP, style);
					}
				}
			}

			return TRUE;
		}

		//
		// コンテキストメニューを表示します。
		//
		BOOL show_context_menu(const POINT& point)
		{
			MY_TRACE_FUNC("");

			// コンテキストメニューが無効の場合は何もしません。
			if (!hive.use_context_menu) return FALSE;

			//
			// 指定されたキーが押されていない場合はTRUEを返します。
			//
			constexpr auto is_key_up = [](DWORD vk) { return ::GetKeyState(vk) >= 0; };

			// 指定された修飾キーが押されていない場合は何もしません。
			if ((hive.use_shift_key && is_key_up(VK_SHIFT)) ||
				(hive.use_ctrl_key && is_key_up(VK_CONTROL)) ||
				(hive.use_alt_key && is_key_up(VK_MENU)) ||
				(hive.use_win_key && (is_key_up(VK_LWIN) && is_key_up(VK_RWIN))))
			{
				return FALSE;
			}

			// ポップアップメニューのアイテムIDです。
			constexpr uint32_t c_copy_font_name = 1;

			// ポップアップメニューを作成します。
			my::menu::unique_ptr<> menu(::CreatePopupMenu());

			// ポップアップメニューに項目を追加します。
			::AppendMenu(menu.get(), MF_STRING, c_copy_font_name, _T("フォント名をコピー"));

			// ポップアップメニューを表示します。
			auto id = ::TrackPopupMenuEx(menu.get(),
				TPM_NONOTIFY | TPM_RETURNCMD, point.x, point.y, *this, nullptr);
			if (!id) return FALSE;

			switch (id)
			{
			case c_copy_font_name: return copy_font_name();
			}

			return TRUE;
		}

		//
		// 選択中のフォント名をクリップボードにコピーします。
		//
		BOOL copy_font_name()
		{
			// クリップボードを開きます。
			struct ClipboardLocker {
				ClipboardLocker(HWND hwnd = nullptr) {
					::OpenClipboard(hwnd);
				}
				~ClipboardLocker() {
					::CloseClipboard();
				}
			} clipboard_locker;

			// フォントコンボボックスを取得します。
			auto combobox = magi.exin.get_font_combobox();

			// 選択中のフォントを取得します。
			auto index = ::SendMessageW(combobox, CB_GETCURSEL, 0, 0);

			// フォント名の長さを取得します。
			auto text_length = ::SendMessageW(combobox, CB_GETLBTEXTLEN, index, 0);
			if (text_length == CB_ERR) return FALSE;

			// クリップボードを空にします。
			::EmptyClipboard();

			// メモリハンドルを作成します。
			auto handle = ::GlobalAlloc(GHND, (text_length + 1) * sizeof(WCHAR));

			// メモリハンドルをロックしてメモリバッファを取得します。
			auto buffer = (LPWSTR)::GlobalLock(handle);

			// フォント名を取得します。
			::SendMessageW(combobox, CB_GETLBTEXT, index, (LPARAM)buffer);

			// メモリハンドルのロックを解除します。
			::GlobalUnlock(handle);

			// メモリハンドルをクリップボードにセットします。
			// (メモリハンドルをセットしたあとは開放できません)
			::SetClipboardData(CF_UNICODETEXT, handle);

			return TRUE;
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_COMMAND:
				{
					auto code = HIWORD(wParam);
					auto id = LOWORD(wParam);
					auto control = (HWND)lParam;

					MY_TRACE_FUNC("WM_COMMAND, {:#06x}, {:#06x}, {:#010x}", code, id, control);

					// フォントコンボボックスがドロップダウンした瞬間に
					if (code == CBN_DROPDOWN && control == magi.exin.get_font_combobox())
					{
						// リストボックスのフックを開始します。
						font_listbox.start_hook(control);
					}

					break;
				}
			case WM_DRAWITEM:
				{
					// フォントコンボボックスの場合は
					auto dis = (DRAWITEMSTRUCT*)lParam;
					if (dis->CtlType == ODT_COMBOBOX && dis->hwndItem == magi.exin.get_font_combobox())
					{
						// オーナードローで描画します。
						on_draw_item(dis);
					}

					break;
				}
			case WM_CONTEXTMENU:
				{
					MY_TRACE_FUNC("WM_CONTEXTMENU, {:#010x}, {:#010x}", wParam, lParam);

					// フォントコンボボックスの場合は
					auto control = (HWND)wParam;
					if (control == magi.exin.get_font_combobox())
					{
						// コンテキストメニューを表示します。
						show_context_menu(my::lp_to_pt(lParam));
					}

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} setting_dialog;
}
