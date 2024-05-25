#pragma once

namespace apn::filer_ui
{
	class App : public CWinApp, public Hive::App
	{
	public:

		Dark::Module dark;
		my::FileUpdateChecker config_file_checker;

		//
		// コンストラクタです。
		//
		App()
		{
			_tsetlocale(LC_ALL, _T(""));

			my::tracer_to_file::init(0);

			MY_TRACE_FUNC("");

			hive.app = this;
		}

		//
		// デストラクタです。
		//
		virtual ~App() override
		{
			MY_TRACE_FUNC("");

			my::tracer_to_file::exit();
		}

		//
		// コマンドライン引数からホストウィンドウを受け取ります。
		//
		HWND receive_host_window()
		{
			auto command_line = ::GetCommandLineW();
			MY_TRACE_STR(command_line);

			auto argc = 0;
			auto argv = ::CommandLineToArgvW(command_line, &argc);
			MY_TRACE_INT(argc);

			auto arg = command_line;
			if (argc >=  2) // 引数が 2 個以上なら
				arg = argv[1]; // 2 番目の引数を使用します。
			MY_TRACE_STR(arg);

			return (HWND)std::wcstoul(arg, 0, 0);
		}

		//
		// 初期化処理を実行します。
		//
		BOOL InitInstance()
		{
			MY_TRACE_FUNC("");

			__super::InitInstance();

			if (!AfxOleInit())
			{
				AfxMessageBox(_T("AfxOleInit()が失敗しました"));

				return FALSE;
			}

			// ホストウィンドウを取得します。
			hive.host_window = receive_host_window();
			MY_TRACE_HWND(hive.host_window);
			if (!hive.host_window) return FALSE;

			// コンフィグ入出力を初期化します。
			if (!config_io.init())
			{
				AfxMessageBox(_T("コンフィグ入出力の初期化に失敗しました"));

				return FALSE;
			}

			// クライアントウィンドウを初期化します。
			if (!client_window.init(hive.host_window))
			{
				AfxMessageBox(_T("クライアントウィンドウの作成に失敗しました"));

				return FALSE;
			}

			// ダークモード化モジュールを初期化します。
			if (!dark.init(hive.host_window, client_window))
				MY_TRACE("ダークモード化に失敗しました\n");

			// ホストウィンドウにクライアントウィンドウのハンドルを設定します。
			share::host_window::set_client_window(hive.host_window, client_window);

			// クライアントウィンドウをメインウィンドウに設定します。
			hive.main_window = client_window;

			// クライアントウィンドウをMFCのメインウィンドウに設定します。
			AfxGetApp()->m_pMainWnd = &client_window;

			// コンフィグファイルチェッカーを作成します。
			config_file_checker.init(hive.config_file_name.c_str());

			// コンフィグファイルを監視するタイマーを作成します。
			client_window.SetTimer(hive.c_timer_id.c_check_config, 1000, 0);

			// ホストウィンドウを監視するタイマーを作成します。
			client_window.SetTimer(hive.c_timer_id.c_check_host_window, 1000, 0);

			// 初期化完了のイベントを作成します。
			my::Event event(EVENT_ALL_ACCESS, FALSE,
				apn::filer::share::event::c_post_init_client);

			// 待機しているホストプロセスに初期化が完了したことを知らせます。
			::SetEvent(event);

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual int ExitInstance() override
		{
			MY_TRACE_FUNC("");

			// ファイラダイアログをすべて削除します。
			FilerDialog::collection.clear();

			// クライアントウィンドウを終了します。
			client_window.exit();

			// コンフィグ入出力を終了します。
			config_io.exit();

			// ダークモード化モジュールを終了します。
			dark.exit();

			AfxOleTerm(FALSE);

			return __super::ExitInstance();
		}

		virtual void on_init() override
		{
			MY_TRACE_FUNC("");

			// ファイラダイアログの作成が完了したので
			// コンフィグを読み込みます。
			config_io.read();
		}

		virtual void on_exit() override
		{
			MY_TRACE_FUNC("");

			// コンフィグチェッカーを終了します。
			client_window.KillTimer(hive.c_timer_id.c_check_config);

			// コンフィグを保存します。
			config_io.write();
		}

		virtual void on_init_filer_window(HWND filer_window, BOOL full) override
		{
			MY_TRACE_FUNC("{:#010x}, {}", filer_window, full);

			FilerDialog::create_instance(filer_window, full);
		}

		virtual void on_check_host_window() override
		{
			if (!::IsWindow(hive.host_window))
			{
				MY_TRACE("ホストウィンドウが無効になったのでプロセスを終了します\n");

				client_window.KillTimer(hive.c_timer_id.c_check_config);
				client_window.KillTimer(hive.c_timer_id.c_check_host_window);
				AfxPostQuitMessage(0);
			}
		}

		virtual void on_check_config() override
		{
			if (::IsWindow(hive.host_window))
			{
				if (config_file_checker.check_update())
				{
					MY_TRACE("ユーザーがコンフィグファイルを更新したので読み込み直します\n");

					config_io.read();
				}
			}
		}

		DECLARE_MESSAGE_MAP()
	};

	BEGIN_MESSAGE_MAP(App, CWinApp)
	END_MESSAGE_MAP()

	inline App theApp;
}
