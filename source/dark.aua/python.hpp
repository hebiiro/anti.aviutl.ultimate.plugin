#pragma once

namespace apn::dark
{
	//
	// このクラスはPhthonで描画します。
	//
	inline struct Python
	{
		std::unique_ptr<py::scoped_interpreter> interpreter;
		py::module sys_module;
		py::module dark_module;
		py::module hive_module;
		py::module boot_module;
		py::module scheme_module;
		py::module skin_module;

		//
		// python文字列(UTF-8)をワイド文字列に変換して返します。
		//
		inline static std::wstring ws(const std::string& s)
		{
			return my::cp_to_wide(s, CP_UTF8);
		}

		//
		// ワイド文字列をpython文字列(UTF-8)に変換して返します。
		//
		inline static std::string u8(const std::wstring& s)
		{
			return my::wide_to_cp(s, CP_UTF8);
		}

		//
		// python例外メッセージをワイド文字列に変換して返します。
		//
		inline static std::wstring convert_what(const std::exception& error)
		{
			return my::cp_to_wide(error.what(), CP_UTF8);
		}

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			try
			{
				interpreter = std::make_unique<py::scoped_interpreter>();

				// システムモジュールを読み込みます。
				sys_module = py::module::import("sys");

				// 読み込むパスを設定します。
				auto path = sys_module.attr("path").cast<py::list>();
				auto folder_name = my::wide_to_cp(hive.assets_folder_name, CP_UTF8);
				path.append(folder_name);

				// キャッシュを作成しないようにします。
				update_dont_write_bytecode();

				// dark.pydを読み込みます。
				dark_module = py::module::import("dark");
				if (!dark_module)
					throw std::exception("dark.pydの読み込みに失敗しました");

				// hive_moduleを作成します。
				hive_module = dark_module.def_submodule("hive");
				if (!hive_module)
					throw std::exception("hive_moduleの作成に失敗しました");

				// dark.pydの初期化関数を実行します。
				if (!dark_module.attr("init")((void*)hive.addin).cast<BOOL>())
					 throw std::exception("dark.init()が失敗しました");

				// boot.pyを読み込みます。
				boot_module = py::module::import("boot");
				if (!boot_module)
					throw std::exception("boot.pyの読み込みに失敗しました");
			}
			catch (const std::exception& error)
			{
				auto what = convert_what(error);
				MY_TRACE_STR(what);

				std::wcout << what << std::endl;

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

			unload_modules();

			try
			{
				skin_module.release();
				boot_module.release();
				hive_module.release();
				dark_module.release();
				sys_module.release();

				interpreter.reset();
			}
			catch (const std::exception& error)
			{
				auto what = convert_what(error);
				MY_TRACE_STR(what);

				std::wcout << what << std::endl;

				return FALSE;
			}

			return TRUE;
		}

		//
		// pythonのdont_write_bytecodeフラグを更新します。
		//
		BOOL update_dont_write_bytecode()
		{
			if (!sys_module) return FALSE;

			// この値がFALSEの場合、__pycache__フォルダが生成されなくなります。
			sys_module.attr("dont_write_bytecode") = !!hive.dont_write_bytecode;

			return TRUE;
		}

		//
		// python側で定義されている関数を削除します。
		//
		BOOL del_python_func(LPCSTR func_name)
		{
			try
			{
				if (!skin_module) return FALSE;
				return py::delattr(skin_module, func_name), TRUE;
			}
			catch (const std::exception& error)
			{
				if (1)
				{
					(void)error;
				}
				else
				{
					auto what = convert_what(error);

					std::wcout << what << std::endl;
				}

				return FALSE;
			}
		}

		inline static auto ws(const py::object& o)
		{
			return my::cp_to_wide(o.cast<std::string>(), CP_UTF8);
		}

		//
		// 読み込まれているモジュールをアンロードします。
		//
		BOOL unload_modules()
		{
			MY_TRACE_FUNC("");

			del_python_func("dark_init");
			del_python_func("dark_update");
			del_python_func("dark_draw_figure");
			del_python_func("dark_draw_text");
			del_python_func("dark_text_out");

			try
			{
				auto __file__ = ws(dark_module.attr("__file__"));
				auto dark_dir_name = std::filesystem::path(__file__).parent_path().wstring();
				MY_TRACE_STR(dark_dir_name);

				std::vector<std::string> unload_modules;

				auto modules = py::cast<py::dict>(sys_module.attr("modules"));
				for (auto pair : modules)
				{
					try
					{
						auto name = pair.first.cast<std::string>();
						auto module = pair.second.cast<py::module>();

						MY_TRACE_STR(name);
						if (name == "__main__") continue;

						auto __file__ = ws(module.attr("__file__"));
						MY_TRACE_STR(__file__);

						auto dir_name = std::filesystem::path(__file__).parent_path().wstring();
						MY_TRACE_STR(dir_name);
						if (!dir_name.starts_with(dark_dir_name)) continue;

						auto base_name = std::filesystem::path(__file__).filename();
						MY_TRACE_STR(base_name);

						if (base_name == L"dark.pyd" || base_name == L"boot.py") continue;

						unload_modules.emplace_back(name);
					}
					catch (const std::exception& error)
					{
						auto what = convert_what(error);
						MY_TRACE_STR(what);
					}
				}

				for (auto& name : unload_modules)
				{
					MY_TRACE("{}をアンロードします\n", name);

					modules.attr("pop")(name);
				}

				skin_module.release();
				scheme_module.release();
			}
			catch (const std::exception& error)
			{
				auto what = convert_what(error);
				MY_TRACE_STR(what);

				std::wcout << what << std::endl;

				return FALSE;
			}

			return TRUE;
		}

		//
		// ハイブモジュールを更新します。
		//
		BOOL update_hive_module()
		{
			MY_TRACE_FUNC("");

			hive_module.attr("skin_name") = &hive.skin_name;
			hive_module.attr("scheme_name") = &hive.scheme_name;
			hive_module.attr("dark_mode") = &hive.dark_mode;
			hive_module.attr("ellipse") = &hive.ellipse;
			hive_module.attr("border_width") = &hive.border_width;

			return TRUE;
		}

		//
		// 各モジュールをインポートします。
		//
		BOOL import_modules(const std::wstring& skin_module_name, const std::wstring& scheme_module_name)
		{
			MY_TRACE_FUNC("{}, {}", skin_module_name, scheme_module_name);

			// キャッシュを作成しないようにします。
			update_dont_write_bytecode();

			unload_modules();
			update_hive_module();

			if (!import_sheme_module(my::hs(scheme_module_name))) return FALSE;
			if (!import_skin_module(my::hs(skin_module_name))) return FALSE;

			return TRUE;
		}

		//
		// スキンモジュールをインポートします。
		//
		BOOL import_skin_module(const std::string& skin_module_name)
		{
			MY_TRACE_FUNC("{}", skin_module_name);

			try
			{
				// Python側に初期化機会を与えるために、
				// ブートモジュールでモジュールをインポートするようにしています。

				// スキンモジュールを読み込みます。
				skin_module = boot_module.attr("boot_skin_module")(skin_module_name).cast<py::module>();
				if (!skin_module)
					throw std::exception(std::format("{}の読み込みに失敗しました", skin_module_name).c_str());

				dark_module.add_object("skin", skin_module, true);

				return call_init();
			}
			catch (const std::exception& error)
			{
				auto what = convert_what(error);
				MY_TRACE_STR(what);

				std::wcout << what << std::endl;

				return FALSE;
			}
		}

		//
		// スキームモジュールをインポートします。
		//
		BOOL import_sheme_module(const std::string& scheme_module_name)
		{
			MY_TRACE_FUNC("{}", scheme_module_name);

			try
			{
				// Python側に初期化機会を与えるために、
				// ブートモジュールでモジュールをインポートするようにしています。

				// スキームモジュールを読み込みます。
				scheme_module.release();
				scheme_module = boot_module.attr("boot_scheme_module")(scheme_module_name).cast<py::module>();
				if (!scheme_module)
				{
					auto message = std::format(L"{}の読み込みに失敗しました", ws(scheme_module_name));

					throw std::exception(u8(message).c_str());
				}

				dark_module.add_object("scheme", scheme_module, true);

				return TRUE;
			}
			catch (const std::exception& error)
			{
				auto what = convert_what(error);
				MY_TRACE_STR(what);

				std::wcout << what << std::endl;

				return FALSE;
			}
		}

		//
		// スキーム・スキンモジュールの初期化関数を呼び出します。
		//
		BOOL call_init()
		{
			try
			{
				if (!scheme_module || !skin_module) return FALSE;

				constexpr auto func_name = "dark_init";
				auto args = share::InitArgs {
					{ std::begin(hive.dark_color), std::end(hive.dark_color) },
					{ std::begin(hive.light_color), std::end(hive.light_color) },
				};

				if (py::hasattr(scheme_module, func_name))
					scheme_module.attr(func_name)(args);

				if (py::hasattr(skin_module, func_name))
					return skin_module.attr(func_name)(args).cast<BOOL>();
			}
			catch (const std::exception& error)
			{
				auto what = convert_what(error);
				MY_TRACE_STR(what);

				std::wcout << what << std::endl;
			}

			return FALSE;
		}

		//
		// python側で定義されている関数を呼び出します。
		//
		BOOL call_python_func(LPCSTR func_name, const auto& args)
		{
			try
			{
				if (!skin_module) return FALSE;
				if (!py::hasattr(skin_module, func_name)) return FALSE;

				return skin_module.attr(func_name)(args).cast<BOOL>();
			}
			catch (const std::exception& error)
			{
				auto what = convert_what(error);
				MY_TRACE_STR(what);

				std::wcout << what << std::endl;

				return FALSE;
			}
		}

		//
		// スキンモジュールの更新関数を呼び出します。
		//
		BOOL call_update()
		{
			return call_python_func(
				"dark_update",
				share::UpdateArgs {
					{ std::begin(hive.dark_color), std::end(hive.dark_color) },
					{ std::begin(hive.light_color), std::end(hive.light_color) },
				});
		}

		//
		// スキンモジュールの図形描画関数を呼び出します。
		//
		BOOL call_draw_figure(HWND hwnd, HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc)
		{
			ExtTextOutLocker locker; // このスコープ内での::ExtTextOutW()のフックをブロックします。

			return call_python_func(
				"dark_draw_figure",
				share::DrawFigureArgs {
					{ (share::HTHEME)theme, part_id, state_id },
					(share::HWND)hwnd, (share::HDC)dc, rc,
				});
		}

		//
		// スキンモジュールの文字列描画関数を呼び出します。
		//
		BOOL call_draw_text(HWND hwnd, HTHEME theme, HDC dc, int part_id, int state_id,
			LPCWSTR text, int c, DWORD text_flags, LPCRECT rc)
		{
			ExtTextOutLocker locker; // このスコープ内での::ExtTextOutW()のフックをブロックします。

			return call_python_func(
				"dark_draw_text",
				share::DrawTextArgs {
					{ (share::HTHEME)theme, part_id, state_id },
					(share::HWND)hwnd, (share::HDC)dc, rc, text, c, text_flags,
				});
		}

		//
		// スキンモジュールの文字列出力関数を呼び出します。
		//
		BOOL call_text_out(HWND hwnd, HTHEME theme, HDC dc, int part_id, int state_id,
			int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx)
		{
			return call_python_func(
				"dark_text_out",
				share::TextOutArgs {
					{ (share::HTHEME)theme, part_id, state_id },
					(share::HWND)hwnd, (share::HDC)dc, rc, text, c, options, x, y, (my::addr_t)dx,
				});
		}

		//
		// ダークモジュールのコンテキストを返します。
		//
		auto get_context()
		{
			return dark_module.attr("context");
		}
	} thread_local python;
}
