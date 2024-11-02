#pragma once

namespace apn::workspace
{
	//
	// このクラスはレイアウトのリストです。
	//
	inline struct LayoutList : Tab
	{
		inline static constexpr auto extension = L".json";

		//
		// レイアウトフォルダのファイルパスです。
		//
		std::filesystem::path folder_name;

		//
		// レイアウトファイル(のステム)のコレクションです。
		//
		std::vector<std::wstring> stems;

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// レイアウトフォルダのファイルパスを取得します。
			folder_name = magi.get_config_file_name(hive.c_name);
			folder_name /= L"layout";
			MY_TRACE_STR(folder_name);

			// レイアウトフォルダを監視するスレッドを作成します。
			DWORD tid = 0;
			::CreateThread(nullptr, 0,
				[](LPVOID param) -> DWORD
				{
					auto folder_name = layout_list.folder_name;
					std::vector<BYTE> buffer(1024);
					DWORD returned = 0;
					my::handle::unique_ptr<> file(::CreateFile(
						folder_name.c_str(),
						FILE_LIST_DIRECTORY,
						FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
						nullptr,
						OPEN_EXISTING,
						FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
						nullptr));
					while (::ReadDirectoryChangesW(
						file.get(),
						buffer.data(), buffer.size(),
						TRUE,
						FILE_NOTIFY_CHANGE_FILE_NAME |
						FILE_NOTIFY_CHANGE_DIR_NAME |
						FILE_NOTIFY_CHANGE_SIZE |
						FILE_NOTIFY_CHANGE_LAST_WRITE,
						&returned,
						nullptr,
						nullptr))
					{
						MY_TRACE("レイアウトフォルダの変更を検出しました\n");

						auto fni = (FILE_NOTIFY_INFORMATION*)buffer.data();
						auto length = fni->FileNameLength / sizeof(WCHAR);
						std::wstring file_name(fni->FileName, length);
						if (1)
							::PostMessage(layout_list, hive.c_message.c_update_layout_list, 0, 0);
					}

					return 0;
				},
				nullptr, 0, &tid);
			MY_TRACE_INT(tid);

			// タブコントロールを作成します。
			return my::Window::create(
				WS_EX_NOPARENTNOTIFY,
				WC_TABCONTROL,
				_T("apn::workspace::layout_list"),
				WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
				TCS_FOCUSNEVER,
				0, 0, 0, 0,
				hive.main_window, nullptr, hive.instance, nullptr);
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return destroy();
		}

		//
		// レイアウトリストを更新します。
		//
		std::wstring get_current_file_name()
		{
			MY_TRACE_FUNC("");

			auto tab = (HWND)*this;
			MY_TRACE_HWND(tab);

			auto index = (size_t)::SendMessage(tab, TCM_GETCURSEL, 0, 0);
			if (index >= stems.size()) return {};

			return (folder_name / stems[index]).wstring() + extension;
		}

		//
		// レイアウトリストを更新します。
		//
		BOOL update_layout_list()
		{
			MY_TRACE_FUNC("");

			// レイアウトファイルを列挙します。

			stems.clear();

			for (const auto& path : std::filesystem::directory_iterator(folder_name))
			{
				if (path.path().filename().wstring().ends_with(extension))
					stems.emplace_back(path.path().stem());
			}

			std::sort(stems.begin(), stems.end());

			// タブコントロールを更新します。

			auto tab = (HWND)*this;
			MY_TRACE_HWND(tab);

			::SendMessage(tab, TCM_DELETEALLITEMS, 0, 0);

			for (size_t i = 0; i < stems.size(); i++)
			{
				TCITEM item = {};
				item.mask = TCIF_PARAM | TCIF_TEXT;
				item.lParam = (LPARAM)0;
				item.pszText = (LPTSTR)stems[i].c_str();
				::SendMessage(tab, TCM_INSERTITEM, i, (LPARAM)&item);
			}

			return TRUE;
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case hive.c_message.c_update_layout_list:
				{
					update_layout_list();

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} layout_list;
}
