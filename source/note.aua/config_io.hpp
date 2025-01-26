#pragma once

namespace apn::note
{
	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct ConfigIO : StdConfigIO
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			hive.config_file_name = magi.get_config_file_name(hive.instance);
			MY_TRACE_STR(hive.config_file_name);

			hive.sharing_note_file_name = magi.get_config_file_name(L"note/sharing_note.txt");
			MY_TRACE_STR(hive.sharing_note_file_name);

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

		//
		// コンフィグを読み込みます。
		//
		BOOL read()
		{
			MY_TRACE_FUNC("");

			return read_file(hive.config_file_name, hive);
		}

		//
		// コンフィグを書き込みます。
		//
		BOOL write()
		{
			MY_TRACE_FUNC("");

			return write_file(hive.config_file_name, hive);
		}

		//
		// コンフィグが更新されたのでコントロールに適用します。
		//
		virtual BOOL update() override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			read_window_pos(root, "sharing_note", sharing_note);
			read_window_pos(root, "project_note", project_note);

			// txtファイルから共通ノートを読み込みます。
			try
			{
				if (std::filesystem::exists(hive.sharing_note_file_name))
				{
					std::ifstream ifs(hive.sharing_note_file_name, std::ios_base::binary);
					auto file_size = (size_t)std::filesystem::file_size(hive.sharing_note_file_name);
					std::vector<char> sharing_text(file_size + sizeof(WCHAR), '\0');
					ifs.read(sharing_text.data(), sharing_text.size());
					::SetWindowText(sharing_note.richedit, (LPCWSTR)sharing_text.data());
				}
			}
			catch (...)
			{
				hive.message_box(L"共通ノートの読み込みに失敗しました");
			}

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			write_window_pos(root, "sharing_note", sharing_note);
			write_window_pos(root, "project_note", project_note);

			// txtファイルに共通ノートを書き込みます。
			try
			{
				std::ofstream ofs(hive.sharing_note_file_name, std::ios_base::binary);
				auto sharing_text = my::get_window_text(sharing_note.richedit);
				ofs.write((const char*)sharing_text.data(), sharing_text.length() * sizeof(WCHAR));
			}
			catch (...)
			{
			}

			return TRUE;
		}
	} config_io;
}
