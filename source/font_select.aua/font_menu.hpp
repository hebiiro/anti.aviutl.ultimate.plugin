#pragma once

namespace apn::font_select
{
	//
	// このクラスはフォント選択メニューです。
	//
	struct FontMenu
	{
		HMENU menu = 0;
		FontCollection fonts;

		//
		// コンストラクタです。
		//
		FontMenu(HWND combobox)
			: fonts(combobox)
		{
			// メインのポップアップメニューを作成します。
			menu = ::CreatePopupMenu();
    
			// フォントリストのテキストファイル名です。
			auto file_name = magi.get_assets_file_name(L"font_select/font_menu.txt");
			MY_TRACE_STR(file_name);

			// テキストファイルストリームを作成します。
			auto file_size = (size_t)std::filesystem::file_size(file_name);
			std::ifstream ifs(file_name, std::ios_base::binary);
			std::wstring str(file_size + 1, L'\0');
			ifs.read((char*)str.data(), file_size);
			std::wstringstream stream(str);
			if (!stream) return;

			// テキストファイルを読み込みます。
			read(stream, menu);
		}

		//
		// デストラクタです。
		//
		~FontMenu()
		{
			// メインのポップアップメニューを破壊します。
			::DestroyMenu(menu);
		}

		operator HMENU() const { return menu; }

		//
		// 文字列をトリムします。
		// https://www.paveway.info/entry/2019/09/29/cpp_trim
		//
		static std::wstring trim(const std::wstring& src)
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
		// 文字列ストリームからメニューを構築します。
		//
		BOOL read(std::wstringstream& stream, HMENU menu)
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
						// サブメニューを作成します。
						// サブメニューは追加先メニュー(メインメニュー)が破壊されるとき同時に破壊されます。
						auto sub_menu = ::CreatePopupMenu();

						// サブメニューをメニューに追加します。
						::AppendMenuW(menu, MF_POPUP, (UINT)sub_menu, &line[1]);

						// 読み込み対象をサブメニューに変えて処理を続けます。
						read(stream, sub_menu);
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
						for (size_t i = 0; i < fonts.collection.size(); i++)
						{
							const auto& font_name = fonts.collection[i];

							// フォント名がlineにマッチするなら
							if (std::regex_search(font_name, re))
							{
								// アイテムをメニューに追加します。
								::AppendMenuW(menu, MF_STRING | MF_OWNERDRAW, i + 1, font_name.c_str());
							}
						}
					}
					catch (...)
					{
						// 正規表現の文法エラーであることをユーザーに知らせます。
						::AppendMenuW(menu, MF_STRING, 0,
							std::format(L"{}は文法エラーです", line).c_str());
					}
				}
			}

			return TRUE;
		}
	};
}
