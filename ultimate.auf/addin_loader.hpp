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
				if (!addin->active) continue;

				// アドインのファイル名を取得します。
				auto file_name = magi.get_addin_file_name(addin->name);
				MY_TRACE_STR(file_name);

				// アドインを読み込みます。
				if (!load_addin(file_name, addin->args))
					0;//MY_TRACE("{}を読み込めませんでした\n", file_name);
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
		// 指定されたアドインを読み込みます。
		//
		BOOL load_addin(const std::wstring& file_name, const std::wstring& args)
		{
			MY_TRACE_FUNC("{}, {}", file_name, args);

			HINSTANCE instance = ::LoadLibraryExW(file_name.c_str(), 0, LOAD_WITH_ALTERED_SEARCH_PATH);
			if (!instance) return FALSE;
			auto get_addin = (Addin* (WINAPI*)(LPCWSTR args))::GetProcAddress(instance, "core_get_addin");
			if (!get_addin) return FALSE;
			return addin_manager.add_addin(instance, get_addin(args.c_str()));
		}
	} addin_loader;
}
