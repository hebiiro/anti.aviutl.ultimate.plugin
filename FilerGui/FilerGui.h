#pragma once

class FilerGuiApp : public CWinApp
{
public:

	FilerGuiApp();
	virtual ~FilerGuiApp();

	//
	// コマンドライン引数からホストウィンドウを受け取ります。
	//
	HWND receiveHostWindow()
	{
		LPWSTR commandLine = ::GetCommandLineW();
		MY_TRACE_WSTR(commandLine);

		int argc = 0;
		LPWSTR* argv = ::CommandLineToArgvW(commandLine, &argc);
		MY_TRACE_INT(argc);

		LPWSTR arg = commandLine;
		if (argc >=  2) // 引数が 2 個以上なら
			arg = argv[1]; // 2 番目の引数を使用します。
		MY_TRACE_WSTR(arg);

		return (HWND)wcstoul(arg, 0, 0);
	}

protected:

	virtual BOOL InitInstance();
	virtual int ExitInstance();
	DECLARE_MESSAGE_MAP()
};

extern FilerGuiApp theApp;
