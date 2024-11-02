#pragma once

namespace my::hook
{
	template <typename T>
	LONG attach(T& hooker)
	{
		return DetourAttach(&(PVOID&)hooker.orig_proc, hooker.hook_proc);
	}

	template <typename T>
	LONG detach(T& hooker)
	{
		return DetourDetach(&(PVOID&)hooker.orig_proc, hooker.hook_proc);
	}

	template <typename T, typename A>
	LONG attach(T& hooker, A address)
	{
		hooker.orig_proc = reinterpret_cast<decltype(hooker.orig_proc)>(address);
		return DetourAttach(&(PVOID&)hooker.orig_proc, hooker.hook_proc);
	}

	template <typename T, typename A>
	void attach_call(T& hooker, A address)
	{
		hooker.orig_proc = tools::set_call(address, hooker.hook_proc);
	}

	template <typename T, typename A>
	void attach_abs_call(T& hooker, A address)
	{
		hooker.orig_proc = tools::set_abs_call(address, hooker.hook_proc);
	}

	template <typename T, typename A>
	void attach_abs_addr(T& hooker, A address)
	{
		hooker.orig_proc = tools::set_abs_addr(address, hooker.hook_proc);
	}

	template <typename F>
	F hook_import(HMODULE module, LPCSTR func_name, F new_func)
	{
		struct Hooker
		{
			LPCSTR func_name;
			F new_func;
			F old_func;

			inline static BOOL CALLBACK detour(
				_In_opt_ PVOID pContext,
				_In_ DWORD nOrdinal,
				_In_opt_ LPCSTR pszFunc,
				_In_opt_ PVOID* ppvFunc)
			{
				if (!pszFunc) return TRUE;
				MY_TRACE_STR(pszFunc);

				auto hooker = (Hooker*)pContext;
				if (::lstrcmpA(pszFunc, hooker->func_name) != 0) return TRUE;

				auto protect = DWORD { PAGE_READWRITE };
				auto result1 = ::VirtualProtect(ppvFunc, sizeof(*ppvFunc), protect, &protect);
				hooker->old_func = tools::set_abs_addr((addr_t)ppvFunc, hooker->new_func);
				auto result2 = ::VirtualProtect(ppvFunc, sizeof(*ppvFunc), protect, &protect);

				return FALSE;
			}

		} hooker = { func_name, new_func };

		DetourEnumerateImportsEx(module, &hooker, nullptr, hooker.detour);

		return hooker.old_func;
	}

	template <typename T>
	void attach_import(T& hooker, HMODULE module, LPCSTR func_name)
	{
		hooker.orig_proc = hook_import(module, func_name, hooker.hook_proc);
	}

	template <typename T>
	void replace(T& hooker, decltype(hooker.orig_proc)& address)
	{
		hooker.orig_proc = address, address = hooker.hook_proc;
	}
}
