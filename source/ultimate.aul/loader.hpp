#pragma once

namespace apn
{
	//
	// このクラスはターゲットをロードします。
	//
	inline struct Loader
	{
		//
		// ターゲットをロードします。
		//
		BOOL load_target()
		{
			MY_TRACE_FUNC("");

			// 日本語が正しく表示されるように文字列のロケールを設定します。
			_tsetlocale(LC_CTYPE, _T(""));

			{
				MY_TRACE("YUY2モードかどうかチェックします\n");

				auto file_name = my::get_module_file_name(nullptr);
				file_name.replace_filename(_T("aviutl.ini"));
				MY_TRACE_STR(file_name);

				if (::GetPrivateProfileInt(_T("system"), _T("yuy2mode"), FALSE, file_name.c_str()))
				{
					MY_TRACE("YUY2モードでは使用できません\n");

					return TRUE;
				}
			}

			auto file_name = my::get_module_file_name(hive.instance);
			file_name.replace_extension(_T(".auf"));
			hive.target = ::LoadLibraryW(file_name.c_str());
			MY_TRACE_HEX(hive.target);

			BOOL (WINAPI* init)(HINSTANCE instance) = nullptr;
			my::tools::get_proc(hive.target, "core_init", init);
			if (init) init(hive.target);

			return TRUE;
		}

		//
		// ターゲットをアンロードします。
		//
		BOOL unload_target()
		{
			MY_TRACE_FUNC("");

			BOOL (WINAPI* exit)() = nullptr;
			my::tools::get_proc(hive.target, "core_exit", exit);
			if (exit) exit();

			return TRUE;
		}

		//
		// ::FreeLibrary()のフックから呼ばれます。
		//
		inline BOOL on_free_library(HINSTANCE instance)
		{
			MY_TRACE_FUNC("{:#010x}", instance);

			if (!instance) return FALSE;

			// このモジュールが開放される場合は
			if (instance == hive.instance)
			{
				// すでにターゲットがロードされている場合は
				// 何もせずにデフォルト処理をスキップします。
				if (hive.target) return TRUE;

				// ターゲットをロードします。
				// さらにデフォルト処理をスキップして
				// このモジュールが開放されるのを防ぎます。
				return load_target();
			}

			// ターゲットが開放される場合は
			if (instance == hive.target)
			{
				// ターゲットをアンロードします。
				// さらにデフォルト処理をスキップして
				// ターゲットが開放されるのを防ぎます。
				return unload_target();
			}

			return FALSE;
		}
	} loader;
}
