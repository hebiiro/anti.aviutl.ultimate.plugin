#pragma once

namespace apn
{
	//
	// リターンアドレスを返します。
	//
	inline constexpr auto ret_addr(auto* arg1)
	{
		return reinterpret_cast<my::addr_t*>(arg1)[-1];
	}

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
				// リターンアドレスを取得します。
				auto from = ret_addr(&module);
//				MY_TRACE_HEX(from);

//				MY_TRACE_FUNC("{:#010x}", module);
//				MY_TRACE_FUNC("{:#010x}, {}", module, my::get_module_file_name(module));

				auto mbi = MEMORY_BASIC_INFORMATION {};
				auto result = ::VirtualQuery((LPCVOID)from, &mbi, sizeof(mbi));
				auto aviutl_module = ::GetModuleHandle(nullptr);
				auto from_module = (HMODULE)mbi.AllocationBase;
//				MY_TRACE("aviutl_module = {:#010x}, {}\n", aviutl_module, my::get_module_file_name(aviutl_module));
//				MY_TRACE("from_module = {:#010x}, {}\n", from_module, my::get_module_file_name(from_module));

				// aviutl以外から呼ばれた場合は何もしないようにします。
				if (from_module == aviutl_module)
				{
					if (loader.on_free_library(module))
						return TRUE;
				}

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
