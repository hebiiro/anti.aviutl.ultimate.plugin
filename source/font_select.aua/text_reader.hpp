#pragma once

namespace apn::font_select
{
	//
	// このクラスは旧形式のテキストファイルを読み込みます。
	//
	struct TextReader
	{
		const std::vector<std::wstring> fonts;

		//
		// コンストラクタです。
		//
		TextReader(const std::vector<std::wstring>& fonts)
			: fonts(fonts)
		{
		}

		//
		// 文字列をトリムします。
		// https://www.paveway.info/entry/2019/09/29/cpp_trim
		//
		inline static std::wstring trim(const std::wstring& src)
		{
			const std::wstring trim_characters = L"\xfeff\r\n \t";

			// trimするとき削除する文字以外の最初の文字の位置を取得します。
			size_t start = src.find_first_not_of(trim_characters);
			if (start == std::wstring::npos) return L"";

			// trimするとき削除する文字以外の最後の文字の位置を取得します。
			size_t end = src.find_last_not_of(trim_characters);
			if (end == std::wstring::npos) return L"";

			// trimするとき削除する文字以外の文字列を切り出します。
			return src.substr(start, end - start + 1);
		}

		//
		// テキストファイルを読み込みます。
		//
		void read()
		{
			// フォントリストのテキストファイル名を取得します。
			auto file_name = magi.get_config_file_name(L"font_select/font_menu.txt");
			MY_TRACE_STR(file_name);
			if (!std::filesystem::exists(file_name))
				auto file_name = magi.get_assets_file_name(L"font_select/font_menu.txt");
			MY_TRACE_STR(file_name);

			// テキストファイルストリームを開きます。
			std::ifstream ifs(file_name, std::ios_base::binary);
			if (!ifs) return;

			// すべてのテキストをバッファに読み込みます。
			auto file_size = (size_t)std::filesystem::file_size(file_name);
			std::wstring buffer(file_size + 1, L'\0');
			ifs.read((LPSTR)buffer.data(), file_size);

			// 文字列ストリームを開きます。
			std::wstringstream stream(buffer);
			if (!stream) return;

			// テキストファイルを読み込みます。
			read(stream, hive.menu_root);
		}

		//
		// 文字列ストリームからメニュー項目を読み込みます。
		//
		BOOL read(std::wstringstream& stream, Hive::Menu::Node& node)
		{
			std::wstring line;
			while (std::getline(stream, line))
			{
				line = trim(line);

				MY_TRACE_STR(line);
				MY_TRACE_INT(line.length());

				// 空行は無視します。
				if (line.empty()) continue;

				// コメント行は無視します。
				if (line.starts_with(L";")) continue;
//				if (line.starts_with(L"//")) continue;

				// グループ行の場合は
				if (line.starts_with(L'/'))
				{
					// 名前なしグループ行の場合は
					if (line.length() == 1)
					{
						return FALSE;
					}
					// 名前ありグループ行の場合は
					else
					{
						// サブメニューを読み込みます。
						read(stream, node.nodes.emplace_back(MF_POPUP, 0, &line[1]));
					}
				}
				// グループ行ではない場合は
				else
				{
					try
					{
						// 正規表現を構築します。
						std::wregex re(line);

						// 拡張編集が読み込んでいるフォント名を列挙します。
						for (size_t i = 0; i < fonts.size(); i++)
						{
							// フォント名を取得します。
							const auto& font_name = fonts[i];

							// フォント名がlineにマッチするなら
							if (std::regex_search(font_name, re))
							{
								// 項目を追加します。
								node.nodes.emplace_back(MF_STRING | MF_OWNERDRAW, i + 1, font_name);
							}
						}
					}
					catch (...)
					{
						// 正規表現の文法エラーであることをユーザーに知らせます。
						node.nodes.emplace_back(MF_STRING, 0, my::format(L"{/}は文法エラーです", line));
					}
				}
			}

			return TRUE;
		}
	};
}
