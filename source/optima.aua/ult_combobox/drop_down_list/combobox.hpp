#pragma once

namespace apn::ult_combobox::drop_down_list
{
	//
	// このクラスは自作コンボボックスです。
	//
	struct ComboBox : Core, std::enable_shared_from_this<ComboBox>
	{
		//
		// テーマハンドルです。
		//
		my::theme::unique_ptr<> theme = {};

		//
		// マウスホバーしているときはTRUEになります。
		//
		BOOL is_hover = {};

		//
		// コンストラクタです。
		//
		ComboBox()
		{
			MY_TRACE_FUNC("");
		}

		//
		// デストラクタです。
		//
		virtual ~ComboBox() override
		{
			MY_TRACE_FUNC("");
		}

		//
		// ドロップダウンリストボックスの表示状態を変更します。
		//
		BOOL show_drop_down(BOOL show, BOOL cancel = FALSE)
		{
			MY_TRACE_FUNC("{/}, {/}", show, cancel);

			if (show)
			{
				// リストボックスの矩形を取得します。
				auto animation_flags = DWORD {};
				auto rc = RECT {}; get_dropped_control_rect(&rc, &animation_flags);

				// リストボックスを表示します。
				return popup::listbox->show(shared_from_this(), &rc, animation_flags);
			}
			else
			{
				// リストボックスを非表示にします。
				return popup::listbox->hide(
					cancel ? CB_ERR : popup::listbox->selected_index);
			}
		}

		//
		// ドロップダウンリストボックスの表示状態を返します。
		//
		BOOL get_dropped_state()
		{
			return ::IsWindowVisible(*popup::listbox);
		}

		//
		// ドロップダウンリストボックスを水平方向にスクロールできる幅を返します。
		//
		int get_horizontal_extent()
		{
			return horizontal_extent;
		}

		//
		// ドロップダウンリストボックスを水平方向にスクロールできる幅をセットします。
		//
		int set_horizontal_extent(int width)
		{
			if (width < 0) return CB_ERR;

			return horizontal_extent = width;
		}

		//
		// アイテムの高さを返します。
		//
		int get_item_height(int index)
		{
			if (index == -1)
			{
				return body_height;
			}
			else
			{
				return __super::get_item_height(index);
			}
		}

		//
		// アイテムの高さをセットします。
		//
		int set_item_height(int index, int height)
		{
			if (index == -1)
			{
				return body_height = height;
			}
			else
			{
				return __super::set_item_height(index, height);
			}
		}

		//
		// 本体の高さを算出します。
		//
		void calc_body_height()
		{
			my::ClientDC dc(*this);
			my::gdi::selector font_selector(dc, font);

			TEXTMETRICW tm = {};
			::GetTextMetricsW(dc, &tm);

			body_height = tm.tmHeight + tm.tmInternalLeading + tm.tmExternalLeading;
			item_height = tm.tmHeight;
		}

		//
		// ドロップダウンリストボックスの高さを算出します。
		//
		void calc_dropped_height(int cy)
		{
			// ドロップダウンリストボックスの高さを算出します。
			dropped_height = std::max(cy - body_height, 0);
		}

