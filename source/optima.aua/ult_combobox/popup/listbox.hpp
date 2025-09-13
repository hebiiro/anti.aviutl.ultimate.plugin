#pragma once

namespace apn::ult_combobox::popup
{
	//
	// このクラスはロックカウントを管理します。
	//
	struct Lockable
	{
		//
		// このクラスはロック可能なオブジェクトをロックします。
		//
		struct Locker {
			Lockable& lockable;
			Locker(Lockable& lockable) : lockable(lockable) { lockable.lock_count++; }
			~Locker() { lockable.lock_count--; }
		};

		//
		// ロックカウントです。
		//
		LONG lock_count = {};

		//
		// ロックカウントが0以外の場合はTRUEを返します。
		//
		BOOL is_locked() const { return !!lock_count; }
	};

	//
	// このクラスは自作コンボボックスのポップアップリストボックスです。
	//
	struct ListBox : Node
	{
		//
		// このクラスは一時的にマウスキャプチャを解除します。
		//
		struct TempReleaseCapture {
			ListBox& listbox;
			TempReleaseCapture(ListBox& listbox) : listbox(listbox) { ::ReleaseCapture(); }
			~TempReleaseCapture() { ::SetCapture(listbox); }
		};

		//
		// このリストボックスを現在利用しているコアです。
		//
		std::shared_ptr<Core> core;

		//
		// コンボボックスです。(コアのウィンドウハンドルです)
		//
		HWND control = {};

		//
		// コンボボックスのコントロールIDです。
		//
		UINT control_id = {};

		//
		// 通知先のウィンドウです。
		//
		HWND parent = {};

		//
		// リストボックスを表示/非表示にするときに使用するアニメーションフラグです。
		//
		DWORD animation_flags = {};

		//
		// 項目の高さです。
		//
		int item_height = {};

		//
		// 表示項目数です。
		//
		int visible_item_count = {};

		//
		// 選択項目です。
		//
		int selected_index = CB_ERR;

		//
		// 現在のヒットテストコードです。
		//
		int current_ht = HTNOWHERE;

		//
		// 現在のマウスカーソルの座標です。
		//
		POINT current_pt = {};

		//
		// マウスメッセージを処理するとTRUEになります。
		//
		BOOL is_mouse_eaten = {};

		//
		// WM_CAPTURECHANGEDをロックします。
		//
		Lockable capture_changed;

		//
		// コンストラクタです。
		//
		ListBox()
		{
			MY_TRACE_FUNC("");
		}

		//
		// デストラクタです。
		//
		virtual ~ListBox() override
		{
			MY_TRACE_FUNC("");
		}

		//
		// 先頭項目を返します。
		//
		int get_top_index()
		{
			return ::GetScrollPos(*this, SB_VERT);
		}

		//
		// 先頭項目をセットします。
		//
		void set_top_index(int index)
		{
			::SetScrollPos(*this, SB_VERT, index, TRUE);
		}

		//
		// 選択項目を返します。
		//
		int get_selected_index()
		{
			return (selected_index != CB_ERR) ?
				selected_index : core->current_index;
		}

		//
		// 選択項目をセットします。
		//
		int set_selected_index(int index)
		{
			selected_index = index;
			selected_index = std::max(selected_index, 0);
			selected_index = std::min(selected_index, core->get_count() - 1);
			return selected_index;
		}

		//
		// 別の項目を選択します。
		//
		BOOL relative_select(int relative_index)
		{
			// 選択項目を取得します。
			auto old_index = get_selected_index();

			// 選択項目を更新します。
			auto new_index = set_selected_index(old_index + relative_index);

			// 選択項目が変化していない場合は何もしません。
			if (new_index == old_index) return FALSE;

			// 表示範囲を取得します。
			auto top_index = get_top_index();
			auto bottom_index = top_index + visible_item_count;

			// 選択項目が表示範囲より上の場合は
			if (new_index < top_index)
			{
				// 選択項目が表示されるようにスクロールします。
				set_top_index(new_index);
			}
			// 選択項目が表示範囲より下の場合は
			else if (new_index >= bottom_index)
			{
				// 選択項目が表示されるようにスクロールします。
				set_top_index(new_index - visible_item_count + 1);
			}

			// 再描画します。
			redraw();

			return TRUE;
		}

