#include "pch.h"
#include "EasingSelectGui.h"
#include "EaseWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

inline int clamp(int n, int min, int max)
{
	if (n < min) return min;
	if (n > max) return max;
	return n;
}

inline double bezier(double p0, double p1, double p2, double p3, double t)
{
	double s = 1 - t;
	return
		1 * s * s * s * p0 +
		3 * s * s * t * p1 +
		3 * s * t * t * p2 +
		1 * t * t * t * p3;
}

CEaseWindow::CEaseWindow()
{
	MY_TRACE_FUNC("");

	m_points[Points::first].x = Position::endBase;
	m_points[Points::first].y = Position::startBase;
	m_points[Points::second].x = Position::startBase;
	m_points[Points::second].y = Position::endBase;
	m_hot = Points::none;

	m_enable = TRUE;
	m_origin = _T("easing");
	m_clamp = TRUE;
	m_horz = _T("left");
	m_vert = _T("center");
	m_alpha = 255;
	m_margin = 16;
	m_hitDistance = 0;
	m_windowSize.cx = 400;
	m_windowSize.cy = 400;
	m_backgroundColor = Color(255, 255, 255, 255);
	m_borderColor = Color(255, 224, 224, 224);
	m_borderWidth = 4.0f;
	m_curveColor = Color(255, 0, 0, 0);
	m_curveWidth = 4.0f;
	m_invalidCurveColor = Color(255, 255, 0, 0);
	m_invalidCurveWidth = 4.0f;
	m_handleColor = Color(255, 128, 192, 255);
	m_handleWidth = 4.0f;
	m_pointColor = Color(255, 0, 128, 255);
	m_hotPointColor = Color(255, 224, 0, 0);
	m_pointRadius = 16;
	m_segmentCount = 100;
	m_hideCursor = FALSE;
	m_immediately = FALSE;
}

CEaseWindow::~CEaseWindow()
{
	MY_TRACE_FUNC("");
}

void CEaseWindow::getWorkarea(LPRECT rc)
{
	GetClientRect(rc);
	::InflateRect(rc, -m_margin, -m_margin);
}

POINT CEaseWindow::LPToClient(POINT point)
{
	CRect rc; getWorkarea(&rc);
	return LPToClient(point, &rc);
}

POINT CEaseWindow::LPToClient(POINT point, LPCRECT rc)
{
	int cw = rc->right - rc->left;
	int ch = rc->bottom - rc->top;
	int lw = Position::endX - Position::startX;
	int lh = Position::endY - Position::startY;

	double fx = (double)(point.x - Position::startX) / lw;
	double fy = (double)(point.y - Position::startY) / lh;

	fy = 1.0 - fy;

	return CPoint(
		rc->left + (int)(cw * fx),
		rc->top  + (int)(ch * fy));
}

POINT CEaseWindow::ClientToLP(POINT point)
{
	CRect rc; getWorkarea(&rc);
	return ClientToLP(point, &rc);
}

POINT CEaseWindow::ClientToLP(POINT point, LPCRECT rc)
{
	int cw = rc->right - rc->left;
	int ch = rc->bottom - rc->top;
	int lw = Position::endX - Position::startX;
	int lh = Position::endY - Position::startY;

	double fx = (double)(point.x - rc->left) / cw;
	double fy = (double)(point.y - rc->top) / ch;

	fy = 1.0 - fy;

	CPoint retValue(
		Position::startX + (int)round(lw * fx),
		Position::startY + (int)round(lh * fy));

	retValue.x = clamp(retValue.x, Position::startX, Position::endX);
	retValue.y = clamp(retValue.y, Position::startY, Position::endY);

	return retValue;
}

int CEaseWindow::hitTest(POINT point)
{
	CRect rc; getWorkarea(&rc);

	for (int i = 0; i < Points::maxSize; i++)
	{
		CPoint point2 = LPToClient(m_points[i], &rc);

		if (abs(point.x - point2.x) <= m_hitDistance + m_pointRadius &&
			abs(point.y - point2.y) <= m_hitDistance + m_pointRadius)
		{
			return i;
		}
	}

	return Points::none;
}

