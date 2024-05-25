#pragma once

namespace apn::dark::hook
{
	//
	// このクラスはモジュール関係のフックを担当します。
	//
	inline struct Program
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			auto kernel32 = ::GetModuleHandle(_T("kernel32.dll"));
			MY_TRACE_HEX(kernel32);

			my::hook::attach(LoadLibraryA, ::GetProcAddress(kernel32, "LoadLibraryA"));
			my::hook::attach(EnumProcessModules, ::GetProcAddress(kernel32, "K32EnumProcessModules"));

			if (DetourTransactionCommit() != NO_ERROR)
			{
				MY_TRACE("APIフックに失敗しました\n");

				return FALSE;
			}

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		inline static struct Auo {
			//
			// 指定されたメモリ範囲から指定された配列の中身を検索して返します。
			//
			template <typename T, size_t N>
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
			template <typename T, size_t N>
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
			template <typename T, size_t N>
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
			template <typename T, size_t N>
			inline static Replacer<T, N> genReplacer(const T(&dst)[N], const T(&src)[N]) {
				return Replacer<T, N>(dst, src);
			}

			void hack(LPVOID base, LPVOID offset)
			{
				MY_TRACE_FUNC("{:#010x}, {:#010x}", base, offset);

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
							MY_TRACE("検索対象の文字列が見つかりました\n");

							// メモリが検索対象の文字列をすべて持つかチェックします。
							LPVOID p2 = memstr(mbi.BaseAddress, mbi.RegionSize, r2.dst);
							LPVOID p3 = memstr(mbi.BaseAddress, mbi.RegionSize, r3.dst);
							if (p2 && p3)
							{
								MY_TRACE("検索対象の文字列がすべて見つかりました\n");

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
		// このクラスは::LoadLibraryA()をフックします。
		//
		struct {
			inline static HMODULE WINAPI hook_proc(LPCSTR file_name)
			{
				MY_TRACE_FUNC("{}", file_name);

				if (!::StrStrIA(file_name, ".auo"))
					return orig_proc(file_name);

				HMODULE module = orig_proc(file_name);

				MY_TRACE("::LoadLibraryA({}) => {:#010x}\n", file_name, module);

				auo.hack((LPVOID)module, (LPVOID)module);

				return module;
			}
			inline static decltype(&hook_proc) orig_proc = ::LoadLibraryA;
		} LoadLibraryA;

		//
		// このクラスは::EnumProcessModules()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(
				_In_ HANDLE hProcess,
				_Out_writes_bytes_(cb) HMODULE* lphModule,
				_In_ DWORD cb,
				_Out_ LPDWORD lpcbNeeded)
			{
				MY_TRACE_FUNC("{:#010x}", hProcess);

				// リターンアドレスを取得します。
				LPVOID offset = (LPVOID)*((DWORD*)&hProcess - 1);

				// リターンアドレスを持つモジュールのベースアドレスを取得します。
				MEMORY_BASIC_INFORMATION mbi = {};
				::VirtualQuery(offset, &mbi, sizeof(mbi));
				LPVOID base = mbi.AllocationBase;

				auo.hack(base, offset);

				return orig_proc(hProcess, lphModule, cb, lpcbNeeded);
			}
			inline static decltype(&hook_proc) orig_proc = ::EnumProcessModules;
		} EnumProcessModules;
	} program;
}
