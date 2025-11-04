#pragma once

namespace my
{
	//
	// このクラスはスリムバーです。
	//
	struct slimbar_t : my::WindowT<slimbar_t>
	{
		//
		// ウィンドウメッセージです。
		//
		inline static constexpr struct message_t {
			inline static const auto c_apply_config = ::RegisterWindowMessageW(L"uah::slimbar::apply_config");
			inline static const auto c_update_layout = ::RegisterWindowMessageW(L"uah::slimbar::update_layout");
		} c_message;

		//
		// このクラスはスリムバーの設定です。
		//
		inline static struct config_t
		{
			//
			// TRUEの場合はメニューバーをタイトルバーと一体化します。
			//
			BOOL flag_use = TRUE;

			//
			// TRUEの場合はタイトルをスリムバー全体の中央に描画します。
			//
			BOOL flag_whole_title = FALSE;

			//
			// スリムバー時のタイトルの書式です。
			//
			std::wstring title_format = L"%title%";

			//
			// ボタンの幅(%)です。
			//
			int32_t button_width = 200;
		} config;

		//
		// このクラスはスリムバーのボタンです。
		//
		struct button_t { int ht; RECT rc, icon_rc; };

		//
		// ホバーまたはクリックされているボタンです。
		//
		int current_ht = {};

		//
		// TRUEの場合はホバー中です。
		//
		BOOL is_hovering = {};

		//
		// ボタンの配列です。
		//
		button_t buttons[4] = {
			{ HTCLOSE, },
			{ HTMAXBUTTON, },
			{ HTMINBUTTON, },
			{ HTSYSMENU, },
		};

		//
		// タイトルの描画位置です。
		//
		RECT title_rc = {};

		//
		// TRUEの場合はアクティブなスリムバーを描画します。
		//
		BOOL is_active = {};

