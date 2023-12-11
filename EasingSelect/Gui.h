#pragma once
#include "Hive.h"

namespace fgo::easing_select
{
	//
	// このクラスはGUIプロセスを管理します。
	//
	struct Gui {
		PROCESS_INFORMATION pi = {};

		//
		// コンストラクタです。
		//
		Gui(HWND hwnd)
		{
			MY_TRACE_FUNC("0x%08X", hwnd);

			TCHAR path[MAX_PATH] = {};
			::GetModuleFileName(hive.instance, path, std::size(path));
			::PathRemoveFileSpec(path);
			::PathAppend(path, _T("EasingSelectGui.exe"));
			MY_TRACE_TSTR(path);

			TCHAR args[MAX_PATH] = {};
			::StringCchPrintf(args, std::size(args), _T("0x%08p"), hwnd);
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
				&pi))           // Pointer to PROCESS_INFORMATION structur
			{
				throw _T("EasingSelectGui.exeの起動に失敗しました\n");
			}
		}

		//
		// デストラクタです。
		//
		~Gui()
		{
			MY_TRACE_FUNC("");

			::CloseHandle(pi.hThread);
			::CloseHandle(pi.hProcess);
		}
	};
}
