#pragma once

namespace apn::item_wave
{
	//
	// このクラスはUIプロセスを管理します。
	//
	inline struct UiProcessManager
	{
		//
		// UIプロセスのプロセス情報です。
		//
		PROCESS_INFORMATION pi = {};

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			my::Event completion_event(nullptr, TRUE, FALSE,
				share::get_completion_event_name(hive.host_window).c_str());
			MY_TRACE_HEX((HANDLE)completion_event);

			// UIプロセスを起動します。

			auto path = magi.get_module_file_name(L"item_wave_ui.exe");
			MY_TRACE_STR(path);

			auto args = my::format(_T("{/hex}"), hive.host_window);
			MY_TRACE_STR(args);

			STARTUPINFO si = { sizeof(si) };
			if (!::CreateProcess(
				path.c_str(),	// No module name (use command line)
				args.data(),	// Command line
				nullptr,		// Process handle not inheritable
				nullptr,		// Thread handle not inheritable
				FALSE,			// Set handle inheritance to FALSE
				0,				// No creation flags
				nullptr,		// Use parent's environment block
				nullptr,		// Use parent's starting directory 
				&si,			// Pointer to STARTUPINFO structure
				&pi))			// Pointer to PROCESS_INFORMATION structure
			{
				MY_TRACE("::CreateProcess()が失敗しました\n");

				return FALSE;
			}

			HANDLE handles[] = { completion_event, pi.hProcess };
			auto result = ::WaitForMultipleObjects(std::size(handles), handles, FALSE, INFINITE);
			MY_TRACE("::WaitForMultipleObjects()が{/hex}を返しました\n", result);
			if (result != WAIT_OBJECT_0 + 0) return FALSE;

			hive.ui_window = share::host_window::get_ui_window(hive.host_window);
			MY_TRACE_HWND(hive.ui_window);
			if (!hive.ui_window) return FALSE;

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}
	} ui_process_manager;
}
