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
			MY_TRACE(_T("GetMessage::init()\n"));

			hook = ::SetWindowsHookEx(WH_GETMESSAGE, hookProc, 0, ::GetCurrentThreadId());
			MY_TRACE_HEX(hook);
		}

		//
		// 後始末処理です。
		//
		void exit()
		{
			MY_TRACE(_T("GetMessage::exit()\n"));

			::UnhookWindowsHookEx(hook), hook = 0;
		}

		static LRESULT CALLBACK hookProc(int code, WPARAM wParam, LPARAM lParam)
		{
			if (code == HC_ACTION && wParam == PM_REMOVE)
			{
				MSG* msg = (MSG*)lParam;

				switch (msg->message)
				{
				case WM_MOUSEWHEEL:
					{
						// クラス名を取得します。
						TCHAR className[MAX_PATH] = {};
						::GetClassName(msg->hwnd, className, MAX_PATH);
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
