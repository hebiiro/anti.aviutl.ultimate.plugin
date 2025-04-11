#pragma once

namespace apn::ease_select_ui
{
	//
	// このクラスはイージンググラフを表示します。
	//
	inline class EaseWindow : public CWnd
	{
	public:

		inline static constexpr struct Points {
			inline static constexpr int c_none = -1;
			inline static constexpr int c_first = 0;
			inline static constexpr int c_second= 1;
			inline static constexpr int c_max_size = 2;
		} c_points;

		inline static constexpr struct Position {
			inline static constexpr int c_start_base = 25;
			inline static constexpr int c_end_base = 75;

			inline static constexpr int c_start_x = 0;
			inline static constexpr int c_start_y = 0;
			inline static constexpr int c_end_x = 99;
			inline static constexpr int c_end_y = 99;
		} c_position;

		CPoint points[2] =
		{
			{ c_position.c_end_base, c_position.c_start_base },
			{ c_position.c_start_base, c_position.c_end_base },
		};
		int hot = c_points.c_none;

		BOOL enabled = TRUE;
		std::wstring origin = _T("easing");
		BOOL clamp = TRUE;
		std::wstring horz = _T("left");
		std::wstring vert = _T("center");
		int alpha = 255;
		int margin = 16;
		int hit_distance = 0;
		CSize window_size = { 400, 400 };
		Color background_color = { 255, 255, 255, 255 };
		Color border_color = { 255, 224, 224, 224 };
		REAL border_width = 4.0f;
		Color curve_color = Color(255, 0, 0, 0);
		REAL curve_width = 4.0f;
		Color invalid_curve_color = Color(255, 255, 0, 0);
		REAL invalid_curve_width = 4.0f;
		Color handle_color = Color(255, 128, 192, 255);
		REAL handle_width = 4.0f;
		Color point_color = Color(255, 0, 128, 255);
		Color hot_point_color = Color(255, 224, 0, 0);
		int point_radius = 10;
		int segment_count = 100;
		BOOL hide_cursor = FALSE;
		BOOL immediately = FALSE;

		//
		// ベジェ曲線の座標を計算して返します。
		//
		inline static double bezier(double p0, double p1, double p2, double p3, double t)
		{
			double s = 1 - t;
			return
				1 * s * s * s * p0 +
				3 * s * s * t * p1 +
				3 * s * t * t * p2 +
				1 * t * t * t * p3;
		}

		//
		// 初期化処理を実行します。
		//
		BOOL init(CWnd* parent)
		{
			MY_TRACE_FUNC("{/hex}", parent);

			return __super::CreateEx(0, nullptr, nullptr, WS_POPUP, CRect(0, 0, 0, 0), parent, 0);
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return DestroyWindow();
		}

		//
		// ワークエリア矩形を返します。
		//
		auto get_workarea()
		{
			CRect rc; GetClientRect(&rc);
			::InflateRect(rc, -margin, -margin);
			return rc;
		}

		//
		// 論理座標をクライアント座標に変換します。
		//
		POINT logical_to_client(POINT point)
		{
			auto rc = get_workarea();
			return logical_to_client(point, &rc);
		}

		//
		// 論理座標をクライアント座標に変換します。
		//
		POINT logical_to_client(POINT point, LPCRECT rc)
		{
			auto cw = rc->right - rc->left;
			auto ch = rc->bottom - rc->top;
			auto lw = c_position.c_end_x - c_position.c_start_x;
			auto lh = c_position.c_end_y - c_position.c_start_y;

			auto fx = (double)(point.x - c_position.c_start_x) / lw;
			auto fy = (double)(point.y - c_position.c_start_y) / lh;

			fy = 1.0 - fy;

			return CPoint(
				rc->left + (int)(cw * fx),
				rc->top  + (int)(ch * fy));
		}

		//
		// クライアント座標を論理座標に変換します。
		//
		POINT client_to_logical(POINT point)
		{
			auto rc = get_workarea();
			return client_to_logical(point, &rc);
		}

		//
		// クライアント座標を論理座標に変換します。
		//
		POINT client_to_logical(POINT point, LPCRECT rc)
		{
			auto cw = rc->right - rc->left;
			auto ch = rc->bottom - rc->top;
			auto lw = c_position.c_end_x - c_position.c_start_x;
			auto lh = c_position.c_end_y - c_position.c_start_y;

			auto fx = (double)(point.x - rc->left) / cw;
			auto fy = (double)(point.y - rc->top) / ch;

			fy = 1.0 - fy;

			CPoint ret_value(
				c_position.c_start_x + (int)std::round(lw * fx),
				c_position.c_start_y + (int)std::round(lh * fy));

			ret_value.x = std::clamp<int>(ret_value.x, c_position.c_start_x, c_position.c_end_x);
			ret_value.y = std::clamp<int>(ret_value.y, c_position.c_start_y, c_position.c_end_y);

			return ret_value;
		}

		//
		// ヒットテストを行います。
		//
		int hittest(POINT point)
		{
			auto rc = get_workarea();

			for (int i = 0; i < c_points.c_max_size; i++)
			{
				auto point2 = logical_to_client(points[i], &rc);

				if (std::abs(point.x - point2.x) <= hit_distance + point_radius &&
					std::abs(point.y - point2.y) <= hit_distance + point_radius)
				{
					return i;
				}
			}

			return c_points.c_none;
		}

		//
		// イージング数値を文字列にして表示します。
		//
		void output_ease_text()
		{
			auto str = my::format(
				_T("({/}, {/}) ({/}, {/}) - {:02d}{:02d}{:02d}{:02d}"),
				points[c_points.c_first].x,
				points[c_points.c_first].y,
				points[c_points.c_second].x,
				points[c_points.c_second].y,
				points[c_points.c_first].x,
				points[c_points.c_first].y,
				points[c_points.c_second].x,
				points[c_points.c_second].y);

			SetWindowText(str.c_str());
		}

		//
		// ターゲットウィンドウからイージング数値を受け取ります。
		//
		void receive_number()
		{
			MY_TRACE_FUNC("");

			auto target = ::GetForegroundWindow();
			if (!target) return;
			MY_TRACE_HWND(target);

			auto child = ::GetWindow(target, GW_CHILD);
			if (!child) return;
			MY_TRACE_HWND(child);
			MY_TRACE_INT(::GetDlgCtrlID(child));

			TCHAR text[MAX_PATH] = {};
			::SendMessage(child, WM_GETTEXT, std::size(text), (LPARAM)text);
			MY_TRACE_STR(text);

			auto n = _ttoi(text);
			if (n < 0) return;

			points[c_points.c_first].x  = std::clamp<int>(n / 100 / 100 / 100 % 100, c_position.c_start_x, c_position.c_end_x);
			points[c_points.c_first].y  = std::clamp<int>(n / 100 / 100 % 100, c_position.c_start_y, c_position.c_end_y);
			points[c_points.c_second].x = std::clamp<int>(n / 100 % 100, c_position.c_start_x, c_position.c_end_x);
			points[c_points.c_second].y = std::clamp<int>(n % 100, c_position.c_start_y, c_position.c_end_y);
		}

		//
		// ターゲットウィンドウにイージング数値を送信します。
		//
		void send_number()
		{
			MY_TRACE_FUNC("");

			auto target = ::GetForegroundWindow();
			if (!target) return;
			MY_TRACE_HWND(target);

			auto child = ::GetWindow(target, GW_CHILD);
			if (!child) return;
			MY_TRACE_HWND(child);
			MY_TRACE_INT(::GetDlgCtrlID(child));

			auto str = my::format(_T("{:02d}{:02d}{:02d}{:02d}"),
				points[c_points.c_first].x, points[c_points.c_first].y,
				points[c_points.c_second].x, points[c_points.c_second].y);
			::SendMessage(child, WM_SETTEXT, 0, (LPARAM)str.c_str());
			::PostMessage(target, WM_COMMAND, IDOK, 0);
		}

		//
		// ウィンドウを表示します。
		//
		void show(HWND number_window, HWND easing_window)
		{
			MY_TRACE_FUNC("{/hex}, {/hex}", number_window, easing_window);

			if (!enabled) return;
			if (IsWindowVisible()) return;

			// ターゲットを取得します。
			auto target = number_window;
			if (origin == _T("easing")) target = easing_window;
			MY_TRACE_HWND(target);

			// 現在値を受け取ります。
			receive_number();
			output_ease_text();

			// ホット状態を初期化します。
			hot = c_points.c_none;

			// 幅と高さを取得します。
			int w, h;
			{
				// クライアントサイズを設定します。
				CRect rc(0, 0, window_size.cx, window_size.cy);
				my::client_to_window(GetSafeHwnd(), &rc);
				w = rc.Width();
				h = rc.Height();
			}

			// x と y を取得します。
			int x, y;
			{
				CRect rc_target; ::GetWindowRect(target, &rc_target);

				if (horz == _T("left"))
				{
					x = rc_target.left - w;
				}
				else if (horz == _T("right"))
				{
					x = rc_target.right;
				}
				else
				{
					x = rc_target.CenterPoint().x - w / 2;
				}

				if (vert == _T("top"))
				{
					y = rc_target.top - h;
				}
				else if (vert == _T("bottom"))
				{
					y = rc_target.bottom;
				}
				else
				{
					y = rc_target.CenterPoint().y - h / 2;
				}

				if (clamp)
				{
					auto rc_monitor = my::get_monitor_rect(target);

					if (x < rc_monitor.left)
						x = rc_monitor.left;
					else if (x + w > rc_monitor.right)
						x = rc_monitor.right - w;

					if (y < rc_monitor.top)
						y = rc_monitor.top;
					else if (y + h > rc_monitor.bottom)
						y = rc_monitor.bottom - h;
				}
			}

			SetLayeredWindowAttributes(0, alpha, LWA_ALPHA);
			SetWindowPos(&wndTopMost, x, y, w, h, SWP_NOACTIVATE);
			ShowWindow(SW_SHOWNA);
			Invalidate(FALSE);
		}

		//
		// ウィンドウを非表示にします。
		//
		void hide()
		{
			MY_TRACE_FUNC("");

			if (!IsWindowVisible()) return;

			ShowWindow(SW_HIDE);
		}

		BOOL PreCreateWindow(CREATESTRUCT& cs)
		{
			MY_TRACE_FUNC("");

			cs.style = WS_POPUP | WS_CAPTION | WS_THICKFRAME | WS_SYSMENU |
				WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
			cs.dwExStyle = WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_LAYERED;
			cs.lpszClass = AfxRegisterWndClass(
				CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
				::LoadCursor(nullptr, IDC_ARROW));

			if (!__super::PreCreateWindow(cs))
				return FALSE;

			return TRUE;
		}

		void OnClose()
		{
			send_number();

//			__super::OnClose();
		}

		void OnPaint() 
		{
			CDoubleBufferPaintDC dc(this);

			Graphics g(dc);

			g.SetSmoothingMode(SmoothingModeAntiAlias);
//			g.SetCompositingMode(CompositingModeSourceOver);
//			g.SetTextRenderingHint(TextRenderingHintAntiAlias);
			g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
//			g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
			g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
//			g.TranslateTransform(-0.5f, -0.5f);

			g.Clear(background_color);

			{
				CRect rc; GetClientRect(&rc);
				auto text = L"✖を押すと適用されます↑";
				Gdiplus::FontFamily font_family(L"メイリオ");
				Gdiplus::Font font(&font_family, 12, FontStyleBold, UnitPoint);
				Gdiplus::SolidBrush brush(border_color);
				Gdiplus::StringFormat format;
				format.SetAlignment(StringAlignmentFar);
				format.SetLineAlignment(StringAlignmentNear);

				g.DrawString(text, -1, &font, MyRectF(rc), &format, &brush);
			}

			auto rc = get_workarea();
			auto rc_paint = rc;
			rc_paint.right--; rc_paint.bottom--;

			MyPoint start(logical_to_client(CPoint(c_position.c_start_base, c_position.c_start_base), &rc_paint));
			MyPoint end(logical_to_client(CPoint(c_position.c_end_base, c_position.c_end_base), &rc_paint));
			MyPoint first(logical_to_client(points[c_points.c_first], &rc_paint));
			MyPoint second(logical_to_client(points[c_points.c_second], &rc_paint));

			{
				// 外枠を描画します。

				Pen pen(border_color, border_width);
				Rect rect;
				rect.X = start.X;
				rect.Y = end.Y;
				rect.Width = end.X - start.X;
				rect.Height = start.Y - end.Y;
				g.DrawRectangle(&pen, rect);
			}

			{
				// ハンドルを描画します。

				Pen pen(handle_color, handle_width);
				g.DrawLine(&pen, start, first);
				g.DrawLine(&pen, second, end);
			}

			{
				// 制御点を描画します。

				SolidBrush brush(point_color);
				SolidBrush hot_brush(hot_point_color);

				for (int i = 0; i < c_points.c_max_size; i++)
				{
					auto point = logical_to_client(points[i]);

					Rect rect;
					rect.X = point.x - point_radius;
					rect.Y = point.y - point_radius;
					rect.Width = point_radius * 2;
					rect.Height = point_radius * 2;

					if (i == hot)
						g.FillEllipse(&hot_brush, rect);
					else
						g.FillEllipse(&brush, rect);
				}
			}

			BOOL invalid = FALSE;

			{
				// ベジェ曲線が有効かチェックする。

				auto prev = (double)start.X;

				for (auto i = 1; i < segment_count; i++)
				{
					auto t = (double)i / segment_count;
					auto x = bezier(start.X, first.X, second.X, end.X, t);

					if (x <= prev) // x が前回より小さかったら無効。
					{
						invalid = TRUE;
						break;
					}

					prev = x;
				}
			}

			{
				// ベジェ曲線を描画します。

				auto color = curve_color;
				auto width = curve_width;

				if (invalid)
				{
					color = invalid_curve_color;
					width = invalid_curve_width;
				}

				Pen pen(color, width);
				g.DrawBezier(&pen, start, first, second, end);
			}
		}

		BOOL OnNcActivate(BOOL bActive)
		{
			MY_TRACE_FUNC("{/}", bActive);

			return __super::OnNcActivate(bActive);
		}

		void OnLButtonDown(UINT nFlags, CPoint point)
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}", nFlags, point.x, point.y);

