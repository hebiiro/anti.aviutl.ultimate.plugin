#pragma once

namespace my::hook
{
	template <typename T>
	LONG attach(T& hooker)
	{
		return DetourAttach((PVOID*)&hooker.orig_proc, hooker.hook_proc);
	}

	template <typename T>
	LONG detach(T& hooker)
	{
		return DetourDetach((PVOID*)&hooker.orig_proc, hooker.hook_proc);
	}

	template <typename T, typename A>
	LONG attach(T& hooker, A address)
	{
		hooker.orig_proc = reinterpret_cast<decltype(hooker.orig_proc)>(address);
		return DetourAttach((PVOID*)&hooker.orig_proc, hooker.hook_proc);
	}

	template <typename T, typename A>
	void attach_call(T& hooker, A address)
	{
		hooker.orig_proc = set_call(address, hooker.hook_proc);
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

	template <typename T>
	void attach_import(T& hooker, HMODULE module, LPCSTR func_name)
	{
		hooker.orig_proc = hookImportFunc(module, func_name, hooker.hook_proc);
	}

	template <typename T>
	void replace(T& hooker, decltype(hooker.orig_proc)& address)
	{
		hooker.orig_proc = address;
		address = hooker.hook_proc;
	}
}
