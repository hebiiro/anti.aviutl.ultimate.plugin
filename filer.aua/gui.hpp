#pragma once

namespace apn::filer
{
	//
	// このクラスはUIを担当するクライアントプロセスです。
	//
	inline struct Gui
	{
		PROCESS_INFORMATION pi = {};

		//
		// 初期化処理を実行します。
		//
		BOOL init(HWND hwnd)
		{
			MY_TRACE_FUNC("{:#010x}", hwnd);

			my::Event event(0, TRUE, FALSE, share::event::c_post_init_client);
			MY_TRACE_HEX((HANDLE)event);

			auto path = magi.get_module_file_name(L"filer_ui.exe");
			MY_TRACE_STR(path);

			auto args = my::format(L"{:#010x}", hwnd);
			MY_TRACE_STR(args);

			STARTUPINFO si = { sizeof(si) };
			if (!::CreateProcessW(
				path.c_str(),	// No module name (use command line)
				args.data(),	// Command line
				0,				// Process handle not inheritable
				0,              // Thread handle not inheritable
				FALSE,			// Set handle inheritance to FALSE
				0,				// No creation flags
				0,				// Use parent's environment block
				0,				// Use parent's starting directory 
				&si,			// Pointer to STARTUPINFO structure
				&pi))			// Pointer to PROCESS_INFORMATION structur
			{
				hive.message_box(std::format(L"{}の起動に失敗しました\n", path));

				return FALSE;
			}

			auto result = ::WaitForSingleObject(event, INFINITE);
			MY_TRACE("::WaitForSingleObject(event, INFINITE)が{:#010x}を返しました\n", result);

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
