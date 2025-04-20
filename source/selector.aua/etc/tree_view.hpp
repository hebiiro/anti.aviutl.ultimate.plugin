#pragma once

namespace apn::selector::tv
{
	//
	// ツリービューにアイテムを追加して返します。
	//
	inline HTREEITEM insert_item(HWND tree_view, HTREEITEM parent, HTREEITEM insert_after, LPCWSTR text, uint32_t state, LPARAM param)
	{
		TVINSERTSTRUCT is = {
			.hParent = parent,
			.hInsertAfter = insert_after,
			.item = {
				.mask = TVIF_STATE | TVIF_TEXT | TVIF_PARAM,
				.state = state,
				.stateMask = (uint32_t)-1,
				.pszText = (LPWSTR)text,
				.lParam = param,
			},
		};
		return TreeView_InsertItem(tree_view, &is);
	}

	//
	// std::wstringを作成して返します。
	//
	inline constexpr std::wstring make_wstring(size_t size, const auto& func)
	{
		std::wstring buffer(size, L'\0');
		func(buffer);
		buffer.resize(wcslen(buffer.c_str()));
		return buffer;
	}

	//
	// 項目のテキストを返します。
	//
	inline std::wstring get_text(HWND tree_view, HTREEITEM item_handle)
	{
		return make_wstring(MAX_PATH, [&](auto& buffer) {
			TVITEM item = {
				.mask = TVIF_HANDLE | TVIF_TEXT,
				.hItem = item_handle,
				.pszText = buffer.data(),
				.cchTextMax = (int)buffer.size(),
			};
			TreeView_GetItem(tree_view, &item);
		});
	}
#if 0
	//
	// アイテムのチェック状態を返します。
	//
	inline uint32_t get_check_state(HWND tree_view, HTREEITEM item_handle)
	{
		TVITEM item = {
			.mask = TVIF_HANDLE | TVIF_STATE,
			.hItem = item_handle,
			.stateMask = TVIS_STATEIMAGEMASK,
		};
		TreeView_GetItem(tree_view, &item);
		return item.state;
	}

	//
	// アイテムのチェック状態を変更します。
	//
	inline BOOL set_check_state(HWND tree_view, HTREEITEM item_handle, uint32_t check_state)
	{
		TVITEM item = {
			.mask = TVIF_HANDLE | TVIF_STATE,
			.hItem = item_handle,
			.state = check_state,
			.stateMask = TVIS_STATEIMAGEMASK,
		};
		return TreeView_SetItem(tree_view, &item);
	}
#endif
}
