#pragma once

namespace fgo::filer
{
	//
	// このクラスは GUI を担当するクライアントプロセスです。
	//
	struct Gui {
		PROCESS_INFORMATION pi = {};

		Gui(HWND hwnd)
		{
			MY_TRACE(_T("Gui::Gui(0x%08X)\n"), hwnd);

			Event event(0, TRUE, FALSE, Share::Filer::Event::ClientInited);
			MY_TRACE_HEX((HANDLE)event);

			TCHAR path[MAX_PATH] = {};
			::GetModuleFileName(hive.instance, path, MAX_PATH);
			::PathRemoveFileSpec(path);
			::PathAppend(path, _T("FilerGui.exe"));
			MY_TRACE_TSTR(path);

			TCHAR args[MAX_PATH] = {};
			::StringCbPrintf(args, sizeof(args), _T("0x%08p"), hwnd);
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
				throw _T("FilerGui.exe の起動に失敗しました\n");
			}

			DWORD result = ::WaitForSingleObject(event, INFINITE);
			MY_TRACE(_T("::WaitForSingleObject(event, INFINITE) が 0x%08X を返しました\n"), result);
		}

		~Gui()
		{
			MY_TRACE(_T("Gui::~Gui()\n"));

			::CloseHandle(pi.hThread);
			::CloseHandle(pi.hProcess);
		}
	};
}
