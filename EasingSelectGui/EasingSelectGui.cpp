#include "pch.h"
#include "EasingSelectGui.h"
#include "MainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CEasingSelectGuiApp, CWinApp)
	ON_THREAD_MESSAGE(Share::EasingSelect::Message::Notify, OnSelectEasingNotify)
END_MESSAGE_MAP()

CEasingSelectGuiApp::CEasingSelectGuiApp() noexcept
{
	Tools::Trace::File::init(0, 0);

	MY_TRACE_FUNC("");

	m_mainProcessWindow = 0;
	m_mainProcessId = 0;
}

CEasingSelectGuiApp::~CEasingSelectGuiApp()
{
	MY_TRACE_FUNC("");

	Tools::Trace::File::exit();
}

BOOL CEasingSelectGuiApp::InitInstance()
{
	MY_TRACE_FUNC("");

	{
		m_mainProcessWindow = (HWND)_tcstoul(::GetCommandLine(), 0, 0);
		::GetWindowThreadProcessId(m_mainProcessWindow, &m_mainProcessId);

		MY_TRACE_HWND(m_mainProcessWindow);
		HWND parent = ::GetParent(m_mainProcessWindow);
		MY_TRACE_HWND(parent);
		if (!::IsWindow(m_mainProcessWindow))
			return FALSE;
	}

	CWinApp::InitInstance();

	if (!AfxOleInit())
		return -1;

	EnableTaskbarInteraction(FALSE);

	m_pMainWnd = new CMainFrame();
	if (!m_pMainWnd->CreateEx(
		WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_LAYERED,
		AfxRegisterWndClass(0),
		_T(""),
		WS_POPUP, 0, 0, 0, 0, 0, 0))
	{
		MY_TRACE(_T("メインウィンドウの作成に失敗しました\n"));
	}

	return TRUE;
}

int CEasingSelectGuiApp::ExitInstance()
{
	MY_TRACE_FUNC("");

	AfxOleTerm(FALSE);

	return CWinApp::ExitInstance();
}

int CEasingSelectGuiApp::Run()
{
	MY_TRACE_FUNC("");

	GdiplusStartupInput gdiSI;
	GdiplusStartupOutput gdiSO;
	ULONG_PTR gdiToken = 0;
	ULONG_PTR gdiHookToken = 0;

	gdiSI.SuppressBackgroundThread = TRUE;
	GdiplusStartup(&gdiToken, &gdiSI, &gdiSO);
	gdiSO.NotificationHook(&gdiHookToken);

	int result = CWinApp::Run();

	gdiSO.NotificationUnhook(gdiHookToken);
	GdiplusShutdown(gdiToken);

	return result;
}

void CEasingSelectGuiApp::OnSelectEasingNotify(WPARAM wParam, LPARAM lParam)
{
	MY_TRACE_FUNC("0x%08X, 0x%08X", wParam, lParam);
#if 1
	m_pMainWnd->PostMessage(WM_TIMER, TIMER_ID, 0);
#else
	CMainFrame* mainFrame = (CMainFrame*)m_pMainWnd;
	mainFrame->OnTimer(TIMER_ID);
#endif
}
