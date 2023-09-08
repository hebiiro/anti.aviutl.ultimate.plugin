#pragma once
#include "PSDToolKit.h"

namespace fgo::nest::hook
{
	//
	// このクラスは拡張編集をフックします。
	//
	struct ExEdit
	{
		struct rikky_memory
		{
			//
			// このクラスは::ScriptParamDlgProc()をフックします。
			//
			inline static struct
			{
				inline static INT_PTR CALLBACK hook(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
				{
					switch (message)
					{
					case WM_INITDIALOG:
						{
							MY_TRACE(_T("ExEdit::ScriptParamDlgProc(WM_INITDIALOG)\n"));

							// rikky_memory用の処理です。
							// 子ウィンドウにはWM_NCACTIVATEが通知されないので手動で通知します。
							::PostMessage(hive.settingDialog, WM_NCACTIVATE, FALSE, (LPARAM)hwnd);

							// デフォルト処理を実行し、コントロールを作成させます。
							INT_PTR result = orig(hwnd, message, wParam, lParam);

							// PSDToolKit用の処理です。
							// ダイアログを表示中でもPSDToolKitウィンドウを自由に使えるように
							// 一時的にドッキングを解除します。
							if (psdtoolkit) psdtoolkit->undock.init(psdtoolkit.get(), hwnd);

							return result;
						}
					case WM_DESTROY:
						{
							MY_TRACE(_T("ExEdit::ScriptParamDlgProc(WM_DESTROY)\n"));

							if (psdtoolkit) psdtoolkit->undock.exit();

							break;
						}
					}

					return orig(hwnd, message, wParam, lParam);
				}
				inline static decltype(&hook) orig = 0;
			} ScriptParamDlgProc;

			//
			// rikky_memoryをフックします。
			//
			static void init(DWORD exedit)
			{
				Tools::attach_abs_addr(ScriptParamDlgProc, exedit + 0x3454 + 1);
			}
		};

		struct Dropper
		{
			//
			// このクラスはスポイトの::GetPixel()をフックします。
			//
			inline static struct
			{
				inline static COLORREF WINAPI hook(HDC _dc, int x, int y)
				{
					MY_TRACE(_T("ExEdit::Dropper::GetPixel(0x%08X, %d, %d)\n"), _dc, x, y);

					// すべてのモニタのすべての場所から色を抽出できるようにします。

					POINT point; ::GetCursorPos(&point);
					::LogicalToPhysicalPointForPerMonitorDPI(0, &point);
					HDC dc = ::GetDC(0);
					COLORREF color = ::GetPixel(dc, point.x, point.y);
					::ReleaseDC(0, dc);
					return color;
				}
				inline static decltype(&hook) orig = 0;
			} GetPixel;

			//
			// スポイト処理をフックします。
			//
			static void init(DWORD exedit)
			{
				Tools::attach_abs_call(GetPixel, exedit + 0x22128);
			}
		};

		struct KeyboardHook
		{
			//
			// このクラスはキーボードフックの::GetActiveWindow()をフックします。
			//
			inline static struct
			{
				static HWND WINAPI hook()
				{
					MY_TRACE(_T("ExEdit::KeyboardHook::GetActiveWindow()\n"));

					HWND activeWindow = ::GetActiveWindow();
					MY_TRACE_HWND(activeWindow);

					HWND focus = ::GetFocus();
					MY_TRACE_HWND(focus);

					// フォーカスが0なら
					if (!focus)
					{
						MY_TRACE(_T("設定ダイアログを返します\n"));
						return hive.settingDialog;
					}

					// 設定ダイアログがフォーカスの祖先なら
					if (isAncestor(focus, hive.settingDialog))
					{
						MY_TRACE(_T("設定ダイアログを返します\n"));
						return hive.settingDialog;
					}

					// 拡張編集ウィンドウがフォーカスの祖先なら
					if (isAncestor(focus, hive.exeditWindow))
					{
						MY_TRACE(_T("拡張編集ウィンドウを返します\n"));
						return hive.exeditWindow;
					}

					return activeWindow;
				}
				inline static decltype(&hook) orig = 0;
			} GetActiveWindow;

			//
			// キーボードフック処理をフックします。
			//
			static void init(DWORD exedit)
			{
				BYTE code[6];
				code[0] = (BYTE)0x90; // NOP
				code[1] = (BYTE)0xBD; // MOV EBP,DWORD
				*(DWORD*)&code[2] = (DWORD)GetActiveWindow.hook;

				writeCode(exedit + 0x30D0E, code, sizeof(code));
			}
		};

		//
		// 拡張編集用フックの初期化処理です。
		//
		inline static BOOL init()
		{
			MY_TRACE(_T("ExEdit::init()\n"));

			auto exedit = magi.auin.GetExEdit();

			rikky_memory::init(exedit);
			Dropper::init(exedit);
			KeyboardHook::init(exedit);

			return TRUE;
		}

		//
		// 拡張編集用フックの後始末処理です。
		//
		inline static BOOL exit()
		{
			MY_TRACE(_T("ExEdit::exit()\n"));

			return TRUE;
		}
	};
}
