#pragma once

namespace apn::dark::kuro::hook
{
	//
	// このクラスはWH_CALLWNDPROCRETフックを担当します。
	//
	inline struct cwpr_t : entry_t::node_t
	{
		my::win_hook::unique_ptr<> win_hook;

		//
		// 初期化処理を実行します。
		//
		virtual BOOL on_init() override
		{
			MY_TRACE_FUNC("");

			win_hook.reset(::SetWindowsHookEx(WH_CALLWNDPROCRET, win_hook_proc, nullptr, ::GetCurrentThreadId()));

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL on_exit() override
		{
			MY_TRACE_FUNC("");

			win_hook.reset();

			return TRUE;
		}

		//
		// WH_CALLWNDPROCRETのフックプロシージャです。
		//
		static LRESULT CALLBACK win_hook_proc(int code, WPARAM w_param, LPARAM l_param)
		{
			auto cs = (CWPRETSTRUCT*)l_param;
#ifdef _DEBUG
			if (0)
			{
				// デバッグ用のコードです。

				auto hwnd = cs->hwnd;
				auto message = cs->message;
				auto w_param = cs->wParam;
				auto l_param = cs->lParam;
				auto l_result = cs->lResult;
				auto class_name = my::get_class_name(hwnd);

				MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/hex}, {/} => {/hex}",
					hwnd, message, w_param, l_param, class_name, l_result);
			}
#endif
			switch (cs->message)
			{
			case WM_CREATE:
				{
					if (cs->lResult < 0) break;

					auto hwnd = cs->hwnd;
#ifdef _DEBUG
					auto class_name = my::get_class_name(hwnd);

					MY_TRACE_FUNC("{/}, {/hex}, {/}", code, hwnd, class_name);
#endif
					// ウィンドウとレンダラーを関連付けます。
					gdi::manager.attach_renderer(hwnd);

					break;
				}
			}

			return ::CallNextHookEx(nullptr, code, w_param, l_param);
		}
	} cwpr;
}
