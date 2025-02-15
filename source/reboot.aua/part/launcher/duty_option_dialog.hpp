#pragma once

namespace apn::reboot::launcher
{
	//
	// このクラスはコマンド用のオプションダイアログです。
	//
	struct DutyOptionDialog : my::Dialog2
	{
		//
		// ファイル選択ダイアログを表示してパスを取得します。
		//
		BOOL get_open_file_name(std::wstring& file_name)
		{
			// バッファを確保します。
			file_name.resize(MAX_PATH, L'\0');

			// ファイル選択ダイアログ用の構造体を設定します。
			OPENFILENAMEW ofn = { sizeof(ofn) };
			ofn.hwndOwner = *this;
			ofn.Flags = OFN_FILEMUSTEXIST;
			ofn.lpstrTitle = L"ファイルを選択";
			ofn.lpstrFile = file_name.data();
			ofn.nMaxFile = file_name.size();
			ofn.lpstrFilter = L"すべてのファイル (*.*)\0*.*\0";

			// ファイル選択ダイアログを表示します。
			if (!::GetOpenFileNameW(&ofn)) return FALSE;

			// 受け取っった文字列の長さをセットします。
			file_name.resize(wcslen(file_name.c_str()));

			return TRUE;
		}

		//
		// ダイアログを作成して表示します。
		//
		int do_modal(HWND parent, int32_t index)
		{
			// ダイアログを作成します。
			create(hive.instance, MAKEINTRESOURCE(IDD_LAUNCHER_COMMAND_OPTION), parent);

			// コマンドを取得します。
			auto& duty = agit.duties[index];

			bind_text(IDC_LAUNCHER_COMMAND_NAME, duty.name);
			bind_text(IDC_LAUNCHER_COMMAND_VERB, duty.verb);
			bind_text(IDC_LAUNCHER_COMMAND_PATH, duty.path);
			bind_text(IDC_LAUNCHER_COMMAND_ARGS, duty.args);

			return do_modal2(parent);
		}

		//
		// ダイアログプロシージャです。
		//
		virtual INT_PTR on_dlg_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_COMMAND:
				{
					auto code = HIWORD(wParam);
					auto id = LOWORD(wParam);
					auto sender = (HWND)lParam;

					MY_TRACE_FUNC("WM_COMMAND, {:#04x}, {:#04x}, {:#010x}", code, id, sender);

					switch (id)
					{
					case IDC_LAUNCHER_COMMAND_PATH_REF:
						{
							auto file_name = get_text(IDC_LAUNCHER_COMMAND_PATH);
							if (get_open_file_name(file_name))
								set_text(IDC_LAUNCHER_COMMAND_PATH, file_name);

							break;
						}
					case IDC_LAUNCHER_COMMAND_ARGS_REF:
						{
							auto file_name = get_text(IDC_LAUNCHER_COMMAND_ARGS);
							if (get_open_file_name(file_name))
								set_text(IDC_LAUNCHER_COMMAND_ARGS, file_name);

							break;
						}
					}

					break;
				}
			}

			return __super::on_dlg_proc(hwnd, message, wParam, lParam);
		}
	};
}
