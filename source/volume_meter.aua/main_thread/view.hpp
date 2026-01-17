#pragma once

namespace apn::volume_meter::main_thread
{
	//
	// このクラスはメインスレッドのビューです。
	// プラグインウィンドウのように振る舞います。
	//
	inline struct view_t : StdAddinWindow
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// ウィンドウを作成します。
			return create_as_plugin(
				hive.instance,
				magi.exin.get_aviutl_window(),
				hive.c_display_name,
				WS_EX_NOPARENTNOTIFY,
				WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				200, 200, 100, 600);
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
			case WM_GETMINMAXINFO:
				{
					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					// ウィンドウの最小サイズを0に変更します。
					auto mmi = (MINMAXINFO*)l_param;
					mmi->ptMinTrackSize.x = 0;
					mmi->ptMinTrackSize.y = 0;

					break;
				}
			case WM_CONTEXTMENU:
				{
					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					constexpr struct command_id_t {
						struct mode_t {
							uint32_t c_off = 1;
							uint32_t c_on = 2;
							uint32_t c_on_without_playing = 3;
						} c_mode;
						uint32_t c_paint_option = 1000;
					} c_command_id;

					// ポップアップメニューを作成します。
					my::menu::unique_ptr<> menu(::CreatePopupMenu());

					// メニュー項目を追加します。
					{
						::AppendMenuW(menu.get(), MF_STRING, c_command_id.c_mode.c_off, L"無効");
						::AppendMenuW(menu.get(), MF_STRING, c_command_id.c_mode.c_on, L"有効");
						::AppendMenuW(menu.get(), MF_STRING, c_command_id.c_mode.c_on_without_playing, L"有効 (再生中以外)");
						::AppendMenuW(menu.get(), MF_SEPARATOR, 0, nullptr);
						::AppendMenuW(menu.get(), MF_STRING, c_command_id.c_paint_option, L"描画設定");
					}

					// メニュー項目にチェックを追加します。
					{
						::CheckMenuRadioItem(menu.get(),
							c_command_id.c_mode.c_off,
							c_command_id.c_mode.c_on_without_playing,
							c_command_id.c_mode.c_off + hive.mode,
							MF_BYCOMMAND);

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
					case c_command_id.c_mode.c_off: hive.mode = hive.c_mode.c_off; break;
					case c_command_id.c_mode.c_on: hive.mode = hive.c_mode.c_on; break;
					case c_command_id.c_mode.c_on_without_playing: hive.mode = hive.c_mode.c_on_without_playing; break;
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
			case hive.c_message.c_post_init:
				{
					MY_TRACE_FUNC("c_post_init, {/hex}, {/hex}", w_param, l_param);

					// 引数で渡されたサブスレッドのビューを
					// ターゲットウィンドウに設定し、サイズを初期化します。
					set_target((HWND)l_param), resize();

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}
	} view;
}
