#pragma once

namespace apn::workspace
{
	//
	// このクラスはAviUtlをワークスペース化します。
	//
	inline struct Workspace : Addin
	{
		//
		// この仮想関数は、このアドインの識別名が必要なときに呼ばれます。
		//
		virtual LPCWSTR get_addin_name() override
		{
			return hive.c_name;
		}

		//
		// この仮想関数は、このアドインの表示名が必要なときに呼ばれます。
		//
		virtual LPCWSTR get_addin_display_name() override
		{
			return hive.c_display_name;
		}

		//
		// この仮想関数は、DLLの初期化のタイミングで呼ばれます。
		//
		virtual BOOL on_dll_init(HINSTANCE instance) override
		{
			MY_TRACE_FUNC("{:#010x}", instance);

			if (!config_io.init()) return FALSE;
			if (!config_io.read()) MY_TRACE("コンフィグの読み込みに失敗しました\n");
			if (!preference_io.init()) return FALSE;
			if (!container_manager.init()) return FALSE;
			if (!main_window->init()) return FALSE;
			if (!layout_list.init()) return FALSE;

			if (!hook_manager.pre_init()) return FALSE;

			return TRUE;
		}

		//
		// この仮想関数は、DLLの後始末のタイミングで呼ばれます。
		//
		virtual BOOL on_dll_exit(HINSTANCE instance) override
		{
			MY_TRACE_FUNC("");

			hook_manager.post_exit();

			layout_list.exit();
			main_window->exit();
			container_manager.exit();
			preference_io.exit();
			config_io.write();
			config_io.exit();

			return TRUE;
		}

		//
		// この仮想関数は、初期化のタイミングで呼ばれます。
		//
		virtual BOOL on_init() override
		{
			MY_TRACE_FUNC("");

			if (!shuttle_manager.init()) return FALSE;
			if (!sub_window_manager.init()) return FALSE;
			if (!sub_process_manager.init()) return FALSE;

			if (!hook_manager.init()) return FALSE;

			return TRUE;
		}

		//
		// この仮想関数は、後始末のタイミングで呼ばれます。
		//
		virtual BOOL on_exit() override
		{
			MY_TRACE_FUNC("");

			// 後始末処理を最適化します。
			{
				// 無駄な処理を減らすために可能な限りサブクラス化を解除します。
				main_window->unsubclass();
				for (const auto& sub_window : SubWindow::collection) sub_window->unsubclass();
				for (const auto& node : shuttle_manager.collection) node.second->unsubclass();

				// 無駄な描画を減らすためにメインウィンドウを非表示にします。
				::ShowWindow(hive.main_window, SW_HIDE);
			}

			hook_manager.exit();

			sub_process_manager.exit();
			sub_window_manager.exit();
			shuttle_manager.exit();

			return TRUE;
		}

		//
		// この仮想関数は、ウィンドウコマンドを実行するときに呼ばれます。
		//
		virtual BOOL on_window_command(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			switch (wParam)
			{
			case magi.c_command_id.c_addin.c_command:
				{
					MY_TRACE_FUNC("magi.c_command_id.c_addin.c_command");

					if (main_window)
						main_window->show_config_dialog();

					break;
				}
			}

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウ関数のタイミングで呼び出されます。
		//
		virtual BOOL on_window_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
//			MY_TRACE_FUNC("{:#010x}, {:#010x}, {:#010x}, {:#010x}", hwnd, message, wParam, lParam);

			switch (message)
			{
			case AviUtl::FilterPlugin::WindowMessage::ChangeWindow:
				{
					MY_TRACE_FUNC("ChangeWindow, {:#010x}, {:#010x}", wParam, lParam);

					static BOOL is_initialized = FALSE;
					if (is_initialized) break;
					is_initialized = TRUE;

					// すべてのウィンドウが生成されたはずなので
					// レイアウトを初期化します。
					::PostMessage(hive.main_window, hive.c_message.c_post_init, 0, 0);

					break;
				}
			}

			return FALSE;
		}

		//
		// この仮想関数は、このアドインと交信するために呼ばれます。
		//
		virtual my::addr_t on_get_address(LPCWSTR name) override
		{
			if (::lstrcmpW(name, L"shuttle") == 0) return (my::addr_t)&exports.shuttle;

			return 0;
		}

		struct Exports {
			struct Shuttle : share::Exports::Shuttle {
				//
				// エクスポート関数です。
				// 指定されたターゲットを保持するシャトル名を変更します。
				//
				virtual BOOL rename(HWND target, LPCWSTR name) override
				{
					MY_TRACE_FUNC("{:#010x}, {}", target, name);

					auto shuttle = apn::workspace::Shuttle::get_pointer(target);
					if (!shuttle) return FALSE;
					return shuttle_manager.rename(shuttle, name);
				}

				//
				// エクスポート関数です。
				// 指定されたシャトル名が使用可能な場合はTRUEを返します。
				//
				virtual BOOL is_available_name(LPCWSTR name) override
				{
					MY_TRACE_FUNC("{}", name);

					return !shuttle_manager.get(name);
				}
			} shuttle;
		} exports;
	} addin;
}