			hot = hittest(point);
			if (hot != c_points.c_none)
			{
				MY_TRACE("ドラッグを開始します\n");

				SetCapture();
				if (hide_cursor) ::ShowCursor(FALSE);
				Invalidate(FALSE);
			}

			__super::OnLButtonDown(nFlags, point);
		}

		void OnLButtonUp(UINT nFlags, CPoint point)
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}", nFlags, point.x, point.y);

			if (GetCapture() == this)
			{
				MY_TRACE("ドラッグを終了します\n");

				hot = c_points.c_none;
				ReleaseCapture();
				Invalidate(FALSE);
			}

			__super::OnLButtonUp(nFlags, point);
		}

		void OnLButtonDblClk(UINT nFlags, CPoint point)
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}", nFlags, point.x, point.y);

			send_number();

			__super::OnLButtonDblClk(nFlags, point);
		}

		void OnRButtonDown(UINT nFlags, CPoint point)
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}", nFlags, point.x, point.y);

			__super::OnRButtonDown(nFlags, point);
		}

		void OnRButtonUp(UINT nFlags, CPoint point)
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}", nFlags, point.x, point.y);

			__super::OnRButtonUp(nFlags, point);
		}

		void OnRButtonDblClk(UINT nFlags, CPoint point)
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}", nFlags, point.x, point.y);

			__super::OnRButtonDblClk(nFlags, point);
		}

		void OnMouseMove(UINT nFlags, CPoint point)
		{
			// ドラッグ中の場合は
			if (GetCapture() == this)
			{
				if (hot >= 0 && hot < c_points.c_max_size)
				{
					points[hot] = client_to_logical(point);
					Invalidate(FALSE);

					output_ease_text();
				}
			}
			// ドラッグ中ではない場合は
			else
			{
				auto ht = hittest(point);
				if (hot != ht)
				{
					hot = ht;
					Invalidate(FALSE);

					TRACKMOUSEEVENT tme = { sizeof(tme) };
					tme.dwFlags = TME_LEAVE;
					tme.hwndTrack = GetSafeHwnd();
					::TrackMouseEvent(&tme);
				}
			}

			__super::OnMouseMove(nFlags, point);
		}

		void OnMouseLeave()
		{
			MY_TRACE_FUNC("");

			// ホット状態を解除します。
			hot = c_points.c_none;
			Invalidate(FALSE);

			__super::OnMouseLeave();
		}

		void OnCaptureChanged(CWnd* pWnd)
		{
			MY_TRACE_FUNC("");

			if (hide_cursor) ::ShowCursor(TRUE);

			__super::OnCaptureChanged(pWnd);
		}

		void OnSize(UINT nType, int cx, int cy)
		{
			__super::OnSize(nType, cx, cy);

			Invalidate(FALSE);
		}

		DECLARE_MESSAGE_MAP()
	} ease_window;

	BEGIN_MESSAGE_MAP(EaseWindow, CWnd)
		ON_WM_CLOSE()
		ON_WM_PAINT()
		ON_WM_NCACTIVATE()
		ON_WM_LBUTTONDOWN()
		ON_WM_LBUTTONUP()
		ON_WM_LBUTTONDBLCLK()
		ON_WM_RBUTTONDOWN()
		ON_WM_RBUTTONUP()
		ON_WM_RBUTTONDBLCLK()
		ON_WM_MOUSEMOVE()
		ON_WM_MOUSELEAVE()
		ON_WM_CAPTURECHANGED()
		ON_WM_SIZE()
	END_MESSAGE_MAP()
}
