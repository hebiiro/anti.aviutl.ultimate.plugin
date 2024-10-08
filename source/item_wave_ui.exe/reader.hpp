#pragma once

namespace apn::item_wave::ui
{
	//
	// このクラスはリーダーです。
	//
	struct Reader
	{
		//
		// リーダープロセスのプロセス情報です。
		//
		PROCESS_INFORMATION pi = {};

		//
		// リーダープロセスとデータをやり取りするための共有メモリです。
		//
		my::SimpleFileMappingT<share::ReaderBuffer> shared;

		//
		// コンストラクタです。
		//
		Reader()
		{
			MY_TRACE_FUNC("");

			// リーダープロセスを作成します。

			auto path = magi.get_module_file_name(L"item_wave_reader.exe");
			MY_TRACE_STR(path);

			auto args = my::format(_T("{:#010x}"), hive.host_window);
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

				return;
			}

			shared.init(share::get_reader_buffer_name(get_id()).c_str());
		}

		//
		// デストラクタです。
		//
		~Reader()
		{
			MY_TRACE_FUNC("{}, {}", pi.dwProcessId, pi.dwThreadId);

			::CloseHandle(pi.hThread);
			::CloseHandle(pi.hProcess);
		}

		//
		// このリーダーのIDを返します。
		//
		DWORD get_id()
		{
			return pi.dwThreadId;
		}

		//
		// このリーダーの共有メモリを返します。
		//
		share::ReaderBuffer* get_buffer()
		{
			return shared.get_buffer();
		}
	};

	//
	// このクラスはリーダーを管理します。
	//
	inline struct ReaderManager
	{
		//
		// リーダーのコレクションです。
		//
		std::map<DWORD, std::shared_ptr<Reader>> collection;

		//
		// 同時に保有できるリーダーの最大数です。
		//
		int32_t max_reader_count = 1;

		//
		// 指定されたIDに対応するリーダーを返します。
		//
		std::shared_ptr<Reader> get_reader(DWORD id)
		{
			auto it = collection.find(id);
			if (it == collection.end()) return nullptr;
			return it->second;
		}

		//
		// 指定されたファイルを読み込むリーダーを作成して返します。
		//
		std::shared_ptr<Reader> create_reader(const std::string& file_name)
		{
			MY_TRACE_FUNC("{}", file_name);

			auto reader = std::make_shared<Reader>();
			auto shared = reader->get_buffer();
			if (!shared) return nullptr;
			share::copy(shared->file_name, file_name);
			collection[reader->get_id()] = reader;
			return reader;
		}

		//
		// 指定されたIDに対応するリーダーを削除します。
		//
		void erase_reader(DWORD id)
		{
			MY_TRACE_FUNC("{}", id);

			collection.erase(id);
		}

		//
		// リーダーを追加できる場合はTRUEを返します。
		//
		BOOL available()
		{
			return (int32_t)collection.size() < max_reader_count;
		}
	} reader_manager;
}