		//
		// スリムバーの設定をウィンドウに適用します。
		//
		BOOL apply_config()
		{
			// スリムバーを使用する場合は
			if (config.flag_use)
			{
				// ウィンドウスタイルからボーダーを削除します。
				my::modify_style(*this, WS_BORDER, 0);
			}
			// スリムバーを使用しない場合は
			else
			{
				// ウィンドウスタイルにキャプションを追加します。
				my::modify_style(*this, 0, WS_CAPTION);
			}

			// ウィンドウレイアウトを更新します。
			::SetWindowPos(*this, nullptr, 0, 0, 0, 0,
				SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

			// スリムバー全体を再描画します。
			return redraw(*this);
		}

		//
		// スリムバーのレイアウトを更新します。
		//
		BOOL update_layout()
		{
			//
			// この関数は指定された矩形の中央に位置する指定された幅の矩形を返します。
			//
			const auto centering = [](const RECT& base_rc, int width)
			{
				auto rc = base_rc;
				rc.left = (base_rc.left + base_rc.right - width) / 2;
				rc.right = rc.left + width;
				return rc;
			};

			// ウィンドウ矩形を取得します。
			auto window_rc = my::get_window_rect(hwnd);

			// スリムバー矩形を取得します。
			auto bar_rc = get_bar_rc(hwnd);
			::OffsetRect(&bar_rc, -window_rc.left, -window_rc.top);

			// タイトル矩形を初期化します。
			title_rc = bar_rc;

			// メニューを取得します。
			auto menu = ::GetMenu(hwnd);

			// メニュー項目の数を取得します。
			auto c = ::GetMenuItemCount(menu);

			// メニュー項目を走査します。
			for (decltype(c) i = 0; i < c; i++)
			{
				// メニュー項目の矩形を取得します。
				auto item_rc = RECT {};
				::GetMenuItemRect(hwnd, menu, i, &item_rc);
				::OffsetRect(&item_rc, -window_rc.left, -window_rc.top);

				// メニュー項目のタイプを取得します。
				MENUITEMINFO mii = { sizeof(mii) };
				mii.fMask = MIIM_FTYPE;
				::GetMenuItemInfo(menu, i, TRUE, &mii);

				// メニュー項目がメニューバーの右側にある場合は
				if (mii.fType & MFT_RIGHTJUSTIFY)
				{
					// タイトル矩形の右端を更新します。
					title_rc.right = item_rc.left;

					// ループを終了します。
					break;
				}
				// メニュー項目がメニューバーの左側にある場合は
				else
				{
					// タイトル矩形の左端を更新します。
					title_rc.left = item_rc.right;
				}
			}

			// ボタンのサイズを算出します。
			auto button_height = my::get_height(title_rc);
			auto button_width = ::MulDiv(button_height, config.button_width, 100);

			// 一番右のボタンの矩形を算出します。
			auto button_rc = title_rc;
			button_rc.left = button_rc.right - button_width;

			// 一番右のボタンのアイコン矩形を算出します。
			auto icon_rc = button_rc;
			::InflateRect(&icon_rc, 0, -4);
			auto icon_size = my::get_height(icon_rc);
			icon_rc = centering(icon_rc, icon_size);

			// ボタンを走査します。
			for (auto& button : buttons)
			{
				// タイトル矩形の右端を更新します。
				title_rc.right = button_rc.left;

				// ボタン位置をセットします。
				button.rc = button_rc;
				button.icon_rc = icon_rc;

				// 次のボタン位置に移動します。
				::OffsetRect(&button_rc, -button_width, 0);
				::OffsetRect(&icon_rc, -button_width, 0);
			}

			// スリムバー全体を再描画します。
			return redraw(*this);
		}

		//
		// スリムバー矩形を返します。
		//
		RECT get_bar_rc(HWND hwnd)
		{
			MENUBARINFO mbi = { sizeof(mbi) };
			::GetMenuBarInfo(hwnd, OBJID_MENU, 0, &mbi);
			return mbi.rcBar;
		}

		//
		// ヒットテストコードを返します。
		// ptはウィンドウ座標で指定します。
		//
		int hittest(POINT pt)
		{
			// スリムバーボタンを走査します。
			for (const auto& button : buttons)
			{
				// スリムバーボタン内の場合は
				if (::PtInRect(&button.rc, pt))
				{
					// スリムバーボタンのヒットテストコードを返します。
					return button.ht;
				}
			}

			return HTNOWHERE;
		}

		//
		// スリムバーを再描画します。
		//
		BOOL redraw(HWND hwnd)
		{
			return ::DrawMenuBar(hwnd);
		}

		//
		// スリムバーを再描画します。
		//
		BOOL redraw(HWND hwnd, auto... _hts)
		{
			// 再描画対象のヒットテストコードです。
			const int hts[] = { _hts... };

			// 再描画対象のリージョンです。
			my::gdi::unique_ptr<HRGN> rgn(
				::CreateRectRgn(0, 0, 0, 0));

			// 再描画フラグです。
			auto redraw = BOOL {};

			// ヒットテストコードを走査します。
			for (auto ht : hts)
			{
				// スリムバーボタンを走査します。
				for (const auto& button : buttons)
				{
					// ヒットテストコードが等しい場合は
					if (button.ht == ht)
					{
						// スリムバーボタンリージョンを作成します。
						my::gdi::unique_ptr<HRGN> button_rgn(
							::CreateRectRgnIndirect(&button.rc));

						// スリムバーボタンリージョンを結合します。
						::CombineRgn(rgn.get(), rgn.get(), button_rgn.get(), RGN_OR);

						// 再描画フラグをセットします。
						redraw = TRUE;

						break;
					}
				}
			}

			// リージョンをスクリーン座標に変換します。
			{
				auto rc = my::get_window_rect(hwnd);

				::OffsetRgn(rgn.get(), rc.left, rc.top);
			}

			// 再描画が必要な場合は
			if (redraw)
			{
				// NC領域を再描画します。
				return ::SendMessage(hwnd, WM_NCPAINT, (WPARAM)rgn.release(), 0), TRUE;
			}

			return FALSE;
		}

		//
		// WM_NCHITTESTを処理します。
		//
		LRESULT on_nc_hittest(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			// スコープ終了時にスリムバーを再描画するようにします。
			my::scope_exit scope_exit([&, prev_ht = current_ht]()
			{
				// 直前のヒットテストコードと異なる場合は
				if (current_ht != prev_ht)
				{
					// スリムバーを再描画します。
					redraw(hwnd, current_ht, prev_ht);
				}
			});

			// ホバー中フラグをリセットします。
			current_ht = HTNOWHERE;
			is_hovering = FALSE;

			// まず、デフォルト処理を実行します。
			auto result = __super::on_wnd_proc(hwnd, message, w_param, l_param);

			// メニュー内の場合は
			if (result == HTMENU)
			{
				// カーソル位置を取得します。
				auto pt = my::lp_to_pt(l_param);

				// メニューを取得します。
				auto menu = ::GetMenu(hwnd);

				// メニュー項目の数を取得します。
				auto c = ::GetMenuItemCount(menu);

				// メニュー項目を走査します。
				for (decltype(c) i = 0; i < c; i++)
				{
					// メニュー項目の矩形を取得します。
					auto rc = RECT {};
					::GetMenuItemRect(hwnd, menu, i, &rc);

					// メニュー項目内の場合はデフォルト処理を実行します。
					if (::PtInRect(&rc, pt)) return result;
				}

				// ウィンドウ矩形を取得します。
				auto rc = my::get_window_rect(hwnd);

				// ヒットテストコードを取得できた場合は
				if (auto ht = hittest(pt - my::get_near(rc)))
				{
					// ホバー中フラグをセットします。
					current_ht = ht;
					is_hovering = TRUE;
#if 1
					// タイマーをセットします。
					::SetTimer(hwnd, 0, 50,
						[](HWND hwnd, UINT message, UINT_PTR timer_id, DWORD time)
					{
						// ウィンドウ矩形を取得します。
						auto rc = my::get_window_rect(hwnd);

						// カーソル位置を取得します。
						auto pt = my::get_cursor_pos();

						// マウスカーソルがウィンドウ外に移動した場合は
						if (!::PtInRect(&rc, pt))
						{
							// タイマーをリセットします。
							::KillTimer(hwnd, timer_id);

							// WM_NCMOUSELEAVEをポストします。
							::PostMessage(hwnd, WM_NCMOUSELEAVE, 0, 0);
						}
					});
#else
					// WM_NCMOUSELEAVEをトラックします。
					my::track_mouse_event(hwnd, TME_LEAVE | TME_NONCLIENT);
#endif
					return ht; // ヒットテストコードを返します。
				}

				// それ以外の場合はキャプションののヒットテストコードを返します。
				return HTCAPTION;
			}

			return result;
		}

		//
		// WM_NCLBUTTONDOWNを処理します。
		//
		LRESULT on_nc_l_button_down(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			// ヒットテストコード取得します。
			auto ht = (int)w_param;

			// ヒットテストコードが無効の場合は何もしません。
			if (ht == HTNOWHERE)
				return __super::on_wnd_proc(hwnd, message, w_param, l_param);

			// ボタンをホバーしていない場合は何もしません。
			if (!is_hovering)
				return __super::on_wnd_proc(hwnd, message, w_param, l_param);

			// ヒットテストコードがシステムメニューの場合は
			if (ht == HTSYSMENU)
			{
				// システムメニューを表示します。
				// ※0x313は非公開メッセージです。
				return __super::on_wnd_proc(hwnd, 0x313, w_param, l_param);
			}

			// ヒットテストコードからボタンのインデックスを取得します。
			auto i = [&]()
			{
				// スリムバーボタンを走査します。
				for (size_t i = 0; i < std::size(buttons); i++)
				{
					// スリムバーボタンを取得します。
					const auto& button = buttons[i];

					// ヒットテストコードが等しい場合はインデックスを返します。
					if (button.ht == ht) return i;
				}

				// ボタンが見つからなかった場合は無効なインデックスを返します。
				return std::size(buttons);
			}();

			// ボタンのインデックスが無効の場合は何もしません。
			if (i >= std::size(buttons))
				return __super::on_wnd_proc(hwnd, message, w_param, l_param);

			// スリムバーボタンを取得します。
			const auto& button = buttons[i];

			// ドラッグ中のメッセージループを処理します。
			[&]()
			{
				// マウスキャプチャを開始します。
				::SetCapture(hwnd);

				// メッセージループを開始します。
				MSG msg = {};
				while (::GetMessage(&msg, nullptr, 0, 0) > 0)
				{
					// マウスキャプチャが解除されている場合はループを終了します。
					if (::GetCapture() != hwnd) break;

					switch (msg.message)
					{
					case WM_MOUSEMOVE:
						{
							// カーソル位置を取得します。
							auto pt = my::lp_to_pt(msg.lParam);
							my::map_window_points(hwnd, nullptr, &pt);
							pt = pt - my::get_near(my::get_window_rect(hwnd));

							// ホバー状態が異なる場合は
							if (is_hovering != ::PtInRect(&button.rc, pt))
							{
								// ホバー状態を更新します。
								is_hovering = !is_hovering;

								// スリムバーを再描画します。
								redraw(hwnd, current_ht);
							}

							break;
						}
					case WM_LBUTTONUP:
						{
							// マウスキャプチャを終了します。
							::ReleaseCapture();

							// カーソル位置を取得します。
							auto pt = my::lp_to_pt(msg.lParam);
							my::map_window_points(hwnd, nullptr, &pt);
							pt = pt - my::get_near(my::get_window_rect(hwnd));

							// ホバー状態の場合はボタンコマンドを実行します。
							if (is_hovering = ::PtInRect(&button.rc, pt))
							{
								// カーソル位置を取得します。
								auto l_param = my::pt_to_lp(my::get_cursor_pos());

								switch (button.ht)
								{
								case HTCLOSE: ::PostMessage(hwnd, WM_SYSCOMMAND, SC_CLOSE, l_param); break;
								case HTMAXBUTTON:
									{
										::PostMessage(hwnd, WM_SYSCOMMAND,
											::IsZoomed(hwnd) ? SC_RESTORE : SC_MAXIMIZE, l_param);

										break;
									}
								case HTMINBUTTON: ::PostMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, l_param); break;
								case HTSYSMENU: ::PostMessage(hwnd, WM_NCLBUTTONDOWN, HTSYSMENU, l_param); break;
								}
							}

							return;
						}
					}
				}
			} ();

			// スリムバーを再描画します。
			redraw(hwnd, ht);

			// この時点で左クリックは処理済みなので、
			// デフォルト処理が実行されないようにします。
			return 0;
		}

