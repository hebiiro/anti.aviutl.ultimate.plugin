#pragma once

namespace my
{
	inline int get_width(const RECT& rc)
	{
		return rc.right - rc.left;
	}

	inline int get_height(const RECT& rc)
	{
		return rc.bottom - rc.top;
	}

	inline int get_center_x(const RECT& rc)
	{
		return (rc.left + rc.right) / 2;
	}

	inline int get_center_y(const RECT& rc)
	{
		return (rc.top + rc.bottom) / 2;
	}

	inline POINT lp_to_pt(LPARAM lParam)
	{
		return { (short)LOWORD(lParam), (short)HIWORD(lParam) };
	}

	inline LPARAM pt_to_lp(POINT pt)
	{
		return MAKELPARAM(pt.x, pt.y);
	}

	inline POINT operator+(const POINT& a, const POINT& b)
	{
		return POINT { a.x + b.x, a.y + b.y };
	}

	inline POINT operator-(const POINT& a, const POINT& b)
	{
		return POINT { a.x - b.x, a.y - b.y };
	}

	inline auto get_cursor_pos()
	{
		POINT pt; ::GetCursorPos(&pt); return pt;
	}

	inline auto get_client_rect(HWND hwnd)
	{
		RECT rc; ::GetClientRect(hwnd, &rc); return rc;
	}

	inline auto get_window_rect(HWND hwnd)
	{
		RECT rc; ::GetWindowRect(hwnd, &rc); return rc;
	}

