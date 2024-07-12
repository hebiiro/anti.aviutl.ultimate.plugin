#pragma once

class MainWindow : public CWnd
{
public:

	//
	// 初期化処理を実行します。
	//
	BOOL init()
	{
		WNDCLASS wc = {};
		wc.lpszClassName = _T("install::main_window");
		wc.lpfnWndProc = AfxWndProc;
		AfxRegisterClass(&wc);

		return __super::CreateEx(
			0,
			wc.lpszClassName,
			wc.lpszClassName,
			WS_CAPTION | WS_SYSMENU | WS_THICKFRAME,
			CRect(100, 100, 400, 400),
			0,
			0);
	}

	//
	// 後始末処理を実行します。
	//
	BOOL exit()
	{
		return DestroyWindow();
	}

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(MainWindow, CWnd)
END_MESSAGE_MAP()
