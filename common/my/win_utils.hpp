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

	inline void client_to_window(HWND hwnd, LPRECT rc)
	{
		auto window_rc = get_window_rect(hwnd);
		auto client_rc = get_client_rect(hwnd);
		::MapWindowPoints(hwnd, 0, (LPPOINT)&client_rc, 2);

		rc->left += window_rc.left - client_rc.left;
		rc->top += window_rc.top - client_rc.top;
		rc->right += window_rc.right - client_rc.right;
		rc->bottom += window_rc.bottom - client_rc.bottom;
	}

	inline void window_to_client(HWND hwnd, LPRECT rc)
	{
		auto window_rc = get_window_rect(hwnd);
		auto client_rc = get_client_rect(hwnd);
		::MapWindowPoints(hwnd, 0, (LPPOINT)&client_rc, 2);

		rc->left -= window_rc.left - client_rc.left;
		rc->top -= window_rc.top - client_rc.top;
		rc->right -= window_rc.right - client_rc.right;
		rc->bottom -= window_rc.bottom - client_rc.bottom;
	}

	inline auto set_window_rect(HWND hwnd, LPCRECT rc, UINT flags = 0)
	{
		auto x = rc->left;
		auto y = rc->top;
		auto w = my::get_width(*rc);
		auto h = my::get_height(*rc);

		return ::SetWindowPos(hwnd, 0, x, y, w, h, SWP_NOZORDER | flags);
	}

	inline auto map_window_points(HWND from, HWND to, LPPOINT pt)
	{
		return ::MapWindowPoints(from, to, pt, 1);
	}

	inline auto map_window_points(HWND from, HWND to, LPRECT rc)
	{
		return ::MapWindowPoints(from, to, (LPPOINT)rc, 2);
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

	inline HANDLE create_file_for_read(LPCTSTR file_name)
	{
		return ::CreateFile(file_name, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	}

	inline HANDLE create_file_for_write(LPCTSTR file_name)
	{
		return ::CreateFile(file_name, GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
	}

	inline HANDLE create_file_for_edit(LPCTSTR file_name)
	{
		return ::CreateFile(file_name, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_ALWAYS, 0, 0);
	}

	inline HANDLE create_file_for_read_write(LPCTSTR file_name)
	{
		return ::CreateFile(file_name, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
	}
}
