#pragma once

namespace apn::ease_select_ui
{
	inline class App : public CWinApp, public Hive::App
	{
	public:

		//
		// コンストラクタです。
		//
		App() noexcept
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
		// コンフィグを読み込みます。
		// さらに他のオブジェクトに新しいコンフィグ値を適用します。
		//
		virtual BOOL read_config() override
		{
			MY_TRACE_FUNC("");

			config_io.read();

			easing_window.load_image();

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

		virtual BOOL InitInstance() override
		{
			MY_TRACE_FUNC("");

			{
				// ホストウィンドウを取得します。
				hive.host_window = (HWND)_tcstoul(::GetCommandLine(), 0, 0);
				::GetWindowThreadProcessId(hive.host_window, &hive.host_process_id);
				MY_TRACE_HWND(hive.host_window);
				if (!::IsWindow(hive.host_window)) return FALSE;
			}

			__super::InitInstance();

			if (!AfxOleInit())
				return -1;

			EnableTaskbarInteraction(FALSE);

			// メインウィンドウを作成します。
			m_pMainWnd = &easing_window;
			if (!m_pMainWnd->CreateEx(
				WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_LAYERED,
				AfxRegisterWndClass(0),
				_T(""),
				WS_POPUP, 0, 0, 0, 0, 0, 0))
			{
				hive.message_box(L"メインウィンドウの作成に失敗しました");

				return FALSE;
			}

			// 各種初期化処理を実行します。
			config_io.init();
			config_checker.init();
			host_checker.init();

			read_config();

			return TRUE;
		}

		virtual int ExitInstance() override
		{
			MY_TRACE_FUNC("");

			write_config();

			// 各種後始末処理を実行します。
			host_checker.exit();
			config_checker.exit();
			config_io.exit();

			AfxOleTerm(FALSE);

			return __super::ExitInstance();
		}

		virtual int Run() override
		{
			MY_TRACE_FUNC("");

			GdiplusStartupInput gdi_si;
			GdiplusStartupOutput gdi_so;
			ULONG_PTR gdi_token = 0;
			ULONG_PTR gdi_hook_token = 0;

			gdi_si.SuppressBackgroundThread = TRUE;
			GdiplusStartup(&gdi_token, &gdi_si, &gdi_so);
			gdi_so.NotificationHook(&gdi_hook_token);

			auto result = __super::Run();

			gdi_so.NotificationUnhook(gdi_hook_token);
			GdiplusShutdown(gdi_token);

			return result;
		}

		//
		// ホストプロセスから通知が来たときに呼び出されます。
		//
		void on_notify(WPARAM wParam, LPARAM lParam)
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}", wParam, lParam);

			// 手動でタイマーメッセージを送信します。
			// これにより、タイマー周期より早く反応できます。
			easing_window.PostMessage(WM_TIMER, easing_window.timer_id, 0);
		}

		DECLARE_MESSAGE_MAP()
	} theApp;

	BEGIN_MESSAGE_MAP(App, CWinApp)
		ON_THREAD_MESSAGE(share::message::c_notify, on_notify)
	END_MESSAGE_MAP()
}
