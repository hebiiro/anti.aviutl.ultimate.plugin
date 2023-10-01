#pragma once

namespace fgo::dark::hook
{
	//
	// このクラスは出力プラグインのリテラル文字列を書き換えます。
	//
	inline struct
	{
		//
		// 指定されたメモリ範囲から指定された配列の中身を検索して返します。
		//
		template <class T, size_t N>
		inline static LPVOID memstr(LPVOID p, size_t c, const T(&str)[N]) {
			size_t length = sizeof(str);
			for (size_t i = 0; i < c - length; i++) {
				auto current = (LPVOID)((ULONG_PTR)p + i);
				if (memcmp(current, str, length) == 0)
					return current;
			}
			return 0;
		}

		//
		// 指定されたメモリに指定された配列の中身を書き込みます。
		//
		template <class T, size_t N>
		inline static BOOL write(HANDLE process, LPVOID p, const T(&str)[N]) {
			size_t length = sizeof(str);
			DWORD protect = PAGE_READWRITE;
			::VirtualProtect(p, length, protect, &protect);
			BOOL result = ::WriteProcessMemory(process, p, str, length, 0);
			::VirtualProtect(p, length, protect, &protect);
			return result;
		}

		//
		// このクラスは書き込み先と書き込み元の配列を保持します。
		//
		template <class T, size_t N>
		struct Replacer {
			T dst[N];
			T src[N];
			Replacer(const T(&dst)[N], const T(&src)[N]) {
				memcpy(this->dst, dst, sizeof(this->dst));
				memcpy(this->src, src, sizeof(this->src));
			}
		};

		//
		// Replacerを生成します。
		//
		template <class T, size_t N>
		inline static Replacer<T, N> genReplacer(const T(&dst)[N], const T(&src)[N]) {
			return Replacer<T, N>(dst, src);
		}

		void hack(LPVOID base, LPVOID offset)
		{
			MY_TRACE_FUNC("0x%08X, 0x%08X", base, offset);

			// モジュールのメモリを検索します。
			MEMORY_BASIC_INFORMATION mbi = {};
			while (::VirtualQuery(offset, &mbi, sizeof(mbi)))
			{
				// 検索結果が別のモジュールの場合はループを終了します。
				if (mbi.AllocationBase != base)
					break;

				// メモリがリードオンリーかチェックします。
				if (mbi.Protect & PAGE_READONLY)
				{
					MY_TRACE_HEX(mbi.BaseAddress);

					// Replacerを生成します。
					auto r1 = genReplacer("DarkenWindow\0\0", "UltimateConfig");
					auto r2 = genReplacer(L"DarkenWindow.aul", L"Dark.aua\0\0\0\0\0\0\0\0");
					auto r3 = genReplacer(L"DarkenWindowSettings.xml", L"Dark.xml\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");

					LPVOID p2 = memstr(mbi.BaseAddress, mbi.RegionSize, r2.dst);
					LPVOID p3 = memstr(mbi.BaseAddress, mbi.RegionSize, r3.dst);

					// メモリが検索対象の文字列を持つかチェックします。
					if (LPVOID p1 = memstr(mbi.BaseAddress, mbi.RegionSize, r1.dst))
					{
						MY_TRACE(_T("検索対象の文字列が見つかりました\n"));

						// メモリが検索対象の文字列をすべて持つかチェックします。
						LPVOID p2 = memstr(mbi.BaseAddress, mbi.RegionSize, r2.dst);
						LPVOID p3 = memstr(mbi.BaseAddress, mbi.RegionSize, r3.dst);
						if (p2 && p3)
						{
							MY_TRACE(_T("検索対象の文字列がすべて見つかりました\n"));

							// カレントプロセスを取得します。
							HANDLE process = ::GetCurrentProcess();
							MY_TRACE_HEX(process);

							// メモリに書き込みます。
							write(process, p1, r1.src);
							write(process, p2, r2.src);
							write(process, p3, r3.src);

							// ループを終了します。
							break;
						}
					}
				}

				// 次のメモリに移動します。
				offset = (LPVOID)((DWORD_PTR)mbi.BaseAddress + mbi.RegionSize);
			}
		}
	} auo;

	//
	// このクラスは::EnumProcessModules()をフックします。
	//
	inline struct
	{
		inline static BOOL WINAPI hook(
			_In_ HANDLE hProcess,
			_Out_writes_bytes_(cb) HMODULE* lphModule,
			_In_ DWORD cb,
			_Out_ LPDWORD lpcbNeeded)
		{
			MY_TRACE_FUNC("0x%08X", hProcess);

			// リターンアドレスを取得します。
			LPVOID offset = (LPVOID)*((DWORD*)&hProcess - 1);

			// リターンアドレスを持つモジュールのベースアドレスを取得します。
			MEMORY_BASIC_INFORMATION mbi = {};
			::VirtualQuery(offset, &mbi, sizeof(mbi));
			LPVOID base = mbi.AllocationBase;

			auo.hack(base, offset);

			return orig(hProcess, lphModule, cb, lpcbNeeded);
		}
		inline static decltype(&hook) orig = ::EnumProcessModules;
	} EnumProcessModules;
}
