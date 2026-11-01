#pragma once

namespace apn::timeline_map::view
{
	//
	// このクラスはビュー層の全体図です。
	// プラグインウィンドウのように振る舞います。
	//
	inline struct overview_t : StdAddinWindow, overview_behavior_t
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// 振る舞いの初期化処理を実行します。
			overview_behavior_t::init();

			// ウィンドウを作成します。
			return create_as_plugin(
				model::property.instance,
				magi.exin.get_aviutl_window(),
				model::property.c_display_name,
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

			// 振る舞いの後始末処理を実行します。
			overview_behavior_t::exit();

			// 描画設定ダイアログを終了します。
			paint_option_dialog.exit();

			// ウィンドウを破壊します。
			return destroy();
		}

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, int32_t type = MB_OK | MB_ICONWARNING)
		{
			return magi.message_box(text, model::property.c_display_name, *this, type);
		}

		//
		// 全体図を再描画します。
		//
		BOOL redraw()
		{
			MY_TRACE_FUNC("");

			// ウィンドウを再描画します。
			::InvalidateRect(*this, nullptr, FALSE);

			// フラグが立っている場合は即時描画します。
			if (model::property.etc.flag_immediate) ::UpdateWindow(*this);

			return TRUE;
		}

		//
		// この仮想関数はタイムラインが更新されたときに呼び出されます。
		//
		virtual BOOL on_update()
		{
			MY_TRACE_FUNC("");

			// D2Dコマンドリストをリセットします。
			reset_command_list();

			// 全体図を再描画します。
			return redraw();
		}

		//
		// この仮想関数はウィンドウハンドルが必要なときに呼び出されます。
		//
		virtual HWND get_hwnd() const override
		{
			return hwnd;
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

					// WM_PAINTを処理します。
					my::PaintDC dc(hwnd);

					on_paint();

					return 0;
				}
			case WM_SIZE:
				{
//					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					on_size();

					break;
				}
			case WM_LBUTTONDOWN:
				{
//					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					// マウスのキャプチャを開始します。
					::SetCapture(hwnd);

					// マウスメッセージを処理します。
					on_mouse(my::lp_to_pt(l_param));

					break;
				}
			case WM_LBUTTONUP:
				{
//					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					// マウスをキャプチャしている場合は
					if (::GetCapture() == hwnd)
					{
						// マウスのキャプチャを終了します。
						::ReleaseCapture();
					}

					break;
				}
			case WM_MOUSEMOVE:
				{
//					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					// マウスをキャプチャしている場合は
					if (::GetCapture() == hwnd)
					{
						// タイムラインを操作対象にしてマウス処理を実行します。
						on_mouse(my::lp_to_pt(l_param));
					}
					// 全体図が操作対象の場合は
					else if (model::property.loupe.flag_overview)
					{
						// 拡大鏡を操作対象にしてマウス処理を実行します。
						on_mouse(my::lp_to_pt(l_param), TRUE);
					}

					break;
				}
			case WM_CONTEXTMENU:
				{
					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					constexpr struct command_id_t {
						uint32_t c_paint_option = 1000;
						uint32_t c_loupe = 1001;
					} c_command_id;

					// ポップアップメニューを作成します。
					my::menu::unique_ptr menu(::CreatePopupMenu());

					// メニュー項目を追加します。
					{
						::AppendMenuW(menu.get(), MF_STRING, c_command_id.c_paint_option, L"描画設定");
						::AppendMenuW(menu.get(), MF_STRING, c_command_id.c_loupe, L"タイムラインルーペを表示");
					}

					// メニュー項目にチェックを追加します。
					{
						if (::IsWindowVisible(paint_option_dialog))
							::CheckMenuItem(menu.get(), c_command_id.c_paint_option, MF_BYCOMMAND | MF_CHECKED);

						if (::IsWindowVisible(loupe))
							::CheckMenuItem(menu.get(), c_command_id.c_loupe, MF_BYCOMMAND | MF_CHECKED);
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
								paint_option_dialog.init(model::property.instance, hwnd);

								// 描画設定ダイアログのコントロールを初期化します。
								paint_option_dialog.update_controls();
							}

							// 描画設定ダイアログを表示します。
							::ShowWindow(paint_option_dialog, SW_SHOW);

							break;
						}
					case c_command_id.c_loupe:
						{
							// 拡大鏡の表示状態を切り替えます。
							::ShowWindow(loupe, ::IsWindowVisible(loupe) ? SW_HIDE : SW_SHOW);

							break;
						}
					}

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}
	} overview;
}
