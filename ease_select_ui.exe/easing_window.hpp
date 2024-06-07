#pragma once

namespace apn::ease_select_ui
{
	//
	// このクラスはイージング画像を表示します。
	//
	inline class EasingWindow : public CWnd
	{
	public:

		inline static constexpr UINT_PTR timer_id = 0;

		int image_version = 1;
		BOOL clamp = FALSE;
		std::wstring horz = _T("left");
		std::wstring vert = _T("center");
		int alpha = 255;
		int scale = 50;
		Color sel_color = { 0x80, 0xff, 0x00, 0x00 };
		Color hot_color = { 0x80, 0x00, 0xff, 0x00 };
		CImage image;
		std::vector<CRect> parts;
		int current_part = -1;
		int hot_part = -1;

		//
		// 画像を読み込みます。
		//
		void load_image()
		{
			MY_TRACE_FUNC("");

			if (!image.IsNull()) image.Destroy();

			parts.clear();

			auto image_number = std::clamp(image_version, 1, 3);
			auto index = image_number - 1;

			auto file_name = magi.get_assets_file_name(
				std::format(L"ease_select/easing{}.png", image_number));
			MY_TRACE_STR(file_name);

			image.Load(file_name.c_str());

			if (!image.IsNull())
			{
				auto w = image.GetWidth() * scale / 100;
				auto h = image.GetHeight() * scale / 100;

				SetWindowPos(0, 0, 0, w, h,
					SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
			}

			const SIZE size[] =
			{
				{ 121, 101 },
				{ 140, 108 },
				{ 160, 120 },
			};

			const POINT origin[] =
			{
				{ 103, 40 },
				{ 20, 98 },
				{ 20, 100 },
			};

			const POINT offset[] =
			{
				{ 125, 178 },
				{ 163, 155 },
				{ 175, 160 },
			};

			const SIZE space[] =
			{
				{ 46, 0 },
				{ 23, 0 },
				{ 15, 0 },
			};

			parts.resize(41);

			// 1番の位置です。
			set_rect(1, origin[index].x, origin[index].y, size[index]);

			auto y = origin[index].y + offset[index].y;
			for (auto i = 0; i < 5; i++, y += offset[index].y)
			{
				// 2番の列のX座標です。
				auto x = origin[index].x;

				for (auto j = 0; j < 4; j++, x += offset[index].x)
					set_rect((i * 8) + j + 2, x, y, size[index]);

				// 6番の列のX座標です。
				x += space[index].cx;

				for (auto j = 0; j < 4; j++, x += offset[index].x)
					set_rect((i * 8) + j + 6, x, y, size[index]);
			}
		}

		//
		// パーツの位置を設定します。
		//
		void set_rect(int number, int _x, int _y, const SIZE& size)
		{
			MY_TRACE_FUNC("{}, {}, {}", number, _x, _y);

			auto x = ::MulDiv(_x, scale, 100);
			auto y = ::MulDiv(_y, scale, 100);
			auto w = ::MulDiv(_x + size.cx, scale, 100);
			auto h = ::MulDiv(_y + size.cy, scale, 100);

			parts[number - 1].SetRect(x, y, w, h);
		}

		//
		// ホットパーツを設定します。
		//
		void set_hot_part(int index)
		{
			if (hot_part == index)
				return;

			hot_part = index;
			Invalidate(FALSE);
		}

		//
		// ターゲットを返します。
		//
		HWND get_target()
		{
			auto hwnd = ::GetForegroundWindow();
			DWORD pid = 0;
			DWORD tid = ::GetWindowThreadProcessId(hwnd, &pid);

			MY_TRACE_HWND(hwnd);
			MY_TRACE_HEX(pid);
			MY_TRACE_HEX(hive.host_process_id);

			if (pid != hive.host_process_id) return 0;

			auto text = my::get_window_text(hwnd);
			MY_TRACE_STR(text);

			if (text != _T("移動フレーム間隔")) return 0;

			return hwnd;
		}

		//
		// イージング番号を返します。
		//
		int get_easing()
		{
			MY_TRACE_FUNC("");

			auto target = ::GetForegroundWindow();
			if (!target) return -1;
			MY_TRACE_HWND(target);

			auto child = ::GetWindow(target, GW_CHILD);
			if (!child) return -1;
			MY_TRACE_HWND(child);
			MY_TRACE_INT(::GetDlgCtrlID(child));

			TCHAR text[MAX_PATH] = {};
			::SendMessage(child, WM_GETTEXT, std::size(text), (LPARAM)text);

			auto easing = _ttoi(text);
			if (ease_window.enabled)
				easing = -easing - 1;
			else
				easing = easing - 1;
			MY_TRACE_INT(easing);

			return easing;
		}

		//
		// イージング番号を設定します。
		//
		void set_easing(int index)
		{
			MY_TRACE_FUNC("{}", index);

			auto target = ::GetForegroundWindow();
			if (!target) return;
			MY_TRACE_HWND(target);

			auto easing = index;
			if (ease_window.enabled)
				easing = -easing - 1;
			else
				easing = easing + 1;
			MY_TRACE_INT(easing);

			auto child = ::GetWindow(target, GW_CHILD);
			if (!child) return;
			MY_TRACE_HWND(child);
			MY_TRACE_INT(::GetDlgCtrlID(child));

			auto text = std::format(L"{}", easing);
			::SendMessage(child, WM_SETTEXT, 0, (LPARAM)text.c_str());
			::PostMessage(target, WM_COMMAND, IDOK, 0);
		}

		//
		// ウィンドウを表示します。
		//
		void show(HWND target)
		{
			MY_TRACE_FUNC("{:#010x}", target);

			if (IsWindowVisible()) return;

			current_part = get_easing();
			hot_part = -1;

			CRect rc; GetWindowRect(&rc);
			CRect rc_target; ::GetWindowRect(target, &rc_target);

			int x, y;

			if (horz == _T("left"))
			{
				x = rc_target.left - rc.Width();
			}
			else if (horz == _T("right"))
			{
				x = rc_target.right;
			}
			else
			{
				x = rc_target.CenterPoint().x - rc.Width() / 2;
			}

			if (vert == _T("top"))
			{
				y = rc_target.top - rc.Height();
			}
			else if (vert == _T("bottom"))
			{
				y = rc_target.bottom;
			}
			else
			{
				y = rc_target.CenterPoint().y - rc.Height() / 2;
			}

			if (clamp)
			{
				auto rc_monitor = my::get_monitor_rect(target);

				if (x < rc_monitor.left)
					x = rc_monitor.left;
				else if (x + rc.Width() > rc_monitor.right)
					x = rc_monitor.right - rc.Width();

				if (y < rc_monitor.top)
					y = rc_monitor.top;
				else if (y + rc.Height() > rc_monitor.bottom)
					y = rc_monitor.bottom - rc.Height();
			}

			SetLayeredWindowAttributes(0, alpha, LWA_ALPHA);
			SetWindowPos(&wndTopMost, x, y, 0, 0,
				SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
			Invalidate(FALSE);
		}

		//
		// ウィンドウを非表示にします。
		//
		void hide()
		{
			MY_TRACE_FUNC("");

			if (!IsWindowVisible())
				return;

			ShowWindow(SW_HIDE);
		}

		BOOL PreCreateWindow(CREATESTRUCT& cs)
		{
			MY_TRACE_FUNC("");

			if (!__super::PreCreateWindow(cs))
				return FALSE;

			return TRUE;
		}

		int OnCreate(LPCREATESTRUCT cs)
		{
			MY_TRACE_FUNC("");

			if (__super::OnCreate(cs) == -1)
				return -1;

			if (!ease_window.init(this))
			{
				MY_TRACE("イーズウィンドウの作成に失敗しました\n");
				return -1;
			}

			SetTimer(timer_id, 1000, 0);

			// ウィンドウの初期化が完了したので、
			// このウィンドウをメインウィンドウに設定します。
			hive.main_window = *this;

			return 0;
		}

		void OnDestroy()
		{
			MY_TRACE_FUNC("");

			image.Destroy();

			KillTimer(timer_id);

			ease_window.exit();

			__super::OnDestroy();
		}

		void OnPaint()
		{
			CPaintDC pdc(this);
			CUxDC dc(pdc);
			if (!dc.is_valid()) return;

			CRect rc; GetClientRect(&rc);

			if (!image.IsNull())
			{
				auto w = image.GetWidth();
				auto h = image.GetHeight();
				image.AlphaBlend(dc, rc.left, rc.top, rc.Width(), rc.Height(), 0, 0, w, h);
			}

			Graphics g(dc);
			g.SetSmoothingMode(SmoothingModeAntiAlias);
//			g.SetCompositingMode(CompositingModeSourceOver);
			g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
			g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
			g.TranslateTransform(-0.5f, -0.5f);

			if (current_part >= 0 && current_part < (int)parts.size())
			{
				SolidBrush brush(sel_color);
				auto x = (REAL)parts[current_part].left;
				auto y = (REAL)parts[current_part].top;
				auto w = (REAL)parts[current_part].Width();
				auto h = (REAL)parts[current_part].Height();

				g.FillRectangle(&brush, x, y, w, h);
			}

			if (hot_part >= 0 && hot_part < (int)parts.size())
			{
				SolidBrush brush(hot_color);
				auto x = (REAL)parts[hot_part].left;
				auto y = (REAL)parts[hot_part].top;
				auto w = (REAL)parts[hot_part].Width();
				auto h = (REAL)parts[hot_part].Height();

				g.FillRectangle(&brush, x, y, w, h);
			}
		}

		void OnTimer(UINT_PTR timer_id)
		{
			if (timer_id == this->timer_id)
			{
				auto target = get_target();
				if (target)
				{
					show(target);
					ease_window.show(target, *this);
				}
				else
				{
					ease_window.hide();
					hide();
				}
			}

			__super::OnTimer(timer_id);
		}

		BOOL OnNcActivate(BOOL bActive)
		{
			MY_TRACE_FUNC("");

			return __super::OnNcActivate(bActive);
		}

		void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
		{
			MY_TRACE_FUNC("");

//			__super::OnNcCalcSize(bCalcValidRects, lpncsp);
		}

		LRESULT OnNcHitTest(CPoint point)
		{
			ScreenToClient(&point);

			for (size_t i = 0; i < parts.size(); i++)
			{
				if (parts[i].PtInRect(point))
				{
					set_hot_part(i);
					return HTCLIENT;
				}
			}

			set_hot_part(-1);
			return HTCAPTION;
//			return __super::OnNcHitTest(point);
		}

		void OnLButtonDown(UINT nFlags, CPoint point)
		{
			MY_TRACE_FUNC("");

			for (size_t i = 0; i < parts.size(); i++)
			{
				if (parts[i].PtInRect(point))
				{
					set_easing(i);
					return;
				}
			}

			__super::OnLButtonDown(nFlags, point);
		}

		DECLARE_MESSAGE_MAP()
	} easing_window;

	BEGIN_MESSAGE_MAP(EasingWindow, CWnd)
		ON_WM_CREATE()
		ON_WM_DESTROY()
		ON_WM_PAINT()
		ON_WM_TIMER()
		ON_WM_NCACTIVATE()
		ON_WM_NCCALCSIZE()
		ON_WM_NCHITTEST()
		ON_WM_LBUTTONDOWN()
	END_MESSAGE_MAP()
}
