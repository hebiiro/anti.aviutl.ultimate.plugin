#pragma once

namespace apn
{
	inline struct HookManager
	{
		//
		// このクラスは::LoadLibraryA()をフックします。
		//
		inline static struct
		{
			//
			// 指定されたプラグインと競合しているアドイン情報を返します。
			//
			inline static const Hive::Addin* conflicts(const std::wstring& file_name)
			{
				for (const auto& addin : hive.addins)
				{
					if (!addin->active) continue;

					for (const auto& conflict : addin->conflicts)
					{
						if (::StrStrIW(file_name.c_str(), conflict.c_str()))
							return addin.get();
					}
				}

				return nullptr;
			}

			inline static HMODULE WINAPI hook_proc(LPCSTR _file_name)
			{
				auto file_name = my::ws(_file_name);

				if (auto addin = conflicts(file_name))
				{
					std::wcout << std::format(L"\033[31m" L"『{}』アドインと競合しているので『{}』プラグインは使用できません" L"\033[m",
						addin->display_name, std::filesystem::path(file_name).filename().wstring()) << std::endl;

					return nullptr;
				}

				auto result = orig_proc(_file_name);

				MY_TRACE("::LoadLibraryA({}) => {:#010x}\n", file_name, result);

				if (::StrStrIW(file_name.c_str(), L"exedit.auf"))
				{
					MY_TRACE("拡張編集が読み込まれたのでアドレスを取得します\n");

					// 拡張編集関連のアドレス情報を初期化します。
					magi.exin.init();
				}

				return result;
			}
			inline static decltype(&hook_proc) orig_proc = ::LoadLibraryA;
		} LoadLibraryA;

		BOOL init()
		{
			MY_TRACE_FUNC("");

			// AviUtl関連のアドレス情報を初期化します。
			magi.auin.init();

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			my::hook::attach(LoadLibraryA);

			return DetourTransactionCommit() == NO_ERROR;
		}

		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}
	} hook_manager;
}
