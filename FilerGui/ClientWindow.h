#pragma once
#include "FilerDialog.h"

class ClientWindow : public CWnd
{
public:

	std::map<std::wstring, std::shared_ptr<FilerDialog>> filerDialogs;
	std::shared_ptr<FileUpdateChecker> config_file_checker;

	//
	// コンストラクタです。
	//
	ClientWindow(HWND hostWindow)
	{
		MY_TRACE(_T("ClientWindow::ClientWindow(0x%08X)\n"), hostWindow);

		// DarkenWindow を読み込みます。
//		loadDarkenWindow();

		// クライアントウィンドウを作成します。
		if (!Create())
			throw _T("クライアントウィンドウの作成に失敗しました");

		// ホストウィンドウにクライアントウィンドウのハンドルを渡します。
		Share::Filer::HostWindow::setClientWindow(hostWindow, GetSafeHwnd());

		// このウィンドウを MFC のメインウィンドウに設定します。
		AfxGetApp()->m_pMainWnd = this;

		// ホストウィンドウを監視するタイマーを作成します。
		SetTimer(Hive::TimerID::CheckHostWindow, 1000, 0);

		// コンフィグファイルチェッカーを作成します。
		config_file_checker = std::make_shared<FileUpdateChecker>(hive->configFileName);

		// コンフィグファイルを監視するタイマーを作成します。
		SetTimer(Hive::TimerID::CheckConfig, 1000, 0);
	}

	//
	// デストラクタです。
	//
	~ClientWindow()
	{
		MY_TRACE(_T("ClientWindow::~ClientWindow()\n"));
	}

	//
	// DarkenWindow が存在する場合は読み込みます。
	//
	void loadDarkenWindow()
	{
		TCHAR fileName[MAX_PATH] = {};
		::GetModuleFileName(AfxGetInstanceHandle(), fileName, MAX_PATH);
		::PathRemoveFileSpec(fileName);
		::PathAppend(fileName, _T("..\\DarkenWindow.aul"));
		MY_TRACE_TSTR(fileName);

		HMODULE DarkenWindow = ::LoadLibrary(fileName);
		MY_TRACE_HEX(DarkenWindow);

		if (DarkenWindow)
		{
			typedef void (WINAPI* Type_DarkenWindow_init)(HWND hwnd);
			Type_DarkenWindow_init DarkenWindow_init =
				(Type_DarkenWindow_init)::GetProcAddress(DarkenWindow, "DarkenWindow_init");
			MY_TRACE_HEX(DarkenWindow_init);

			if (DarkenWindow_init)
				DarkenWindow_init(GetSafeHwnd());
		}
	}

	//
	// ウィンドウを作成します。
	//
	BOOL Create()
	{
		const static LPCTSTR className = _T("FilerGui");

		WNDCLASS wc = {};
		wc.lpszClassName = className;
		wc.lpfnWndProc = AfxWndProc;
		BOOL result = AfxRegisterClass(&wc);

		return __super::CreateEx(
			0,
			className,
			className,
			WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME,
			CRect(100, 100, 400, 400),
			0,
			0);
	}

	//
	// ファイラダイアログを作成して返します。
	// すでに作成済みの場合はそのファイラダイアログを返します。
	//
	std::shared_ptr<FilerDialog> createFilerDialog(HWND filerWindow)
	{
		MY_TRACE(_T("HostDialog::createFilerDialog(0x%08X)\n"), filerWindow);

		WCHAR name[MAX_PATH] = {};
		::GetWindowTextW(filerWindow, name, std::size(name));
		MY_TRACE_WSTR(name);

		auto it = filerDialogs.find(name);
		if (it != filerDialogs.end()) return it->second;
		return filerDialogs[name] = std::make_shared<FilerDialog>(filerWindow);
	}

protected:

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnFilerWindowCreated(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
}; inline std::shared_ptr<ClientWindow> clientWindow;
