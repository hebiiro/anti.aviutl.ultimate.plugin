#pragma once

namespace apn::color_code
{
	//
	// このクラスはフックを管理します。
	//
	inline struct HookManager
	{
		//
		// 初期化処理です。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// 拡張編集のモジュールハンドルを取得します。
			auto exedit = magi.exin.get_exedit();

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			my::tools::assign(hive.unlock, exedit + 0x134E64);
			my::tools::assign(hive.r, exedit + 0x11F2D0);
			my::tools::assign(hive.g, exedit + 0x11F0AC);
			my::tools::assign(hive.b, exedit + 0x11F064);

			my::hook::attach_import(SetDIBitsToDevice, (HMODULE)exedit, "SetDIBitsToDevice");
			my::hook::attach_call(color_dialog.show_dialog, exedit + 0x4D386);
			my::hook::attach(color_dialog.set_color, exedit + 0x22420);
			my::hook::attach(color_dialog.update_color_circle, exedit + 0x21300);
			my::hook::attach(color_dialog.update_controls, exedit + 0x216A0);

			my::hook::attach_abs_call(dropper.GetPixel, exedit + 0x22128);

			if (DetourTransactionCommit() == NO_ERROR)
			{
				MY_TRACE("拡張編集のフックに成功しました\n");

				return TRUE;
			}
			else
			{
				MY_TRACE("拡張編集のフックに失敗しました\n");

				return FALSE;
			}
		}

		//
		// 後始末処理です。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// このクラスは拡張編集内の::SetDIBitsToDevice()の呼び出しをフックします。
		//
		struct {
			inline static int WINAPI hook_proc(HDC hdc,
				int xDest, int yDest, DWORD w, DWORD h,
				int xSrc, int ySrc, UINT StartScan, UINT cLines,
				CONST VOID * lpvBits, CONST BITMAPINFO * lpbmi, UINT ColorUse)
			{
				MY_TRACE_FUNC("{}, {}, 変更前", xDest, yDest);

				// 描画位置をオフセットの分だけずらします。

				xDest += hive.offset.x;
				yDest += hive.offset.y;

				MY_TRACE_FUNC("{}, {}, 変更後", xDest, yDest);

				return orig_proc(hdc, xDest, yDest, w, h, xSrc, ySrc, StartScan, cLines, lpvBits, lpbmi, ColorUse);
			}
			inline static decltype(&hook_proc) orig_proc = ::SetDIBitsToDevice;
		} SetDIBitsToDevice;

		struct {
			inline static void update_color_code(int32_t r, int32_t g, int32_t b)
			{
				// フラグをセットしてからコントロールの値を更新します。

				*hive.unlock = FALSE;

				::SetDlgItemText(hive.dialog, IDC_COLOR_CODE,
					std::format(_T("{:02x}{:02x}{:02x}"), r, g, b).c_str());

				*hive.unlock = TRUE;
			}

			inline static void set_color_value(int32_t r, int32_t g, int32_t b)
			{
				*hive.r = r;
				*hive.g = g;
				*hive.b = b;
			}