		//
		// コンボボックスの親ウィンドウに通知コードを送信します。
		//
		BOOL notify(UINT code)
		{
			return ::SendMessage(parent, WM_COMMAND, MAKEWPARAM(control_id, code), (LPARAM)control);
		}

		//
		// リストボックスを表示します。
		//
		BOOL show(const std::shared_ptr<Core>& core, LPCRECT rc, DWORD animation_flags)
		{
			// リストボックがすでに表示されている場合は何もしません。
			if (::IsWindowVisible(*this)) return FALSE;

			this->core = core;
			this->control = *core;
			this->control_id = ::GetDlgCtrlID(control);
			this->parent = ::GetParent(control);
			this->animation_flags = animation_flags;
			this->item_height = core->get_item_height(0);
			this->visible_item_count = my::get_height(*rc) / item_height;
			this->selected_index = core->get_cur_sel();
			this->current_ht = HTNOWHERE;
			this->current_pt = { LONG_MIN, LONG_MIN };
			this->is_mouse_eaten = {};

			// コンボボックスの親ウィンドウに通知コードを送信します。
			notify(CBN_DROPDOWN);

			// コンボボックスのウィンドウスタイルを取得します。
			auto combobox_style = my::get_style(*core);

			// 垂直スクロールバーを初期化します。
			{
				SCROLLINFO si = { sizeof(si) };
				si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
				si.nPos = selected_index;
				si.nMin = 0;
				si.nMax = core->get_count();
				si.nPage = visible_item_count + 1;

				if (combobox_style & CBS_DISABLENOSCROLL)
					si.fMask |= SIF_DISABLENOSCROLL;

				::SetScrollInfo(*this, SB_VERT, &si, FALSE);
			}
#if 1
			// リストボックスのサイズを変更します。
			my::set_window_pos(*this, HWND_TOPMOST, rc, SWP_NOACTIVATE | SWP_FRAMECHANGED);

			// アニメーションさせながら
			// リストボックスを表示します。
			::AnimateWindow(*this, 0, animation_flags);

			// ::AnimateWindow()を使用すると
			// WM_NCPAINTが送られないので手動で送信します。
			::SendMessage(*this, WM_NCPAINT, 0, 0);
#else
			// リストボックスのサイズを変更＆表示します。
			my::set_window_pos(*this, HWND_TOPMOST, rc,
				SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
#endif
			// マウスキャプチャを開始します。
			::SetCapture(*this);

			return TRUE;
		}

		//
		// リストボックスを非表示にします。
		//
		BOOL hide(int selected_index)
		{
			MY_TRACE_FUNC("{/}", selected_index);

			// リストボックがすでに非表示の場合は何もしません。
			if (!::IsWindowVisible(*this)) return FALSE;

			{
				// マウスキャプチャを終了する前にロックします。
				Lockable::Locker locker(capture_changed);

				// マウスキャプチャを終了します。
				::ReleaseCapture();
			}
#if 0
			// アニメーションさせながら
			// リストボックスを非表示にします。
			auto reverse_animation_flags = animation_flags &
				~(AW_HOR_POSITIVE | AW_HOR_NEGATIVE | AW_VER_POSITIVE | AW_VER_NEGATIVE);
			if (animation_flags & AW_HOR_POSITIVE) reverse_animation_flags |= AW_HOR_NEGATIVE;
			if (animation_flags & AW_HOR_NEGATIVE) reverse_animation_flags |= AW_HOR_POSITIVE;
			if (animation_flags & AW_VER_POSITIVE) reverse_animation_flags |= AW_VER_NEGATIVE;
			if (animation_flags & AW_VER_NEGATIVE) reverse_animation_flags |= AW_VER_POSITIVE;
			::AnimateWindow(*this, 0, AW_HIDE | reverse_animation_flags);
#else
			// リストボックスを非表示にします。
			::ShowWindow(*this, SW_HIDE);
#endif
			// 選択項目が有効の場合は
			if (selected_index != CB_ERR)
			{
				// コンボボックスの親ウィンドウに通知コードを送信します。
				notify(CBN_SELENDOK);
				notify(CBN_CLOSEUP);

				// コンボボックスの選択項目を変更します。
				core->set_cur_sel(selected_index);

				// コンボボックスの親ウィンドウに通知コードを送信します。
				notify(CBN_SELCHANGE);
			}
			// 選択項目が無効の場合は
			else
			{
				// コンボボックスの親ウィンドウに通知コードを送信します。
				notify(CBN_SELENDCANCEL);
				notify(CBN_CLOSEUP);
			}

			return TRUE;
		}

		//
		// 指定された座標にある項目を返します。
		//
		int hittest(const POINT& client_pt)
		{
			// クライアント矩形を取得します。
			auto rc = my::get_client_rect(*this);

			// クライアント矩形外の場合はCB_ERRを返します。
			if (!::PtInRect(&rc, client_pt)) return CB_ERR;

			// 先頭項目を取得します。
			auto top_index = get_top_index();

			// 項目の高さを取得します。
			auto item_height = core->get_item_height(0);

			// 指定された座標にある項目を返します。
			return top_index + (client_pt.y - rc.top) / item_height;
		}

		//
		// リストボックスを描画します。
		//
		LRESULT paint(HDC dc, const RECT& rc)
		{
			// デバイスコンテキストを初期化します。
			my::gdi::selector font_selector(dc, core->font);

			// 親ウィンドウからブラシを取得します。
			auto listbox_brush = fire_ctl_color(parent, hwnd, dc, WM_CTLCOLORLISTBOX);

			// 背景を描画します。
			::FillRect(dc, &rc, listbox_brush);

			// 項目の文字色を取得します。
			auto item_text_color = ::GetTextColor(dc);

			// 項目の背景色を取得します。
			auto item_bk_color = ::GetBkColor(dc);

			// 選択項目の文字色を取得します。
			auto selected_item_text_color = ::GetSysColor(COLOR_HIGHLIGHTTEXT);

			// 選択項目の背景色を取得します。
			auto selected_item_bk_color = ::GetSysColor(COLOR_HIGHLIGHT);

			// 先頭項目を取得します。
			auto top_index = get_top_index();

			// 選択項目を取得します。
			auto selected_index = (this->selected_index != CB_ERR) ?
				this->selected_index : core->get_cur_sel();

			// 表示項目を走査します。
			for (decltype(visible_item_count) i = 0; i < visible_item_count; i++)
			{
				// 項目を取得します。
				auto index = top_index + i;

				// 項目が無効の場合はループを終了します。
				if ((size_t)index >= core->items.size()) break;

				// 選択項目の場合は
				if (index == selected_index)
				{
					// 選択色をセットします。
					::SetTextColor(dc, selected_item_text_color);
					::SetBkColor(dc, selected_item_bk_color);
				}
				// それ以外の場合は
				else
				{
					// 通常色をセットします。
					::SetTextColor(dc, item_text_color);
					::SetBkColor(dc, item_bk_color);
				}

				// 項目のテキストを取得します。
				auto text = core->items[index]->text;

				// 項目の矩形を取得します。
				auto text_rc = rc;
				text_rc.top += i * item_height;
				text_rc.bottom = text_rc.top + item_height;

				// 項目のテキストを描画します。
				::ExtTextOutW(dc, text_rc.left + core->c_padding, text_rc.top,
					ETO_OPAQUE, &text_rc, text.c_str(), (UINT)text.length(), nullptr);
			}

			return 0;
		}

		//
		// WM_PAINTを処理します。
		//
		LRESULT on_paint(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			// デバイスコンテキストを作成します。
			auto rc = my::get_client_rect(hwnd);
			my::PaintDC pdc(hwnd);
			my::DoubleBufferDC dc(pdc, &rc);

			return paint(dc, rc);
		}

		//
		// WM_PRINTCLIENTを処理します。
		//
		LRESULT on_print_client(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			if (l_param & PRF_CLIENT)
				paint((HDC)w_param, my::get_client_rect(hwnd));

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}

		//
		// WM_NCHITTESTを処理します。
		//
		LRESULT on_nc_hittest(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			// ヒットテストコードを取得しておきます。
			current_ht = __super::on_wnd_proc(hwnd, message, w_param, l_param);

			MY_TRACE_FUNC("{/hex}, {/}, w_param = {/hex}, l_param = {/hex}", hwnd, my::message_to_string(message), w_param, l_param);
			MY_TRACE_INT(current_ht);

			return current_ht;
		}

		//
		// WM_LBUTTONDOWNを処理します。
		//
		LRESULT on_l_button_down(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			MY_TRACE_FUNC("{/hex}, {/}, w_param = {/hex}, l_param = {/hex}", hwnd, my::message_to_string(message), w_param, l_param);
			MY_TRACE_INT(current_ht);
			MY_TRACE_INT(is_mouse_eaten);

			// ウィンドウ領域外の場合は
			if (current_ht == HTNOWHERE)
			{
				// ウィンドウ領域外での左クリックなので、
				// リストボックスを非表示にします。

				// リストボックスを非表示にします。
				hide(CB_ERR);
			}
			// クライアント領域外の場合は
			else if (current_ht != HTCLIENT)
			{
				// NC領域での左クリックなので、
				// スクロールバーなどのマウス処理ができるようにします。

				// マウスキャプチャを終了する前にロックします。
				Lockable::Locker locker(capture_changed);

				// 一時的にマウスキャプチャを終了します。
				TempReleaseCapture temp_release_capture(*this);

				// WM_NCLBUTTONDOWNを送信します。
				auto client_pt = my::lp_to_pt(l_param);
				auto screen_pt = client_to_screen(hwnd, client_pt);
				::SendMessage(hwnd, WM_NCLBUTTONDOWN, current_ht, my::pt_to_lp(screen_pt));
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}

		//
		// WM_LBUTTONUPを処理します。
		//
		LRESULT on_l_button_up(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			MY_TRACE_FUNC("{/hex}, {/}, w_param = {/hex}, l_param = {/hex}", hwnd, my::message_to_string(message), w_param, l_param);
			MY_TRACE_INT(current_ht);
			MY_TRACE_INT(is_mouse_eaten);

			// マウスメッセージが処理済みの場合は
			if (is_mouse_eaten)
			{
				// リストボックス内でマウス操作を行ったあとなので、
				// コンボボックスに選択項目を通知してリストボックスを非表示にします。

				// マウスカーソルの位置にある項目を取得します。
				selected_index = hittest(my::lp_to_pt(l_param));

				// コンボボックスで項目を選択して
				// リストボックスを非表示にします。
				hide(selected_index);
			}
			else
			{
				// マウスメッセージを処理したのでフラグをセットします。
				is_mouse_eaten = TRUE;
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}

		//
		// WM_RBUTTONUPを処理します。
		//
		LRESULT on_r_button_up(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			MY_TRACE_FUNC("{/hex}, {/}, w_param = {/hex}, l_param = {/hex}", hwnd, my::message_to_string(message), w_param, l_param);
			MY_TRACE_INT(current_ht);
			MY_TRACE_INT(is_mouse_eaten);

			// クライアント領域外の場合は
			if (current_ht != HTCLIENT)
			{
				// NC領域での右クリックなので、
				// スクロールバーメニューを表示できるようにします。

				// マウスキャプチャを終了する前にロックします。
				Lockable::Locker locker(capture_changed);

				// 一時的にマウスキャプチャを終了します。
				TempReleaseCapture temp_release_capture(*this);

				return __super::on_wnd_proc(hwnd, message, w_param, l_param);
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}

		//
		// WM_MOUSEMOVEを処理します。
		//
		LRESULT on_mouse_move(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			MY_TRACE_FUNC("{/hex}, {/}, w_param = {/hex}, l_param = {/hex}", hwnd, my::message_to_string(message), w_param, l_param);
			MY_TRACE_INT(current_ht);
			MY_TRACE_INT(is_mouse_eaten);

			// 引数を取得します。
			auto client_pt = my::lp_to_pt(l_param);

			// マウスカーソルの座標が変化していない場合は何もしません。
			if (client_pt.x == current_pt.x && client_pt.y == current_pt.y)
				return __super::on_wnd_proc(hwnd, message, w_param, l_param);

			// 現在のマウスカーソルの座標を更新します。
			current_pt = client_pt;

			// マウスキャプチャ中の場合は
			if (hwnd == ::GetCapture())
			{
				// WM_NCHITTESTが発生しないので手動で送信します。
				auto screen_pt = client_to_screen(hwnd, client_pt);
				::SendMessage(hwnd, WM_NCHITTEST, 0, my::pt_to_lp(screen_pt));
			}

			// クライアント領域内の場合は
			if (current_ht == HTCLIENT)
			{
				// マウスメッセージを処理したのでフラグをセットします。
				is_mouse_eaten = TRUE;
			}

			// マウスカーソルの座標にある項目を取得します。
			auto index = hittest(client_pt);

			// 選択項目と異なる場合は
			if (selected_index != index)
			{
				// 選択項目を更新します。
				selected_index = index;

				// 再描画します。
				redraw();
			}

			// 左クリックしている場合は
			if (is_mouse_eaten && w_param & MK_LBUTTON)
			{
				// クライアント矩形を取得します。
				auto rc = my::get_client_rect(hwnd);

				// マウスカーソルのX座標がクライント矩形内の場合は
//				if (client_pt.x >= rc.left && client_pt.x <= rc.right)
				{
					// マウスカーソルのY座標がクライント矩形より上の場合は
					if (client_pt.y < rc.top)
					{
						// 項目を上方向にスクロールします。
						return on_v_scroll(hwnd, WM_VSCROLL, MAKEWPARAM(SB_LINEUP, 0), 0);
					}
					// マウスカーソルのY座標がクライント矩形より下の場合は
					else if (client_pt.y > rc.bottom)
					{
						// 項目を下方向にスクロールします。
						return on_v_scroll(hwnd, WM_VSCROLL, MAKEWPARAM(SB_LINEDOWN, 0), 0);
					}
				}
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}

		//
		// WM_MOUSEWHEELを処理します。
		//
		LRESULT on_mouse_wheel(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			// マウスホイール後すぐに項目を選択したいので、
			// スコープ終了時に処理します。
			my::scope_exit scope_exit([&]()
			{
				auto screen_pt = my::lp_to_pt(l_param);
				auto client_pt = screen_to_client(hwnd, screen_pt);

				// マウスカーソルの座標にある項目を取得します。
				auto index = hittest(client_pt);

				// 選択項目と異なる場合は
				if (selected_index != index)
				{
					// 選択項目を更新します。
					selected_index = index;

					// 再描画します。
					redraw();
				}
			});

			// 引数を取得します。
			auto delta = (short)HIWORD(w_param);

			// スクロールのタイプを決定します。
			auto type = (delta > 0) ? SB_PAGEUP : SB_PAGEDOWN;

			// 垂直方向にスクロールします。
			return on_v_scroll(hwnd, WM_VSCROLL, MAKEWPARAM(type, 0), 0);
		}

		//
		// WM_CAPTURECHANGEDを処理します。
		//
		LRESULT on_capture_changed(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			// ロックされていない場合は
			// 選択をキャンセルしてリストボックスを非表示にします。
			if (!capture_changed.is_locked()) hide(CB_ERR);

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}

		//
		// WM_VSCROLLを処理します。
		//
		LRESULT on_v_scroll(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			auto scroll_lines = UINT {};
			::SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &scroll_lines, 0);

			// スクロール情報を取得します。
			SCROLLINFO si = { sizeof(si), SIF_ALL };
			::GetScrollInfo(hwnd, SB_VERT, &si);

			// 現在のスクロール位置を取得します。
			auto old_pos = si.nPos;
			auto new_pos = old_pos;

			switch (LOWORD(w_param))
			{
			case SB_TOP: new_pos = si.nMin; break;
			case SB_BOTTOM: new_pos = si.nMax; break;
			case SB_LINEUP: new_pos--; break;
			case SB_LINEDOWN: new_pos++; break;
			case SB_PAGEUP: new_pos -= scroll_lines; break;
			case SB_PAGEDOWN: new_pos += scroll_lines; break;
			case SB_THUMBPOSITION:
			case SB_THUMBTRACK: new_pos = HIWORD(w_param); break;
			}

			// スクロール位置が異なる場合は
			if (new_pos != old_pos)
			{
				// スクロール位置を更新します。
				::SetScrollPos(hwnd, SB_VERT, new_pos, TRUE);

				// 再描画します。
				redraw();
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}

		//
		// WM_KEYDOWNを処理します。
		//
		LRESULT on_key_down(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}

		//
		// WM_SYSKEYDOWNを処理します。
		//
		LRESULT on_sys_key_down(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
			MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

			switch (message)
			{
			case WM_MOUSEACTIVATE:
				{
					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					return MA_NOACTIVATE; // クリックでアクティブ化しないようにします。
				}
			case WM_ERASEBKGND:
				{
					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					return TRUE; // 背景の描画を省略します。
				}
			case WM_PAINT: return on_paint(hwnd, message, w_param, l_param);
			case WM_PRINTCLIENT: return on_print_client(hwnd, message, w_param, l_param);
			case WM_NCHITTEST: return on_nc_hittest(hwnd, message, w_param, l_param);
			case WM_LBUTTONDBLCLK:
			case WM_LBUTTONDOWN: return on_l_button_down(hwnd, message, w_param, l_param);
			case WM_LBUTTONUP: return on_l_button_up(hwnd, message, w_param, l_param);
			case WM_RBUTTONUP: return on_r_button_up(hwnd, message, w_param, l_param);
			case WM_MOUSEMOVE: return on_mouse_move(hwnd, message, w_param, l_param);
			case WM_MOUSEWHEEL: return on_mouse_wheel(hwnd, message, w_param, l_param);
			case WM_CAPTURECHANGED: return on_capture_changed(hwnd, message, w_param, l_param);
			case WM_VSCROLL: return on_v_scroll(hwnd, message, w_param, l_param);
			case WM_KEYDOWN: return on_key_down(hwnd, message, w_param, l_param);
			case WM_SYSKEYDOWN: return on_sys_key_down(hwnd, message, w_param, l_param);
			case LB_ADDSTRING: return (LRESULT)core->add_string((LPCWSTR)l_param);
			case LB_DELETESTRING: return (LRESULT)core->delete_string((int)w_param);
			case LB_GETCOUNT: return (LRESULT)core->get_count();
			case LB_GETCURSEL: return (LRESULT)core->get_cur_sel();
			case LB_GETTEXT: return (LRESULT)core->get_lb_text((int)w_param, (LPWSTR)l_param);
			case LB_GETTEXTLEN: return (LRESULT)core->get_lb_text_len((int)w_param);
			case LB_INSERTSTRING: return (LRESULT)core->insert_string((int)w_param, (LPCWSTR)l_param);
			case LB_RESETCONTENT: return (LRESULT)core->reset_content();
			case LB_FINDSTRING: return (LRESULT)core->find_string((int)w_param, (LPCWSTR)l_param);
			case LB_SELECTSTRING: return (LRESULT)core->select_string((int)w_param, (LPCWSTR)l_param);
			case LB_SETCURSEL: return (LRESULT)core->set_cur_sel((int)w_param);
			case LB_GETITEMDATA: return (LRESULT)core->get_item_data((int)w_param);
			case LB_SETITEMDATA: return (LRESULT)core->set_item_data((int)w_param, (void*)l_param);
			case LB_SETITEMHEIGHT: return (LRESULT)core->set_item_height((int)w_param, (int)l_param);
			case LB_GETITEMHEIGHT: return (LRESULT)core->get_item_height((int)w_param);
			case LB_FINDSTRINGEXACT: return (LRESULT)core->find_string_exact((int)w_param, (LPCWSTR)l_param);
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}
	}; std::shared_ptr<ListBox> listbox = std::make_shared<ListBox>();
}
