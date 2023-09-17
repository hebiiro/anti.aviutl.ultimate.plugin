#pragma once

namespace fgo
{
	struct Rayshift
	{
		HMODULE aul = 0;
		HMODULE auf = 0;

		BOOL start(HMODULE module)
		{
//			MY_TRACE(_T("Rayshift::start(0x%08X)\n"), module);

			if (module == aul)
			{
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
			else if (auf && module == auf)
			{
				MY_TRACE(_T("DllMain()の外でUltimate.aufをアンロードします\n"));

				BOOL (WINAPI* exit)() = 0;
				Tools::get_proc(auf, "see_you", exit);
				if (exit) exit();

				return TRUE;
			}

			return FALSE;
		}

		//
		// このクラスは::FreeLibrary()をフックします。
		//
		inline static struct
		{
			inline static BOOL WINAPI hook(HMODULE module)
			{
//				MY_TRACE(_T("::FreeLibrary(0x%08X)\n"), module);

				if (rayshift.start(module))
					return TRUE; // レイシフトに成功した場合はデフォルト処理を行いません。

				return orig(module);
			}
			inline static decltype(&hook) orig = ::FreeLibrary;
		} FreeLibrary;

		BOOL init(HINSTANCE instance)
		{
			MY_TRACE(_T("Rayshift::init(0x%08X)\n"), instance);

			aul = instance;
			MY_TRACE_HEX(aul);

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			Tools::attach(FreeLibrary);

			return DetourTransactionCommit() == NO_ERROR;
		}

		BOOL exit()
		{
			MY_TRACE(_T("Rayshift::exit()\n"));

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			Tools::detach(FreeLibrary);

			return DetourTransactionCommit() == NO_ERROR;
		}
	} rayshift;
}