		//
		// WM_NCRBUTTONDOWNを処理します。
		//
		LRESULT on_nc_r_button_down(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			// キャプションで右クリックが発生した場合は
			if (w_param == HTCAPTION)
			{
				// システムメニューを表示します。
				return __super::on_wnd_proc(hwnd, 0x313, w_param, l_param);
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}

		//
		// WM_NCMOUSELEAVEを処理します。
		//
		LRESULT on_nc_mouse_leave(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			// 直前のヒットテストコードを取得しておきます。
			auto prev_ht = current_ht;

			// ホバー中フラグをリセットします。
			current_ht = HTNOWHERE;
			is_hovering = FALSE;

			// スリムバーを再描画します。
			redraw(hwnd, current_ht, prev_ht);

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}

		//
		// WM_STYLECHANGEDを処理します。
		// キャプションを外したのでキャプションが存在するかのように偽装します。
		// これにより、キャプションなしでもスリムバーがテーマで描画されるようになります。
		//
		LRESULT on_style_changed(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
#if 1
			return 0; // デフォルト処理を省略するだけでも正常に動作するようです。
#else
			if (w_param == GWL_STYLE)
			{
				auto ss = (STYLESTRUCT*)l_param;
				ss->styleNew |= WS_CAPTION;
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
#endif
		}

		//
		// WM_WINDOWPOSCHANGINGを処理します。
		// キャプションを外したので最大化位置を手動で調整します。
		//
		LRESULT on_window_pos_changing(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			// 引数を取得します。
			auto wp = (WINDOWPOS*)l_param;

			// ウィンドウが最大化される場合は
			if (!(wp->flags & SWP_NOMOVE) && !(wp->flags & SWP_NOSIZE) && ::IsZoomed(hwnd))
			{
#if 1
				// マウスカーソルが存在するモニタを取得します。
				auto monitor = ::MonitorFromPoint(my::get_cursor_pos(), MONITOR_DEFAULTTONEAREST);
#else
				// ウィンドウが存在するモニタを取得します。
				auto monitor = ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
#endif
				// モニタ情報を取得します。
				MONITORINFOEX mi = { sizeof(mi) };
				::GetMonitorInfo(monitor, &mi);

				// ワークエリア矩形を取得します。
				auto rc = mi.rcWork;

				// 最大化位置になるように矩形を広げます。
				::InflateRect(&rc, rc.left - wp->x, rc.top - wp->y);

				// 算出した矩形をウィンドウ位置に設定します。
				wp->x = rc.left;
				wp->y = rc.top;
				wp->cx = my::get_width(rc);
				wp->cy = my::get_height(rc);
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}

		//
		// WM_SIZEを処理します。
		// スリムバーボタンの矩形を更新します。
		//
		LRESULT on_size(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			// スリムバーのレイアウトを更新します。
			update_layout();

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}

		//
		// WM_SETTEXTを処理します。
		// ウィンドウテキストが変更されるのでスリムバーを再描画します。
		//
		LRESULT on_set_text(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			// スコープ終了時にスリムバーを再描画するようにします。
			my::scope_exit scope_exit([&]()
			{
				// ウィンドウテキストが更新されたので
				// スリムバー全体を再描画します。
				redraw(hwnd);
			});

			// デフォルト処理を実行してウィンドウテキストを更新します。
			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}

		//
		// WM_UAHDRAWMENUを処理します。
		// メニューバーを描画した後にスリムバーを描画します。
		//
		LRESULT on_uah_draw_menu(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			// デフォルト処理の後に実行します。
			my::scope_exit scope_exit([&]()
			{
				// 引数を取得します。
				auto uah_menu = (UAHMENU*)l_param;

				// テーマを取得します。
				my::theme::unique_ptr<> theme(::OpenThemeData(hwnd, VSCLASS_MENU));
				auto part_id = MENU_BARBACKGROUND;
				auto state_id = is_active ? MB_ACTIVE : MB_INACTIVE;

				// ウィンドウ矩形を取得します。
				auto window_rc = my::get_window_rect(hwnd);

				// スリムバー矩形を取得します。
				auto bar_rc = get_bar_rc(hwnd);
				::OffsetRect(&bar_rc, -window_rc.left, -window_rc.top);

				// スリムバーを描画します。
				return on_draw(hwnd, theme.get(), uah_menu->hdc, part_id, state_id, &bar_rc);
			});

			// デフォルト処理を実行します。
			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}

		//
		// スリムバーを描画します。
		//
		BOOL on_draw(HWND hwnd, HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc)
		{
			// スリムバー矩形を取得します。
			auto bar_rc = *rc;

			// スリムバーの中央にタイトルを描画します。
			{
				// タイトル描画位置の矩形を取得します。
				auto text_rc = bar_rc;

				// タイトルをスリムバー全体の中央に描画する場合は
				if (config.flag_whole_title)
				{
					// 縮小量を算出します。
					auto deflate = std::max(title_rc.left - bar_rc.left, bar_rc.right - title_rc.right);

					// メニュー項目と干渉しないように描画範囲を縮小します。
					::InflateRect(&text_rc, -deflate, 0);
				}
				// それ以外の場合は
				else
				{
					// タイトル矩形のスペースに描画します。
					text_rc = title_rc;
				}

				// ウィンドウテキストを取得します。
				auto text = my::get_window_text(hwnd);

				// タイトルの書式が指定されている場合は
				if (config.title_format.length())
				{
					// タイトルを書式化します。
					text = my::replace(config.title_format, L"%title%", text);
				}

				// タイトル描画用フォントをセットします。
				NONCLIENTMETRICS ncm = { sizeof(ncm) };
				::SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
				my::gdi::unique_ptr<HFONT> font(::CreateFontIndirect(&ncm.lfMenuFont));
				my::gdi::selector font_selector(dc, font.get());

				// タイトルを描画します。
				::DrawThemeText(theme, dc, part_id, state_id,
					text.c_str(), (int)text.length(),
					DT_CENTER | DT_VCENTER | DT_SINGLELINE |
					DT_END_ELLIPSIS | DT_NOPREFIX | DT_NOCLIP, 0, &text_rc);
			}

			// スリムバーボタンを走査します。
			for (const auto& button : buttons)
			{
				// ホット状態を取得します。
				auto is_hot = is_hovering && current_ht == button.ht;

				// ホット状態の場合は
				if (is_hot)
				{
					// 背景を描画します。
					::DrawThemeBackground(theme, dc, MENU_BARITEM, MBI_HOT, &button.rc, nullptr);
				}

				// アイコン矩形を取得します。
				auto icon_rc = button.icon_rc;
				::InflateRect(&icon_rc, 2, 2);

				// アイコンのパートIDです。
				auto part_id = 0;

				switch (button.ht)
				{
				case HTCLOSE: part_id = MENU_SYSTEMCLOSE; break;
				case HTMAXBUTTON: part_id = ::IsZoomed(hwnd) ? MENU_SYSTEMRESTORE : MENU_SYSTEMMAXIMIZE; break;
				case HTMINBUTTON: part_id = MENU_SYSTEMMINIMIZE; break;
				case HTSYSMENU:
					{
						// システムメニューアイコンを描画します。
						auto icon = (HICON)::GetClassLongPtr(hwnd, GCLP_HICON);
						auto icon_size = my::get_height(icon_rc);
						::DrawIconEx(dc, icon_rc.left, icon_rc.top,
							icon, icon_size, icon_size, 0, nullptr, DI_NORMAL);

						break;
					}
				}

				// アイコンのパートIDが有効の場合は
				if (part_id)
				{
					// テーマを使用してアイコンを描画します。
					::DrawThemeBackground(theme, dc, part_id, state_id, &icon_rc, nullptr);
				}
			}

			return TRUE;
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/hex}, {/hex}", hwnd, my::message_to_string(message), w_param, l_param);

			//
			// このクラスはデバッグ用のテキストを出力します。
			//
			struct ScopeText
			{
#ifdef _DEBUG
				LPCWSTR text;

				ScopeText(LPCWSTR text) : text(text)
				{
					MY_TRACE("*** {/} を開始します ***\n", text);
				}

				~ScopeText()
				{
					MY_TRACE("*** {/} を終了します ***\n", text);
				}
#else
				ScopeText(LPCWSTR text) {}
				~ScopeText() {}
#endif
			};

			switch (message)
			{
			case WM_NCACTIVATE:
				{
					ScopeText scope_text(L"WM_NCACTIVATE");

					// アクティブ状態を取得しておきます。
					is_active = !!w_param;

					return __super::on_wnd_proc(hwnd, message, w_param, l_param);
				}
			}

			if (message == c_message.c_apply_config)
			{
				ScopeText scope_text(L"c_message.c_apply_config");

				return apply_config();
			}

			// スリムバーを使用しない場合は以下のメッセージは処理しません。
			if (!config.flag_use)
				return __super::on_wnd_proc(hwnd, message, w_param, l_param);

			switch (message)
			{
			case WM_ACTIVATE:
				{
					ScopeText scope_text(L"WM_ACTIVATE");

					return __super::on_wnd_proc(hwnd, message, w_param, l_param);
				}
			case WM_NCPAINT:
				{
					ScopeText scope_text(L"WM_NCPAINT");

					return __super::on_wnd_proc(hwnd, message, w_param, l_param);
				}
			case WM_NCCALCSIZE:
				{
					ScopeText scope_text(L"WM_NCCALCSIZE");

					return __super::on_wnd_proc(hwnd, message, w_param, l_param);
				}
			case WM_NCHITTEST:
				{
					ScopeText scope_text(L"WM_NCHITTEST");

					return on_nc_hittest(hwnd, message, w_param, l_param);
				}
			case WM_NCLBUTTONDOWN:
				{
					ScopeText scope_text(L"WM_NCLBUTTONDOWN");

					return on_nc_l_button_down(hwnd, message, w_param, l_param);
				}
			case WM_NCRBUTTONDOWN:
				{
					ScopeText scope_text(L"WM_NCRBUTTONDOWN");

					return on_nc_r_button_down(hwnd, message, w_param, l_param);
				}
			case WM_NCMOUSEMOVE:
				{
					ScopeText scope_text(L"WM_NCMOUSEMOVE");

					return __super::on_wnd_proc(hwnd, message, w_param, l_param);
				}
			case WM_NCMOUSELEAVE:
				{
					ScopeText scope_text(L"WM_NCMOUSELEAVE");

					return on_nc_mouse_leave(hwnd, message, w_param, l_param);
				}
			case WM_STYLECHANGED:
				{
					ScopeText scope_text(L"WM_STYLECHANGED");

					return on_style_changed(hwnd, message, w_param, l_param);
				}
			case WM_WINDOWPOSCHANGING:
				{
					ScopeText scope_text(L"WM_WINDOWPOSCHANGING");

					return on_window_pos_changing(hwnd, message, w_param, l_param);
				}
			case WM_SIZE:
				{
					ScopeText scope_text(L"WM_SIZE");

					return on_size(hwnd, message, w_param, l_param);
				}
			case WM_SETTEXT:
				{
					ScopeText scope_text(L"WM_SETTEXT");

					return on_set_text(hwnd, message, w_param, l_param);
				}
			case WM_UAHDRAWMENU:
				{
					ScopeText scope_text(L"WM_UAHDRAWMENU");

					return on_uah_draw_menu(hwnd, message, w_param, l_param);
				}
			}

			if (message == c_message.c_update_layout)
			{
				ScopeText scope_text(L"c_message.c_update_layout");

				return update_layout();
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}
	};
}