		//
		// リストボックスの矩形を返します。
		//
		BOOL get_dropped_control_rect(LPRECT rc, DWORD* ret_animation_flags)
		{
			// アニメーションフラグです。
			auto animation_flags = DWORD {};

			// スコープ終了時にアニメーションフラグをセットします。
			my::scope_exit scope_exit([&]()
			{
				if (ret_animation_flags)
					*ret_animation_flags = animation_flags | AW_SLIDE;
			});

			// 項目の高さが無効の場合は失敗します。
			if (item_height <= 0) return *rc = {}, FALSE;

			// 項目の数を取得します。
			auto item_count = std::max(dropped_height / item_height, 1);
			if (items.size()) item_count = std::min(item_count, (int)items.size());

			auto combobox_rc = my::get_window_rect(*this);
			auto listbox_client_rc = my::get_client_rect(*popup::listbox);
			auto listbox_window_rc = my::get_window_rect(*popup::listbox);
			auto border_size = SIZE {
				my::get_width(listbox_window_rc) - my::get_width(listbox_client_rc),
				my::get_height(listbox_window_rc) - my::get_height(listbox_client_rc),
			};
			auto x = combobox_rc.left;
			auto y = combobox_rc.bottom;
			auto w = my::get_width(combobox_rc);
			auto h = (item_count * item_height) + border_size.cy;

			if (dropped_width > 0)
				w = std::max<int>(w, dropped_width + border_size.cx);

			auto is_horz_available = FALSE;
			auto monitor_rc = my::get_monitor_rect(*this);

			// リストボックスのボトムがモニタのボトムからはみ出す場合は
			if (combobox_rc.bottom + h > monitor_rc.bottom)
			{
				// リストボックスのトップがモニタのトップからはみ出す場合は
				if (combobox_rc.top - h < monitor_rc.top)
				{
					h = std::min<int>(h, my::get_height(monitor_rc));

					// 可能な限りコンボボックスの真横に配置します。
					y = my::get_center_y(combobox_rc) - h / 2;
					y = std::max<int>(y, monitor_rc.top);
					y = std::min<int>(y, monitor_rc.bottom - h);

					// リストボックスのレフトがモニタのレフトからはみ出す場合は
					if (combobox_rc.left - w < monitor_rc.left)
					{
						// リストボックスをコンボボックスの右側に配置します。
						x = combobox_rc.right;
						w = std::min<int>(w, monitor_rc.right - x);
						animation_flags = AW_HOR_POSITIVE;
					}
					else
					{
						// リストボックスをコンボボックスの左側に配置します。
						x = combobox_rc.left - w;
						animation_flags = AW_HOR_NEGATIVE;
					}

					// xとwをセットしたのでフラグを立てます。
					is_horz_available = TRUE;
				}
				else
				{
					// リストボックスをコンボボックスの上側に配置します。
					y = combobox_rc.top - h;
					animation_flags = AW_VER_NEGATIVE;
				}
			}
			else
			{
				// リストボックスをコンボボックスの下側に配置します。
				y = combobox_rc.bottom;
				animation_flags = AW_VER_POSITIVE;
			}

			// xとwがまだセットされていない場合は
			if (!is_horz_available)
			{
				// リストボックスの幅がモニタの幅を超えないように調整します。
				w = std::min<int>(w, my::get_width(monitor_rc));

				// リストボックスのレフトがモニタのレフトからはみ出す場合は
				if (combobox_rc.left < monitor_rc.left)
				{
					// リストボックスのレフトがモニタのレフトと一致するように配置します。
					x = monitor_rc.left;
				}
				// リストボックスのライトがモニタのライトからはみ出す場合は
				else if (combobox_rc.left + w > monitor_rc.right)
				{
					// リストボックスのライトがモニタのライトと一致するように配置します。
					x = monitor_rc.right - w;
				}
				else
				{
					// リストボックスのレフトがコンボボックスのレフトと一致するように配置します。
					x = combobox_rc.left;
				}
			}

			rc->left = x;
			rc->top = y;
			rc->right = x + w;
			rc->bottom = y + h;

			return TRUE;
		}

