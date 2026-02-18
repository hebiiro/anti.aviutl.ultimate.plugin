#pragma once

namespace apn::timeline_map::main_thread
{
	//
	// このクラスはメインスレッドのビューです。
	// プラグインウィンドウのように振る舞います。
	//
	inline struct view_t : StdAddinWindow, paint_option_dialog_t::listner_t
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// ビューを描画設定ダイアログのリスナーに設定します。
			paint_option_dialog.listener = this;

			// ウィンドウを作成します。
			return create_as_plugin(
				hive.instance,
				magi.exin.get_aviutl_window(),
				hive.c_display_name,
				WS_EX_NOPARENTNOTIFY,
				WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				200, 200, 600, 600);
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			// 描画設定ダイアログを終了します。
			paint_option_dialog.exit();

			// ウィンドウを破壊します。
			return destroy();
		}

		//
		// タイムラインマップを再描画します。
		//
		BOOL redraw()
		{
			MY_TRACE_FUNC("");

			// ウィンドウを再描画します。
//			return ::InvalidateRect(*this, nullptr, FALSE);
			return ::InvalidateRect(*this, nullptr, FALSE), ::UpdateWindow(*this);
		}

		//
		// この仮想関数は描画設定ダイアログで変更があったときに呼び出されます。
		//
		virtual void on_from_ui(BOOL recreate_resources)
		{
			// リソースを再作成します。
			if (recreate_resources)
				state.recreate_resources();

			// タイムラインマップを再描画します。
			redraw();
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
			switch (message)
			{
			case WM_CREATE:
				{
					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					// このウィンドウをメインウィンドウに設定します。
					hive.main_window = hwnd;

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					break;
				}
			case WM_PAINT:
				{
//					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					context_t(hwnd).on_paint();

					return 0;
				}
			case WM_SIZE:
				{
//					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					context_t(hwnd).on_size();

					break;
				}
			case WM_LBUTTONDOWN:
				{
//					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					::SetCapture(hwnd);

					context_t(hwnd).on_click(my::lp_to_pt(l_param));

					break;
				}
			case WM_LBUTTONUP:
				{
//					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					if (::GetCapture() == hwnd)
						::ReleaseCapture();

					break;
				}
			case WM_MOUSEMOVE:
				{
//					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					if (::GetCapture() == hwnd)
						context_t(hwnd).on_click(my::lp_to_pt(l_param));

					break;
				}
			case WM_CONTEXTMENU:
				{
					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					constexpr struct command_id_t {
						uint32_t c_paint_option = 1000;
					} c_command_id;

					// ポップアップメニューを作成します。
					my::menu::unique_ptr<> menu(::CreatePopupMenu());

					// メニュー項目を追加します。
					{
						::AppendMenuW(menu.get(), MF_STRING, c_command_id.c_paint_option, L"描画設定");
					}

					// メニュー項目にチェックを追加します。
					{
						if (::IsWindowVisible(paint_option_dialog))
							::CheckMenuItem(menu.get(), c_command_id.c_paint_option, MF_BYCOMMAND | MF_CHECKED);
					}

					// ポップアップメニューの表示位置を取得します。
					auto pt = my::lp_to_pt(l_param);

					// ポップアップメニューを表示します。
					auto command_id = ::TrackPopupMenuEx(menu.get(),
						TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, hwnd, nullptr);

					switch (command_id)
					{
					case c_command_id.c_paint_option:
						{
							// 描画設定ダイアログがまだ作成されていない場合は
							if (!::IsWindow(paint_option_dialog))
							{
								// 描画設定ダイアログを作成します。
								paint_option_dialog.init(hive.instance, hwnd);

								// 描画設定ダイアログのコントロールを初期化します。
								paint_option_dialog.update_controls();
							}

							// 描画設定ダイアログを表示します。
							::ShowWindow(paint_option_dialog, SW_SHOW);

							break;
						}
					}

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}
	} view;
}
