#pragma once
#include "Resource.h"

namespace fgo::color_code
{
	//
	// このクラスはエディットボックスを微調整します。
	//
	inline struct ColorCode : Servant
	{
		HINSTANCE instance = 0;
		POINT offset = { 0, 0 };

		//
		// この仮想関数は、このサーヴァントの識別名が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_name() override
		{
			return L"ColorCode";
		}

		//
		// この仮想関数は、このサーヴァントの表示名が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_display_name() override
		{
			return L"カラーコード";
		}

		//
		// この仮想関数は、DLLの初期化のタイミングで呼ばれます。
		//
		BOOL on_dll_init(HINSTANCE instance) override
		{
			MY_TRACE_FUNC("0x%08X", instance);

			this->instance = instance;

			return TRUE;
		}

		//
		// この仮想関数は、DLLの後始末のタイミングで呼ばれます。
		//
		BOOL on_dll_exit(HINSTANCE instance) override
		{
			MY_TRACE_FUNC("0x%08X", instance);

			return TRUE;
		}

		//
		// この仮想関数は、初期化のタイミングで呼ばれます。
		//
		BOOL on_init() override
		{
			if (::GetModuleHandleW(L"AddColorCode.auf"))
			{
				::MessageBoxW(magi.fp->hwnd,
					L"ColorCodeとAddColorCode.aufが競合しています"
					L"\nどちらかを除外してからAviUtlを再起動してください",
					get_servant_display_name(), MB_OK | MB_ICONWARNING);
			}

			if (!load()) return FALSE;
			if (!init()) return FALSE;
			return TRUE;
		}

		//
		// この仮想関数は、後始末のタイミングで呼ばれます。
		//
		BOOL on_exit() override
		{
			if (!save()) return FALSE;
			if (!exit()) return FALSE;
			return TRUE;
		}

		//
		// コンフィグファイルのフルパスを返します。
		//
		inline static std::wstring getConfigFileName()
		{
			return magi.getConfigFileName(L"ColorCode.ini");
		}

		//
		// コンフィグファイル名を取得し、設定を読み込みます。
		//
		BOOL load()
		{
			return load(getConfigFileName().c_str());
		}

		//
		// コンフィグファイルから設定を読み込みます。
		//
		BOOL load(LPCWSTR path)
		{
			getPrivateProfileInt(path, L"Config", L"offset.x", offset.x);
			getPrivateProfileInt(path, L"Config", L"offset.y", offset.y);

			return TRUE;
		}

		//
		// コンフィグファイル名を取得し、設定を保存します。
		//
		BOOL save()
		{
			return save(getConfigFileName().c_str());
		}

		//
		// コンフィグファイルに設定を保存します。
		//
		BOOL save(LPCWSTR path)
		{
			setPrivateProfileInt(path, L"Config", L"offset.x", offset.x);
			setPrivateProfileInt(path, L"Config", L"offset.y", offset.y);

			return TRUE;
		}

		//
		// 初期化処理です。
		// 拡張編集の関数をフックします。
		// 拡張編集のコンスト値 (コードメモリ) を書き換えます。
		//
		BOOL init()
		{
			MY_TRACE(_T("initHook()\n"));

			// 拡張編集のモジュールハンドルを取得します。
			auto exedit = magi.auin.GetExEdit();
			if (!exedit) return FALSE;

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			Tools::get_pointer(colorDialog.canHandleCommand, exedit + 0x134E64);
			Tools::get_pointer(colorDialog.r, exedit + 0x11F2D0);
			Tools::get_pointer(colorDialog.g, exedit + 0x11F0AC);
			Tools::get_pointer(colorDialog.b, exedit + 0x11F064);

			Tools::attach_import(SetDIBitsToDevice, (HMODULE)exedit, "SetDIBitsToDevice");
			Tools::attach_call(ShowColorDialog, exedit + 0x4D386);

			Tools::attach(colorDialog.SetColor, exedit + 0x22420);
			Tools::attach(colorDialog.UpdateColorCircle, exedit + 0x21300);
			Tools::attach(colorDialog.UpdateControls, exedit + 0x216A0);

			Tools::attach_abs_call(dropper.GetPixel, exedit + 0x22128);

			if (DetourTransactionCommit() == NO_ERROR)
			{
				MY_TRACE(_T("拡張編集のフックに成功しました\n"));
			}
			else
			{
				MY_TRACE(_T("拡張編集のフックに失敗しました\n"));
			}

			return TRUE;
		}

		//
		// 後始末処理です。
		//
		BOOL exit()
		{
			return TRUE;
		}

		//
		// このクラスは拡張編集内の::SetDIBitsToDevice()の呼び出しをフックします。
		//
		inline static struct {
			inline static int WINAPI hook(HDC hdc, int xDest, int yDest, DWORD w, DWORD h, int xSrc, int ySrc,
				UINT StartScan, UINT cLines, CONST VOID * lpvBits, CONST BITMAPINFO * lpbmi, UINT ColorUse)
			{
				MY_TRACE_FUNC("%d, %d, 変更前", xDest, yDest);

				// 描画位置をオフセットの分だけずらします。

				xDest += servant.offset.x;
				yDest += servant.offset.y;

				MY_TRACE_FUNC("%d, %d, 変更後", xDest, yDest);

				return orig(hdc, xDest, yDest, w, h, xSrc, ySrc, StartScan, cLines, lpvBits, lpbmi, ColorUse);
			}
			inline static decltype(&hook) orig = ::SetDIBitsToDevice;
		} SetDIBitsToDevice;

		//
		// このクラスは拡張編集内のShowColorDialog()をフックします。
		//
		inline static struct {
			inline static INT_PTR CDECL hook(HINSTANCE instance, LPCSTR templateName, HWND parent, DLGPROC dialogProc)
			{
				MY_TRACE_FUNC("0x%08X, %hs, 0x%08X, 0x%08X", instance, templateName, parent, dialogProc);

				if (_stricmp(templateName, "GET_COLOR") == 0)
				{
					MY_TRACE(_T("「色の選択」ダイアログをフックします\n"));

					servant.colorDialog.DialogProc.orig = dialogProc;

					return orig(servant.instance, templateName, parent, servant.colorDialog.DialogProc.hook);
				}

				return orig(instance, templateName, parent, dialogProc);
			}
			inline static decltype(&hook) orig = 0;
		} ShowColorDialog;

		inline static struct ColorDialog {
			inline static HWND dialog = 0;
			inline static BOOL* canHandleCommand = 0;
			inline static int* r = 0;
			inline static int* g = 0;
			inline static int* b = 0;

			inline static void UpdateColorCode(int r, int g, int b)
			{
				// フラグをセットしてからコントロールの値を更新します。

				*canHandleCommand = FALSE;

				TCHAR text[MAX_PATH] = {};
				::StringCchPrintf(text, std::size(text), _T("%02x%02x%02x"), r, g, b);
				::SetDlgItemText(dialog, IDC_COLOR_CODE, text);

				*canHandleCommand = TRUE;
			}

			inline static void SetColorValue(int _r, int _g, int _b)
			{
				*r = _r;
				*g = _g;
				*b = _b;
			}

			//
			// このクラスは拡張編集内のColorDialog::DialogProc()をフックします。
			//
			inline static struct {
				inline static INT_PTR CALLBACK hook(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
				{
					switch (message)
					{
					case WM_INITDIALOG:
						{
							MY_TRACE_FUNC("WM_INITDIALOG, 0x%08X, 0x%08X", wParam, lParam);

							dialog = hdlg;
							MY_TRACE_HEX(dialog);

							break;
						}
					case WM_COMMAND:
						{
							UINT code = HIWORD(wParam);
							UINT id = LOWORD(wParam);
							HWND sender = (HWND)lParam;

							MY_TRACE_FUNC("WM_COMMAND, 0x%04X, 0x%04X, 0x%08X", code, id, sender);

							if (code == EN_UPDATE && id == IDC_COLOR_CODE && *canHandleCommand)
							{
								// エディットボックスからテキストを取得します。
								TCHAR text[MAX_PATH] = {};
								::GetDlgItemText(hdlg, IDC_COLOR_CODE, text, std::size(text));
								MY_TRACE_TSTR(text);

								// テキスト内の数値開始位置へのオフセットを取得します。
								int offset = 0;
								if (text[0] == _T('#')) offset = 1;

								// テキストを数値に変換します。
								DWORD color = _tcstoul(text + offset, 0, 16);
								MY_TRACE_HEX(color);

								int r, g, b;

								if (::lstrlen(text + offset) > 3)
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

								MY_TRACE(_T("%d, %d, %d\n"), r, g, b);

								SetColorValue(r, g, b);
								SetColor.orig(r, g, b);
								UpdateColorCircle.orig(hdlg);
								UpdateControls.orig(hdlg, r, g, b);
							}

							break;
						}
					case WM_LBUTTONDOWN:
					case WM_LBUTTONUP:
					case WM_MOUSEMOVE:
						{
							// マウス位置をオフセットの分だけずらします。

							POINT point = LP2PT(lParam);
							point.x -= servant.offset.x;
							point.y -= servant.offset.y;
							lParam = PT2LP(point);

							break;
						}
					}

					return orig(hdlg, message, wParam, lParam);
				}
				inline static decltype(&hook) orig = 0;
			} DialogProc;

			//
			// このクラスは拡張編集内のColorDialog::SetColor()をフックします。
			//
			inline static struct {
				inline static void CDECL hook(int r, int g, int b)
				{
					MY_TRACE_FUNC("%d, %d, %d", r, g, b);

					orig(r, g, b);

					UpdateColorCode(r, g, b);
				}
				inline static decltype(&hook) orig = 0;
			} SetColor;

			//
			// このクラスは拡張編集内のColorDialog::UpdateColorCircle()をフックします。
			//
			inline static struct {
				inline static void CDECL hook(HWND hdlg)
				{
					MY_TRACE_FUNC("0x%08X", hdlg);

					orig(hdlg);
				}
				inline static decltype(&hook) orig = 0;
			} UpdateColorCircle;

			//
			// このクラスは拡張編集内のColorDialog::UpdateControls()をフックします。
			//
			inline static struct {
				inline static void CDECL hook(HWND hdlg, int r, int g, int b)
				{
					MY_TRACE_FUNC("0x%08X, %d, %d, %d", hdlg, r, g, b);

					orig(hdlg, r, g, b);

					UpdateColorCode(r, g, b);
				}
				inline static decltype(&hook) orig = 0;
			} UpdateControls;
		} colorDialog;

		inline static struct Dropper
		{
			//
			// このクラスは拡張編集内の::GetPixel()の呼び出しをフックします。
			//
			inline static struct {
				inline static COLORREF WINAPI hook(HDC _dc, int x, int y)
				{
					MY_TRACE_FUNC("0x%08X, %d, %d", _dc, x, y);

					// すべてのモニタのすべての場所から色を抽出できるようにします。

					HWND hwnd = 0;
					POINT point; ::GetCursorPos(&point);
					::LogicalToPhysicalPointForPerMonitorDPI(hwnd, &point);
					HDC dc = ::GetDC(hwnd);
					COLORREF color = ::GetPixel(dc, point.x, point.y);
					::ReleaseDC(hwnd, dc);
					return color;
				}
				inline static decltype(&hook) orig = 0;
			} GetPixel;
		} dropper;
	} servant;
}
