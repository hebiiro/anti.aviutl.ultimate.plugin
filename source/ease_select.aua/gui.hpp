#pragma once

namespace apn::ease_select
{
	//
	// このクラスはクライアントプロセスを管理します。
	//
	inline struct Gui
	{
		PROCESS_INFORMATION pi = {};

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			auto path = magi.get_module_file_name(L"ease_select_ui.exe");
			MY_TRACE_STR(path);

			auto args = my::format(L"{:#010x}", hive.main_window);
			MY_TRACE_STR(args);

			STARTUPINFO si = { sizeof(si) };
			if (!::CreateProcessW(
				path.c_str(),	// No module name (use command line)
				args.data(),	// Command line
				nullptr,		// Process handle not inheritable
				nullptr,		// Thread handle not inheritable
				FALSE,			// Set handle inheritance to FALSE
				0,				// No creation flags
				nullptr,		// Use parent's environment block
				nullptr,		// Use parent's starting directory 
				&si,			// Pointer to STARTUPINFO structure
				&pi))			// Pointer to PROCESS_INFORMATION structur
			{
				hive.message_box(std::format(L"{}の起動に失敗しました\n", path));

				return FALSE;
			}

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			::CloseHandle(pi.hThread);
			::CloseHandle(pi.hProcess);

			return TRUE;
		}
	} gui;
}