void CEaseWindow::outputEaseText()
{
	CString text;
	text.Format(_T("(%d, %d) (%d, %d) - %02d%02d%02d%02d"),
		m_points[Points::first].x,
		m_points[Points::first].y,
		m_points[Points::second].x,
		m_points[Points::second].y,
		m_points[Points::first].x,
		m_points[Points::first].y,
		m_points[Points::second].x,
		m_points[Points::second].y);

	SetWindowText(text);
}

void CEaseWindow::receiveNumber()
{
	MY_TRACE_FUNC("");

	HWND target = ::GetForegroundWindow();
	if (!target) return;
	MY_TRACE_HWND(target);

	HWND child = ::GetWindow(target, GW_CHILD);
	if (!child) return;
	MY_TRACE_HWND(child);
	MY_TRACE_INT(::GetDlgCtrlID(child));

	TCHAR text[MAX_PATH] = {};
	::SendMessage(child, WM_GETTEXT, std::size(text), (LPARAM)text);
	MY_TRACE_TSTR(text);

	int n = _ttoi(text);
	if (n < 0) return;

	m_points[Points::first].x  = clamp(n / 100 / 100 / 100 % 100, Position::startX, Position::endX);
	m_points[Points::first].y  = clamp(n / 100 / 100 % 100, Position::startY, Position::endY);
	m_points[Points::second].x = clamp(n / 100 % 100, Position::startX, Position::endX);
	m_points[Points::second].y = clamp(n % 100, Position::startY, Position::endY);
}

void CEaseWindow::sendNumber()
{
	MY_TRACE_FUNC("");

	HWND target = ::GetForegroundWindow();
	if (!target)
		return;

	MY_TRACE_HWND(target);

	HWND child = ::GetWindow(target, GW_CHILD);
	if (!child)
		return;

	MY_TRACE_HWND(child);
	MY_TRACE_INT(::GetDlgCtrlID(child));

	TCHAR text[MAX_PATH] = {};
	::StringCbPrintf(text, sizeof(text), _T("%02d%02d%02d%02d"),
		m_points[Points::first].x, m_points[Points::first].y,
		m_points[Points::second].x, m_points[Points::second].y);
	::SendMessage(child, WM_SETTEXT, 0, (LPARAM)text);
	::PostMessage(target, WM_COMMAND, IDOK, 0);
}

void CEaseWindow::show(HWND numberWindow, HWND easingWindow)
{
	MY_TRACE_FUNC("0x%08X, 0x%08X", numberWindow, easingWindow);

	if (!m_enable) return;
	if (IsWindowVisible()) return;

	// ターゲットを取得する。
	HWND target = numberWindow;
	if (::lstrcmpi(m_origin, _T("easing")) == 0)
		target = easingWindow;
	MY_TRACE_HWND(target);

	// 現在値を受け取る。
	receiveNumber();
	outputEaseText();

	// ホット状態を初期化する。
	m_hot = Points::none;

	// 幅と高さを取得する。
	int w, h;
	{
		// クライアントサイズを設定する。
		CRect rc(0, 0, m_windowSize.cx, m_windowSize.cy);
		clientToWindow(GetSafeHwnd(), &rc);
		w = rc.Width();
		h = rc.Height();
	}

	// x と y を取得する。
	int x, y;
	{
		CRect rcTarget; ::GetWindowRect(target, &rcTarget);

		if (::lstrcmpi(m_horz, _T("left")) == 0)
		{
			x = rcTarget.left - w;
		}
		else if (::lstrcmpi(m_horz, _T("right")) == 0)
		{
			x = rcTarget.right;
		}
		else
		{
			x = rcTarget.CenterPoint().x - w / 2;
		}

		if (::lstrcmpi(m_vert, _T("top")) == 0)
		{
			y = rcTarget.top - h;
		}
		else if (::lstrcmpi(m_vert, _T("bottom")) == 0)
		{
			y = rcTarget.bottom;
		}
		else
		{
			y = rcTarget.CenterPoint().y - h / 2;
		}

		if (m_clamp)
		{
			HMONITOR monitor = ::MonitorFromWindow(target, MONITOR_DEFAULTTONEAREST);
			MONITORINFO mi = { sizeof(mi) }; ::GetMonitorInfo(monitor, &mi);

			if (x < mi.rcWork.left)
				x = mi.rcWork.left;
			else if (x + w > mi.rcWork.right)
				x = mi.rcWork.right - w;

			if (y < mi.rcWork.top)
				y = mi.rcWork.top;
			else if (y + h > mi.rcWork.bottom)
				y = mi.rcWork.bottom - h;
		}
	}

	SetLayeredWindowAttributes(0, m_alpha, LWA_ALPHA);
	SetWindowPos(&wndTopMost, x, y, w, h, SWP_NOACTIVATE);
	ShowWindow(SW_SHOWNA);
	Invalidate(FALSE);
}

