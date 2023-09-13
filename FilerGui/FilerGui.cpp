#include "pch.h"
#include "FilerGui.h"
#include "ClientWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

FilerGuiApp theApp;

BEGIN_MESSAGE_MAP(FilerGuiApp, CWinApp)
END_MESSAGE_MAP()

FilerGuiApp::FilerGuiApp()
{
	_tsetlocale(LC_ALL, _T(""));

	Tools::Trace::File::init(0, 0, TRUE);
}

FilerGuiApp::~FilerGuiApp()
{
	Tools::Trace::File::exit();
}

BOOL FilerGuiApp::InitInstance()
{
	MY_TRACE(_T("FilerGuiApp::InitInstance()\n"));

	CWinApp::InitInstance();

	if (!AfxOleInit())
	{
		AfxMessageBox(_T("AfxOleInit() failed."));

		return FALSE;
	}

	// ホストプロセスのウィンドウを取得します。
	HWND hostWindow = receiveHostWindow();
	MY_TRACE_HWND(hostWindow);

	BOOL retValue = FALSE;

	try
	{
		// ハイブを作成します。
		hive = std::make_shared<Hive>(hostWindow);

		// クライアントウィンドウを作成します。
		clientWindow = std::make_shared<ClientWindow>(hostWindow);

		retValue = TRUE;
	}
	catch (LPCTSTR e)
	{
		AfxMessageBox(e);

		retValue = FALSE;
	}

	// 待機しているホストプロセスに初期化が完了したことを知らせます。
	Event event(EVENT_ALL_ACCESS, FALSE, Share::Filer::Event::PostInitClient);
	::SetEvent(event);

	return retValue;
}

int FilerGuiApp::ExitInstance()
{
	MY_TRACE(_T("FilerGuiApp::ExitInstance()\n"));

	// ファイラダイアログをすべて削除します。
	FilerDialog::collection.clear();

	// クライアントウィンドウを削除します。
	clientWindow = 0;

	// ハイブを削除します。
	hive = 0;

	return CWinApp::ExitInstance();
}
