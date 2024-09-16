#pragma once

namespace apn::dark::hook
{
	//
	// このクラスはdllのフックを担当します。
	//
	inline struct Dll
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			auto kernel32 = ::GetModuleHandle(_T("kernel32.dll"));
			MY_TRACE_HEX(kernel32);

			my::hook::attach(EnumProcessModules, ::GetProcAddress(kernel32, "K32EnumProcessModules"));

			if (DetourTransactionCommit() != NO_ERROR)
			{
				MY_TRACE("APIフックに失敗しました\n");

				return FALSE;
			}

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

		//
		// このクラスは::EnumProcessModules()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(
				_In_ HANDLE process,
				_Out_writes_bytes_(cb) HMODULE* modules,
				_In_ DWORD cb,
				_Out_ LPDWORD cb_needed)
			{
				MY_TRACE_FUNC("{:#010x}", process);

				// まず、デフォルト処理を行います。
				auto result = orig_proc(process, modules, cb, cb_needed);
				if (!result) return FALSE;

				// ファイル選択ダイアログ以降を除外する場合は
				if (hive.file_dialog_mode == hive.c_file_dialog_mode.c_omit)
				{
					auto c = *cb_needed / sizeof(HMODULE);
					for (decltype(c) i = 0; i < c; i++)
					{
						if (modules[i] == hive.darken_window)
						{
							// 黒窓を無効にします。
							modules[i] = hive.instance;

							break;
						}
					}
				}
				// ファイル選択ダイアログ以降にも適用する場合は
				else
				{
					// 黒窓の設定ファイルを構築します。
					app->deploy_darken_window_settings();
				}

				return result;
			}
			inline static decltype(&hook_proc) orig_proc = ::EnumProcessModules;
		} EnumProcessModules;
	} dll;
}
