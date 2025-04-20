#pragma once

namespace apn::selector::custom_object
{
	//
	// このクラスはカスタムオブジェクトのスペックを読み込みます。
	//
	inline struct SpecReader
	{
		//
		// tsvファイルを読み込みます。
		//
		BOOL read_tsv_file(const std::filesystem::path& tsv_file_path)
		{
			MY_TRACE_FUNC("{/}", tsv_file_path);

			try
			{
				// ファイルを相対参照する場合の原点となるパスを取得します。
				auto origin = tsv_file_path.parent_path();

				// ファイルストリームを開きます。
				std::ifstream file(tsv_file_path);

				// 一行ずつ読み込みます。
				std::string line;
				std::getline(file, line); // 一行目は読み飛ばします。
				while (std::getline(file, line))
				{
					// 末尾のキャリッジリターンを除去します。
					if (line.length() && line.back() == '\r') line.pop_back();

					// 文字列ストリームを開きます。
					std::wstringstream stream(my::ws(line));

					constexpr size_t c_author = 0;
					constexpr size_t c_distributor = 1;
					constexpr size_t c_script_name = 2;
					constexpr size_t c_folder_name = 3;
					constexpr size_t c_file_name = 4;
					constexpr size_t c_tag = 5;
					constexpr size_t c_desc = 6;
					constexpr size_t c_max_size = 7;

					std::vector<std::wstring> cells(c_max_size);

					// タブ区切りでセルを読み込みます。
					for (auto& cell : cells)
						std::getline(stream, cell, L'\t');

					// カスタムオブジェクトを作成します。
					auto custom_object = std::make_shared<Agit::List::Node>();
					custom_object->script_name = cells[c_script_name];
					custom_object->folder_name = cells[c_folder_name];
					custom_object->file_name = cells[c_file_name];
					custom_object->author = cells[c_author];
					custom_object->tag = cells[c_tag];
					custom_object->desc = cells[c_desc];
					if (cells[c_distributor].length()) custom_object->distributor = origin / cells[c_distributor];

					// スクリプト名が無効の場合は除外します。
					if (custom_object->script_name.empty()) continue;

					// カスタムオブジェクトをリストに追加します。
					auto existing = agit.list.add(custom_object);

					// カスタムオブジェクトを追加できなかった場合は
					if (existing != custom_object)
					{
						// 既存のカスタムオブジェクトの設定を上書きします。
						existing->author = custom_object->author;
						existing->tag = custom_object->tag;
						existing->desc = custom_object->desc;
						existing->distributor = custom_object->distributor;
					}
				}

				return TRUE;
			}
			catch (...)
			{
			}

			return FALSE;
		}

		//
		// フォルダを読み込みます。
		//
		BOOL read_folder(const std::filesystem::path& origin)
		{
			MY_TRACE_FUNC("{/}", origin);

			auto result = FALSE;

			try
			{
				// フォルダ内のファイルとフォルダを列挙します。
				for (const auto& entry : std::filesystem::directory_iterator(origin))
				{
					// パスを取得します。
					auto path = entry.path();

					// パスがフォルダの場合は
					if (std::filesystem::is_directory(path))
					{
						// フォルダを再帰的に読み込みます。
						result |= read_folder(path);
					}
					// パスがファイルの場合は
					else
					{
						// tsvファイルの場合は
						if (::lstrcmpiW(path.extension().c_str(), L".tsv") == 0)
						{
							// tsvファイルを読み込みます。
							result |= read_tsv_file(path);
						}
					}
				}
			}
			catch (...)
			{
			}

			return result;
		}

		//
		// アセットフォルダ内のカスタムオブジェクトのスペックファイルを読み込みます。
		//
		BOOL read()
		{
			MY_TRACE_FUNC("");

			try
			{
				// 読み込むフォルダのパスを取得します。
				auto origin = get_assets_file_name(L"custom_object");

				// フォルダを読み込みます。
				return read_folder(origin);
			}
			catch (...)
			{
			}

			return FALSE;
		}
	} spec_reader;
}
