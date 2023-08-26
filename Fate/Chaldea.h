#pragma once

namespace fgo
{
	//
	// このクラスはこのプラグインのモジュールです。
	//
	inline struct Chaldea
	{
		//
		// 指定された名前のエクスポート関数を返します。
		//
		static FARPROC WINAPI get_proc(LPCSTR procName)
		{
			HMODULE module = ::GetModuleHandleW(L"Ultimate.auf");
			return ::GetProcAddress(module, procName);
		}
	} chaldea;
}
