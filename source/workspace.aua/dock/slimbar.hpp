#pragma once

namespace apn::workspace
{
	//
	// このクラスはスリムバーです。
	//
	struct SlimBar : my::Window, share::SlimBar
	{
		//
		// このクラスはスリムバーのボタンです。
		//
		struct Button { int ht; RECT rc, icon_rc; };

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
		Button buttons[4] = {
			{ HTCLOSE, },
			{ HTMAXBUTTON, },
			{ HTMINBUTTON, },
			{ HTSYSMENU, },
		};

		//
		// 仮想デストラクタです。
		//
		virtual ~SlimBar()
		{
		}

		//
		// スリムバーの設定をウィンドウに適用します。
		//
		BOOL apply_slimbar()
		{
			// スリムバーを使用する場合は
			if (hive.slimbar.flag_use)
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
#if 0
			return ::DrawMenuBar(hwnd);
#else
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
//				return ::SendMessage(hwnd, WM_NCPAINT, 0, 0), TRUE;
				return ::SendMessage(hwnd, WM_NCPAINT, (WPARAM)rgn.release(), 0), TRUE;
			}

			return FALSE;
#endif
		}

		//
		// WM_NCHITTESTを処理します。
		//
		LRESULT on_nc_hittest(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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
			auto result = __super::on_wnd_proc(hwnd, message, wParam, lParam);

			// メニュー内の場合は
			if (result == HTMENU)
			{
				// カーソル位置を取得します。
				auto pt = my::lp_to_pt(lParam);

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
		LRESULT on_nc_l_button_down(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			// ヒットテストコード取得します。
			auto ht = (int)wParam;

			// ヒットテストコードが無効の場合は何もしません。
			if (ht == HTNOWHERE)
				return __super::on_wnd_proc(hwnd, message, wParam, lParam);

			// ボタンをホバーしていない場合は何もしません。
			if (!is_hovering)
				return __super::on_wnd_proc(hwnd, message, wParam, lParam);

			// ヒットテストコードがシステムメニューの場合は
			if (ht == HTSYSMENU)
			{
				// システムメニューを表示します。
				// ※0x313は非公開メッセージです。
				return __super::on_wnd_proc(hwnd, 0x313, wParam, lParam);
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
				return __super::on_wnd_proc(hwnd, message, wParam, lParam);

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
								auto lParam = my::pt_to_lp(my::get_cursor_pos());

								switch (button.ht)
								{
								case HTCLOSE: ::PostMessage(hwnd, WM_SYSCOMMAND, SC_CLOSE, lParam); break;
								case HTMAXBUTTON:
									{
										::PostMessage(hwnd, WM_SYSCOMMAND,
											::IsZoomed(hwnd) ? SC_RESTORE : SC_MAXIMIZE, lParam);

										break;
									}
								case HTMINBUTTON: ::PostMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, lParam); break;
								case HTSYSMENU: ::PostMessage(hwnd, WM_NCLBUTTONDOWN, HTSYSMENU, lParam); break;
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
		LRESULT on_nc_r_button_down(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			// キャプションで右クリックが発生した場合は
			if (wParam == HTCAPTION)
			{
				// システムメニューを表示します。
				return __super::on_wnd_proc(hwnd, 0x313, wParam, lParam);
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		//
		// WM_NCMOUSELEAVEを処理します。
		//
		LRESULT on_nc_mouse_leave(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			// 直前のヒットテストコードを取得しておきます。
			auto prev_ht = current_ht;

			// ホバー中フラグをリセットします。
			current_ht = HTNOWHERE;
			is_hovering = FALSE;

			// スリムバーを再描画します。
			redraw(hwnd, current_ht, prev_ht);

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		//
		// WM_STYLECHANGEDを処理します。
		// キャプションを外したのでキャプションが存在するかのように偽装します。
		// これにより、キャプションなしでもスリムバーがテーマで描画されるようになります。
		//
		LRESULT on_style_changed(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
#if 0
			return 0; // デフォルト処理を省略するだけでも正常に動作するようです。
#else
			if (wParam == GWL_STYLE)
			{
				auto ss = (STYLESTRUCT*)lParam;
				ss->styleNew |= WS_CAPTION;
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
#endif
		}

		//
		// WM_GETMINMAXINFOを処理します。
		// キャプションを外したので最大化位置を手動で調整します。
		//
		LRESULT on_get_min_max_info(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			auto mmi = (MINMAXINFO*)lParam;
			auto rc = my::get_monitor_rect(hwnd);

			mmi->ptMaxSize.y = my::get_height(rc) - mmi->ptMaxPosition.y * 2; // 何故か2倍にすると丁度良くなります。

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		//
		// WM_SIZEを処理します。
		// スリムバーボタンの矩形を更新します。
		//
		LRESULT on_size(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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
			auto rc = my::get_window_rect(hwnd);

			// スリムバー矩形を取得します。
			auto bar_rc = get_bar_rc(hwnd);
			::OffsetRect(&bar_rc, -rc.left, -rc.top);

			// 一番右のボタンの矩形を算出します。
			auto button_rc = bar_rc;
			auto button_height = my::get_height(button_rc);
//			auto button_width = ::MulDiv(button_height, 2, 1);
//			auto button_width = ::MulDiv(button_height, 99, 70); // 高精度白銀比
//			auto button_width = ::MulDiv(button_height, 1618, 1000); // 黄金比
			auto button_width = ::MulDiv(button_height, 1732, 1000); // 白金比
//			auto button_width = ::MulDiv(button_height, 3303, 1000); // 青銅比
//			auto button_width = ::MulDiv(button_height, 1000 + 1414, 1000); // 1+白銀比
//			auto button_width = ::MulDiv(button_height, 1000 + 1618, 1000); // 1+黄金比
			button_rc.left = button_rc.right - button_width;

			// 一番右のボタンのアイコン矩形を算出します。
			auto icon_rc = button_rc;
			::InflateRect(&icon_rc, 0, -4);
			auto icon_size = my::get_height(icon_rc);
			icon_rc = centering(icon_rc, icon_size);

			// ボタンを走査します。
			for (auto& button : buttons)
			{
				// ボタン位置をセットします。
				button.rc = button_rc;
				button.icon_rc = icon_rc;

				// 次のボタン位置に移動します。
				::OffsetRect(&button_rc, -button_width, 0);
				::OffsetRect(&icon_rc, -button_width, 0);
			}

			// スリムバー全体を再描画します。
			redraw(hwnd);

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		//
		// WM_SETTEXTを処理します。
		// ウィンドウテキストが変更されるのでスリムバーを再描画します。
		//
		LRESULT on_set_text(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			// 現在のウィンドウテキストを取得しておきます。
			auto prev_text = my::get_window_text(hwnd);

			// スコープ終了時の処理です。
			my::scope_exit scope_exit([&]()
			{
				// ウィンドウテキストが更新された場合は
				if (prev_text != my::get_window_text(hwnd))
				{
					// スリムバー全体を再描画します。
					redraw(hwnd);
				}
			});

			// デフォルト処理を実行してウィンドウテキストを更新します。
			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		//
		// スリムバーを描画します。
		//
		BOOL on_draw(HWND hwnd, HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc)
		{
			// スリムバーを使用しない場合は何もしません。
			if (!hive.slimbar.flag_use) return FALSE;

			// スリムバー矩形を取得します。
			auto bar_rc = *rc;

			// スリムバーの中央にタイトルを描画します。
			{
				// ウィンドウテキストを取得します。
				auto text = my::get_window_text(hwnd);

				// タイトルの書式が指定されている場合は
				if (hive.slimbar.title_format.length())
				{
					// タイトルを書式化します。
					text = my::replace(hive.slimbar.title_format, L"%title%", text);
				}

				// タイトル描画用フォントをセットします。
				NONCLIENTMETRICS ncm = { sizeof(ncm) };
				::SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
				my::gdi::unique_ptr<HFONT> font(::CreateFontIndirect(&ncm.lfMenuFont));
				my::gdi::selector font_selector(dc, font.get());

				// タイトルを描画します。
				::DrawThemeText(theme, dc, part_id, state_id,
					text.c_str(), (int)text.length(),
					DT_CENTER | DT_VCENTER | DT_SINGLELINE, 0, &bar_rc);
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

				switch (button.ht)
				{
				case HTCLOSE:
					{
						// 閉じるボタンを描画します。
						::DrawThemeBackground(theme, dc, MENU_SYSTEMCLOSE, state_id, &button.icon_rc, nullptr);

						break;
					}
				case HTMAXBUTTON:
					{
						// 最大化されている場合は
						if (::IsZoomed(hwnd))
						{
							// 元に戻すボタンを描画します。
							::DrawThemeBackground(theme, dc, MENU_SYSTEMRESTORE, state_id, &button.icon_rc, nullptr);
						}
						// 最大化されていない場合は
						else
						{
							// 最大化ボタンを描画します。
							::DrawThemeBackground(theme, dc, MENU_SYSTEMMAXIMIZE, state_id, &button.icon_rc, nullptr);
						}

						break;
					}
				case HTMINBUTTON:
					{
						// 最小化ボタンを描画します。
						::DrawThemeBackground(theme, dc, MENU_SYSTEMMINIMIZE, state_id, &button.icon_rc, nullptr);

						break;
					}
				case HTSYSMENU:
					{
						// システムメニューアイコンを描画します。
						auto icon = (HICON)::GetClassLongPtr(hwnd, GCLP_HICON);
						auto icon_size = my::get_height(button.icon_rc);
						::DrawIconEx(dc, button.icon_rc.left, button.icon_rc.top,
							icon, icon_size, icon_size, 0, nullptr, DI_NORMAL);

						break;
					}
				}
			}

			return TRUE;
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/hex}, {/hex}", hwnd, my::message_to_string(message), wParam, lParam);

			// スリムバーを使用しない場合は何もしません。
			if (!hive.slimbar.flag_use)
				return __super::on_wnd_proc(hwnd, message, wParam, lParam);

			//
			// このクラスはデバッグ用のテキストを出力します。
			//
			struct ScopeText
			{
#ifdef _DEBUG
				std::wstring text;

				ScopeText(const std::wstring& text) : text(text)
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
			case WM_ACTIVATE:
				{
					ScopeText scope_text(my::message_to_string(message));

					return __super::on_wnd_proc(hwnd, message, wParam, lParam);
				}
			case WM_NCPAINT:
				{
					ScopeText scope_text(my::message_to_string(message));

					return __super::on_wnd_proc(hwnd, message, wParam, lParam);
				}
			case WM_NCACTIVATE:
				{
					ScopeText scope_text(my::message_to_string(message));

					return __super::on_wnd_proc(hwnd, message, wParam, lParam);
				}
			case WM_NCCALCSIZE:
				{
					ScopeText scope_text(my::message_to_string(message));

					return __super::on_wnd_proc(hwnd, message, wParam, lParam);
				}
			case WM_NCHITTEST:
				{
					ScopeText scope_text(my::message_to_string(message));

					return on_nc_hittest(hwnd, message, wParam, lParam);
				}
			case WM_NCLBUTTONDOWN:
				{
					ScopeText scope_text(my::message_to_string(message));

					return on_nc_l_button_down(hwnd, message, wParam, lParam);
				}
			case WM_NCRBUTTONDOWN:
				{
					ScopeText scope_text(my::message_to_string(message));

					return on_nc_r_button_down(hwnd, message, wParam, lParam);
				}
			case WM_NCMOUSEMOVE:
				{
					ScopeText scope_text(my::message_to_string(message));

					return __super::on_wnd_proc(hwnd, message, wParam, lParam);
				}
			case WM_NCMOUSELEAVE:
				{
					ScopeText scope_text(my::message_to_string(message));

					return on_nc_mouse_leave(hwnd, message, wParam, lParam);
				}
			case WM_STYLECHANGED:
				{
					ScopeText scope_text(my::message_to_string(message));

					return on_style_changed(hwnd, message, wParam, lParam);
				}
			case WM_GETMINMAXINFO:
				{
					ScopeText scope_text(my::message_to_string(message));

					return on_get_min_max_info(hwnd, message, wParam, lParam);
				}
			case WM_SIZE:
				{
					ScopeText scope_text(my::message_to_string(message));

					return on_size(hwnd, message, wParam, lParam);
				}
			case WM_SETTEXT:
				{
					ScopeText scope_text(my::message_to_string(message));

					return on_set_text(hwnd, message, wParam, lParam);
				}
			}

			if (message == c_message.c_draw)
			{
				if (auto context = (DrawContext*)lParam)
				{
					ScopeText scope_text(L"c_message.c_draw");

					return on_draw(hwnd, context->theme, context->dc, context->part_id, context->state_id, context->rc);
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	};
}
