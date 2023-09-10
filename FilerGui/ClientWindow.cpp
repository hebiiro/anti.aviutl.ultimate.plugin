#include "pch.h"
#include "ClientWindow.h"

BEGIN_MESSAGE_MAP(ClientWindow, CWnd)
	ON_WM_TIMER()
	ON_MESSAGE(Share::Filer::Message::PreInit, OnPreInit)
	ON_MESSAGE(Share::Filer::Message::PostInit, OnPostInit)
	ON_MESSAGE(Share::Filer::Message::PreExit, OnPreExit)
	ON_MESSAGE(Share::Filer::Message::PostExit, OnPostExit)
	ON_MESSAGE(Share::Filer::Message::PostInitFilerWindow, OnPostInitFilerWindow)
END_MESSAGE_MAP()

void ClientWindow::OnTimer(UINT_PTR timerId)
{
	switch (timerId)
	{
	case Hive::TimerID::CheckHostWindow:
		{
			if (hive && !::IsWindow(hive->hostWindow))
			{
				MY_TRACE(_T("ホストウィンドウが無効になったのでプロセスを終了します\n"));

				KillTimer(Hive::TimerID::CheckHostWindow);
				AfxPostQuitMessage(0);
			}

			break;
		}
	case Hive::TimerID::CheckConfig:
		{
			if (hive && ::IsWindow(hive->hostWindow))
			{
				if (config_file_checker && config_file_checker->isFileUpdated())
				{
					MY_TRACE(_T("ユーザーがコンフィグファイルを更新したので読み込み直します\n"));

					loadConfig();
				}
			}

			break;
		}
	}

	CWnd::OnTimer(timerId);
}

LRESULT ClientWindow::OnPreInit(WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("ClientWindow::OnPreInit(0x%08X, 0x%08X)\n"), wParam, lParam);

	return 0;
}

LRESULT ClientWindow::OnPostInit(WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("ClientWindow::OnPostInit(0x%08X, 0x%08X)\n"), wParam, lParam);

	return loadConfig();
}

LRESULT ClientWindow::OnPreExit(WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("ClientWindow::OnPreExit(0x%08X, 0x%08X)\n"), wParam, lParam);

	KillTimer(Hive::TimerID::CheckConfig);

	return 0;
}

LRESULT ClientWindow::OnPostExit(WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("ClientWindow::OnPostExit(0x%08X, 0x%08X)\n"), wParam, lParam);

	return saveConfig();
}

LRESULT ClientWindow::OnPostInitFilerWindow(WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("ClientWindow::OnPostInitFilerWindow(0x%08X, 0x%08X)\n"), wParam, lParam);

	HWND filerWindow = (HWND)wParam;
	BOOL full = (BOOL)lParam;

	try
	{
		FilerDialog::create(filerWindow, full);
	}
	catch (LPCTSTR e)
	{
		AfxMessageBox(e, MB_OK);
	}

	return 0;
}