		//
		// WM_PAINTを処理します。
		//
		LRESULT on_paint(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			// デバイスコンテキストを作成します。

			my::PaintDC pdc(hwnd);
			auto rc = my::get_client_rect(hwnd);
			my::DoubleBufferDC dc(pdc, &rc);
			my::gdi::selector font_selector(dc, font);

			// 親ウィンドウからブラシを取得します。
			auto edit_brush = fire_ctl_color(hwnd, dc, WM_CTLCOLOREDIT);
			auto static_brush = fire_ctl_color(hwnd, dc, WM_CTLCOLORSTATIC);

			// 背景を描画します。
			::FillRect(dc, &rc, static_brush);

			// ステートを取得します。

			auto body_state_id = CBXS_NORMAL;
			auto button_state_id = CBXS_NORMAL;

			if (!::IsWindowEnabled(hwnd))
			{
				body_state_id = CBXS_DISABLED;
				button_state_id = CBXS_DISABLED;
			}
			else if (is_hover)
			{
				body_state_id = CBXS_HOT;
//				button_state_id = CBXS_HOT; // ボタンに縁が付きます。
			}

			// 寸法を算出します。

//			TEXTMETRICW tm = {};
//			::GetTextMetricsW(dc, &tm);
			auto right_drop_down = rc.right - body_height;

			auto drop_down_rc = rc;
			drop_down_rc.left = right_drop_down;

			auto text_rc = rc;
			text_rc.right = right_drop_down;
			::InflateRect(&text_rc, -c_padding, -c_padding);

			auto text = std::wstring {};
			if ((size_t)current_index < items.size())
				text = items[current_index]->text;

			// テーマを使用して描画します。

			::DrawThemeBackground(theme.get(), dc, CP_READONLY, body_state_id, &rc, nullptr);
			::DrawThemeBackground(theme.get(), dc, CP_DROPDOWNBUTTONRIGHT, button_state_id, &drop_down_rc, nullptr);
			::DrawThemeText(theme.get(), dc, CP_READONLY, body_state_id, text.c_str(), (int)text.length(), DT_SINGLELINE | DT_NOPREFIX, 0, &text_rc);

			if (hwnd == ::GetFocus())
			{
				auto focus_rc = text_rc;
#if 1
				// フォーカス矩形を少しだけ広げます。
				::InflateRect(&focus_rc, 1, 1);
#else
				// フォーカス矩形を広げます。
				::InflateRect(&focus_rc, c_padding / 2, c_padding / 2);
#endif

#if 1
				// 破線を描画するためのコスメティックペンを作成します。
				LOGBRUSH lb = {};
				my::gdi::unique_ptr<HPEN> pen(::ExtCreatePen(PS_COSMETIC | PS_DOT, 1, &lb, 0, 0));
				my::gdi::selector pen_selector(dc, pen.get());
				my::gdi::selector brush_selector(dc, ::GetStockObject(NULL_BRUSH));

				// 破線を反転描画します。
				auto old_rop2 = ::SetROP2(dc, R2_NOT);
				::Rectangle(dc, focus_rc.left, focus_rc.top, focus_rc.right, focus_rc.bottom);
				::SetROP2(dc, old_rop2);
#else
				// このAPIでは破線になりません。
				::DrawFocusRect(dc, &focus_rc);
#endif
			}

			return 0;
		}

		//
		// WM_LBUTTONDOWNを処理します。
		//
		LRESULT on_l_button_down(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			// フォーカスをセットします。
			::SetFocus(hwnd);

			// リストボックスを表示します。
			show_drop_down(TRUE);

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}

