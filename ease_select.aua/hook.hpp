#pragma once

namespace apn::ease_select
{
	//
	// このクラスはフック処理を担当します。
	//
	struct HookManager
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// 拡張編集のアドレスを取得します。
			auto exedit = magi.exin.get_exedit();

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			my::hook::attach_import(DialogBoxParamA, (HMODULE)exedit, "DialogBoxParamA");

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
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// このクラスは拡張編集内の::DialogBoxParamA()の呼び出しをフックします。
		//
		inline static struct {
			inline static INT_PTR WINAPI hook_proc(HINSTANCE instance, LPCSTR template_name, HWND parent, DLGPROC dlg_proc, LPARAM init_param)
			{
				if ((DWORD)template_name > 0x0000FFFF &&
					strcmp(template_name, "GET_PARAM_SMALL") == 0)
				{
					MY_TRACE("ダイアログをサブクラス化します\n");

					struct : my::Window
					{
						//
						// ウィンドウプロシージャです。
						//
						virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
						{
							switch (message)
							{
							case WM_ACTIVATE:
							case WM_DESTROY:
								{
									::SendMessage(hive.main_window, share::message::c_fire_notify, 0, 0);

									break;
								}
							}

							return __super::on_wnd_proc(hwnd, message, wParam, lParam);
						}
					} dialog;

					dialog.associator.start(&dialog);
					auto result = orig_proc(instance, template_name, parent, dlg_proc, init_param);
					dialog.associator.stop();

					return result;
				}

				return orig_proc(instance, template_name, parent, dlg_proc, init_param);
			}
			inline static decltype(&hook_proc) orig_proc = ::DialogBoxParamA;
		} DialogBoxParamA;
	} hook_manager;
}
