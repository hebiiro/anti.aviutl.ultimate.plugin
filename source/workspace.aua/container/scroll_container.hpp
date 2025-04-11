#pragma once

namespace apn::workspace
{
	//
	// このクラスはターゲットウィンドウをクライアント領域全体まで広げるコンテナです。
	//
	struct ScrollContainer : Container
	{
		//
		// コンストラクタです。
		//
		ScrollContainer(Content* content, DWORD style, DWORD ex_style)
			: Container(content, style, ex_style)
		{
		}

		//
		// デストラクタです。
		//
		virtual ~ScrollContainer() override
		{
		}

		//
		// スクロール範囲を更新します。
		//
		BOOL update_scrollbar()
		{
			MY_TRACE_FUNC("");

			// コンテンツのHWNDを取得します。
			auto hwnd = content->get_hwnd();

			// コンテンツのクライアント矩形を取得します。
			auto rc = my::get_client_rect(hwnd);

			// コンテンツのクライアント矩形の幅と高さを算出します。
			int content_size[2] = { my::get_width(rc), my::get_height(rc) };

			// コンテナのクライアント矩形を取得します。
			auto rc_container = my::get_client_rect(*this);

			// スクロールバーの有無によってコンテナのクライアント矩形を調整します。
			auto style = my::get_style(*this);
			if (style & WS_VSCROLL) rc_container.right += ::GetSystemMetrics(SM_CXVSCROLL);
			if (style & WS_HSCROLL) rc_container.bottom += ::GetSystemMetrics(SM_CYHSCROLL);

			// コンテナのクライアント矩形の幅と高さを算出します。
			int container_size[2] = { my::get_width(rc_container), my::get_height(rc_container) };

			constexpr auto h = 0;
			constexpr auto v = 1;

			if (content_size[h] > container_size[h])
			{
				// コンテンツの横幅の方が大きいので水平スクロールバーが必要です。
				// なので、コンテナの縦幅を水平スクロールバーの太さの分だけ小さくします。
				container_size[v] -= ::GetSystemMetrics(SM_CYHSCROLL);

				if (content_size[v] > container_size[v])
				{
					// コンテンツの縦幅の方が大きいので垂直スクロールバーが必要です。
					// なので、コンテナの横幅を垂直スクロールバーの太さの分だけ小さくします。
					container_size[h] -= ::GetSystemMetrics(SM_CXVSCROLL);
				}
			}
			else if (content_size[v] > container_size[v])
			{
				// コンテンツの縦幅の方が大きいので垂直スクロールバーが必要です。
				// なので、コンテナの横幅を垂直スクロールバーの太さの分だけ小さくします。
				container_size[h] -= ::GetSystemMetrics(SM_CXVSCROLL);

				if (content_size[h] > container_size[h])
				{
					// コンテンツの横幅の方が大きいので水平スクロールバーが必要です。
					// なので、コンテナの縦幅を水平スクロールバーの太さの分だけ小さくします。
					container_size[v] -= ::GetSystemMetrics(SM_CYHSCROLL);
				}
			}

			// スクロール範囲を変更します。
			SCROLLINFO si = { sizeof(si) };
			si.fMask = SIF_PAGE | SIF_RANGE;
			si.nMin = 0;
			for (size_t i = 0; i < std::size(content_size); i++)
			{
				si.nMax = content_size[i] - 1;
				si.nPage = container_size[i];
				::SetScrollInfo(*this, i, &si, TRUE);
			}

			// NC領域の変更を通知します。
			::SetWindowPos(*this, nullptr, 0, 0, 0, 0,
				SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED);

			return TRUE;
		}

		//
		// スクロール位置を変更します。
		//
		void scroll(int bar, WPARAM wParam, int delta = 1)
		{
			MY_TRACE_FUNC("");

			SCROLLINFO si = { sizeof(si) };
			si.fMask = SIF_POS | SIF_RANGE;
			::GetScrollInfo(*this, bar, &si);

			switch (LOWORD(wParam))
			{
			case SB_LEFT:      si.nPos = si.nMin; break;
			case SB_RIGHT:     si.nPos = si.nMax; break;
			case SB_LINELEFT:  si.nPos += -10 * delta; break;
			case SB_LINERIGHT: si.nPos +=  10 * delta; break;
			case SB_PAGELEFT:  si.nPos += -60 * delta; break;
			case SB_PAGERIGHT: si.nPos +=  60 * delta; break;
			case SB_THUMBTRACK:
			case SB_THUMBPOSITION: si.nPos = HIWORD(wParam); break;
			}

			::SetScrollInfo(*this, bar, &si, TRUE);
		}

		//
		// コンテンツの位置を更新します(位置のみ変更します)。
		//
		BOOL update_content_pos()
		{
			MY_TRACE_FUNC("");

			// コンテンツのHWNDを取得します。
			auto hwnd = content->get_hwnd();

			// スクロール量を取得します。
			int scroll_pos[2] = {};
			for (size_t i = 0; i < std::size(scroll_pos); i++)
				scroll_pos[i] = ::GetScrollPos(*this, i);

			// コンテナのクライアント矩形(の左上座標)を取得します。
			auto rc = RECT { 0, 0 };

			// クライアント矩形をウィンドウ矩形に変換します。
			my::client_to_window(hwnd, &rc);

			// ウィンドウ矩形の左上の座標を取得します。
			int pos[2] = { rc.left, rc.top };

			// 座標をスクロールの分だけずらします。
			for (size_t i = 0; i < std::size(pos); i++)
				pos[i] -= scroll_pos[i];

			// コンテンツの位置を変更します。
			::SetWindowPos(hwnd, nullptr, pos[0], pos[1], 0, 0,
				SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);

			// 強制再描画でスクロールに素早く追従させます。
			::UpdateWindow(hwnd);

			return TRUE;
		}