		//
		// WM_MOUSEMOVEを処理します。
		//
		LRESULT on_mouse_move(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			// マウスホバー状態ではない場合は
			if (!is_hover)
			{
				// マウスリーブイベントをハンドルできるようにします。
				my::track_mouse_event(hwnd);

				// マウスホバーのフラグをセットします。
				is_hover = TRUE;

				// 再描画します。
				redraw();
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}

		//
		// WM_MOUSELEAVEを処理します。
		//
		LRESULT on_mouse_leave(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			// マウスホバーのフラグをリセットします。
			is_hover = FALSE;

			// 再描画します。
			redraw();

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}

		//
		// WM_MOUSEWHEELを処理します。
		//
		LRESULT on_mouse_wheel(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			// 現在の選択項目を取得しておきます。
			auto current_index = get_cur_sel();

			// 引数を取得します。
			auto delta = (short)HIWORD(w_param);

			// 選択項目を変更します。
			set_cur_sel(get_cur_sel() + ((delta > 0) ? -1 : +1));

			// 選択項目が変更された場合は
			if (get_cur_sel() != current_index)
			{
				// 親ウィンドウに通知コードを送信します。
				fire_command(hwnd, CBN_SELCHANGE);
			}

			return 0; // 親ウィンドウに処理させないようにします。
		}

		//
		// WM_KEYDOWNを処理します。
		//
		LRESULT on_key_down(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			// ドロップダウン状態の場合は
			if (get_dropped_state())
			{
				// 矢印キーでリストボックスの選択項目を変更します。
				// EnterまたはEscキーでリストボックスを非表示にします。

				switch (w_param)
				{
				case VK_LEFT:
				case VK_UP: popup::listbox->relative_select(-1); break;
				case VK_RIGHT:
				case VK_DOWN: popup::listbox->relative_select(+1); break;
				case VK_RETURN: show_drop_down(FALSE); break;
				case VK_ESCAPE: show_drop_down(FALSE, TRUE); break;
				}
			}
			// ドロップダウン状態ではない場合は
			else
			{
				// 矢印キーで選択項目を変更します。

				auto current_index = get_cur_sel();

				switch (w_param)
				{
				case VK_LEFT:
				case VK_UP: set_cur_sel(current_index - 1); break;
				case VK_RIGHT:
				case VK_DOWN: set_cur_sel(current_index + 1); break;
				}

				if (get_cur_sel() != current_index)
					fire_command(hwnd, CBN_SELCHANGE);
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}

		//
		// WM_KEYUPを処理します。
		//
		LRESULT on_key_up(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			// ドロップダウン状態の場合は
			if (get_dropped_state())
			{
				// EnterまたはEscキーでリストボックスを非表示にします。

				switch (w_param)
				{
				case VK_RETURN: show_drop_down(FALSE); break;
				case VK_ESCAPE: show_drop_down(FALSE, TRUE); break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}

		//
		// WM_SYSKEYDOWNを処理します。
		//
		LRESULT on_sys_key_down(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			switch (w_param)
			{
			case VK_LEFT:
			case VK_UP: show_drop_down(FALSE); break;
			case VK_RIGHT:
			case VK_DOWN: show_drop_down(TRUE); break;
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}

		//
		// 再描画します。
		//
		virtual BOOL redraw() override
		{
			return ::InvalidateRect(hwnd, nullptr, FALSE);
		}

		//
		// ウィンドウを作成します。
		//
		inline static std::shared_ptr<ComboBox> create(DWORD ex_style, LPCTSTR class_name, LPCTSTR window_name,
			DWORD style, int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param)
		{
			// インスタンスを作成します。
			auto p = std::make_shared<ComboBox>();

			// ウィンドウの作成に成功した場合は
			if (p->__super::create(ex_style, class_name, window_name, style, x, y, w, h, parent, menu, instance, param))
			{
				// マネージャに登録してからインスタンスを返します。
				return manager.add(p), p;
			}

			// それ以外の場合は{}を返します。
			return {};
		}

		//
		// ウィンドウが破壊されたときの最終処理を実行します。
		//
		virtual void post_nc_destroy() override
		{
			// ウィンドウハンドルを取得しておきます。
			auto hwnd = (HWND)*this;

			// 基本クラスの最終処理を実行します。
			__super::post_nc_destroy();

			// コレクションから削除します。
			// これにより、このインスタンスがメモリから開放されます。
			manager.remove(hwnd);
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
			MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

			switch (message)
			{
			case WM_CREATE:
				{
					// 引数を取得します。
					auto cs = (CREATESTRUCT*)l_param;

					// コンボボックスのテーマを取得します。
					theme.reset(::OpenThemeData(hwnd, VSCLASS_COMBOBOX));

					// 本体の高さを算出します。
					calc_body_height();

					// サイズ変更処理(WM_WINDOWPOSCHANGING)を促します。
					::SetWindowPos(hwnd, nullptr, 0, 0, cs->cx, cs->cy,
						SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE | SWP_FRAMECHANGED);

					break;
				}
			case WM_SETFONT:
				{
					// フォントをセットします。
					font = (HFONT)w_param;

					// 本体の高さを算出します。
					calc_body_height();

					// 必要なら再描画します。
					if (l_param) redraw();

					break;
				}
			case WM_WINDOWPOSCHANGING:
				{
					// 引数を取得します。
					auto wp = (WINDOWPOS*)l_param;

					// サイズが変更されない場合は何もしません。
					if (wp->flags & SWP_NOSIZE) break;

					// ドロップダウンリストボックスの高さを算出します。
					calc_dropped_height(wp->cy);

					// ウィンドウの高さを本体の高さに変更します。
					wp->cy = body_height;

					break;
				}
			case WM_SETFOCUS:
				{
					// 親ウィンドウに通知コードを送信します。
					fire_command(hwnd, CBN_SETFOCUS);

					// フォーカス状態が変化したので
					// 再描画します。
					redraw();

					break;
				}
			case WM_KILLFOCUS:
				{
					// 親ウィンドウに通知コードを送信します。
					fire_command(hwnd, CBN_KILLFOCUS);

					// フォーカス状態が変化したので
					// 再描画します。
					redraw();

					break;
				}
			case WM_NCCALCSIZE:
				{
					// 引数を取得します。
					auto nc_size = (NCCALCSIZE_PARAMS*)l_param;

					return 0; // NC領域を省略します。
				}
			case WM_NCHITTEST:
				{
					return HTCLIENT; // NC領域を省略します。
				}
			case WM_CTLCOLOR:
			case WM_CTLCOLORMSGBOX:
			case WM_CTLCOLOREDIT:
			case WM_CTLCOLORLISTBOX:
			case WM_CTLCOLORBTN:
			case WM_CTLCOLORDLG:
			case WM_CTLCOLORSCROLLBAR:
			case WM_CTLCOLORSTATIC:
			case WM_MEASUREITEM:
			case WM_DRAWITEM:
				{
					// メッセージをそのまま親ウィンドウに転送します。
					return ::SendMessage(::GetParent(hwnd), message, w_param, l_param);
				}
			case WM_ERASEBKGND:
				{
					return TRUE; // 背景の描画を省略します。
				}
			case WM_PAINT: return on_paint(hwnd, message, w_param, l_param);
			case WM_LBUTTONDBLCLK:
			case WM_LBUTTONDOWN: return on_l_button_down(hwnd, message, w_param, l_param);
			case WM_MOUSEMOVE: return on_mouse_move(hwnd, message, w_param, l_param);
			case WM_MOUSELEAVE: return on_mouse_leave(hwnd, message, w_param, l_param);
			case WM_MOUSEWHEEL: return on_mouse_wheel(hwnd, message, w_param, l_param);
			case WM_KEYDOWN: return on_key_down(hwnd, message, w_param, l_param);
			case WM_KEYUP: return on_key_up(hwnd, message, w_param, l_param);
			case WM_SYSKEYDOWN: return on_sys_key_down(hwnd, message, w_param, l_param);
			case WM_GETTEXT: return (LRESULT)get_window_text((int)w_param, (LPWSTR)l_param);
			case WM_GETTEXTLENGTH: return (LRESULT)get_window_text_len();
			case CB_GETEDITSEL:
			case CB_LIMITTEXT:
			case CB_SETEDITSEL:
				break;
			case CB_ADDSTRING: return (LRESULT)add_string((LPCWSTR)l_param);
			case CB_DELETESTRING: return (LRESULT)delete_string((int)w_param);
			case CB_DIR:
				break;
			case CB_GETCOUNT: return (LRESULT)get_count();
			case CB_GETCURSEL: return (LRESULT)get_cur_sel();
			case CB_GETLBTEXT: return (LRESULT)get_lb_text((int)w_param, (LPWSTR)l_param);
			case CB_GETLBTEXTLEN: return (LRESULT)get_lb_text_len((int)w_param);
			case CB_INSERTSTRING: return (LRESULT)insert_string((int)w_param, (LPCWSTR)l_param);
			case CB_RESETCONTENT: return (LRESULT)reset_content();
			case CB_FINDSTRING: return (LRESULT)find_string((int)w_param, (LPCWSTR)l_param);
			case CB_SELECTSTRING: return (LRESULT)select_string((int)w_param, (LPCWSTR)l_param);
			case CB_SETCURSEL: return (LRESULT)set_cur_sel((int)w_param);
			case CB_SHOWDROPDOWN: return (LRESULT)show_drop_down((BOOL)w_param);
			case CB_GETITEMDATA: return (LRESULT)get_item_data((int)w_param);
			case CB_SETITEMDATA: return (LRESULT)set_item_data((int)w_param, (void*)l_param);
			case CB_GETDROPPEDCONTROLRECT: return (LRESULT)get_dropped_control_rect((LPRECT)l_param, nullptr);
			case CB_SETITEMHEIGHT: return (LRESULT)set_item_height((int)w_param, (int)l_param);
			case CB_GETITEMHEIGHT: return (LRESULT)get_item_height((int)w_param);
			case CB_SETEXTENDEDUI: return (LRESULT)set_extended_ui((BOOL)w_param);
			case CB_GETEXTENDEDUI: return (LRESULT)get_extended_ui();
			case CB_GETDROPPEDSTATE: return (LRESULT)get_dropped_state();
			case CB_FINDSTRINGEXACT: return (LRESULT)find_string_exact((int)w_param, (LPCWSTR)l_param);
			case CB_SETLOCALE:
			case CB_GETLOCALE:
			case CB_GETTOPINDEX:
			case CB_SETTOPINDEX:
				break;
			case CB_GETHORIZONTALEXTENT: return (LRESULT)get_horizontal_extent();
			case CB_SETHORIZONTALEXTENT: return (LRESULT)set_horizontal_extent((int)w_param);
			case CB_GETDROPPEDWIDTH: return (LRESULT)get_dropped_width();
			case CB_SETDROPPEDWIDTH: return (LRESULT)set_dropped_width((int)w_param);
			case CB_INITSTORAGE:
			case CB_GETCOMBOBOXINFO:
				break;
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}
	};
}