			//
			// このクラスは拡張編集内のshow_dialog()をフックします。
			//
			struct {
				//
				// このクラスは拡張編集内の「色の選択」ダイアログをフックします。
				//
				inline static struct {
					inline static INT_PTR CALLBACK hook_proc(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
					{
						switch (message)
						{
						case WM_INITDIALOG:
							{
								MY_TRACE_FUNC("WM_INITDIALOG, {:#010x}, {:#010x}", wParam, lParam);

								hive.dialog = hdlg;
								MY_TRACE_HEX(hive.dialog);

								break;
							}
						case WM_COMMAND:
							{
								auto code = HIWORD(wParam);
								auto id = LOWORD(wParam);
								auto sender = (HWND)lParam;

								MY_TRACE_FUNC("WM_COMMAND, {:#04x}, {:#04x}, {:#010x}", code, id, sender);

								if (code == EN_UPDATE && id == IDC_COLOR_CODE && *hive.unlock)
								{
									// エディットボックスからテキストを取得します。
									auto text = my::get_dlg_item_text(hdlg, IDC_COLOR_CODE);
									MY_TRACE_STR(text);

									// テキスト内の数値開始位置へのオフセットを取得します。
									auto offset = 0;
									if (text[0] == _T('#')) offset = 1;

									// テキストを数値部分だけにします。
									text = text.substr(offset);

									// テキストを数値に変換します。
									auto color = wcstoul(text.c_str(), nullptr, 16);
									MY_TRACE_HEX(color);

									int r, g, b;

									if (text.length() > 3)
									{
										// rrggbbの形式からRGBを取得します。
										r = (color >> 16) & 0xff;
										g = (color >>  8) & 0xff;
										b = (color >>  0) & 0xff;
									}
									else
									{
										// rgbの形式からRGBを取得します。
										r = (color >> 8) & 0x0f;
										g = (color >> 4) & 0x0f;
										b = (color >> 0) & 0x0f;

										r |= r << 4;
										g |= g << 4;
										b |= b << 4;
									}

									MY_TRACE("{}, {}, {}\n", r, g, b);

									set_color_value(r, g, b);
									set_color.orig_proc(r, g, b);
									update_color_circle.orig_proc(hdlg);
									update_controls.orig_proc(hdlg, r, g, b);
								}

								break;
							}
						case WM_LBUTTONDOWN:
						case WM_LBUTTONUP:
						case WM_MOUSEMOVE:
							{
								// マウス位置をオフセットの分だけずらします。

								auto point = my::lp_to_pt(lParam);
								point.x -= hive.offset.x;
								point.y -= hive.offset.y;
								lParam = my::pt_to_lp(point);

								break;
							}
						}

						return orig_proc(hdlg, message, wParam, lParam);
					}
					inline static decltype(&hook_proc) orig_proc = nullptr;
				} hooker;

				inline static INT_PTR CDECL hook_proc(HINSTANCE instance, LPCSTR template_name, HWND parent, DLGPROC dlg_proc)
				{
					MY_TRACE_FUNC("{:#010x}, {}, {:#010x}, {:#010x}", instance, template_name, parent, dlg_proc);

					if (::lstrcmpiA(template_name, "GET_COLOR") == 0)
					{
						MY_TRACE("「色の選択」ダイアログをフックします\n");

						hooker.orig_proc = dlg_proc;

						return orig_proc(hive.instance, template_name, parent, hooker.hook_proc);
					}

					return orig_proc(instance, template_name, parent, dlg_proc);
				}
				inline static decltype(&hook_proc) orig_proc = nullptr;
			} show_dialog;

			//
			// このクラスは拡張編集内のset_color()をフックします。
			//
			inline static struct {
				inline static void CDECL hook_proc(int r, int g, int b)
				{
					MY_TRACE_FUNC("{}, {}, {}", r, g, b);

					orig_proc(r, g, b);

					update_color_code(r, g, b);
				}
				inline static decltype(&hook_proc) orig_proc = nullptr;
			} set_color;

			//
			// このクラスは拡張編集内のupdate_color_circle()をフックします。
			//
			inline static struct {
				inline static void CDECL hook_proc(HWND hdlg)
				{
					MY_TRACE_FUNC("{:#010x}", hdlg);

					orig_proc(hdlg);
				}
				inline static decltype(&hook_proc) orig_proc = nullptr;
			} update_color_circle;

			//
			// このクラスは拡張編集内のupdate_controls()をフックします。
			//
			inline static struct {
				inline static void CDECL hook_proc(HWND hdlg, int r, int g, int b)
				{
					MY_TRACE_FUNC("{:#010x}, {}, {}, {}", hdlg, r, g, b);

					orig_proc(hdlg, r, g, b);

					update_color_code(r, g, b);
				}
				inline static decltype(&hook_proc) orig_proc = nullptr;
			} update_controls;
		} color_dialog;

		struct {
			//
			// このクラスは拡張編集内の::GetPixel()の呼び出しをフックします。
			//
			struct {
				inline static COLORREF WINAPI hook_proc(HDC _dc, int x, int y)
				{
					MY_TRACE_FUNC("{:#010x}, {}, {}", _dc, x, y);

					// すべてのモニタのすべての場所から色を抽出できるようにします。

					auto hwnd = (HWND)nullptr;
					auto point = my::get_cursor_pos();
					::LogicalToPhysicalPointForPerMonitorDPI(hwnd, &point);

					my::ClientDC dc(hwnd);

					return ::GetPixel(dc, point.x, point.y);
				}
				inline static decltype(&hook_proc) orig_proc = nullptr;
			} GetPixel;
		} dropper;
	} hook_manager;
}
