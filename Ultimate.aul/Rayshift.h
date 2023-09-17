#pragma once

namespace fgo
{
	struct Rayshift
	{
		HMODULE aul = 0;
		HMODULE auf = 0;

		BOOL examine(HMODULE module)
		{
			MY_TRACE_FUNC("0x%08X", module);

			if (module == aul)
				return start(module);
			else if (auf && module == auf)
				return end(module);

			return FALSE;
		}

		BOOL start(HMODULE module)
		{
			MY_TRACE_FUNC("0x%08X", module);

			// 日本語が正しく表示されるように文字列のロケールを設定します。
			_tsetlocale(LC_CTYPE, _T(""));

			MY_TRACE(_T("DllMain()の外でUltimate.aufをロードします\n"));

			WCHAR moduleFileName[MAX_PATH] = {};
			::GetModuleFileNameW(aul, moduleFileName, std::size(moduleFileName));
			::PathRenameExtensionW(moduleFileName, L".auf");
			auf = ::LoadLibraryW(moduleFileName);
			MY_TRACE_HEX(auf);

			BOOL (WINAPI* init)(HINSTANCE instance) = 0;
			Tools::get_proc(auf, "hello_world", init);
			if (init) init(auf);

			return TRUE;
		}

		BOOL end(HMODULE module)
		{
			MY_TRACE_FUNC("0x%08X", module);

			MY_TRACE(_T("DllMain()の外でUltimate.aufをアンロードします\n"));

			BOOL (WINAPI* exit)() = 0;
			Tools::get_proc(auf, "see_you", exit);
			if (exit) exit();

			return TRUE;
		}

		//
		// このクラスは::FreeLibrary()をフックします。
		//
		inline static struct {
			inline static BOOL WINAPI hook(HMODULE module)
			{
//				MY_TRACE_FUNC("0x%08X", module);

				if (rayshift.examine(module))
					return TRUE; // レイシフトに成功した場合はデフォルト処理を行いません。

				return orig(module);
			}
			inline static decltype(&hook) orig = ::FreeLibrary;
		} FreeLibrary;

		BOOL init(HINSTANCE instance)
		{
			MY_TRACE_FUNC("0x%08X", instance);

			aul = instance;
			MY_TRACE_HEX(aul);

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			Tools::attach(FreeLibrary);

			return DetourTransactionCommit() == NO_ERROR;
		}

		BOOL exit()
		{
			MY_TRACE_FUNC("");

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			Tools::detach(FreeLibrary);

			return DetourTransactionCommit() == NO_ERROR;
		}
	} rayshift;
}
