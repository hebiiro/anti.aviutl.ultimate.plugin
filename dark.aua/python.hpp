#pragma once

namespace apn::dark
{
	//
	// このクラスはPhthonで描画します。
	//
	inline struct Python
	{
		std::unique_ptr<py::scoped_interpreter> interpreter;
		py::module dark_module;
		py::module hive_module;
		py::module boot_module;
		py::module scheme_module;
		py::module skin_module;

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			try
			{
				interpreter = std::make_unique<py::scoped_interpreter>();

				// 読み込むパスを設定します。
				auto sys = py::module::import("sys");
				auto path = sys.attr("path").cast<py::list>();
				auto folder_name = my::wide_to_cp(hive.assets_folder_name, CP_UTF8);
				path.append(folder_name);

				// キャッシュを作成しないようにします。
				sys.attr("dont_write_bytecode") = !!hive.dont_write_bytecode;

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
				MY_TRACE_STR(error.what());

				std::cout << error.what() << std::endl;

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

				interpreter.reset();
			}
			catch (const std::exception& error)
			{
				std::cout << error.what() << std::endl;

				return FALSE;
			}

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
				(void)error;

//				std::cout << error.what() << std::endl;

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

				auto sys = py::module::import("sys");
				auto modules = py::cast<py::dict>(sys.attr("modules"));
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
						(void)error;

						MY_TRACE_STR(error.what());
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
				MY_TRACE_STR(error.what());

				std::cout << error.what() << std::endl;

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
				MY_TRACE_STR(error.what());

				std::cout << error.what() << std::endl;

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
					throw std::exception(std::format("{}の読み込みに失敗しました", scheme_module_name).c_str());

				dark_module.add_object("scheme", scheme_module, true);

				return TRUE;
			}
			catch (const std::exception& error)
			{
				MY_TRACE_STR(error.what());

				std::cout << error.what() << std::endl;

				return FALSE;
			}
		}

		//
		// スキンモジュールの初期化関数を呼び出します。
		//
		BOOL call_init()
		{
			try
			{
				return skin_module && skin_module.attr("dark_init")(
					share::InitArgs {
						hive.main_background_color,
						hive.main_text_color,
						hive.sub_background_color,
						hive.sub_text_color,
					}
				).cast<BOOL>();
			}
			catch (const std::exception& error)
			{
				MY_TRACE_STR(error.what());

				std::cout << error.what() << std::endl;

				return FALSE;
			}
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
				MY_TRACE_STR(error.what());

				std::cout << error.what() << std::endl;

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
					hive.main_background_color,
					hive.main_text_color,
					hive.sub_background_color,
					hive.sub_text_color,
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
