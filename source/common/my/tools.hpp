#pragma once

#pragma pack(push)
#pragma pack(1)
namespace my::tools
{
	//
	// 指定されたアドレスを指定されたポインタにキャストして格納します。
	//
	template <typename T>
	inline void assign(T& p, addr_t address)
	{
		p = (T)address;
	}

	//
	// 指定されたモジュールの指定された名前の関数をprocに格納します。
	//
	template <typename T>
	inline void get_proc(HMODULE module, LPCSTR name, T& proc)
	{
		proc = (T)::GetProcAddress(module, name);
	}

	// 相対CALLを書き換えます
	template <typename T>
	inline T set_call(addr_t address, T hook_proc)
	{
		auto process = ::GetCurrentProcess();

		struct {
			BYTE call;
			addr_t func;
		} orig = {
			0xE8, // CALL
		};

		// 元の関数を取得します。
		::ReadProcessMemory(process, (LPVOID)address, &orig, sizeof(orig), nullptr);

		if (orig.call != 0xE8)
			return {}; // 相対CALLではなかったので{}を返します。

		struct {
			BYTE call;
			addr_t func;
		} code = {
			0xE8, // CALL
			(addr_t)hook_proc - (address + sizeof(code)),
		};

		// 相対CALLを書き換えます。
		::WriteProcessMemory(process, (LPVOID)address, &code, sizeof(code), nullptr);
		::FlushInstructionCache(process, (LPVOID)address, sizeof(code));

		// 元の関数を返します。
		return (T)(orig.func + (address + sizeof(orig)));
	}

	// 絶対CALLを書き換えます。
	template <typename T>
	inline T set_abs_call(addr_t address, T hook_proc)
	{
		auto process = ::GetCurrentProcess();

		struct {
			BYTE call[2];
			T* func;
		} orig = {};

		// 元の関数を取得します。
		::ReadProcessMemory(process, (LPVOID)address, &orig, sizeof(orig), nullptr);

		if (orig.call[0] != 0xFF || orig.call[1] != 0x15)
			return {}; // 絶対CALLではなかったので{}を返します。

		struct {
			BYTE call;
			addr_t func;
			BYTE next;
		} code = {
			0xE8, // CALL
			(addr_t)hook_proc - (address + offsetof(code, next)),
			0x90, // NOP
		};

		// 絶対CALLを書き換えます。
		::WriteProcessMemory(process, (LPVOID)address, &code, sizeof(code), nullptr);
		::FlushInstructionCache(process, (LPVOID)address, sizeof(code));

		// 元の関数を返します。
		return *orig.func;
	}

	// 絶対アドレスを書き換えます。
	template <typename T>
	inline T set_abs_addr(addr_t address, T x)
	{
		auto process = ::GetCurrentProcess();

		// 絶対アドレスから読み込みます。
		T orig = {};
		::ReadProcessMemory(process, (LPVOID)address, &orig, sizeof(orig), nullptr);

		// 絶対アドレスを書き換えます。
		::WriteProcessMemory(process, (LPVOID)address, &x, sizeof(x), nullptr);
		::FlushInstructionCache(process, (LPVOID)address, sizeof(x));

		// 元の値を返します。
		return orig;
	}

	// 絶対アドレスを書き換えます。
	template <typename T>
	inline void set_abs_addr_block(addr_t address, const T& x)
	{
		auto process = ::GetCurrentProcess();

		// 絶対アドレスから読み込みます。
		T orig = {};
		::ReadProcessMemory(process, (LPVOID)address, &orig, sizeof(orig), nullptr);

		// 絶対アドレスを書き換えます。
		::WriteProcessMemory(process, (LPVOID)address, &x, sizeof(x), nullptr);
		::FlushInstructionCache(process, (LPVOID)address, sizeof(x));
	}

	// 指定アドレスの値に指定された値を加算します。
	template <typename T>
	inline void add_int32(addr_t address, T value)
	{
		// プロセスハンドルを取得します。
		auto process = ::GetCurrentProcess();

		// アドレスの値を取得します。
		T x = {};
		::ReadProcessMemory(process, (LPVOID)address, &x, sizeof(x), nullptr);

		// 指定された値を加算します。
		x += value;

		// アドレスの値を書き換えます。
		::WriteProcessMemory(process, (LPVOID)address, &x, sizeof(x), nullptr);
		::FlushInstructionCache(process, (LPVOID)address, sizeof(x));
	}
}
#pragma pack(pop)
