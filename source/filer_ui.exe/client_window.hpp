#pragma once

namespace apn::filer_ui
{
	class ClientWindow : public CWnd
	{
	public:

		inline static constexpr LPCTSTR c_class_name = _T("apn::filer_gui::client_window");

		//
		// 初期化処理を実行します。
		//
		BOOL init(HWND host_window)
		{
			MY_TRACE_FUNC("{/hex}", host_window);

			WNDCLASS wc = {};
			wc.lpszClassName = c_class_name;
			wc.lpfnWndProc = AfxWndProc;
			AfxRegisterClass(&wc);

			return __super::CreateEx(
				0,
				wc.lpszClassName,
				wc.lpszClassName,
				WS_CAPTION | WS_SYSMENU | WS_THICKFRAME,
				CRect(100, 100, 400, 400),
				0,
				0);
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return DestroyWindow();
		}

		void OnTimer(UINT_PTR timer_id)
		{
			switch (timer_id)
			{
			case hive.c_timer_id.c_check_host_window: hive.app->on_check_host_window(); break;
			case hive.c_timer_id.c_check_config: hive.app->on_check_config(); break;
			}

			CWnd::OnTimer(timer_id);
		}

		LRESULT on_init(WPARAM wParam, LPARAM lParam)
		{
			MY_TRACE_FUNC("{/hex}, {/hex}", wParam, lParam);

			return hive.app->on_init(), 0;
		}

		LRESULT on_exit(WPARAM wParam, LPARAM lParam)
		{
			MY_TRACE_FUNC("{/hex}, {/hex}", wParam, lParam);

			return hive.app->on_exit(), 0;
		}

		LRESULT on_init_filer_window(WPARAM wParam, LPARAM lParam)
		{
			MY_TRACE_FUNC("{/hex}, {/hex}", wParam, lParam);

			auto filer_window = (HWND)wParam;
			auto full = (BOOL)lParam;

			return hive.app->on_init_filer_window(filer_window, full), 0;
		}

		DECLARE_MESSAGE_MAP()
	} client_window;

	BEGIN_MESSAGE_MAP(ClientWindow, CWnd)
		ON_WM_TIMER()
		ON_MESSAGE(share::message::c_init, on_init)
		ON_MESSAGE(share::message::c_exit, on_exit)
		ON_MESSAGE(share::message::c_init_filer_window, on_init_filer_window)
	END_MESSAGE_MAP()
}
