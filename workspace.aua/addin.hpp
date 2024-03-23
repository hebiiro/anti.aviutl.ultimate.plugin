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
		BOOL on_dll_init(HINSTANCE instance) override
		{
			MY_TRACE_FUNC("{:#010x}", instance);

			if (!config_io.init()) return FALSE;
			if (!config_io.read()) MY_TRACE("コンフィグの読み込みに失敗しました\n");
			if (!preference_io.init()) return FALSE;
			if (!container_manager.init()) return FALSE;
			if (!main_window->init()) return FALSE;

			if (!hook_manager.pre_init()) return FALSE;

			return TRUE;
		}

		//
		// この仮想関数は、DLLの後始末のタイミングで呼ばれます。
		//
		BOOL on_dll_exit(HINSTANCE instance) override
		{
			MY_TRACE_FUNC("");

			hook_manager.post_exit();

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

			// 無駄な描画を減らすためにメインウィンドウを非表示にします。
			::ShowWindow(*main_window, SW_HIDE);

			hook_manager.exit();

			sub_process_manager.exit();
			sub_window_manager.exit();
			shuttle_manager.exit();

			return TRUE;
		}

		virtual my::addr_t on_get_address(LPCWSTR name) override
		{
			if (wcscmp(name, L"shuttle") == 0) return (my::addr_t)&exports.shuttle;

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
