#pragma once

namespace apn::note
{
	//
	// このクラスはメモ帳です。
	//
	inline struct Note : Addin
	{
		inline static constexpr struct CommandID {
			inline static constexpr auto c_sharing_note = 1;
			inline static constexpr auto c_project_note = 2;
		} c_command_id;

		//
		// プロジェクトに保存するデータです。
		//
		std::wstring save_data;

		//
		// この仮想関数は、このアドインの識別名が必要なときに呼ばれます。
		//
		virtual LPCWSTR get_addin_name() override
		{
			return hive.c_name;
		}

		//
		// この仮想関数は、このアドインの表示名が必要なときに呼ばれます。
		//
		virtual LPCWSTR get_addin_display_name() override
		{
			return hive.c_display_name;
		}

		//
		// この仮想関数は、ウィンドウの初期化を実行するときに呼ばれます。
		//
		virtual BOOL on_window_init(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			::LoadLibraryW(L"riched20.dll");

			// 予め使用予定のコンフィグフォルダを作成しておきます。
			magi.create_config_folders(hive.c_name, {});

			if (!sharing_note.init()) return FALSE;
			if (!project_note.init()) return FALSE;
			if (!config_io.init()) return FALSE;

			if (!config_io.read()) MY_TRACE("コンフィグの読み込みに失敗しました\n");

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウの後始末を実行するときに呼ばれます。
		//
		virtual BOOL on_window_exit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			config_io.write();

			config_io.exit();
			project_note.exit();
			sharing_note.exit();

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウコマンドを実行するときに呼ばれます。
		//
		virtual BOOL on_window_command(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			switch (wParam)
			{
			case magi.c_command_id.c_addin.c_command:
				{
					MY_TRACE_FUNC("magi.c_command_id.c_addin.c_command");

					my::menu::unique_ptr<> menu(::CreatePopupMenu());

					::AppendMenuW(menu.get(), MF_STRING, c_command_id.c_sharing_note, L"共通ノートを表示");
					::AppendMenuW(menu.get(), MF_STRING, c_command_id.c_project_note, L"プロジェクトノートを表示");

					if (::IsWindowVisible(sharing_note)) ::CheckMenuItem(menu.get(), c_command_id.c_sharing_note, MF_CHECKED);
					if (::IsWindowVisible(project_note)) ::CheckMenuItem(menu.get(), c_command_id.c_project_note, MF_CHECKED);

					auto pos = my::get_cursor_pos();
					auto id = ::TrackPopupMenuEx(menu.get(), TPM_NONOTIFY | TPM_RETURNCMD, pos.x, pos.y, hwnd, nullptr);

					switch (id)
					{
					case c_command_id.c_sharing_note:
						{
							sharing_note.show();

							break;
						}
					case c_command_id.c_project_note:
						{
							project_note.show();

							break;
						}
					}

					break;
				}
			}

			return FALSE;
		}

		//
		// プロジェクトファイルからデータを読み込みます。
		//
		virtual BOOL on_project_load(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void* data, int32_t size) override
		{
			try
			{
				// データをワイド文字列として読み込みます。
				auto s = std::wstring((LPCWSTR)data, size / sizeof(WCHAR));

				// 不正なjson構文を正常な構文に変換します。
				{
					constexpr auto from = L"{\"ultimate\":{/}";
					constexpr auto to = L"{\"ultimate\":{ }";

					if (auto pos = s.find(from); pos != s.npos)
						memcpy(&s[pos], to, wcslen(to) * sizeof(WCHAR));
				}

				// jsonとして解析します。
				auto root = nlohmann::json::parse(my::wide_to_cp(s, CP_UTF8));
				{
					n_json note_node;
					read_child_node(root, "note", note_node);
					std::wstring project_text;
					read_string(note_node, "project_text", project_text);

					::SetWindowText(project_note.richedit, project_text.c_str());
				}

				return TRUE;
			}
			catch (std::exception& error)
			{
				auto what = my::cp_to_wide(error.what(), CP_UTF8);
				MY_TRACE_STR(what);

				std::wcout << L"\033[31m" L"プロジェクトデータの読み込みに失敗しました" L"\033[m" << std::endl;
				std::wcout << what << std::endl;

				return FALSE;
			}
		}

		//
		// プロジェクトファイルに保存するデータを返します。
		//
		virtual LPCWSTR on_get_project_save_data(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp) override
		{
			try
			{
				auto project_text = my::get_window_text(project_note.richedit);

				n_json note_node;
				write_string(note_node, "project_text", project_text);
				save_data = my::cp_to_wide(note_node.dump(), CP_UTF8);

				return save_data.c_str();
			}
			catch (...)
			{
				return nullptr;
			}
		}
	} addin;
}