		//
		// コンテナを正規化されたrcの位置に移動します。
		//
		virtual void set_container_position(LPCRECT rc) override
		{
			MY_TRACE_FUNC("{/}, {/}, {/}, {/}", rc->left, rc->top, rc->right, rc->bottom);

			// ロックされている場合は何もしません。
			if (is_locked()) return;

			// ロックします。
			Locker locker(this);

			// コンテンツのHWNDを取得します。
			auto hwnd = content->get_hwnd();

			// コンテンツが格納されている現在のコンテナを取得します。
			// (ドッキング完了後に呼び出された場合はparentはこのコンテナになります)
			auto parent = ::GetParent(hwnd);

			// 現在のコンテナのスクロール位置を取得しておきます。
			int scroll_pos[2] = {};
			for (size_t i = 0; i < std::size(scroll_pos); i++)
				scroll_pos[i] = ::GetScrollPos(parent, i);

			// このコンテナの位置を変更します。
			__super::set_container_position(rc);

			// このコンテナのサイズが変更されたのでスクロール範囲を更新します。
			// スクロール位置を更新する前にこの処理を行う必要があります。
			update_scrollbar();

			// 現在のコンテナのスクロール位置をこのコンテナに適用します。
			for (size_t i = 0; i < std::size(scroll_pos); i++)
				::SetScrollPos(*this, i, scroll_pos[i], TRUE);

			// スクロール位置が更新されたのでコンテンツの位置を更新します。
			update_content_pos();
		}

		//
		// コンテナがフローティング状態になるときに呼ばれます。
		//
		virtual void on_float_container() override
		{
			MY_TRACE_FUNC("");

			// コンテンツのHWNDを取得します。
			auto hwnd = content->get_hwnd();

			// コンテンツが格納されている現在のコンテナ(ドッキングコンテナ)を取得します。
			auto parent = ::GetParent(hwnd);

			// 現在のコンテナのクライアント矩形を取得します。
			auto rc = my::get_client_rect(parent);

			// クライント座標からスクリーン座標に変換します。
			my::map_window_points(parent, nullptr, &rc);

			// 現在のコンテナのクライアント矩形をこのコンテナのウィンドウ矩形に変換します。
			my::client_to_window(*this, &rc);

			// このコンテナの位置を変更します。
			set_container_position(&rc);
		}

		//
		// コンテンツの位置を修正するために呼ばれます。
		//
		virtual BOOL revise_content_position(WINDOWPOS* wp) override
		{
			MY_TRACE_FUNC("");

			if (!__super::revise_content_position(wp)) return FALSE;

			// コンテンツの位置をスクロールの分だけずらします。
			wp->x -= ::GetScrollPos(*this, SB_HORZ);
			wp->y -= ::GetScrollPos(*this, SB_VERT);

			return TRUE;
		}

		//
		// コンテンツの位置を変更します。
		//
		virtual BOOL set_content_position() override
		{
			MY_TRACE_FUNC("");

			// スクロールバーとコンテナの位置を更新します。
			update_scrollbar();
			update_content_pos();

			return TRUE;
		}

		//
		// コンテンツの位置にフィットするようにコンテナの位置を変更します。
		//
		virtual BOOL fit_container_position(const WINDOWPOS* content_wp) override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// コンテンツの位置が変更されたときに(コンテンツから)呼ばれます。
		//
		BOOL on_content_pos_changed(WINDOWPOS* wp) override
		{
			MY_TRACE_FUNC("");

			if (!__super::on_content_pos_changed(wp)) return FALSE;

			update_scrollbar();

			return TRUE;
		}

		//
		// コンテナのウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			auto result = __super::on_wnd_proc(hwnd, message, wParam, lParam);

			switch (message)
			{
			case WM_VSCROLL:
				{
					// 縦スクロールに応じてスクロールしてからコンテンツの位置を更新します。
					Locker lcoker(this);
					scroll(SB_VERT, wParam);
					update_content_pos();

					break;
				}
			case WM_HSCROLL:
				{
					// 横スクロールに応じてスクロールしてからコンテンツの位置を更新します。
					Locker lcoker(this);
					scroll(SB_HORZ, wParam);
					update_content_pos();

					break;
				}
			case WM_MOUSEWHEEL:
				{
					MY_TRACE_FUNC("WM_MOUSEWHEEL, {/hex}, {/hex}", wParam, lParam);

					// ホイールに応じてスクロールしてからコンテンツの位置を更新します。
					Locker lcoker(this);
					scroll(SB_VERT, SB_PAGEUP, GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
					update_content_pos();

					break;
				}
			case WM_MOUSEHWHEEL:
				{
					MY_TRACE_FUNC("WM_MOUSEHWHEEL, {/hex}, {/hex}", wParam, lParam);

					// ホイールに応じてスクロールしてからコンテンツの位置を更新します。
					Locker lcoker(this);
					scroll(SB_HORZ, SB_PAGELEFT, GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
					update_content_pos();

					break;
				}
			}

			return result;
		}
	};
}
