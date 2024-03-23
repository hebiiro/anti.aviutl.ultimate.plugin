#pragma once

namespace apn::font_preview
{
	//
	// このクラスは設定ダイアログのサブクラスです。
	//
	inline struct SettingDialog : my::Window
	{
		//
		// コンボボックス内の指定されたアイテムのテキストを返します。
		//
		static std::wstring get_text_from_combobox(HWND combobox, int index)
		{
			if (index == -1)
				index = (int)::SendMessageW(combobox, CB_GETCURSEL, 0, 0);

			// テキストの長さを取得します。
			auto text_length = (int)::SendMessageW(combobox, CB_GETLBTEXTLEN, index, 0);
			if (text_length <= 0) return L"";

			// テキストを取得します。
			std::wstring text(text_length, L'\0');
			::SendMessageW(combobox, CB_GETLBTEXT, index, (LPARAM)text.data());

			return text;
		}

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
		// ドロップダウン時の処理です。
		// ドロップダウンリストの位置を調整します。
		//
		BOOL on_drop_down(HWND combobox)
		{
			MY_TRACE_FUNC("{:#010x}", combobox);

			COMBOBOXINFO cbi = { sizeof(cbi) };
			::GetComboBoxInfo(combobox, &cbi);

			auto listbox = cbi.hwndList;

			// 一旦リストボックスを非表示にします。
			::ShowWindow(listbox, SW_HIDE);

			auto rc_combobox = my::get_window_rect(combobox);
			auto rc_listbox = RECT { 0, 0, hive.window_size.cx, hive.window_size.cy };
			my::client_to_window(listbox, &rc_listbox);
			auto x = 0;
			auto y = 0;
			auto w = my::get_width(rc_listbox);
			auto h = my::get_height(rc_listbox);

			{
				// 一旦リストボックスにサイズを適用します。
				::SetWindowPos(listbox, 0, 0, 0, w, h,
					SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

				// 改めてサイズを取得します。
				rc_listbox = my::get_window_rect(listbox);
				w = my::get_width(rc_listbox);
				h = my::get_height(rc_listbox);
			}

			auto flag = FALSE;
			auto rc_monitor = my::get_monitor_rect(combobox);

			// リストボックスのボトムがモニタのボトムからはみ出す場合は
			if (rc_combobox.bottom + h > rc_monitor.bottom)
			{
				// リストボックスのトップがモニタのトップからはみ出す場合は
				if (rc_combobox.top - h < rc_monitor.top)
				{
					y = rc_monitor.top;
					h = std::min<int>(h, my::get_height(rc_monitor));

					// リストボックスのレフトがモニタのレフトからはみ出す場合は
					if (rc_combobox.left - w < rc_monitor.left)
					{
						// リストボックスをコンボボックスの右側に配置します。
						x = rc_combobox.right;
						w = std::min<int>(w, rc_monitor.right - x);
					}
					else
					{
						// リストボックスをコンボボックスの左側に配置します。
						x = rc_combobox.left - w;
					}

					// xとwをセットしたのでフラグを立てます。
					flag = TRUE;
				}
				else
				{
					// リストボックスをコンボボックスの上側に配置します。
					y = rc_combobox.top - h;
				}
			}
			else
			{
				// リストボックスをコンボボックスの下側に配置します。
				y = rc_combobox.bottom;
			}

			// xとwがまだセットされていない場合は
			if (!flag)
			{
				// リストボックスの幅がモニタの幅を超えないように調整します。
				w = std::min<int>(w, my::get_width(rc_monitor));

				// リストボックスのレフトがモニタのレフトからはみ出す場合は
				if (rc_combobox.left < rc_monitor.left)
				{
					// リストボックスのレフトがモニタのレフトと一致するように配置します。
					x = rc_monitor.left;
				}
				// リストボックスのライトがモニタのライトからはみ出す場合は
				else if (rc_combobox.left + w > rc_monitor.right)
				{
					// リストボックスのライトがモニタのライトと一致するように配置します。
					x = rc_monitor.right - w;
				}
				else
				{
					// リストボックスのレフトがコンボボックスのレフトと一致するように配置します。
					x = rc_combobox.left;
				}
			}

			// リストボックス内のアイテムの高さを変更します。
			::SendMessage(combobox, CB_SETITEMHEIGHT, 0, hive.item_size.cy);

			// リストボックス本体の位置を変更します。
			::SetWindowPos(listbox, 0, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

			// リストボックスを表示します。
			::ShowWindow(listbox, SW_SHOW);

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
			auto font_name = get_text_from_combobox(dis->hwndItem, dis->itemID);

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
					DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX, style);
			}
			else
			{
				// ドロップダウンリストのアイテムを描画します。

				auto style = paint.c_style.c_normal;

				if (dis->itemState & ODS_FOCUS) return FALSE;
				if (dis->itemState & ODS_SELECTED) style = paint.c_style.c_select;

				// 背景を描画します。
				paint.draw_background(dc, &rc, style);

				if (hive.single_line)
				{
					// フォント名とサンプルをまとめて描画します。

					auto text = font_name + hive.sample;

					my::gdi::unique_ptr<HFONT> font(::CreateFont(
						my::get_height(rc), 0, 0, 0, 0, FALSE, FALSE, FALSE,
						DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
						DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name.c_str()));
					my::gdi::selector font_selector(dc, font.get());

					paint.draw_sample(dc, text.c_str(), -1, &rc, DT_LEFT | DT_TOP | DT_NOPREFIX, style);
				}
				else
				{
					{
						// サンプルを描画します。

						auto text = hive.sample;

						my::gdi::unique_ptr<HFONT> font(::CreateFont(
							my::get_height(rc), 0, 0, 0, 0, FALSE, FALSE, FALSE,
							DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
							DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name.c_str()));
						my::gdi::selector font_selector(dc, font.get());

						paint.draw_sample(dc, text.c_str(), -1, &rc, DT_RIGHT | DT_BOTTOM | DT_NOPREFIX, style);
					}

					{
						// フォント名を描画します。

						auto text = font_name;

						paint.draw_font_name(dc, text.c_str(), -1, &rc, DT_LEFT | DT_TOP | DT_NOPREFIX, style);
					}
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
			case WM_COMMAND:
				{
					auto code = HIWORD(wParam);
					auto id = LOWORD(wParam);
					auto sender = (HWND)lParam;

					MY_TRACE_FUNC("WM_COMMAND, {:#04x}, {:#04x}, {:#010x}", code, id, sender);

					if (id == hive.c_control_id.c_font_combobox && code == CBN_DROPDOWN)
					{
						// このタイミングでは早すぎるので
						// 一旦メッセージをポストしてから処理します。
						::PostMessage(hwnd, hive.c_message.c_drop_down, 0, (LPARAM)sender);
					}

					break;
				}
			case WM_DRAWITEM:
				{
					auto dis = (DRAWITEMSTRUCT*)lParam;
					if (dis->CtlType == ODT_COMBOBOX && dis->CtlID == hive.c_control_id.c_font_combobox)
						on_draw_item(dis);

					break;
				}
			}

			if (message == hive.c_message.c_drop_down)
			{
				// CBN_DROPDOWNからの続きを処理します。
				on_drop_down((HWND)lParam);
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} setting_dialog;
}