void CEaseWindow::hide()
{
	MY_TRACE_FUNC("");

	if (!IsWindowVisible()) return;

	ShowWindow(SW_HIDE);
}

BOOL CEaseWindow::Create(CWnd* parent)
{
	MY_TRACE_FUNC("0x%08X", parent);

	return CWnd::CreateEx(0, 0, 0, WS_POPUP, CRect(0, 0, 0, 0), parent, 0);
}

BOOL CEaseWindow::PreCreateWindow(CREATESTRUCT& cs)
{
	MY_TRACE_FUNC("");

	cs.style = WS_POPUP | WS_CAPTION | WS_THICKFRAME | WS_SYSMENU |
		WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	cs.dwExStyle = WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_LAYERED;
	cs.lpszClass = AfxRegisterWndClass(
		CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(0, IDC_ARROW));

	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	return TRUE;
}

BEGIN_MESSAGE_MAP(CEaseWindow, CWnd)
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

void CEaseWindow::OnClose()
{
	sendNumber();

//	CWnd::OnClose();
}

void CEaseWindow::OnPaint() 
{
	CDoubleBufferPaintDC dc(this);

	Graphics g(dc);

	g.SetSmoothingMode(SmoothingModeAntiAlias);
//	g.SetCompositingMode(CompositingModeSourceOver);
//	g.SetTextRenderingHint(TextRenderingHintAntiAlias);
	g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
//	g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
	g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
//	g.TranslateTransform(-0.5f, -0.5f);

	g.Clear(m_backgroundColor);

	{
		CRect rc; GetClientRect(&rc);
		LPCWSTR text = L"✖を押すと適用されます↑";
		Gdiplus::FontFamily fontFamily(L"メイリオ");
		Gdiplus::Font font(&fontFamily, 12, FontStyleBold, UnitPoint);
		Gdiplus::SolidBrush brush(m_borderColor);
		Gdiplus::StringFormat format;
		format.SetAlignment(StringAlignmentFar);
		format.SetLineAlignment(StringAlignmentNear);

		g.DrawString(text, -1, &font, MyRectF(rc), &format, &brush);
	}

	CRect rc; getWorkarea(&rc);
	CRect rcPaint = rc;
	rcPaint.right--; rcPaint.bottom--;

	MyPoint start(LPToClient(CPoint(Position::startBase, Position::startBase), &rcPaint));
	MyPoint end(LPToClient(CPoint(Position::endBase, Position::endBase), &rcPaint));
	MyPoint first(LPToClient(m_points[Points::first], &rcPaint));
	MyPoint second(LPToClient(m_points[Points::second], &rcPaint));

	{
		// 外枠を描画する。

		Pen pen(m_borderColor, m_borderWidth);
		Rect rect;
		rect.X = start.X;
		rect.Y = end.Y;
		rect.Width = end.X - start.X;
		rect.Height = start.Y - end.Y;
		g.DrawRectangle(&pen, rect);
	}

	{
		// ハンドルを描画する。

		Pen pen(m_handleColor, m_handleWidth);
		g.DrawLine(&pen, start, first);
		g.DrawLine(&pen, second, end);
	}

	{
		// 制御点を描画する。

		SolidBrush brush(m_pointColor);
		SolidBrush hotBrush(m_hotPointColor);

		for (int i = 0; i < Points::maxSize; i++)
		{
			CPoint point = LPToClient(m_points[i]);

			Rect rect;
			rect.X = point.x - m_pointRadius;
			rect.Y = point.y - m_pointRadius;
			rect.Width = m_pointRadius * 2;
			rect.Height = m_pointRadius * 2;

			if (i == m_hot)
				g.FillEllipse(&hotBrush, rect);
			else
				g.FillEllipse(&brush, rect);
		}
	}

	BOOL invalid = FALSE;

	{
		// ベジェ曲線が有効かチェックする。

		double prev = start.X;

		for (int i = 1; i < m_segmentCount; i++)
		{
			double t = (double)i / m_segmentCount;
			double x = bezier(start.X, first.X, second.X, end.X, t);

			if (x <= prev) // x が前回より小さかったら無効。
			{
				invalid = TRUE;
				break;
			}

			prev = x;
		}
	}

	{
		// ベジェ曲線を描画する。

		Color color = m_curveColor;
		REAL width = m_curveWidth;

		if (invalid)
		{
			color = m_invalidCurveColor;
			width = m_invalidCurveWidth;
		}

		Pen pen(color, width);
		g.DrawBezier(&pen, start, first, second, end);
	}
}

