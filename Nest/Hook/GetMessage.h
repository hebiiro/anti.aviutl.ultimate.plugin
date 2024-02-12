#pragma once

namespace fgo::nest::hook
{
	//
	// このクラスはWH_GETMESSAGEをフックします。
	//
	inline static struct GetMessage
	{
		HHOOK hook = 0;

		//
		// 初期化処理です。
		//
		void init()
		{
			MY_TRACE_FUNC("");

			hook = ::SetWindowsHookEx(WH_GETMESSAGE, hookProc, 0, ::GetCurrentThreadId());
			MY_TRACE_HEX(hook);
		}

		//
		// 後始末処理です。
		//
		void exit()
		{
			MY_TRACE_FUNC("");

			::UnhookWindowsHookEx(hook), hook = 0;
		}

		//
		// 指定されたウィンドウを内包するペインを返します。
		//
		static Pane* getPane(HWND hwnd)
		{
			while (hwnd)
			{
				MY_TRACE_HWND(hwnd);

				auto pane = Pane::getPane(hwnd);
				if (pane) return pane;
				hwnd = ::GetParent(hwnd);
			}

			return 0;
		}

		//
		// WH_GETMESSAGEのフックプロシージャです。
		//
		static LRESULT CALLBACK hookProc(int code, WPARAM wParam, LPARAM lParam)
		{
			if (code == HC_ACTION && wParam == PM_REMOVE)
			{
				auto msg = (MSG*)lParam;

				switch (msg->message)
				{
				case WM_KEYUP:
				case WM_SYSKEYUP:
					{
						MY_TRACE(_T("ショートカットキーを発動するかどうかチェックします\n"));

						// ショートカットキーshowCaptionが活性化されている場合は
						if (hive.shortcutKey.showCaption.isActive(msg->wParam))
						{
							// ウィンドウからペインを取得します。
							auto pane = getPane(msg->hwnd);

							// ペインが取得できた場合は
							if (pane)
							{
								// ペインのキャプションを表示します。
								pane->setCaptionMode(Pane::CaptionMode::Show);
								pane->recalcLayout();
							}
						}

						break;
					}
				case WM_MOUSEWHEEL:
					{
						// クラス名を取得します。
						TCHAR className[MAX_PATH] = {};
						::GetClassName(msg->hwnd, className, std::size(className));
						MY_TRACE_TSTR(className);

						// コンボボックスの場合
						if (_tcsicmp(className, WC_COMBOBOX) == 0)
						{
							// スクロールを優先するように指定されているなら
							if (hive.forceScroll)
							{
								// 親ウィンドウにマウスホイールメッセージを処理させます。
								msg->hwnd = ::GetParent(msg->hwnd);
							}
						}
						// トラックバーの場合
						else if (_tcsicmp(className, TRACKBAR_CLASS) == 0)
						{
							// 親ウィンドウにマウスホイールメッセージを処理させます。
							msg->hwnd = ::GetParent(msg->hwnd);
						}

						break;
					}
				}
			}

			return ::CallNextHookEx(0, code, wParam, lParam);
		}
	} get_message;
}
