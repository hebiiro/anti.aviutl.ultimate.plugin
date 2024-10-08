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

					MY_TRACE_FUNC("WM_COMMAND, {:#06x}, {:#06x}, {:#010x}", code, id, sender);

					if (id == hive.c_control_id.c_font_combobox && code == CBN_DROPDOWN)
						font_listbox.start_hook(sender);

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

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} setting_dialog;
}
