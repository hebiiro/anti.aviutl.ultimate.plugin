#pragma once
#include "Hive.h"
#include "FontCollection.h"

namespace fgo::font_select
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
			std::wstring fileName = magi.getConfigFileName(L"FontSelectList.txt");

			// テキストファイルストリームを作成します。
			Handle file = createFileForRead(fileName.c_str());
			auto fileSize = ::GetFileSize(file, 0);
			std::wstring string(fileSize / sizeof(WCHAR) + 1, _T('\0'));
			readFile(file, &string[0], fileSize);
			std::wstringstream stream(string);
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

				MY_TRACE_WSTR(line.c_str());
				MY_TRACE_INT(line.length());

				// 空行は無視します。
				if (wcslen(line.c_str()) == 0) continue;

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
						HMENU subMenu = ::CreatePopupMenu();

						// サブメニューをメニューに追加します。
						::AppendMenuW(menu, MF_POPUP, (UINT)subMenu, &line[1]);

						// 読み込み対象をサブメニューに変えて処理を続けます。
						read(stream, subMenu);
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
							const auto& fontName = fonts.collection[i];

							// フォント名がlineにマッチするなら
							if (std::regex_search(fontName, re))
							{
								// アイテムをメニューに追加します。
								::AppendMenuW(menu, MF_STRING | MF_OWNERDRAW, i + 1, fontName.c_str());
							}
						}
					}
					catch (...)
					{
						// 文法エラー用テキストを構築します。
						WCHAR text[MAX_PATH] = {};
						::StringCchPrintfW(text, std::size(text), L"%sは文法エラーです", line.c_str());

						// 正規表現の文法エラーであることをユーザーに知らせます。
						::AppendMenuW(menu, MF_STRING, 0, text);
					}
				}
			}

			return TRUE;
		}
	};
}
