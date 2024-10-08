#pragma once

namespace apn
{
	//
	// このクラスはAPIをフックします。
	//
	inline struct HookManager
	{
		//
		// このクラスは::FreeLibrary()をフックします。
		//
		inline static struct {
			inline static BOOL WINAPI hook_proc(HMODULE module)
			{
//				MY_TRACE_FUNC("{:#010x}", module);

				if (loader.on_free_library(module))
					return TRUE;

				return orig_proc(module);
			}
			inline static decltype(&hook_proc) orig_proc = ::FreeLibrary;
		} FreeLibrary;

		//
		// 初期化を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			my::hook::attach(FreeLibrary);

			return DetourTransactionCommit() == NO_ERROR;
		}

		//
		// 後始末を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			my::hook::detach(FreeLibrary);

			return DetourTransactionCommit() == NO_ERROR;
		}
	} hook_manager;
}
