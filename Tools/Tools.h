﻿#pragma once
#pragma pack(push)
#pragma pack(1)
namespace Tools {
	using addr_t = DWORD;


	// 指定されたアドレスを指定されたポインタにキャストして格納します。
	template<class T>
	inline void get_pointer(T& x, addr_t address)
	{
		x = (T)address;
	}

	//
	// 指定されたモジュールの指定された名前の関数をprocに格納します。
	//
	template<class T>
	inline void get_proc(HMODULE module, LPCSTR name, T& proc)
	{
		proc = (T)::GetProcAddress(module, name);
	}

	// 相対 CALL を書き換える。
	template<class T>
	inline T set_call(addr_t address, T hookProc)
	{
		HANDLE process = ::GetCurrentProcess();

		struct {
			BYTE call;
			addr_t func;
		} orig = {
			0xE8, // CALL
		};

		// 元の関数を取得する。
		::ReadProcessMemory(process, (LPVOID)address, &orig, sizeof(orig), NULL);

		if (orig.call != 0xE8)
			return 0; // 相対 CALL ではなかった。

		struct {
			BYTE call;
			addr_t func;
		} code = {
			0xE8, // CALL
			(addr_t)hookProc - (address + sizeof(code)),
		};

		// CALL を書き換える。そのあと命令キャッシュをフラッシュする。
		::WriteProcessMemory(process, (LPVOID)address, &code, sizeof(code), NULL);
		::FlushInstructionCache(process, (LPVOID)address, sizeof(code));

		// 元の関数を返す。
		return (T)(orig.func + (address + sizeof(orig)));
	}

	// 絶対 CALL を書き換える。
	template<class T>
	inline T set_abs_call(addr_t address, T hookProc)
	{
		HANDLE process = ::GetCurrentProcess();

		struct {
			BYTE call[2];
			T* func;
		} orig = {};

		// 元の関数を取得する。
		::ReadProcessMemory(process, (LPVOID)address, &orig, sizeof(orig), NULL);

		if (orig.call[0] != 0xFF || orig.call[1] != 0x15)
			return 0; // 絶対 CALL ではなかった。

		struct {
			BYTE call;
			addr_t func;
			BYTE next;
		} code = {
			0xE8, // CALL
			(addr_t)hookProc - (address + offsetof(code, next)),
			0x90, // NOP
		};

		// CALL を書き換える。そのあと命令キャッシュをフラッシュする。
		::WriteProcessMemory(process, (LPVOID)address, &code, sizeof(code), NULL);
		::FlushInstructionCache(process, (LPVOID)address, sizeof(code));

		// 元の関数を返す。
		return *orig.func;
	}

	// 絶対アドレスを書き換える。
	template<class T>
	inline T set_abs_addr(addr_t address, T x)
	{
		HANDLE process = ::GetCurrentProcess();

		// 絶対アドレスから読み込む。
		T retValue = 0;
		::ReadProcessMemory(process, (LPVOID)address, &retValue, sizeof(retValue), NULL);
		// 絶対アドレスを書き換える。
		::WriteProcessMemory(process, (LPVOID)address, &x, sizeof(x), NULL);
		// 命令キャッシュをフラッシュする。
		::FlushInstructionCache(process, (LPVOID)address, sizeof(x));
		return retValue;
	}

	// 絶対アドレスを書き換える。
	template<class T>
	inline void set_abs_addr_block(addr_t address, const T& x)
	{
		HANDLE process = ::GetCurrentProcess();

		// 絶対アドレスから読み込む。
		T retValue = {};
		::ReadProcessMemory(process, (LPVOID)address, &retValue, sizeof(retValue), NULL);
		// 絶対アドレスを書き換える。
		::WriteProcessMemory(process, (LPVOID)address, &x, sizeof(x), NULL);
		// 命令キャッシュをフラッシュする。
		::FlushInstructionCache(process, (LPVOID)address, sizeof(x));
	}

	// 指定アドレスの値に x を加算する。
	inline void add_int32(addr_t address, int value)
	{
		// プロセスハンドルを取得する。
		HANDLE process = ::GetCurrentProcess();

		// アドレスの値を取得する。
		int x = 0;
		::ReadProcessMemory(process, (LPVOID)address, &x, sizeof(x), NULL);

		// value を加算する。
		x += value;

		// アドレスの値を書き換える。
		::WriteProcessMemory(process, (LPVOID)address, &x, sizeof(x), NULL);

		// 命令キャッシュをフラッシュする。
		::FlushInstructionCache(process, (LPVOID)address, sizeof(x));
	}
}
#pragma pack(pop)
