#pragma once

namespace apn::font_select
{
	//
	// このクラスはメインダイアログです。
	//
	inline struct AddinDialog : my::Dialog
	{
		BOOL locked = FALSE;
		COLORREF colors[16] = {};

		//
		// 初期化処理を実行します。
		//
		BOOL init(HWND parent)
		{
			MY_TRACE_FUNC("{:#010x}", parent);

			return __super::create(hive.instance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), parent);
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
		// コントロールの値を更新します。
		//
		BOOL update_controls()
		{
			if (locked) return FALSE;

			locked = TRUE;

			set_int(IDC_ITEM_SIZE_W, hive.item_size.cx);
			set_int(IDC_ITEM_SIZE_H, hive.item_size.cy);
			set_int(IDC_FONT_SIZE_H, hive.font_size.cy);
			set_text(IDC_SAMPLE, hive.sample);
			set_combobox(IDC_PAINT_MODE, paint.mode);
			set_check(IDC_SINGLE_LINE, hive.single_line);
			set_uint(IDC_NORMAL_FILL, paint.palette[paint.c_style.c_normal].color.background);
			set_uint(IDC_NORMAL_TEXT, paint.palette[paint.c_style.c_normal].color.sample);
			set_uint(IDC_NORMAL_FONT, paint.palette[paint.c_style.c_normal].color.font_name);
			set_uint(IDC_SELECT_FILL, paint.palette[paint.c_style.c_select].color.background);
			set_uint(IDC_SELECT_TEXT, paint.palette[paint.c_style.c_select].color.sample);
			set_uint(IDC_SELECT_FONT, paint.palette[paint.c_style.c_select].color.font_name);
			set_check(IDC_USE_CONTEXT_MENU, hive.use_context_menu);
			set_check(IDC_USE_BACKWARD, hive.use_backward);
			set_check(IDC_USE_FORWARD, hive.use_forward);

			locked = FALSE;

			return TRUE;
		}

		//
		// コンフィグの値を更新します。
		//
		BOOL update_config()
		{
			if (locked) return FALSE;

			hive.item_size.cx = get_int(IDC_ITEM_SIZE_W);
			hive.item_size.cy = get_int(IDC_ITEM_SIZE_H);
			hive.font_size.cy = get_int(IDC_FONT_SIZE_H);
			hive.sample = get_text(IDC_SAMPLE);
			hive.single_line = get_check(IDC_SINGLE_LINE);
			paint.mode = get_combobox(IDC_PAINT_MODE);
			paint.palette[paint.c_style.c_normal].color.background = get_uint(IDC_NORMAL_FILL);
			paint.palette[paint.c_style.c_normal].color.sample = get_uint(IDC_NORMAL_TEXT);
			paint.palette[paint.c_style.c_normal].color.font_name = get_uint(IDC_NORMAL_FONT);
			paint.palette[paint.c_style.c_select].color.background = get_uint(IDC_SELECT_FILL);
			paint.palette[paint.c_style.c_select].color.sample = get_uint(IDC_SELECT_TEXT);
			paint.palette[paint.c_style.c_select].color.font_name = get_uint(IDC_SELECT_FONT);
			hive.use_context_menu = get_check(IDC_USE_CONTEXT_MENU);
			hive.use_backward = get_check(IDC_USE_BACKWARD);
			hive.use_forward = get_check(IDC_USE_FORWARD);

			auto hwnd = magi.exin.get_setting_dialog();
			auto combobox = ::GetDlgItem(hwnd, hive.c_control_id.c_font_combobox);
			::InvalidateRect(combobox, 0, TRUE);

			return TRUE;
		}

		//
		// ダイアログプロシージャです。
		//
		virtual INT_PTR on_dlg_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_INITDIALOG:
				{
					MY_TRACE_FUNC("WM_INITDIALOG, {:#010x}, {:#010x}", wParam, lParam);

					std::fill(std::begin(colors), std::end(colors), RGB(0xff, 0xff, 0xff));

					init_combobox(IDC_PAINT_MODE, _T("自動"), _T("システムカラー"), _T("テーマ"), _T("カスタムカラー"));

					break;
				}
			case WM_COMMAND:
				{
					MY_TRACE_FUNC("WM_COMMAND, {:#010x}, {:#010x}", wParam, lParam);

					auto code = HIWORD(wParam);
					auto id = LOWORD(wParam);
					auto control = (HWND)lParam;

					switch (id)
					{
					case IDOK:
					case IDCANCEL: return 0;
					case IDC_ITEM_SIZE_W:
					case IDC_ITEM_SIZE_H:
					case IDC_FONT_SIZE_H:
					case IDC_SAMPLE:
					case IDC_NORMAL_THEME:
					case IDC_SELECT_THEME:
						{
							if (code == EN_UPDATE) update_config();

							break;
						}
					case IDC_PAINT_MODE:
						{
							if (code == CBN_SELCHANGE) update_config();

							break;
						}
					case IDC_NORMAL_FILL:
					case IDC_NORMAL_TEXT:
					case IDC_NORMAL_FONT:
					case IDC_SELECT_FILL:
					case IDC_SELECT_TEXT:
					case IDC_SELECT_FONT:
						{
							auto color = get_uint(id);

							CHOOSECOLOR cc { sizeof(cc) };
							cc.hwndOwner = hwnd;
							cc.lpCustColors = colors;
							cc.rgbResult = color;
							cc.Flags = CC_RGBINIT | CC_FULLOPEN;
							if (!::ChooseColor(&cc)) return TRUE;

							color = cc.rgbResult;

							set_uint(id, color);
							::InvalidateRect(control, 0, FALSE);

							return TRUE;
						}
					case IDC_SINGLE_LINE:
					case IDC_USE_CONTEXT_MENU:
					case IDC_USE_BACKWARD:
					case IDC_USE_FORWARD:
						{
							update_config();

							break;
						}
					}

					break;
				}
			case WM_DRAWITEM:
				{
					auto id = (UINT)wParam;

					switch (id)
					{
					case IDC_NORMAL_FILL:
					case IDC_NORMAL_TEXT:
					case IDC_NORMAL_FONT:
					case IDC_SELECT_FILL:
					case IDC_SELECT_TEXT:
					case IDC_SELECT_FONT:
						{
							auto dis = (DRAWITEMSTRUCT*)lParam;
							auto color = get_uint(id);

							::FillRect(dis->hDC, &dis->rcItem,
								my::gdi::unique_ptr<HBRUSH>(::CreateSolidBrush(color)).get());

							return TRUE;
						}
					}

					break;
				}
			}

			return __super::on_dlg_proc(hwnd, message, wParam, lParam);
		}
	} addin_dialog;
}
