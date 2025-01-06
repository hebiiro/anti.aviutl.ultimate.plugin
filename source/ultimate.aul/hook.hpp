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

			my::hook::attach_import(FreeLibrary, nullptr, "FreeLibrary");

			return TRUE;
		}

		//
		// 後始末を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}
	} hook_manager;
}
