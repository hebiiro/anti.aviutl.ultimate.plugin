#include "pch.h"
#include "ClientWindow.h"

BEGIN_MESSAGE_MAP(ClientWindow, CWnd)
	ON_WM_TIMER()
	ON_MESSAGE(Share::Filer::Message::FilerWindowCreated, OnFilerWindowCreated)
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
			if (config_file_checker && config_file_checker->isFileUpdated())
			{
				MY_TRACE(_T("ユーザーがコンフィグファイルを更新したので読み込み直します\n"));

//				loadConfig();
			}

			break;
		}
	}

	CWnd::OnTimer(timerId);
}

LRESULT ClientWindow::OnFilerWindowCreated(WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("ClientWindow::OnFilerWindowCreated(0x%08X, 0x%08X)\n"), wParam, lParam);

	HWND filerWindow = (HWND)wParam;

	try
	{
		auto filerDialog = createFilerDialog(filerWindow);
	}
	catch (LPCTSTR e)
	{
		AfxMessageBox(e, MB_OK);
	}

	return 0;
}
