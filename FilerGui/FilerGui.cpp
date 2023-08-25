#include "pch.h"
#include "FilerGui.h"
#include "ClientWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CFilerGuiApp theApp;
#if 0
	ON_REGISTERED_THREAD_MESSAGE(WM_FILER_LOAD_TALENTS, OnFilerLoadTalents)
	ON_REGISTERED_THREAD_MESSAGE(WM_FILER_SAVE_TALENTS, OnFilerSaveTalents)
	ON_REGISTERED_THREAD_MESSAGE(WM_FILER_CREATE_TALENT, OnFilerCreateTalent)
	ON_REGISTERED_THREAD_MESSAGE(WM_FILER_DESTROY_TALENT, OnFilerDestroyTalent)
	ON_REGISTERED_THREAD_MESSAGE(WM_FILER_EDIT_TALENT, OnFilerEditTalent)
#endif
BEGIN_MESSAGE_MAP(CFilerGuiApp, CWinApp)
END_MESSAGE_MAP()

CFilerGuiApp::CFilerGuiApp()
{
	_tsetlocale(LC_ALL, _T(""));

	trace_init(0, 0, TRUE);
}

CFilerGuiApp::~CFilerGuiApp()
{
	trace_term();
}

BOOL CFilerGuiApp::InitInstance()
{
	MY_TRACE(_T("CFilerGuiApp::InitInstance()\n"));

	CWinApp::InitInstance();

	if (!AfxOleInit())
	{
		AfxMessageBox(_T("AfxOleInit() failed."));

		return FALSE;
	}

	// ホストプロセスのウィンドウを取得します。
	HWND hostWindow = receiveHostWindow();
	MY_TRACE_HWND(hostWindow);

	try
	{
		// ハイブを作成します。
		hive = std::make_shared<Hive>(hostWindow);

		// クライアントウィンドウを作成します。
		clientWindow = std::make_shared<ClientWindow>(hostWindow);

		Event event(EVENT_ALL_ACCESS, FALSE, Share::Filer::Event::ClientInited);

		::SetEvent(event);
	}
	catch (LPCTSTR e)
	{
		AfxMessageBox(e);

		return FALSE;
	}

	return TRUE;
}

int CFilerGuiApp::ExitInstance()
{
	MY_TRACE(_T("CFilerGuiApp::ExitInstance()\n"));

	// ディレクターを削除します。
	clientWindow = 0;

	// ハイブを削除します。
	hive = 0;

	return CWinApp::ExitInstance();
}

void CFilerGuiApp::OnFilerLoadTalents(WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("CFilerGuiApp::OnFilerLoadTalents(0x%08X, 0x%08X)\n"), wParam, lParam);

//	m_dialog.showTalents((int)wParam);
}

void CFilerGuiApp::OnFilerSaveTalents(WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("CFilerGuiApp::OnFilerSaveTalents(0x%08X, 0x%08X)\n"), wParam, lParam);

//	m_dialog.showTalents((int)wParam);
}

void CFilerGuiApp::OnFilerCreateTalent(WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("CFilerGuiApp::OnFilerCreateTalent(0x%08X, 0x%08X)\n"), wParam, lParam);

//	m_dialog.createTalent();
}

void CFilerGuiApp::OnFilerDestroyTalent(WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("CFilerGuiApp::OnFilerDestroyTalent(0x%08X, 0x%08X)\n"), wParam, lParam);

//	m_dialog.destroyTalent((HWND)wParam);
}

void CFilerGuiApp::OnFilerEditTalent(WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("CFilerGuiApp::OnFilerEditTalent(0x%08X, 0x%08X)\n"), wParam, lParam);

//	m_dialog.editTalent((HWND)wParam, (HWND)lParam);
}
