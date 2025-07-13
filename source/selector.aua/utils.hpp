#pragma once

namespace my
{
	template <typename T, auto deleter>
	struct ptr_utils {
		using remove_pointer_type = std::remove_pointer<T>::type;
		using unique_ptr = std::unique_ptr<remove_pointer_type, decltype(deleter)>;
		struct shared_ptr : std::shared_ptr<remove_pointer_type> {
			shared_ptr() {}
			shared_ptr(T x) : std::shared_ptr<remove_pointer_type>(x, deleter) {}
			auto reset(T x) { return __super::reset(x, deleter); }
		};
		using weak_ptr = std::weak_ptr<remove_pointer_type>;
	};

	using image_list = ptr_utils<HIMAGELIST, [](HIMAGELIST x) { ::ImageList_Destroy(x); }>;
	using registry_key = ptr_utils<HKEY, [](HKEY x) { ::RegCloseKey(x); }>;

	//
	// レジストリキーを開いて返します。
	//
	inline HKEY open_registry_key(HKEY key, LPCTSTR sub_key, DWORD options, REGSAM sam_desired)
	{
		auto handle = HKEY {};
		::RegOpenKeyEx(key, sub_key, options, sam_desired, &handle);
		return handle;
	}

	//
	// このクラスはレジストリキーです。
	//
	struct RegistryKey
	{
		//
		// レジストリキーのハンドルです。
		//
		HKEY handle = {};

		//
		// コンストラクタです。
		//
		RegistryKey(HKEY key, LPCTSTR sub_key, DWORD options, REGSAM sam_desired)
		{
			::RegOpenKeyEx(key, sub_key, options, sam_desired, &handle);
		}

		//
		// デストラクタです。
		//
		~RegistryKey()
		{
			if (handle) ::RegCloseKey(handle);
		}

		//
		// ハンドルを返します。
		//
		operator HKEY() const { return handle; }
	};
}

namespace apn::selector
{
	//
	// レジストリの文字列を取得して返します。
	//
	inline std::wstring get_registry_string(HKEY key, LPCWSTR value_name)
	{
		// データサイズを取得します。
		auto data_size = DWORD {};
		if (::RegQueryValueEx(key, value_name, nullptr, nullptr, nullptr, &data_size) != ERROR_SUCCESS)
			return {};

		// データを取得します。
		std::wstring buffer(data_size / sizeof(wchar_t), L'\0');
		if (::RegQueryValueEx(key, value_name, nullptr, nullptr, (BYTE*)buffer.data(), &data_size) != ERROR_SUCCESS)
			return {};

		// 文字列を正規化します。
		buffer.resize(::lstrlenW(buffer.data()));

		// 取得した文字列を返します。
		return buffer;
	}

	//
	// レジストリキーから値を取得して返します。
	//
	inline std::wstring get_registry_value(HKEY root_key, LPCWSTR sub_key, LPCWSTR value_name, DWORD options = 0, REGSAM sam_desired = KEY_READ)
	{
		my::registry_key::unique_ptr key(my::open_registry_key(root_key, sub_key, options, sam_desired));

		return get_registry_string(key.get(), value_name);
	}

	//
	// デフォルトのテキストエディタのファイルパスを返します。
	//
	inline std::wstring get_default_text_editor()
	{
		// プログラム識別子を取得します。
		auto prog_id = get_registry_value(HKEY_CLASSES_ROOT, L".txt", nullptr);

		// コマンドを取得します。
		auto command_key = prog_id + L"\\shell\\open\\command";
		auto command = get_registry_value(HKEY_CLASSES_ROOT, command_key.c_str(), nullptr);

		// コマンドから実行ファイルのパスを抽出して返します。
		if (command.starts_with(L'"'))
		{
			auto quote = command.find(L'"', 1);
			if (quote != command.npos)
				return command.substr(1, quote - 1);
		}
		else
		{
			auto space = command.find(L' ');
			if (space != command.npos)
				return command.substr(0, space);
		}

		// コマンドが無効の場合は空文字列を返します。
		return {};
	}

