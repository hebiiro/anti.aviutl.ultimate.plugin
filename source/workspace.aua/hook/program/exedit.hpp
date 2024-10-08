#pragma once

namespace apn::workspace::hook
{
	//
	// このクラスは拡張編集をフックします。
	//
	inline struct ExEdit
	{
		inline static struct {
			//
			// このクラスは拡張編集のscript_param_dlg_proc()をフックします。
			//
			inline static struct {
				inline static INT_PTR CALLBACK hook_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
				{
					switch (message)
					{
					case WM_INITDIALOG:
						{
							MY_TRACE_FUNC("WM_INITDIALOG, {:#010x}, {:#010x}", wParam, lParam);

							// rikky_memory用の処理です。
							// 子ウィンドウにはWM_NCACTIVATEが通知されないので手動で通知します。
							::PostMessage(hive.setting_dialog, WM_NCACTIVATE, FALSE, (LPARAM)hwnd);

							// デフォルト処理を実行し、コントロールを作成させます。
							auto result = orig_proc(hwnd, message, wParam, lParam);

							// PSDToolKit用の処理です。
							// ダイアログを表示中でもPSDToolKitウィンドウを自由に使えるように
							// 一時的にドッキングを解除します。
							if (psdtoolkit) psdtoolkit->undock.init(psdtoolkit.get(), hwnd);

							return result;
						}
					case WM_DESTROY:
						{
							MY_TRACE_FUNC("WM_DESTROY, {:#010x}, {:#010x}", wParam, lParam);

							if (psdtoolkit) psdtoolkit->undock.exit();

							break;
						}
					}

					return orig_proc(hwnd, message, wParam, lParam);
				}
				inline static decltype(&hook_proc) orig_proc = 0;
			} script_param_dlg_proc;

			//
			// rikky_memoryをフックします。
			//
			void init(my::addr_t exedit)
			{
				my::hook::attach_abs_addr(script_param_dlg_proc, exedit + 0x3454 + 1);
			}
		} rikky_memory;

		inline static struct {
			//
			// このクラスはスポイトの::GetPixel()をフックします。
			//
			inline static struct {
				inline static COLORREF WINAPI hook_proc(HDC _dc, int x, int y)
				{
					MY_TRACE_FUNC("{:#010x}, {}, {}", _dc, x, y);

					// すべてのモニタのすべての場所から色を抽出できるようにします。

					auto point = my::get_cursor_pos();
					::LogicalToPhysicalPointForPerMonitorDPI(0, &point);

					my::ClientDC dc(0);

					return ::GetPixel(dc, point.x, point.y);
				}
				inline static decltype(&hook_proc) orig_proc = 0;
			} GetPixel;

			//
			// スポイト処理をフックします。
			//
			inline static void init(my::addr_t exedit)
			{
#if 1
				my::hook::attach_import(GetPixel, (HMODULE)exedit, "GetPixel");
#else
				my::hook::attach_abs_call(GetPixel, exedit + 0x22128);
				my::hook::attach_abs_call(GetPixel, exedit + 0x2AB49);
#endif
			}
		} dropper;

		inline static struct {
			//
			// このクラスはキーボードフックの::GetActiveWindow()をフックします。
			//
			inline static struct {
				inline static HWND WINAPI hook_proc()
				{
					MY_TRACE_FUNC("");

					auto active_window = ::GetActiveWindow();
					MY_TRACE_HWND(active_window);

					auto focus = ::GetFocus();
					MY_TRACE_HWND(focus);

					// フォーカスが存在しない場合は
					if (!focus)
					{
						MY_TRACE("設定ダイアログを返します\n");

						return hive.setting_dialog;
					}

					// 設定ダイアログがフォーカスの祖先なら
					if (my::is_ancestor(focus, hive.setting_dialog))
					{
						MY_TRACE("設定ダイアログを返します\n");

						return hive.setting_dialog;
					}

					// 拡張編集ウィンドウがフォーカスの祖先なら
					if (my::is_ancestor(focus, hive.exedit_window))
					{
						MY_TRACE("拡張編集ウィンドウを返します\n");

						return hive.exedit_window;
					}

					return active_window;
				}
				inline static decltype(&hook_proc) orig_proc = 0;
			} GetActiveWindow;

			//
			// キーボードフック処理をフックします。
			//
			void init(my::addr_t exedit)
			{
				BYTE code[6];
				code[0] = (BYTE)0x90; // NOP
				code[1] = (BYTE)0xBD; // MOV EBP,DWORD
				*(DWORD*)&code[2] = (DWORD)GetActiveWindow.hook_proc;

				my::tools::set_abs_addr_block(exedit + 0x30D0E, code);
			}
		} keyboard_hook;

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			auto exedit = magi.exin.get_exedit();
			MY_TRACE_HEX(exedit);

			rikky_memory.init(exedit);
			dropper.init(exedit);
			keyboard_hook.init(exedit);

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}
	} exedit;
}
