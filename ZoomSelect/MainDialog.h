#pragma once
#include "Resource.h"
#include "Hive.h"

namespace fgo::zoom_select
{
	//
	// このクラスはメインダイアログです。
	//
	struct MainDialog : Tools::Dialog
	{
		static constexpr auto ItemIDOffset = IDC_ZOOM_1 - IDC_ZOOM_0;

		BOOL lock = FALSE;

		//
		// ウィンドウを作成します。
		//
		BOOL create(HWND parent)
		{
			if (!__super::create(hive.instance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), parent))
				return FALSE;

			return TRUE;
		}

		//
		// コントロールの値を更新します。
		//
		BOOL updateControls()
		{
			if (lock) return FALSE;

			lock = TRUE;

			for (size_t i = 0; i < std::size(hive.zoom); i++)
			{
				setInt(IDC_ZOOM_0 + i * ItemIDOffset, hive.zoom[i]);
			}

			lock = FALSE;

			return TRUE;
		}

		//
		// コンフィグの値を更新します。
		//
		BOOL updateConfig()
		{
			if (lock) return FALSE;

			for (size_t i = 0; i < std::size(hive.zoom); i++)
			{
				hive.zoom[i] = getInt(IDC_ZOOM_0 + i * ItemIDOffset);
			}

			return TRUE;
		}

		//
		// ダイアログプロシージャです。
		//
		INT_PTR onDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_COMMAND:
				{
					MY_TRACE_FUNC("WM_COMMAND, 0x%08X, 0x%08X", wParam, lParam);

					UINT code = HIWORD(wParam);
					UINT id = LOWORD(wParam);
					HWND control = (HWND)lParam;

					switch (id)
					{
					case IDOK:
					case IDCANCEL: return 0;
					}

					for (size_t i = 0; i < std::size(hive.zoom); i++)
					{
						if (id == IDC_ZOOM_0 + i * ItemIDOffset)
						{
							if (code == EN_UPDATE) updateConfig();
						}
						else if (id == IDC_ZOOM_0_APPLY + i * ItemIDOffset)
						{
							auto exedit = magi.auin.GetExEdit();
							auto hscroll = *(int*)(exedit + 0x001A5304);
							auto setZoom = (void (*)(int zoom, int hscroll))(exedit + 0x000395E0);

							if (hive.zoom[i] >= Hive::Zoom::Min || hive.zoom[i] <= Hive::Zoom::Max)
								setZoom(hive.zoom[i], hscroll);
						}
					}

					break;
				}
			case WM_NOTIFY:
				{
					auto header = (NMHDR*)lParam;

					for (size_t i = 0; i < std::size(hive.zoom); i++)
					{
						if (header->idFrom == IDC_ZOOM_0_SPIN + i * ItemIDOffset)
						{
							if (header->code == UDN_DELTAPOS)
							{
								auto nm = (NMUPDOWN*)lParam;

								int value = getInt(IDC_ZOOM_0 + i * ItemIDOffset);
								if (nm->iDelta < 0) value++;
								else value--;
								value = std::max(value, Hive::Zoom::Min);
								value = std::min(value, Hive::Zoom::Max);
								setInt(IDC_ZOOM_0 + i * ItemIDOffset, value);
							}
						}
					}

					break;
				}
			}

			return __super::onDlgProc(hwnd, message, wParam, lParam);
		}
	};
}