	//
	// ファイル選択ダイアログを表示して読み込み用のパスを取得します。
	//
	inline std::wstring get_open_file_name(
		HWND owner,
		LPCWSTR title,
		LPCWSTR filter,
		LPCWSTR default_file_name,
		DWORD flags = OFN_FILEMUSTEXIST)
	{
		std::wstring file_name = default_file_name;

		// バッファを確保します。
		file_name.resize(MAX_PATH, L'\0');

		// ファイル選択ダイアログ用の構造体を設定します。
		OPENFILENAMEW ofn = { sizeof(ofn) };
		ofn.hwndOwner = owner;
		ofn.Flags = flags;
		ofn.lpstrTitle = title;
		ofn.lpstrFile = file_name.data();
		ofn.nMaxFile = (DWORD)file_name.size();
		ofn.lpstrFilter = filter;

		// ファイル選択ダイアログを表示します。
		if (!::GetOpenFileNameW(&ofn)) return {};

		// 受け取った文字列の長さをセットします。
		file_name.resize(wcslen(file_name.c_str()));

		return file_name;
	}

	//
	// ファイル選択ダイアログを表示して書き込み用のパスを取得します。
	//
	inline std::wstring get_save_file_name(
		HWND owner,
		LPCWSTR title,
		LPCWSTR filter,
		LPCWSTR default_file_name,
		LPCWSTR default_extension,
		DWORD flags = OFN_OVERWRITEPROMPT)
	{
		std::wstring file_name = default_file_name;

		// バッファを確保します。
		file_name.resize(MAX_PATH, L'\0');

		// ファイル選択ダイアログ用の構造体を設定します。
		OPENFILENAMEW ofn = { sizeof(ofn) };
		ofn.hwndOwner = owner;
		ofn.Flags = flags;
		ofn.lpstrTitle = title;
		ofn.lpstrFile = file_name.data();
		ofn.nMaxFile = (DWORD)file_name.size();
		ofn.lpstrFilter = filter;
		ofn.lpstrDefExt = default_extension;

		// ファイル選択ダイアログを表示します。
		if (!::GetSaveFileNameW(&ofn)) return {};

		// 受け取った文字列の長さをセットします。
		file_name.resize(wcslen(file_name.c_str()));

		return file_name;
	}

	//
	// 指定されたファイルを実行します。
	//
	inline BOOL shell_execute(LPCTSTR verb, LPCTSTR file, LPCTSTR parameters)
	{
		SHELLEXECUTEINFO sei = { sizeof(sei) };
		sei.lpVerb = verb;
		sei.nShow = SW_SHOWNORMAL;
		sei.lpFile = file;
		sei.lpParameters = parameters;
		return ::ShellExecuteEx(&sei);
	}

	//
	// 与えられた文字列を'\0'区切りのコレクションにして返します。
	//
	inline std::vector<std::string> split_by_zero(LPCSTR p)
	{
		// 文字列のコレクションです。
		std::vector<std::string> strings;

		// 文字列の先頭を取得します。
		auto begin = p;

		while (TRUE)
		{
			// 文字列の末尾を取得します。
			auto end = p++;

			// 文字列の末尾が区切り文字の場合は
			if (*end == '\0')
			{
				// 文字列をコレクションに追加します。
				strings.emplace_back(begin, end);

				// 文字列の先頭を更新します。
				begin = p;

				// 次の文字列の先頭が無効の場合はループを終了します。
				if (!*begin) break;
			}
		}

		// 文字列のコレクションを返します。
		return strings;
	}

	//
	// 文字列を指定された文字で区切って返します。
	//
	inline std::pair<std::string, std::string> split(const std::string& s, char ch)
	{
		auto c = s.length();
		for (decltype(c) i = 0; i < c; i++)
		{
			// リードバイトの場合は
			if (::IsDBCSLeadByte(s[i]))
			{
				// リードバイトをスキップします。
				// このあとfor文内でトレイルバイトもスキップされます。
				i++;

				continue;
			}

			// 指定された文字が見つかった場合は
			if (s[i] == ch)
			{
				// 文字列を分割して返します。
				return { s.substr(0, i), s.substr(i + 1) };
			}
		}

		// 文字が見つからなかった場合の処理です。
		return { s, "" };
	}

	//
	// アセットファイルのパスを返します。
	//
	inline std::filesystem::path get_assets_file_name(const std::wstring& file_name)
	{
		// ".../assets/アドイン名/file_name"を返します。
		std::filesystem::path folder_name =
			magi.get_assets_file_name(hive.c_name);
		return folder_name / file_name;
	}

	//
	// コンフィグファイルのパスを返します。
	//
	inline std::filesystem::path get_config_file_name(const std::wstring& file_name)
	{
		// ".../config/アドイン名/file_name"を返します。
		std::filesystem::path folder_name =
			magi.get_config_file_name(hive.c_name);
		return folder_name / file_name;
	}

	//
	// ダウンロードフォルダのパスを返します。
	//
	inline std::filesystem::path get_download_folder_name()
	{
		return get_config_file_name(L"download");
	}

