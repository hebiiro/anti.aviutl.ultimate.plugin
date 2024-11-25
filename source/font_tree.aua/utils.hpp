#pragma once

namespace apn::font_tree::cb
{
	//
	// コンボボックスの項目数を返します。
	//
	inline int32_t get_count(HWND hwnd)
	{
		return (int32_t)::SendMessageA(hwnd, CB_GETCOUNT, 0, 0);
	}

	//
	// コンボボックスの選択項目を返します。
	//
	inline int32_t get_cur_sel(HWND hwnd)
	{
		return (int32_t)::SendMessageA(hwnd, CB_GETCURSEL, 0, 0);
	}

	//
	// コンボボックスの項目の文字列を返します。
	//
	inline std::wstring get_text(HWND hwnd, int32_t index)
	{
		if (index == -1) index = get_cur_sel(hwnd);

		auto text_length = ::SendMessageW(hwnd, CB_GETLBTEXTLEN, index, 0);
		if (text_length == CB_ERR) return {};
		std::wstring text(text_length, L'\0');
		::SendMessageW(hwnd, CB_GETLBTEXT, index, (LPARAM)text.data());
		return text;
	}

	//
	// 指定されたフォント名のインデックスを返します。
	// (CB_FINDSTRINGは部分一致でもヒットするので意図した動作になりません)
	//
	inline int32_t find_string(HWND combobox, const std::wstring& font_name)
	{
		// コンボボックスの項目を走査します。
		auto count = get_count(combobox);
		for (auto i = 0; i < count; i++)
		{
			// 項目のテキストを取得します。
			auto text = get_text(combobox, i);
			if (text.empty()) continue;

			// テキストがフォント名と完全に一致する場合は
			if (text == font_name)
				return i; // インデックスを返します。
		}

		// 指定されたフォント名が見つからなかった場合の処理です。
		return CB_ERR;
	}

	//
	// コンボボックス内の指定された文字列の項目を選択します。
	//
	inline void select_text(HWND combobox, const std::wstring& text)
	{
		// コンボボックスの選択項目を切り替えます。
		ComboBox_SelectString(combobox, 0, text.c_str());

		// 親ウィンドウに通知メッセージを送信します。
		::SendMessage(
			::GetParent(combobox),
			WM_COMMAND,
			MAKEWPARAM(::GetDlgCtrlID(combobox), CBN_SELCHANGE),
			(LPARAM)combobox);
	}
}

namespace apn::font_tree::lb
{
	//
	// リストボックスの項目数を返します。
	//
	inline int32_t get_count(HWND hwnd)
	{
		return (int32_t)::SendMessageA(hwnd, LB_GETCOUNT, 0, 0);
	}

	//
	// リストボックスの選択項目を返します。
	//
	inline int32_t get_cur_sel(HWND hwnd)
	{
		return (int32_t)::SendMessageA(hwnd, LB_GETCURSEL, 0, 0);
	}

	//
	// リストボックスの項目の文字列を返します。
	//
	inline std::wstring get_text(HWND hwnd, int32_t index)
	{
		if (index == -1) index = get_cur_sel(hwnd);

		auto text_length = ::SendMessageW(hwnd, LB_GETTEXTLEN, index, 0);
		if (text_length == LB_ERR) return {};
		std::wstring text(text_length, L'\0');
		::SendMessageW(hwnd, LB_GETTEXT, index, (LPARAM)text.data());
		return text;
	}
}
