#pragma once

namespace apn
{
	//
	// このクラスはモジュールです。
	//
	inline struct Module
	{
		//
		// 指定された名前のエクスポート関数を返します。
		//
		static FARPROC WINAPI get_proc(LPCSTR proc_name)
		{
			auto module = ::GetModuleHandleW(L"ultimate.auf");

			return ::GetProcAddress(module, proc_name);
		}
	} module;
}
