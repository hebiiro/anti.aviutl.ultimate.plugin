#pragma once

namespace apn::workspace::hook::win_hook
{
	//
	// このクラスはWH_GETMESSAGEをフックします。
	//
	inline struct GetMessage : Entity
	{
		my::win_hook::unique_ptr<> hook;

		//
		// 初期化処理を実行します。
		//
		virtual BOOL on_dll_init() override
		{
			MY_TRACE_FUNC("");

			hook.reset(::SetWindowsHookEx(WH_GETMESSAGE, hook_proc, nullptr, ::GetCurrentThreadId()));
			MY_TRACE_HEX(hook.get());

			return !!hook;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL on_dll_exit() override
		{
			MY_TRACE_FUNC("");

			hook.reset();

			return TRUE;
		}

		//
		// 指定されたウィンドウを保有するドッキングサイトを返します。
		//
		inline static HWND get_dock_site(HWND hwnd)
		{
			while (hwnd)
			{
				MY_TRACE_HWND(hwnd);

				if (SubWindow::is_dock_site(hwnd))
					return hwnd;

				hwnd = ::GetParent(hwnd);
			}

			return nullptr;
		}

		//
		// WH_GETMESSAGEのフックプロシージャです。
		//
		static LRESULT CALLBACK hook_proc(int code, WPARAM wParam, LPARAM lParam)
		{
			if (code == HC_ACTION && wParam == PM_REMOVE)
			{
				auto msg = (MSG*)lParam;

				if (hive.bypass_keyboard_message)
				{
					if (msg->message >= WM_KEYFIRST && msg->message <= WM_KEYLAST)
					{
						MY_TRACE_HWND(msg->hwnd);

						auto class_name = my::get_class_name(msg->hwnd);
						if (!my::contains(class_name, WC_EDIT))
						{
							MY_TRACE("キーボードメッセージを拡張編集ウィンドウにバイパスします\n");

							msg->hwnd = hive.exedit_window;
						}
					}
				}

				switch (msg->message)
				{
				case WM_KEYDOWN:
				case WM_SYSKEYDOWN:
					{
						auto vk_code = LOWORD(msg->wParam);
						auto repeat_count = LOWORD(msg->lParam);

//						MY_TRACE("ショートカットキーを発動するかどうかチェックします\n");

						// ショートカットキーshow_captionが活性化されている場合は
						if (hive.shortcut_key.show_caption.is_active(msg->wParam))
						{
							// カーソル位置を取得します。
							auto point = my::get_cursor_pos();

							// カーソル位置にあるウィンドウを取得します。
							auto hwnd = ::WindowFromPoint(point);
							if (!hwnd) break;

							// ウィンドウからドッキングサイトを取得します。
							auto dock_site = get_dock_site(hwnd);
							if (!dock_site) break;

							// ドッキングサイトからルートペインを取得します。
							auto root = SubWindow::get_root_pane(dock_site);
							if (!root) break;

							// カーソル位置をクライアント座標に変換します。
							my::map_window_points(nullptr, dock_site, &point);

							// カーソル位置にあるペインを取得します。
							auto pane = root->hittest_pane(point);

							// ペインが取得できた場合は
							if (pane)
							{
								// ペインのキャプションの表示状態を切り替えます。
								if (pane->caption_mode == pane->c_caption_mode.c_show)
									pane->set_caption_mode(pane->c_caption_mode.c_hide);
								else
									pane->set_caption_mode(pane->c_caption_mode.c_show);

								// ペインのレイアウトを更新します。
								pane->update_origin();
							}

							// ウィンドウにメッセージが送られないようにします。
							msg->hwnd = nullptr;
						}

						break;
					}
				case WM_MOUSEWHEEL:
					{
						// クラス名を取得します。
						auto class_name = my::get_class_name(msg->hwnd);
						MY_TRACE_STR(class_name);

						// コンボボックスの場合は
						if (::lstrcmpi(class_name.c_str(), WC_COMBOBOX) == 0)
						{
							// スクロールを優先する場合は
							if (hive.scroll_force)
							{
								// 親ウィンドウにマウスホイールメッセージを処理させます。
								msg->hwnd = ::GetParent(msg->hwnd);
							}
						}
						// トラックバーの場合は
						else if (::lstrcmpi(class_name.c_str(), TRACKBAR_CLASS) == 0)
						{
							// 親ウィンドウにマウスホイールメッセージを処理させます。
							msg->hwnd = ::GetParent(msg->hwnd);
						}

						break;
					}
				}
			}

			return ::CallNextHookEx(nullptr, code, wParam, lParam);
		}
	} get_message;
}
