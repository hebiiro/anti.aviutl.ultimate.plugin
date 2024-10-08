#pragma once

namespace apn::audio_visualizer::ui
{
	//
	// このクラスはアプリケーションです。
	//
	inline struct App : AppInterface
	{
		//
		// コンストラクタです。
		//
		App() { app = this; }

		//
		// コマンドライン引数からホストウィンドウを受け取ります。
		//
		inline static HWND receive_host_window()
		{
			auto command_line = ::GetCommandLineW();
			MY_TRACE_STR(command_line);

			auto argc = 0;
			auto argv = ::CommandLineToArgvW(command_line, &argc);
			MY_TRACE_INT(argc);

			auto arg = command_line;
			if (argc >=  2) // 引数が2個以上なら
				arg = argv[1]; // 2番目の引数を使用します。
			MY_TRACE_STR(arg);

			return (HWND)wcstoul(arg, nullptr, 0);
		}

		//
		// 初期化処理を実行します。
		//
		virtual BOOL init() override
		{
			MY_TRACE_FUNC("");

			hive.host_window = receive_host_window();
			MY_TRACE_HWND(hive.host_window);
			if (!hive.host_window) return FALSE;

			auto pid = DWORD {};
			auto tid = ::GetWindowThreadProcessId(hive.host_window, &pid);
			hive.host_process.reset(::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid));
			if (!hive.host_process) return FALSE;

			if (!config_io.init()) return FALSE;
			if (!main_window.init()) return FALSE;
			if (!visual_manager.init()) return FALSE;
			if (!host_process_checker.init()) return FALSE;
			if (!config_file_checker.init()) return FALSE;

			// 初期化完了のイベントを作成します。
			my::Event event(EVENT_ALL_ACCESS, FALSE, share::get_app_ready_event_name(hive.host_window).c_str());
			MY_TRACE_HEX((HANDLE)event);

			// 待機しているホストプロセスに初期化が完了したことを知らせます。
			::SetEvent(event);

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL exit() override
		{
			MY_TRACE_FUNC("");

			config_file_checker.exit();
			host_process_checker.exit();
			visual_manager.exit();
			main_window.exit();
			config_io.exit();

			return TRUE;
		}

		//
		// メッセージループです。
		//
		virtual BOOL run() override
		{
			MY_TRACE_FUNC("");

			MSG msg = {};
			while (::GetMessage(&msg, 0, 0, 0) > 0)
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}

			return TRUE;
		}

		//
		// コンフィグを読み込みます。
		//
		virtual BOOL read_config() override
		{
			MY_TRACE_FUNC("");

			config_io.read();

			return TRUE;
		}

		//
		// コンフィグを書き込みます。
		//
		virtual BOOL write_config() override
		{
			MY_TRACE_FUNC("");

			config_io.write();

			return TRUE;
		}
	} app_impl;
}
