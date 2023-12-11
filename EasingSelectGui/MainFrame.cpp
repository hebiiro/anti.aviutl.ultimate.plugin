#include "pch.h"
#include "EasingSelectGui.h"
#include "MainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMainFrame::CMainFrame() noexcept
{
	MY_TRACE_FUNC("");

	CString fileName = theApp.getFullPath(_T("EasingSelect.xml"));
	MY_TRACE_TSTR((LPCTSTR)fileName);

	m_fileUpdateChecker.init(fileName);
	m_isSettingsFileLoaded = FALSE;

	m_imageVersion = 1;
	m_clamp = FALSE;
	m_horz = _T("left");
	m_vert = _T("center");
	m_alpha = 255;
	m_scale = 100;
	m_selectedColor = Color(0x80, 0xff, 0x00, 0x00);
	m_hotColor = Color(0x80, 0x00, 0xff, 0x00);

	m_currentPart = -1;
	m_hotPart = -1;
}

CMainFrame::~CMainFrame()
{
	MY_TRACE_FUNC("");
}

void CMainFrame::loadImage()
{
	MY_TRACE_FUNC("");

	if (!m_image.IsNull()) m_image.Destroy();
	m_parts.clear();

	switch (m_imageVersion)
	{
	case 3:
		{
			CString path = theApp.getFullPath(_T("easing3.png"));
			MY_TRACE_TSTR((LPCTSTR)path);

			m_image.Load(path);

			if (!m_image.IsNull())
			{
				int w = m_image.GetWidth() * m_scale / 100;
				int h = m_image.GetHeight() * m_scale / 100;

				SetWindowPos(0, 0, 0, w, h,
					SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
			}

			m_parts.resize(41);

			// (1) の位置。
			int x = 20;
			int y = 100;

			setRect(1, x, y);

			for (int i = 0; i < 5; i++)
			{
				y += 160;

				// (2) と (6) の位置。
				int x1 = 20;
				int x2 = 735;

				for (int j = 0; j < 4; j++)
				{
					setRect((i * 8) + j + 2 + 0, x1, y);
					setRect((i * 8) + j + 2 + 4, x2, y);

					x1 += 175;
					x2 += 175;
				}
			}

			break;
		}
	case 2:
		{
			CString path = theApp.getFullPath(_T("easing2.png"));
			MY_TRACE_TSTR((LPCTSTR)path);

			m_image.Load(path);

			if (!m_image.IsNull())
			{
				int w = m_image.GetWidth() * m_scale / 100;
				int h = m_image.GetHeight() * m_scale / 100;

				SetWindowPos(0, 0, 0, w, h,
					SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
			}

			m_parts.resize(41);

			int x = 20;
			int y = 98;

			setRect(1, x, y);

			for (int i = 0; i < 5; i++)
			{
				y += 155;

				int x1 = 20;
				int x2 = 695;

				for (int j = 0; j < 4; j++)
				{
					setRect((i * 8) + j + 2 + 0, x1, y);
					setRect((i * 8) + j + 2 + 4, x2, y);

					x1 += 163;
					x2 += 163;
				}
			}

			break;
		}
	default:
		{
			CString path = theApp.getFullPath(_T("easing.png"));
			MY_TRACE_TSTR((LPCTSTR)path);

			m_image.Load(path);

			if (!m_image.IsNull())
			{
				int w = m_image.GetWidth() * m_scale / 100;
				int h = m_image.GetHeight() * m_scale / 100;

				SetWindowPos(0, 0, 0, w, h,
					SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
			}

			m_parts.resize(41);

			int x = 103;
			int y = 40;

			setRect(1, x, y);

			for (int i = 0; i < 5; i++)
			{
				y += 178;

				int x1 = 103;
				int x2 = 649;

				for (int j = 0; j < 4; j++)
				{
					setRect((i * 8) + j + 2 + 0, x1, y);
					setRect((i * 8) + j + 2 + 4, x2, y);

					x1 += 125;
					x2 += 125;
				}
			}

			break;
		}
	}
}

void CMainFrame::setRect(int number, int _x, int _y)
{
	MY_TRACE_FUNC("%d, %d, %d", number, _x, _y);

	switch (m_imageVersion)
	{
	case 3:
		{
			int x = ::MulDiv(_x, m_scale, 100);
			int y = ::MulDiv(_y, m_scale, 100);
			int w = ::MulDiv(_x + 160, m_scale, 100);
			int h = ::MulDiv(_y + 120, m_scale, 100);

			m_parts[number - 1].SetRect(x, y, w, h);

			break;
		}
	case 2:
		{
			int x = ::MulDiv(_x, m_scale, 100);
			int y = ::MulDiv(_y, m_scale, 100);
			int w = ::MulDiv(_x + 140, m_scale, 100);
			int h = ::MulDiv(_y + 108, m_scale, 100);

			m_parts[number - 1].SetRect(x, y, w, h);

			break;
		}
	default:
		{
			int x = ::MulDiv(_x, m_scale, 100);
			int y = ::MulDiv(_y, m_scale, 100);
			int w = ::MulDiv(_x + 121, m_scale, 100);
			int h = ::MulDiv(_y + 101, m_scale, 100);

			m_parts[number - 1].SetRect(x, y, w, h);

			break;
		}
	}
}

void CMainFrame::setHotPart(int index)
{
	if (m_hotPart == index)
		return;

	m_hotPart = index;
	Invalidate(FALSE);
}

HWND CMainFrame::getTarget()
{
	HWND hwnd = ::GetForegroundWindow();
	DWORD pid = 0;
	DWORD tid = ::GetWindowThreadProcessId(hwnd, &pid);

//	MY_TRACE_HWND(hwnd);
//	MY_TRACE_HEX(pid);
//	MY_TRACE_HEX(theApp.m_mainProcessId);

	if (pid != theApp.m_mainProcessId)
		return 0;

	TCHAR text[MAX_PATH];
	::GetWindowText(hwnd, text, std::size(text));

//	MY_TRACE_TSTR(text);

	if (::lstrcmp(text, _T("移動フレーム間隔")) != 0)
		return 0;

	return hwnd;
}

int CMainFrame::getEasing()
{
	MY_TRACE_FUNC("");

	HWND target = ::GetForegroundWindow();
	if (!target) return -1;
	MY_TRACE_HWND(target);

	HWND child = ::GetWindow(target, GW_CHILD);
	if (!child) return -1;
	MY_TRACE_HWND(child);
	MY_TRACE_INT(::GetDlgCtrlID(child));

	TCHAR text[MAX_PATH] = {};
	::SendMessage(child, WM_GETTEXT, std::size(text), (LPARAM)text);

	int easing = _ttoi(text);
	if (m_easeWindow.m_enable)
		easing = -easing - 1;
	else
		easing = easing - 1;
	MY_TRACE_INT(easing);

	return easing;
}

void CMainFrame::setEasing(int index)
{
	MY_TRACE_FUNC("%d", index);

	HWND target = ::GetForegroundWindow();
	if (!target) return;
	MY_TRACE_HWND(target);

	int easing = index;
	if (m_easeWindow.m_enable)
		easing = -easing - 1;
	else
		easing = easing + 1;
	MY_TRACE_INT(easing);

	HWND child = ::GetWindow(target, GW_CHILD);
	if (!child) return;
	MY_TRACE_HWND(child);
	MY_TRACE_INT(::GetDlgCtrlID(child));

	TCHAR text[MAX_PATH] = {};
	_itot_s(easing, text, 10);
	::SendMessage(child, WM_SETTEXT, 0, (LPARAM)text);
	::PostMessage(target, WM_COMMAND, IDOK, 0);
}

void CMainFrame::show(HWND target)
{
	MY_TRACE_FUNC("0x%08X", target);

	if (IsWindowVisible())
		return;

	m_currentPart = getEasing();
	m_hotPart = -1;

	CRect rc; GetWindowRect(&rc);
	CRect rcTarget; ::GetWindowRect(target, &rcTarget);

	int x, y;

	if (::lstrcmpi(m_horz, _T("left")) == 0)
	{
		x = rcTarget.left - rc.Width();
	}
	else if (::lstrcmpi(m_horz, _T("right")) == 0)
	{
		x = rcTarget.right;
	}
	else
	{
		x = rcTarget.CenterPoint().x - rc.Width() / 2;
	}

	if (::lstrcmpi(m_vert, _T("top")) == 0)
	{
		y = rcTarget.top - rc.Height();
	}
	else if (::lstrcmpi(m_vert, _T("bottom")) == 0)
	{
		y = rcTarget.bottom;
	}
	else
	{
		y = rcTarget.CenterPoint().y - rc.Height() / 2;
	}

	if (m_clamp)
	{
		HMONITOR monitor = ::MonitorFromWindow(target, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi = { sizeof(mi) }; ::GetMonitorInfo(monitor, &mi);

		if (x < mi.rcWork.left)
			x = mi.rcWork.left;
		else if (x + rc.Width() > mi.rcWork.right)
			x = mi.rcWork.right - rc.Width();

		if (y < mi.rcWork.top)
			y = mi.rcWork.top;
		else if (y + rc.Height() > mi.rcWork.bottom)
			y = mi.rcWork.bottom - rc.Height();
	}

	SetLayeredWindowAttributes(0, m_alpha, LWA_ALPHA);
	SetWindowPos(&wndTopMost, x, y, 0, 0,
		SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
	Invalidate(FALSE);
}

void CMainFrame::hide()
{
	MY_TRACE_FUNC("");

	if (!IsWindowVisible())
		return;

	ShowWindow(SW_HIDE);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	MY_TRACE_FUNC("");

	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	return TRUE;
}

BEGIN_MESSAGE_MAP(CMainFrame, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_NCACTIVATE()
	ON_WM_NCCALCSIZE()
	ON_WM_NCHITTEST()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	MY_TRACE_FUNC("");

	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_easeWindow.Create(this))
	{
		MY_TRACE(_T("EaseWindowの作成に失敗しました\n"));
		return -1;
	}

	if (loadSettings() != S_OK)
	{
		MY_TRACE(_T("設定ファイルの読み込みに失敗しました\n"));
		return -1;
	}

	SetTimer(TIMER_ID, 1000, 0);

	return 0;
}

void CMainFrame::OnDestroy()
{
	MY_TRACE_FUNC("");

	KillTimer(TIMER_ID);

	m_image.Destroy();

	saveSettings();

	CWnd::OnDestroy();
}

void CMainFrame::OnPaint()
{
	CPaintDC paintDC(this);

	CUxDC dc(paintDC);
	if (!dc.isValid()) return;

	CRect rc; GetClientRect(&rc);

	if (!m_image.IsNull())
	{
		int w = m_image.GetWidth();
		int h = m_image.GetHeight();
		m_image.AlphaBlend(dc, rc.left, rc.top, rc.Width(), rc.Height(), 0, 0, w, h);
	}

	Graphics g(dc);
	g.SetSmoothingMode(SmoothingModeAntiAlias);
//	g.SetCompositingMode(CompositingModeSourceOver);
	g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
	g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	g.TranslateTransform(-0.5f, -0.5f);

	if (m_currentPart >= 0 && m_currentPart < (int)m_parts.size())
	{
		SolidBrush brush(m_selectedColor);
		REAL x = (REAL)m_parts[m_currentPart].left;
		REAL y = (REAL)m_parts[m_currentPart].top;
		REAL w = (REAL)m_parts[m_currentPart].Width();
		REAL h = (REAL)m_parts[m_currentPart].Height();

		g.FillRectangle(&brush, x, y, w, h);
	}

	if (m_hotPart >= 0 && m_hotPart < (int)m_parts.size())
	{
		SolidBrush brush(m_hotColor);
		REAL x = (REAL)m_parts[m_hotPart].left;
		REAL y = (REAL)m_parts[m_hotPart].top;
		REAL w = (REAL)m_parts[m_hotPart].Width();
		REAL h = (REAL)m_parts[m_hotPart].Height();

		g.FillRectangle(&brush, x, y, w, h);
	}
}

void CMainFrame::OnTimer(UINT_PTR timerId)
{
	if (timerId == TIMER_ID)
	{
		if (!::IsWindow(theApp.m_mainProcessWindow))
		{
			KillTimer(TIMER_ID);
			PostMessage(WM_CLOSE);
		}

		HWND target = getTarget();

		if (target)
		{
			show(target);
			m_easeWindow.show(target, GetSafeHwnd());
		}
		else
		{
			m_easeWindow.hide();
			hide();
		}

		if (m_fileUpdateChecker.isFileUpdated())
			loadSettings();
	}

	CWnd::OnTimer(timerId);
}

BOOL CMainFrame::OnNcActivate(BOOL bActive)
{
	MY_TRACE_FUNC("");

	return CWnd::OnNcActivate(bActive);
}

void CMainFrame::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	MY_TRACE_FUNC("");

//	CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);
}

LRESULT CMainFrame::OnNcHitTest(CPoint point)
{
	ScreenToClient(&point);

	for (size_t i = 0; i < m_parts.size(); i++)
	{
		if (m_parts[i].PtInRect(point))
		{
			setHotPart(i);
			return HTCLIENT;
		}
	}

	setHotPart(-1);
	return HTCAPTION;
//	return CWnd::OnNcHitTest(point);
}

void CMainFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	MY_TRACE_FUNC("");

	for (size_t i = 0; i < m_parts.size(); i++)
	{
		if (m_parts[i].PtInRect(point))
		{
			setEasing(i);

			return;
		}
	}

	CWnd::OnLButtonDown(nFlags, point);
}
