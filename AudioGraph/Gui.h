#pragma once
#include "Hive.h"

namespace fgo::audio_graph
{
	inline struct Gui
	{
		//
		// クライアントプロセスのプロセス情報です。
		//
		PROCESS_INFORMATION pi = {};

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			TCHAR path[MAX_PATH] = {};
			::GetModuleFileName(hive.instance, path, std::size(path));
			::PathRemoveExtension(path);
			::StringCchCat(path, std::size(path), _T("Gui.exe"));
			MY_TRACE_TSTR(path);

			TCHAR args[MAX_PATH] = {};
			::StringCchPrintf(args, std::size(args), _T("0x%08p"), hive.mainWindow);
			MY_TRACE_TSTR(args);

			STARTUPINFO si = { sizeof(si) };
			if (!::CreateProcess(
				path,           // No module name (use command line)
				args,           // Command line
				NULL,           // Process handle not inheritable
				NULL,           // Thread handle not inheritable
				FALSE,          // Set handle inheritance to FALSE
				0,              // No creation flags
				NULL,           // Use parent's environment block
				NULL,           // Use parent's starting directory 
				&si,            // Pointer to STARTUPINFO structure
				&pi))         // Pointer to PROCESS_INFORMATION structur
			{
				MY_TRACE(_T("::CreateProcess() failed.\n"));

				return FALSE;
			}

			// クライアントプロセスのメッセージループが開始されるまで待機します。
			::WaitForInputIdle(pi.hProcess, INFINITE);

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");
#if 1
			// クライアントプロセスを終了させます。
			::PostThreadMessage(pi.dwThreadId, WM_QUIT, 0, 0);
#else
			// クライアントプロセスを強制的に終了させます。
			::TerminateProcess(pi.hProcess, 0);
#endif
			return TRUE;
		}
	} gui;
}
