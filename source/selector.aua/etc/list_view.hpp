#pragma once

namespace apn::selector::lv
{
	//
	// リストビューに列を追加します。
	//
	inline int32_t insert_column(HWND list_view, int32_t index, LPCWSTR text, int32_t width)
	{
		MY_TRACE_FUNC("");

		LVCOLUMN col = {
			.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
			.fmt = LVCFMT_LEFT,
			.cx = width,
			.pszText = (LPWSTR)text,
			.iSubItem = index,
		};

		return ListView_InsertColumn(list_view, index, &col);
	}

	//
	// リストビューに行を追加します。
	//
	inline int32_t insert_item(HWND list_view, int32_t index, int32_t sub_index, LPCWSTR text)
	{
		MY_TRACE_FUNC("");

		if (sub_index)
		{
			LVITEM item {
				.mask = LVIF_TEXT,
				.iItem = index,
				.iSubItem = sub_index,
				.pszText = (LPWSTR)text,
			};
			return ListView_SetItem(list_view, &item);
		}
		else
		{
			LVITEM item {
				.mask = LVIF_TEXT | LVIF_PARAM,
				.iItem = index,
				.iSubItem = sub_index,
				.pszText = (LPWSTR)text,
				.lParam = index,
			};
			return ListView_InsertItem(list_view, &item);
		}
	}

	//
	// リストビューの指定された位置にあるテキストを返します。
	//
	inline std::wstring get_item_text(HWND list_view, int32_t index, int32_t sub_item)
	{
		TCHAR buffer[MAX_PATH] = {};
		ListView_GetItemText(list_view, index, sub_item, buffer, std::size(buffer));
		return buffer;
	}

	//
	// リストビューの指定された位置にあるテキストを設定します。
	//
	inline void set_item_text(HWND list_view, int32_t index, int32_t sub_item, LPCWSTR text)
	{
		ListView_SetItemText(list_view, index, sub_item, (LPWSTR)text);
	}

	//
	// リストビューの指定された位置にあるパラメータを返します。
	//
	inline LPARAM get_item_param(HWND list_view, int32_t index)
	{
		LVITEM item {
			.mask = LVIF_PARAM,
			.iItem = index,
			.lParam = -1,
		};
		ListView_GetItem(list_view, &item);
		return item.lParam;
	}
}
