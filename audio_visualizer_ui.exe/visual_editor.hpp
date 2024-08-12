#pragma once

namespace apn::audio_visualizer::ui
{
	//
	// このクラスはビジュアルエディタです。
	// ビジュアルを編集するためのブラウザを保持します。
	//
	struct VisualEditor : my::Window
	{
		struct VisualEditorBrowser : Browser
		{
			//
			// ビジュアルエディタです。
			//
			VisualEditor& editor;

			//
			// コンストラクタです。
			//
			VisualEditorBrowser(VisualEditor& editor)
				: editor(editor)
			{
			}

			//
			// 初期化処理を実行します。
			//
			BOOL init()
			{
				MY_TRACE_FUNC("");

				if (!create())
				{
					MY_TRACE("ビジュアルエディタブラウザの作成に失敗しました\n");

					return FALSE;
				}

				return __super::init(hive.instance);
			}

			//
			// 後始末処理を実行します。
			//
			BOOL exit()
			{
				MY_TRACE_FUNC("");

				return __super::exit();
			}

			//
			// ウィンドウを作成します。
			//
			BOOL create()
			{
				MY_TRACE_FUNC("");

				WNDCLASSEX wc = { sizeof(wc) };
				wc.style = CS_HREDRAW | CS_VREDRAW;
				wc.lpfnWndProc = ::DefWindowProc;
				wc.hInstance = hive.instance;
				wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
				wc.lpszClassName = _T("audio_visualizer.visual_editor.browser");
				::RegisterClassEx(&wc);

				return __super::create(
					WS_EX_NOPARENTNOTIFY,
					wc.lpszClassName,
					wc.lpszClassName,
					WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
					0, 0, 0, 0,
					editor, nullptr, hive.instance, nullptr);
			}

			//
			// スクリプトにビジュアルの設定を要求します。
			//
			BOOL request_preference(BOOL notify)
			{
				MY_TRACE_FUNC("{}", notify);

				return post_web_message_as_json(
					create_web_message_as_json("request_preference",
					{
						{ "notify", !!notify },
					}));
			}

			//
			// スクリプトにビジュアルの設定を送信します。
			//
			BOOL post_preference(BOOL notify)
			{
				MY_TRACE_FUNC("{}", notify);

				return post_web_message_as_json(
					create_web_message_as_json("update_preference",
					{
						{ "notify", !!notify },
						{ "preference", editor.preference },
					}));
			}

			//
			// この仮想関数は、タイトルバーを持つウィンドウを返します。
			//
			virtual HWND get_title_window() override
			{
				return editor;
			}

			//
			// この仮想関数は、初期化が完了したあとに呼ばれます。
			//
			virtual BOOL on_post_init() override
			{
				MY_TRACE_FUNC("");

				// コントローラを表示します。
				show_controller(TRUE);

				// デフォルト処理を実行します。
				return __super::on_post_init();
			}

			//
			// この仮想関数は、ナビゲーションが完了したあとに呼ばれます。
			//
			virtual BOOL on_navigation_completed(BOOL success) override
			{
				MY_TRACE_FUNC("{}", success);

				if (success)
				{
					// ビジュアルの設定が有効の場合は
					if (!editor.preference.is_null())
					{
						// ビジュアルの設定をスクリプトに送信します。通知はオフにします。
						post_preference(FALSE);
					}
				}

				// デフォルト処理を実行します。
				return __super::on_navigation_completed(success);
			}

			//
			// この仮想関数は、スクリプトからメッセージが発行されたときに呼ばれます。
			//
			virtual BOOL on_web_message_as_json(const std::string& key, const n_json& json) override
			{
				try
				{
					if (key == "response_preference")
					{
						// スクリプトからプリファレンスを受け取ります。
						editor.preference = json["preference"];

						// 通知が有効の場合は
						if (json["notify"].get<bool>())
						{
							// ビジュアルにプリファレンスの変更を通知します。
							::PostMessage(editor.visual, hive.c_message.c_notify_change_preference, 0, 0);
						}
					}
				}
				catch (...)
				{
				}

				return TRUE;
			}
		} browser;

		//
		// ビジュアル(のウィンドウハンドル)です。
		//
		HWND visual = nullptr;

		//
		// ビジュアルの設定です。
		//
		n_json preference;

		//
		// コンストラクタです。
		//
		VisualEditor()
			: browser(*this)
		{
		}

		//
		// 初期化処理を実行します。
		//
		BOOL init(HWND visual)
		{
			MY_TRACE_FUNC("{:#010x}", visual);

			this->visual = visual;

			if (!create())
			{
				MY_TRACE("ビジュアルエディタの作成に失敗しました\n");

				return FALSE;
			}

			return browser.init();
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return browser.exit();
		}

		//
		// ウィンドウを作成します。
		//
		BOOL create()
		{
			MY_TRACE_FUNC("");

			WNDCLASSEX wc = { sizeof(wc) };
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = hive.instance;
			wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
			wc.lpszClassName = _T("audio_visualizer.visual_editor");
			::RegisterClassEx(&wc);

			return __super::create(
				WS_EX_NOPARENTNOTIFY | WS_EX_TOOLWINDOW,
				wc.lpszClassName,
				wc.lpszClassName,
				WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				0, 0, 0, 0,
				visual, nullptr, hive.instance, nullptr);
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_SIZE:
				{
					MY_TRACE_FUNC("WM_SIZE, {:#010x}, {:#010x}", wParam, lParam);

					// ブラウザをクライアント領域全体に広げます。
					auto rc = my::get_client_rect(hwnd);
					auto x = rc.left;
					auto y = rc.top;
					auto w = my::get_width(rc);
					auto h = my::get_height(rc);
					::SetWindowPos(browser, nullptr, x, y, w, h, SWP_NOZORDER);

					break;
				}
			case WM_CLOSE:
				{
					MY_TRACE_FUNC("WM_CLOSE, {:#010x}, {:#010x}", wParam, lParam);

					// ウィンドウを非表示にします。
					::ShowWindow(hwnd, SW_HIDE);

					// ウィンドウが閉じないようにデフォルト処理をスキップします。
					return 0;
				}
			case hive.c_message.c_notify_change_preference:
				{
					MY_TRACE_FUNC("c_notify_change_preference, {:#010x}, {:#010x}", wParam, lParam);

					// ビジュアルの設定をスクリプトに送信します。
					browser.post_preference(FALSE);

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	};
}
