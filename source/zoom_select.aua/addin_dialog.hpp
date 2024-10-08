#pragma once

namespace apn::zoom_select
{
	//
	// このクラスはアドインダイアログです。
	//
	inline struct AddinDialog : my::Dialog
	{
		inline static constexpr auto c_item_id_offset = IDC_ZOOM_1 - IDC_ZOOM_0;

		BOOL locked = FALSE;

		//
		// 初期化処理を実行します。
		//
		BOOL init(HWND parent)
		{
			return __super::create(hive.instance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), parent);
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			return destroy();
		}

		//
		// コントロールの値を更新します。
		//
		BOOL update_controls()
		{
			if (locked) return FALSE;

			locked = TRUE;

			for (size_t i = 0; i < std::size(hive.zoom_values); i++)
			{
				set_int(IDC_ZOOM_0 + i * c_item_id_offset, hive.zoom_values[i]);
			}

			locked = FALSE;

			return TRUE;
		}

		//
		// コンフィグの値を更新します。
		//
		BOOL update_config()
		{
			if (locked) return FALSE;

			for (size_t i = 0; i < std::size(hive.zoom_values); i++)
			{
				hive.zoom_values[i] = get_int(IDC_ZOOM_0 + i * c_item_id_offset);
			}

			return TRUE;
		}

		//
		// ダイアログプロシージャです。
		//
		INT_PTR on_dlg_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_INITDIALOG:
				{
					MY_TRACE_FUNC("WM_INITDIALOG, {:#010x}, {:#010x}", wParam, lParam);

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
					}

					for (size_t i = 0; i < std::size(hive.zoom_values); i++)
					{
						if (id == IDC_ZOOM_0 + i * c_item_id_offset)
						{
							if (code == EN_UPDATE) update_config();
						}
						else if (id == IDC_ZOOM_0_APPLY + i * c_item_id_offset)
						{
							auto exedit = magi.exin.get_exedit();
							auto horz_scroll = *(int*)(exedit + 0x001A5304);
							auto set_zoom = (void (*)(int zoom, int horz_scroll))(exedit + 0x000395E0);

							if (hive.zoom_values[i] >= hive.c_zoom.c_min || hive.zoom_values[i] <= hive.c_zoom.c_max)
								set_zoom(hive.zoom_values[i], horz_scroll);
						}
					}

					break;
				}
			case WM_NOTIFY:
				{
					auto header = (NMHDR*)lParam;

					for (size_t i = 0; i < std::size(hive.zoom_values); i++)
					{
						if (header->idFrom == IDC_ZOOM_0_SPIN + i * c_item_id_offset)
						{
							if (header->code == UDN_DELTAPOS)
							{
								auto nm = (NMUPDOWN*)lParam;

								auto value = get_int(IDC_ZOOM_0 + i * c_item_id_offset);
								if (nm->iDelta < 0) value++;
								else value--;
								value = std::max(value, hive.c_zoom.c_min);
								value = std::min(value, hive.c_zoom.c_max);
								set_int(IDC_ZOOM_0 + i * c_item_id_offset, value);
							}
						}
					}

					break;
				}
			}

			return __super::on_dlg_proc(hwnd, message, wParam, lParam);
		}
	} addin_dialog;
}
