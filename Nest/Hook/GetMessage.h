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
		// 指定されたウィンドウを保有するドッキングサイトを返します。
		//
		static HWND getDockSite(HWND hwnd)
		{
			while (hwnd)
			{
				MY_TRACE_HWND(hwnd);

				if (SubWindow::isDockSite(hwnd))
					return hwnd;

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
				case WM_KEYDOWN:
				case WM_SYSKEYDOWN:
					{
						WORD vkCode = LOWORD(msg->wParam);
						WORD repeatCount = LOWORD(msg->lParam);

						MY_TRACE(_T("ショートカットキーを発動するかどうかチェックします\n"));

						// ショートカットキーshowCaptionが活性化されている場合は
						if (hive.shortcutKey.showCaption.isActive(msg->wParam))
						{
							// カーソル位置を取得します。
							POINT point; ::GetCursorPos(&point);

							// カーソル位置にあるウィンドウを取得します。
							HWND hwnd = ::WindowFromPoint(point);
							if (!hwnd) break;

							// ウィンドウからドッキングサイトを取得します。
							HWND dockSite = getDockSite(hwnd);
							if (!dockSite) break;

							// ドッキングサイトからルートペインを取得します。
							auto root = SubWindow::getRootPane(dockSite);
							if (!root) break;

							// カーソル位置をクライアント座標に変換します。
							::MapWindowPoints(0, dockSite, &point, 1);

							// カーソル位置にあるペインを取得します。
							auto pane = root->hitTestPane(point);

							// ペインが取得できた場合は
							if (pane)
							{
								// ペインのキャプションの表示状態を切り替えます。
								if (pane->captionMode == Pane::CaptionMode::Show)
									pane->setCaptionMode(Pane::CaptionMode::Hide);
								else
									pane->setCaptionMode(Pane::CaptionMode::Show);

								// ペインのレイアウトを更新します。
								pane->update();
							}

							// ウィンドウにメッセージが送られないようにします。
							msg->hwnd = 0;
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