BOOL CEaseWindow::OnNcActivate(BOOL bActive)
{
	MY_TRACE_FUNC("%d", bActive);

	return CWnd::OnNcActivate(bActive);
}

void CEaseWindow::OnLButtonDown(UINT nFlags, CPoint point)
{
	MY_TRACE_FUNC("0x%08X, %d, %d", nFlags, point.x, point.y);

	m_hot = hitTest(point);
	if (m_hot != Points::none)
	{
		MY_TRACE(_T("ドラッグを開始します\n"));

		SetCapture();
		if (m_hideCursor) ::ShowCursor(FALSE);
		Invalidate(FALSE);
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CEaseWindow::OnLButtonUp(UINT nFlags, CPoint point)
{
	MY_TRACE_FUNC("0x%08X, %d, %d", nFlags, point.x, point.y);

	if (GetCapture() == this)
	{
		MY_TRACE(_T("ドラッグを終了します\n"));

		m_hot = Points::none;
		ReleaseCapture();
		Invalidate(FALSE);
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CEaseWindow::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	MY_TRACE_FUNC("0x%08X, %d, %d", nFlags, point.x, point.y);

	sendNumber();

	CWnd::OnLButtonDblClk(nFlags, point);
}

void CEaseWindow::OnRButtonDown(UINT nFlags, CPoint point)
{
	MY_TRACE_FUNC("0x%08X, %d, %d", nFlags, point.x, point.y);

	CWnd::OnRButtonDown(nFlags, point);
}

void CEaseWindow::OnRButtonUp(UINT nFlags, CPoint point)
{
	MY_TRACE_FUNC("0x%08X, %d, %d", nFlags, point.x, point.y);

	CWnd::OnRButtonUp(nFlags, point);
}

void CEaseWindow::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	MY_TRACE_FUNC("0x%08X, %d, %d", nFlags, point.x, point.y);

	CWnd::OnRButtonDblClk(nFlags, point);
}

void CEaseWindow::OnMouseMove(UINT nFlags, CPoint point)
{
	if (GetCapture() == this)
	{
		// ドラッグ中。

		if (m_hot >= 0 && m_hot < Points::maxSize)
		{
			m_points[m_hot] = ClientToLP(point);
			Invalidate(FALSE);

			outputEaseText();
		}
	}
	else
	{
		// ドラッグ中ではない。

		int hot = hitTest(point);
		if (m_hot != hot)
		{
			m_hot = hot;
			Invalidate(FALSE);

			TRACKMOUSEEVENT tme = { sizeof(tme) };
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = GetSafeHwnd();
			::TrackMouseEvent(&tme);
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CEaseWindow::OnMouseLeave()
{
	MY_TRACE_FUNC("");

	// ホット状態を解除する。
	m_hot = Points::none;
	Invalidate(FALSE);

	CWnd::OnMouseLeave();
}

void CEaseWindow::OnCaptureChanged(CWnd* pWnd)
{
	MY_TRACE_FUNC("");

	if (m_hideCursor) ::ShowCursor(TRUE);

	CWnd::OnCaptureChanged(pWnd);
}

void CEaseWindow::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	Invalidate(FALSE);
}
