#pragma once

namespace apn
{
	//
	// このクラスはアドインをロードします。
	//
	inline struct AddinLoader
	{
		//
		// 初期化を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// 登録されているアドイン情報を読み込みます。
			for (auto& addin : hive.addins)
			{
				// 非アクティブなアドインは読み込みません。
				if (!addin->active)
				{
					std::wcout << std::format(L"{}は非アクティブなので読み込みませんでした", addin->name) << std::endl;

					continue;
				}

				// 競合していないかチェックします。
				if (conflicts(addin)) continue;

				// アドインのファイル名を取得します。
				auto file_name = magi.get_addin_file_name(addin->name);
				MY_TRACE_STR(file_name);

				// アドインを読み込みます。
				load_addin(file_name, addin->args);
			}

			return TRUE;
		}

		//
		// 後始末を実行します。
		//
		BOOL exit()
		{
			return TRUE;
		}

		//
		// 指定されたアドインが他のモジュールと競合している場合はTRUEを返します。
		//
		BOOL conflicts(const auto& addin)
		{
			BOOL result = FALSE;

			for (const auto& conflict : addin->conflicts)
			{
				if (::GetModuleHandleW(conflict.c_str()))
				{
					std::wcout << std::format(L"\033[31m{}と競合しているので{}は使用できません\033[m", conflict, addin->name) << std::endl;

					result = TRUE;
				}
			}

			return result;
		}

		//
		// 指定されたアドインを読み込みます。
		//
		BOOL load_addin(const std::wstring& file_name, const std::wstring& args)
		{
			MY_TRACE_FUNC("{}, {}", file_name, args);

			auto instance = ::LoadLibraryExW(file_name.c_str(), 0, LOAD_WITH_ALTERED_SEARCH_PATH);
			if (!instance)
			{
				std::wcout << std::format(L"\033[31m{}の読み込みに失敗しました\033[m", file_name) << std::endl;

				return FALSE;
			}

			auto get_addin = (Addin* (WINAPI*)(LPCWSTR args))::GetProcAddress(instance, "core_get_addin");
			if (!get_addin)
			{
				std::wcout << std::format(L"\033[31m{}のエクスポート関数を取得できませんでした\033[m", file_name) << std::endl;

				return FALSE;
			}

			if (!addin_manager.add_addin(instance, get_addin(args.c_str())))
			{
				std::wcout << std::format(L"\033[31m{}を追加できませんでした\033[m", file_name) << std::endl;

				return FALSE;
			}

			return TRUE;
		}
	} addin_loader;
}
