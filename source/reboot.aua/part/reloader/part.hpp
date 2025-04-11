#pragma once

namespace apn::reboot::reloader
{
	//
	// このクラスはリローダーのパートです。
	//
	inline struct Reloader : Part
	{
		//
		// このパートの名前を返します。
		//
		virtual std::wstring on_get_name()
		{
			return L"reloader";
		}

		//
		// 初期化処理を実行します。
		//
		virtual BOOL on_init() override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL on_exit() override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// コンフィグを読み込みます。
		//
		virtual BOOL on_read(n_json& root) override
		{
			MY_TRACE_FUNC("");

			read_bool(root, "with_write", agit.with_write);

			return TRUE;
		}

		//
		// コンフィグを書き込みます。
		//
		virtual BOOL on_write(n_json& root) override
		{
			MY_TRACE_FUNC("");

			write_bool(root, "with_write", agit.with_write);

			return TRUE;
		}

		//
		// パートの状態を更新します。
		//
		virtual BOOL on_update(AddinDialogInterface* addin_dialog) override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// コマンドを実行します。
		//
		virtual BOOL on_command(AddinDialogInterface* addin_dialog, uint32_t code, uint32_t control_id, HWND control) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}", code, control_id, control);

			// このパート用のコントロールではない場合は何もしません。
			if (control_id != IDC_RELOADER_REBOOT_AVIUTL) return FALSE;

			// aviutlウィンドウを取得します。
			auto aviutl_window = hive.aviutl_window;

			// フラグが立っている場合は
			if (agit.with_write)
			{
				// プロジェクトを上書き保存します。
				project::write();
			}

			// 再起動用プロセスのパスを取得しておきます。
			auto path = magi.get_module_file_name(L"reboot.exe");

			// 再起動に必要な変数を取得しておきます。
			my::SimpleFileMappingT<share::RebootSignal> shared(nullptr);
			auto buffer = shared.get_buffer();
			{
				// aviutlの実行ファイル名を取得します。
				wcscpy_s(buffer->path, std::size(buffer->path), my::get_module_file_name(nullptr).c_str());

				// プロジェクトのファイル名を取得します。
				AviUtl::SysInfo si = {};
				magi.fp->exfunc->get_sys_info(magi.exin.get_editp(), &si);
				wcscpy_s(buffer->args, std::size(buffer->args), my::ws(si.project_name).c_str());
			}

			// aviutlウィンドウにWM_CLOSEを送信します。
			::SendMessage(aviutl_window, WM_CLOSE, 0, 0);

			// aviutlウィンドウが破壊されなかった場合は
			// 終了処理がキャンセルされているので何もしません。
			if (::IsWindow(aviutl_window)) return FALSE;

			// 再起動用プロセスを起動します。
			PROCESS_INFORMATION pi = {};
			STARTUPINFO si = { sizeof(si) };
			if (!::CreateProcess(
				path.c_str(),	// No module name (use command line)
				nullptr,		// Command line
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

			// 共有メモリを複製します。
			auto shared_handle = HANDLE {};
			auto result1 = ::DuplicateHandle(
				::GetCurrentProcess(), shared.handle,
				pi.hProcess, &shared_handle,
				0, FALSE, DUPLICATE_SAME_ACCESS);

			// プロセスハンドルを複製します。
			auto process_handle = HANDLE {};
			auto result2 = ::DuplicateHandle(
				::GetCurrentProcess(), ::GetCurrentProcess(),
				pi.hProcess, &process_handle,
				0, FALSE, DUPLICATE_SAME_ACCESS);

			// 再起動用プロセスがアイドル状態になるまで待機します。
			auto result3 = ::WaitForInputIdle(pi.hProcess, INFINITE);

			// 再起動用プロセスに複製したハンドルを送信します。
			auto result4 = ::PostThreadMessage(pi.dwThreadId,
				share::c_message.c_reboot, (WPARAM)process_handle, (LPARAM)shared_handle);

			// プロセス情報のハンドルを閉じます。
			::CloseHandle(pi.hProcess);
			::CloseHandle(pi.hThread);

			return TRUE;
		}

		//
		// オプションを変更します。
		//
		virtual BOOL on_option(AddinDialogInterface* addin_dialog, uint32_t control_id, HWND control, const POINT& point) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}", control_id, control);

			// このパート用のコントロールではない場合は何もしません。
			if (control_id != IDC_RELOADER_REBOOT_AVIUTL) return FALSE;

			// ポップアップメニューのアイテムIDです。
			constexpr uint32_t c_with_write = 1;

			// ポップアップメニューを作成します。
			my::menu::unique_ptr<> menu(::CreatePopupMenu());

			::AppendMenu(menu.get(), MF_STRING, c_with_write, _T("再起動前に上書き保存する"));
			if (agit.with_write)
				::CheckMenuItem(menu.get(), c_with_write, MF_CHECKED);

			auto id = ::TrackPopupMenuEx(menu.get(),
				TPM_NONOTIFY | TPM_RETURNCMD, point.x, point.y, control, nullptr);
			if (!id) return FALSE;

			switch (id)
			{
			case c_with_write:
				{
					agit.with_write = !agit.with_write;

					break;
				}
			}

			return TRUE;
		}
	} part;
}
