#pragma once

namespace apn::dialog_size
{
	//
	// このクラスは`エイリアスの作成`ダイアログをフックします。
	//
	inline struct SaveObjectDialog : my::Window
	{
		inline static constexpr struct CommandID {
			inline static constexpr uint32_t c_alias_name = 171;
			inline static constexpr uint32_t c_folder_name = 172;
			inline static constexpr uint32_t c_folder_list = 2024;
		} c_command_id;

		//
		// 指定されたフォルダにエイリアスが含まれている場合はTRUEを返します。
		//
		BOOL has_alias(const auto& folder_path)
		{
			// 検索パターンを取得します。
			auto pattern = folder_path / _T("*.exa");

			WIN32_FIND_DATA ffd = {};
			my::find_file::unique_ptr<> handle(
				::FindFirstFile(pattern.c_str(), &ffd));

			return handle.get() != INVALID_HANDLE_VALUE;
		}

		//
		// フォルダリストを初期化します。
		//
		BOOL init_folder_list(HWND folder_list)
		{
			MY_TRACE_FUNC("");

			// 起点となるフォルダのパスを取得します。
			auto origin = my::get_module_file_name(nullptr);
			origin.remove_filename();

			// 検索パターンを作成します。
			auto pattern = origin / _T("*.*");

			WIN32_FIND_DATA ffd = {};
			my::find_file::unique_ptr<> handle(
				::FindFirstFile(pattern.c_str(), &ffd));

			if (handle.get() == INVALID_HANDLE_VALUE)
			{
				MY_TRACE("::FindFirstFile()が失敗しました\n");

				return FALSE;
			}

			do
			{
				// 特殊パスは除外します。
				if (::lstrcmp(ffd.cFileName, _T(".")) == 0 ||
					::lstrcmp(ffd.cFileName, _T("..")) == 0)
				{
					continue;
				}

				// フォルダ以外は除外します。
				if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
					continue;

				// エイリアスが存在しないフォルダは除外します。
				if (!has_alias(origin / ffd.cFileName))
					continue;

				// フォルダ名をフォルダリストに追加します。
				my::combobox::add_text(folder_list, ffd.cFileName);
			}
			while (::FindNextFile(handle.get(), &ffd));

			return TRUE;
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_INITDIALOG:
				{
					MY_TRACE_FUNC("WM_INITDIALOG, {/hex}, {/hex}", wParam, lParam);

					// フォルダリストのコンボボックスを初期化します。
					auto folder_list = ::GetDlgItem(hwnd, c_command_id.c_folder_list);
					init_folder_list(folder_list);

					break;
				}
			case WM_COMMAND:
				{
					auto code = HIWORD(wParam);
					auto control_id = LOWORD(wParam);
					auto control = (HWND)lParam;

					MY_TRACE_FUNC("WM_COMMAND, {/hex16}, {/hex16}, {/hex}", code, control_id, control);

					switch (control_id)
					{
					case c_command_id.c_folder_list:
						{
							if (code != CBN_SELCHANGE) break;

							auto index = my::combobox::get_cur_sel(control);
							auto text = my::combobox::get_text(control, index);

							// フォルダ名を変更します。
							::SetDlgItemTextW(hwnd, c_command_id.c_folder_name, text.c_str());

							break;
						}
					}

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} save_object_dialog;
}