	//
	// スクリプトのフォルダーパスを返します。
	//
	inline std::filesystem::path get_script_folder_path(const auto& node)
	{
		return my::get_module_file_name(nullptr).parent_path() / L"script" / node->folder_name;
	}

	//
	// スクリプトのパスを返します。
	//
	inline std::wstring get_script_path(const auto& node, const auto& extension)
	{
		return get_script_folder_path(node) / (node->file_name + extension);
	}

	//
	// スクリプトのファイルタイムを文字列で返します。
	//
	inline std::wstring get_file_time_string(const std::wstring& path)
	{
		my::handle::unique_ptr<> file(::CreateFileW(path.c_str(), GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr));
		if (file.get() == INVALID_HANDLE_VALUE) return {};

		FILETIME file_time = {};
		::GetFileTime(file.get(), nullptr, nullptr, &file_time);

		FILETIME local_file_time = {};
		::FileTimeToLocalFileTime(&file_time, &local_file_time);

		SYSTEMTIME local_time = {};
		::FileTimeToSystemTime(&local_file_time, &local_time);

		return my::format(
			L"{/04d}/{/02d}/{/02d} {/02d}:{/02d}:{/02d}",
			local_time.wYear,
			local_time.wMonth,
			local_time.wDay,
			local_time.wHour,
			local_time.wMinute,
			local_time.wSecond);
	}

	//
	// スクリプトのIDを返します。
	//
	inline std::wstring get_script_id(const auto& node)
	{
		return node->folder_name + L"/" + node->file_name + L"/" + node->script_name;
	}

	//
	// 拡張編集のカスタムオブジェクトを再初期化します。
	//
	inline auto reinit_exedit_custom_objects()
	{
		auto exedit = magi.exin.get_exedit();
		auto custom_object_filter = (ExEdit::Filter*)(exedit + 0x9D3E0);
		auto init_custom_object_filter = (BOOL (*)(ExEdit::Filter* filter))(exedit + 0x1530);

		return init_custom_object_filter(custom_object_filter);
	}

	//
	// このクラスはサブプロセスです。
	//
	struct SubProcess
	{
		HANDLE job_object = {};
		PROCESS_INFORMATION pi = {};

		//
		// コンストラクタです。
		//
		SubProcess(const std::wstring& dir, const std::wstring& arg_command_line, BOOL show = TRUE)
		{
			// ジョブオブジェクトを作成します。
			job_object = ::CreateJobObject({}, {});

			// メインプロセスが終了した時にサブプロセスも終了させたいので
			// ジョブオブジェクトにJOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSEを設定します。
			auto job_object_info = JOBOBJECT_EXTENDED_LIMIT_INFORMATION {};
			job_object_info.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
			auto r2 = ::SetInformationJobObject(job_object,
				JobObjectExtendedLimitInformation, &job_object_info, sizeof(job_object_info));

			// 一旦コマンドラインをコピーします。
			WCHAR command_line[4096] = {};
			wcsncpy_s(command_line, arg_command_line.c_str(), _TRUNCATE);

			// サブプロセスを作成します。
			STARTUPINFOW si = { sizeof(si) };
			if (!show)
			{
				// 非表示で起動するようにします。
				si.dwFlags = STARTF_USESHOWWINDOW;
				si.wShowWindow = SW_HIDE;
			}
			auto create_result = ::CreateProcessW(
				nullptr,		// application_name
				command_line,	// command_line
				nullptr,		// process_attributes
				nullptr,		// thread_attributes
				FALSE,			// inherit_handles
				CREATE_NEW_CONSOLE | // このフラグを指定すると新しいコンソールウィンドウが開きます。
				CREATE_SUSPENDED,	// create_flags
				nullptr,		// enviroment
				dir.c_str(),	// current_directory
				&si,			// startup_info
				&pi);			// process_information

			// サブプロセスにジョブオブジェクトを関連付けます。
			auto assign_result = ::AssignProcessToJobObject(job_object, pi.hProcess);

			// サブプロセスを実行します。
			::ResumeThread(pi.hThread);
		}

		//
		// デストラクタです。
		//
		~SubProcess()
		{
			// サブプロセスのハンドルを閉じます。
			::CloseHandle(job_object);
			::CloseHandle(pi.hProcess);
			::CloseHandle(pi.hThread);
		}

		//
		// サブプロセスが終了するまで待機します。
		//
		DWORD wait(DWORD time = INFINITE)
		{
			return ::WaitForSingleObject(pi.hProcess, time);
		}
	};
}