	//
	// ポイントが存在するモニタの矩形を返します。
	//
	inline auto get_monitor_rect(POINT point)
	{
		auto monitor = ::MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST);
		MONITORINFOEX monitor_info = {};
		monitor_info.cbSize = sizeof(monitor_info);
		::GetMonitorInfo(monitor, &monitor_info);
		return monitor_info.rcWork;
	}

	//
	// ウィンドウが表示されているモニタの矩形を返します。
	//
	inline auto get_monitor_rect(HWND hwnd)
	{
		auto monitor = ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFOEX monitor_info = {};
		monitor_info.cbSize = sizeof(monitor_info);
		::GetMonitorInfo(monitor, &monitor_info);
		return monitor_info.rcWork;
	}

	inline auto map_window_points(HWND from, HWND to, LPPOINT pt)
	{
		return ::MapWindowPoints(from, to, pt, 1);
	}

	inline auto map_window_points(HWND from, HWND to, LPRECT rc)
	{
		return ::MapWindowPoints(from, to, (LPPOINT)rc, 2);
	}

	inline void client_to_window(HWND hwnd, LPRECT rc)
	{
		auto window_rc = get_window_rect(hwnd);
		auto client_rc = get_client_rect(hwnd);
		map_window_points(hwnd, nullptr, &client_rc);

		rc->left += window_rc.left - client_rc.left;
		rc->top += window_rc.top - client_rc.top;
		rc->right += window_rc.right - client_rc.right;
		rc->bottom += window_rc.bottom - client_rc.bottom;
	}

	inline void window_to_client(HWND hwnd, LPRECT rc)
	{
		auto window_rc = get_window_rect(hwnd);
		auto client_rc = get_client_rect(hwnd);
		map_window_points(hwnd, nullptr, &client_rc);

		rc->left -= window_rc.left - client_rc.left;
		rc->top -= window_rc.top - client_rc.top;
		rc->right -= window_rc.right - client_rc.right;
		rc->bottom -= window_rc.bottom - client_rc.bottom;
	}

	inline auto set_window_pos(HWND hwnd, HWND insert_after, LPCRECT rc, UINT flags)
	{
		auto x = rc->left;
		auto y = rc->top;
		auto w = my::get_width(*rc);
		auto h = my::get_height(*rc);

		return ::SetWindowPos(hwnd, insert_after, x, y, w, h, flags);
	}

	inline auto defer_window_pos(HDWP dwp, HWND hwnd, HWND insert_after, LPCRECT rc, UINT flags)
	{
		auto x = rc->left;
		auto y = rc->top;
		auto w = my::get_width(*rc);
		auto h = my::get_height(*rc);

		return ::DeferWindowPos(dwp, hwnd, insert_after, x, y, w, h, flags);
	}

	inline auto set_window_rect(HWND hwnd, LPCRECT rc, UINT flags = 0)
	{
		return set_window_pos(hwnd, nullptr, rc, flags | SWP_NOZORDER);
	}

	//
	// hwnd2がhwnd1の祖先ウィンドウの場合はTRUEを返します。
	//
	inline BOOL is_ancestor(HWND hwnd1, HWND hwnd2)
	{
		return hwnd1 == hwnd2 || ::IsChild(hwnd2, hwnd1);
	}

	inline DWORD get_style(HWND hwnd)
	{
		return ::GetWindowLong(hwnd, GWL_STYLE);
	}

	inline DWORD set_style(HWND hwnd, DWORD style)
	{
		return ::SetWindowLong(hwnd, GWL_STYLE, style);
	}

	inline DWORD modify_style(HWND hwnd, DWORD remove, DWORD add)
	{
		auto style = get_style(hwnd);
		style &= ~remove;
		style |= add;
		return set_style(hwnd, style);
	}

	inline DWORD get_ex_style(HWND hwnd)
	{
		return ::GetWindowLong(hwnd, GWL_EXSTYLE);
	}

	inline DWORD set_ex_style(HWND hwnd, DWORD ex_style)
	{
		return ::SetWindowLong(hwnd, GWL_EXSTYLE, ex_style);
	}

	inline DWORD modify_ex_style(HWND hwnd, DWORD remove, DWORD add)
	{
		auto ex_style = get_ex_style(hwnd);
		ex_style &= ~remove;
		ex_style |= add;
		return set_ex_style(hwnd, ex_style);
	}

	inline void set_editbox_text_no_notify(HWND editbox, LPCTSTR text)
	{
		auto id = ::SetWindowLongPtr(editbox, GWLP_ID, 0);
		::SetWindowText(editbox, text);
		::SetWindowLongPtr(editbox, GWLP_ID, id);
	}

	inline void set_editbox_text_no_notify(HWND dialog, UINT id, LPCTSTR text)
	{
		set_editbox_text_no_notify(::GetDlgItem(dialog, id), text);
	}

	inline BOOL invalidate(HWND hwnd, LPCRECT rc = nullptr, BOOL erase = FALSE)
	{
		return ::InvalidateRect(hwnd, rc, erase);
	}

	inline BOOL track_mouse_event(HWND hwnd, DWORD flags = TME_LEAVE)
	{
		TRACKMOUSEEVENT tme = { sizeof(tme) };
		tme.dwFlags = flags;
		tme.hwndTrack = hwnd;
		return ::TrackMouseEvent(&tme);
	}

	inline HANDLE create_file_for_read(LPCTSTR file_name)
	{
		return ::CreateFile(file_name, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	}

	inline HANDLE create_file_for_write(LPCTSTR file_name)
	{
		return ::CreateFile(file_name, GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, 0, nullptr);
	}

	inline HANDLE create_file_for_edit(LPCTSTR file_name)
	{
		return ::CreateFile(file_name, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, 0, nullptr);
	}

	inline HANDLE create_file_for_read_write(LPCTSTR file_name)
	{
		return ::CreateFile(file_name, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
	}
}

// コンボボックス操作用のユーティリティです。
namespace my::combobox
{
	//
	// コンボボックスの項目数を返します。
	//
	inline int32_t get_count(HWND combobox)
	{
		return (int32_t)::SendMessageW(combobox, CB_GETCOUNT, 0, 0);
	}

	//
	// コンボボックスの選択項目を返します。
	//
	inline int32_t get_cur_sel(HWND combobox)
	{
		return (int32_t)::SendMessageW(combobox, CB_GETCURSEL, 0, 0);
	}

	//
	// コンボボックスの項目の文字列を返します。
	//
	inline std::wstring get_text(HWND combobox, int32_t index)
	{
		if (index == -1) index = get_cur_sel(combobox);

		auto text_length = ::SendMessageW(combobox, CB_GETLBTEXTLEN, index, 0);
		if (text_length == CB_ERR) return {};
		std::wstring text(text_length, L'\0');
		::SendMessageW(combobox, CB_GETLBTEXT, index, (LPARAM)text.data());
		return text;
	}

	//
	// コンボボックスに項目を追加します。
	//
	inline void add_text(HWND combobox, const std::wstring& text)
	{
		::SendMessageW(combobox, CB_ADDSTRING, 0, (LPARAM)text.data());
	}

	//
	// コンボボックスに項目を追加します。
	//
	inline void insert_text(HWND combobox, const std::wstring& text, int32_t index = -1)
	{
		::SendMessageW(combobox, CB_INSERTSTRING, index, (LPARAM)text.data());
	}

	//
	// コンボボックスの項目を削除します。
	//
	inline void erase_text(HWND combobox, int32_t index)
	{
		::SendMessageW(combobox, CB_DELETESTRING, index, 0);
	}

	//
	// コンボボックスの項目を消去します。
	//
	inline void clear(HWND combobox)
	{
		::SendMessageW(combobox, CB_RESETCONTENT, 0, 0);
	}

	//
	// 指定された項目名のインデックスを返します。
	// (CB_FINDSTRINGは部分一致でもヒットするので意図した動作になりません)
	//
	inline int32_t find_string(HWND combobox, const std::wstring& item_name)
	{
		// コンボボックスの項目を走査します。
		auto count = get_count(combobox);
		for (auto i = 0; i < count; i++)
		{
			// 項目のテキストを取得します。
			auto text = get_text(combobox, i);
			if (text.empty()) continue;

			// テキストが項目名と完全に一致する場合は
			if (text == item_name)
				return i; // インデックスを返します。
		}

		// 指定された項目名が見つからなかった場合の処理です。
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
		::SendMessageW(
			::GetParent(combobox),
			WM_COMMAND,
			MAKEWPARAM(::GetDlgCtrlID(combobox), CBN_SELCHANGE),
			(LPARAM)combobox);
	}
}

// リストボックス操作用のユーティリティです。
namespace my::listbox
{
	//
	// リストボックスの項目数を返します。
	//
	inline int32_t get_count(HWND listbox)
	{
		return (int32_t)::SendMessageW(listbox, LB_GETCOUNT, 0, 0);
	}

	//
	// リストボックスの選択項目を返します。
	//
	inline int32_t get_cur_sel(HWND listbox)
	{
		return (int32_t)::SendMessageW(listbox, LB_GETCURSEL, 0, 0);
	}

	//
	// リストボックスの項目の文字列を返します。
	//
	inline std::wstring get_text(HWND listbox, int32_t index)
	{
		if (index == -1) index = get_cur_sel(listbox);

		auto text_length = ::SendMessageW(listbox, LB_GETTEXTLEN, index, 0);
		if (text_length == LB_ERR) return {};
		std::wstring text(text_length, L'\0');
		::SendMessageW(listbox, LB_GETTEXT, index, (LPARAM)text.data());
		return text;
	}
}
