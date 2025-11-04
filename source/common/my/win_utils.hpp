#pragma once

inline POINT operator+(const POINT& a, const POINT& b)
{
	return POINT { a.x + b.x, a.y + b.y };
}

inline POINT operator-(const POINT& a, const POINT& b)
{
	return POINT { a.x - b.x, a.y - b.y };
}

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

	inline POINT get_near(const RECT& rc)
	{
		return { rc.left, rc.top };
	}

	inline POINT get_far(const RECT& rc)
	{
		return { rc.right, rc.bottom };
	}

	inline POINT lp_to_pt(LPARAM lParam)
	{
		return { (short)LOWORD(lParam), (short)HIWORD(lParam) };
	}

	inline LPARAM pt_to_lp(POINT pt)
	{
		return MAKELPARAM(pt.x, pt.y);
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

	//
	// ウィンドウメッセージを文字列に変換して返します。
	//
	inline std::wstring message_to_string(UINT message)
	{
		switch (message)
		{
		case 0x0000: return L"WM_NULL";
		case 0x0001: return L"WM_CREATE";
		case 0x0002: return L"WM_DESTROY";
		case 0x0003: return L"WM_MOVE";
		case 0x0005: return L"WM_SIZE";
		case 0x0006: return L"WM_ACTIVATE";
		case 0x0007: return L"WM_SETFOCUS";
		case 0x0008: return L"WM_KILLFOCUS";
		case 0x000A: return L"WM_ENABLE";
		case 0x000B: return L"WM_SETREDRAW";
		case 0x000C: return L"WM_SETTEXT";
		case 0x000D: return L"WM_GETTEXT";
		case 0x000E: return L"WM_GETTEXTLENGTH";
		case 0x000F: return L"WM_PAINT";
		case 0x0010: return L"WM_CLOSE";
		case 0x0011: return L"WM_QUERYENDSESSION";
		case 0x0013: return L"WM_QUERYOPEN";
		case 0x0016: return L"WM_ENDSESSION";
		case 0x0012: return L"WM_QUIT";
		case 0x0014: return L"WM_ERASEBKGND";
		case 0x0015: return L"WM_SYSCOLORCHANGE";
		case 0x0018: return L"WM_SHOWWINDOW";
		case 0x001A: return L"WM_SETTINGCHANGE";
		case 0x001B: return L"WM_DEVMODECHANGE";
		case 0x001C: return L"WM_ACTIVATEAPP";
		case 0x001D: return L"WM_FONTCHANGE";
		case 0x001E: return L"WM_TIMECHANGE";
		case 0x001F: return L"WM_CANCELMODE";
		case 0x0020: return L"WM_SETCURSOR";
		case 0x0021: return L"WM_MOUSEACTIVATE";
		case 0x0022: return L"WM_CHILDACTIVATE";
		case 0x0023: return L"WM_QUEUESYNC";
		case 0x0024: return L"WM_GETMINMAXINFO";
		case 0x0026: return L"WM_PAINTICON";
		case 0x0027: return L"WM_ICONERASEBKGND";
		case 0x0028: return L"WM_NEXTDLGCTL";
		case 0x002A: return L"WM_SPOOLERSTATUS";
		case 0x002B: return L"WM_DRAWITEM";
		case 0x002C: return L"WM_MEASUREITEM";
		case 0x002D: return L"WM_DELETEITEM";
		case 0x002E: return L"WM_VKEYTOITEM";
		case 0x002F: return L"WM_CHARTOITEM";
		case 0x0030: return L"WM_SETFONT";
		case 0x0031: return L"WM_GETFONT";
		case 0x0032: return L"WM_SETHOTKEY";
		case 0x0033: return L"WM_GETHOTKEY";
		case 0x0037: return L"WM_QUERYDRAGICON";
		case 0x0039: return L"WM_COMPAREITEM";
		case 0x003D: return L"WM_GETOBJECT";
		case 0x0041: return L"WM_COMPACTING";
		case 0x0044: return L"WM_COMMNOTIFY";  /* no longer suported */
		case 0x0046: return L"WM_WINDOWPOSCHANGING";
		case 0x0047: return L"WM_WINDOWPOSCHANGED";
		case 0x0048: return L"WM_POWER";
		case 0x004A: return L"WM_COPYDATA";
		case 0x004B: return L"WM_CANCELJOURNAL";
		case 0x004E: return L"WM_NOTIFY";
		case 0x0050: return L"WM_INPUTLANGCHANGEREQUEST";
		case 0x0051: return L"WM_INPUTLANGCHANGE";
		case 0x0052: return L"WM_TCARD";
		case 0x0053: return L"WM_HELP";
		case 0x0054: return L"WM_USERCHANGED";
		case 0x0055: return L"WM_NOTIFYFORMAT";
		case 0x007B: return L"WM_CONTEXTMENU";
		case 0x007C: return L"WM_STYLECHANGING";
		case 0x007D: return L"WM_STYLECHANGED";
		case 0x007E: return L"WM_DISPLAYCHANGE";
		case 0x007F: return L"WM_GETICON";
		case 0x0080: return L"WM_SETICON";
		case 0x0081: return L"WM_NCCREATE";
		case 0x0082: return L"WM_NCDESTROY";
		case 0x0083: return L"WM_NCCALCSIZE";
		case 0x0084: return L"WM_NCHITTEST";
		case 0x0085: return L"WM_NCPAINT";
		case 0x0086: return L"WM_NCACTIVATE";
		case 0x0087: return L"WM_GETDLGCODE";
		case 0x0088: return L"WM_SYNCPAINT";
		case 0x00A0: return L"WM_NCMOUSEMOVE";
		case 0x00A1: return L"WM_NCLBUTTONDOWN";
		case 0x00A2: return L"WM_NCLBUTTONUP";
		case 0x00A3: return L"WM_NCLBUTTONDBLCLK";
		case 0x00A4: return L"WM_NCRBUTTONDOWN";
		case 0x00A5: return L"WM_NCRBUTTONUP";
		case 0x00A6: return L"WM_NCRBUTTONDBLCLK";
		case 0x00A7: return L"WM_NCMBUTTONDOWN";
		case 0x00A8: return L"WM_NCMBUTTONUP";
		case 0x00A9: return L"WM_NCMBUTTONDBLCLK";
		case 0x00AB: return L"WM_NCXBUTTONDOWN";
		case 0x00AC: return L"WM_NCXBUTTONUP";
		case 0x00AD: return L"WM_NCXBUTTONDBLCLK";
		case 0x00FE: return L"WM_INPUT_DEVICE_CHANGE";
		case 0x00FF: return L"WM_INPUT";
		case 0x0100: return L"WM_KEYDOWN";
		case 0x0101: return L"WM_KEYUP";
		case 0x0102: return L"WM_CHAR";
		case 0x0103: return L"WM_DEADCHAR";
		case 0x0104: return L"WM_SYSKEYDOWN";
		case 0x0105: return L"WM_SYSKEYUP";
		case 0x0106: return L"WM_SYSCHAR";
		case 0x0107: return L"WM_SYSDEADCHAR";
		case 0x0109: return L"WM_UNICHAR";
		case 0x010D: return L"WM_IME_STARTCOMPOSITION";
		case 0x010E: return L"WM_IME_ENDCOMPOSITION";
		case 0x010F: return L"WM_IME_COMPOSITION";
		case 0x0110: return L"WM_INITDIALOG";
		case 0x0111: return L"WM_COMMAND";
		case 0x0112: return L"WM_SYSCOMMAND";
		case 0x0113: return L"WM_TIMER";
		case 0x0114: return L"WM_HSCROLL";
		case 0x0115: return L"WM_VSCROLL";
		case 0x0116: return L"WM_INITMENU";
		case 0x0117: return L"WM_INITMENUPOPUP";
		case 0x0119: return L"WM_GESTURE";
		case 0x011A: return L"WM_GESTURENOTIFY";
		case 0x011F: return L"WM_MENUSELECT";
		case 0x0120: return L"WM_MENUCHAR";
		case 0x0121: return L"WM_ENTERIDLE";
		case 0x0122: return L"WM_MENURBUTTONUP";
		case 0x0123: return L"WM_MENUDRAG";
		case 0x0124: return L"WM_MENUGETOBJECT";
		case 0x0125: return L"WM_UNINITMENUPOPUP";
		case 0x0126: return L"WM_MENUCOMMAND";
		case 0x0127: return L"WM_CHANGEUISTATE";
		case 0x0128: return L"WM_UPDATEUISTATE";
		case 0x0129: return L"WM_QUERYUISTATE";
		case 0x0132: return L"WM_CTLCOLORMSGBOX";
		case 0x0133: return L"WM_CTLCOLOREDIT";
		case 0x0134: return L"WM_CTLCOLORLISTBOX";
		case 0x0135: return L"WM_CTLCOLORBTN";
		case 0x0136: return L"WM_CTLCOLORDLG";
		case 0x0137: return L"WM_CTLCOLORSCROLLBAR";
		case 0x0138: return L"WM_CTLCOLORSTATIC";
		case 0x01E1: return L"MN_GETHMENU";
		case 0x0200: return L"WM_MOUSEMOVE";
		case 0x0201: return L"WM_LBUTTONDOWN";
		case 0x0202: return L"WM_LBUTTONUP";
		case 0x0203: return L"WM_LBUTTONDBLCLK";
		case 0x0204: return L"WM_RBUTTONDOWN";
		case 0x0205: return L"WM_RBUTTONUP";
		case 0x0206: return L"WM_RBUTTONDBLCLK";
		case 0x0207: return L"WM_MBUTTONDOWN";
		case 0x0208: return L"WM_MBUTTONUP";
		case 0x0209: return L"WM_MBUTTONDBLCLK";
		case 0x020A: return L"WM_MOUSEWHEEL";
		case 0x020B: return L"WM_XBUTTONDOWN";
		case 0x020C: return L"WM_XBUTTONUP";
		case 0x020D: return L"WM_XBUTTONDBLCLK";
		case 0x020E: return L"WM_MOUSEHWHEEL";
		case 0x0210: return L"WM_PARENTNOTIFY";
		case 0x0211: return L"WM_ENTERMENULOOP";
		case 0x0212: return L"WM_EXITMENULOOP";
		case 0x0213: return L"WM_NEXTMENU";
		case 0x0214: return L"WM_SIZING";
		case 0x0215: return L"WM_CAPTURECHANGED";
		case 0x0216: return L"WM_MOVING";
		case 0x0218: return L"WM_POWERBROADCAST";
		case 0x0219: return L"WM_DEVICECHANGE";
		case 0x0220: return L"WM_MDICREATE";
		case 0x0221: return L"WM_MDIDESTROY";
		case 0x0222: return L"WM_MDIACTIVATE";
		case 0x0223: return L"WM_MDIRESTORE";
		case 0x0224: return L"WM_MDINEXT";
		case 0x0225: return L"WM_MDIMAXIMIZE";
		case 0x0226: return L"WM_MDITILE";
		case 0x0227: return L"WM_MDICASCADE";
		case 0x0228: return L"WM_MDIICONARRANGE";
		case 0x0229: return L"WM_MDIGETACTIVE";
		case 0x0230: return L"WM_MDISETMENU";
		case 0x0231: return L"WM_ENTERSIZEMOVE";
		case 0x0232: return L"WM_EXITSIZEMOVE";
		case 0x0233: return L"WM_DROPFILES";
		case 0x0234: return L"WM_MDIREFRESHMENU";
		case 0x0238: return L"WM_POINTERDEVICECHANGE";
		case 0x0239: return L"WM_POINTERDEVICEINRANGE";
		case 0x023A: return L"WM_POINTERDEVICEOUTOFRANGE";
		case 0x0240: return L"WM_TOUCH";
		case 0x0241: return L"WM_NCPOINTERUPDATE";
		case 0x0242: return L"WM_NCPOINTERDOWN";
		case 0x0243: return L"WM_NCPOINTERUP";
		case 0x0245: return L"WM_POINTERUPDATE";
		case 0x0246: return L"WM_POINTERDOWN";
		case 0x0247: return L"WM_POINTERUP";
		case 0x0249: return L"WM_POINTERENTER";
		case 0x024A: return L"WM_POINTERLEAVE";
		case 0x024B: return L"WM_POINTERACTIVATE";
		case 0x024C: return L"WM_POINTERCAPTURECHANGED";
		case 0x024D: return L"WM_TOUCHHITTESTING";
		case 0x024E: return L"WM_POINTERWHEEL";
		case 0x024F: return L"WM_POINTERHWHEEL";
		case 0x0250: return L"DM_POINTERHITTEST";
		case 0x0251: return L"WM_POINTERROUTEDTO";
		case 0x0252: return L"WM_POINTERROUTEDAWAY";
		case 0x0253: return L"WM_POINTERROUTEDRELEASED";
		case 0x0281: return L"WM_IME_SETCONTEXT";
		case 0x0282: return L"WM_IME_NOTIFY";
		case 0x0283: return L"WM_IME_CONTROL";
		case 0x0284: return L"WM_IME_COMPOSITIONFULL";
		case 0x0285: return L"WM_IME_SELECT";
		case 0x0286: return L"WM_IME_CHAR";
		case 0x0288: return L"WM_IME_REQUEST";
		case 0x0290: return L"WM_IME_KEYDOWN";
		case 0x0291: return L"WM_IME_KEYUP";
		case 0x02A1: return L"WM_MOUSEHOVER";
		case 0x02A3: return L"WM_MOUSELEAVE";
		case 0x02A0: return L"WM_NCMOUSEHOVER";
		case 0x02A2: return L"WM_NCMOUSELEAVE";
		case 0x02B1: return L"WM_WTSSESSION_CHANGE";
		case 0x02c0: return L"WM_TABLET_FIRST";
		case 0x02df: return L"WM_TABLET_LAST";
		case 0x02E0: return L"WM_DPICHANGED";
		case 0x02E2: return L"WM_DPICHANGED_BEFOREPARENT";
		case 0x02E3: return L"WM_DPICHANGED_AFTERPARENT";
		case 0x02E4: return L"WM_GETDPISCALEDSIZE";
		case 0x0300: return L"WM_CUT";
		case 0x0301: return L"WM_COPY";
		case 0x0302: return L"WM_PASTE";
		case 0x0303: return L"WM_CLEAR";
		case 0x0304: return L"WM_UNDO";
		case 0x0305: return L"WM_RENDERFORMAT";
		case 0x0306: return L"WM_RENDERALLFORMATS";
		case 0x0307: return L"WM_DESTROYCLIPBOARD";
		case 0x0308: return L"WM_DRAWCLIPBOARD";
		case 0x0309: return L"WM_PAINTCLIPBOARD";
		case 0x030A: return L"WM_VSCROLLCLIPBOARD";
		case 0x030B: return L"WM_SIZECLIPBOARD";
		case 0x030C: return L"WM_ASKCBFORMATNAME";
		case 0x030D: return L"WM_CHANGECBCHAIN";
		case 0x030E: return L"WM_HSCROLLCLIPBOARD";
		case 0x030F: return L"WM_QUERYNEWPALETTE";
		case 0x0310: return L"WM_PALETTEISCHANGING";
		case 0x0311: return L"WM_PALETTECHANGED";
		case 0x0312: return L"WM_HOTKEY";
		case 0x0317: return L"WM_PRINT";
		case 0x0318: return L"WM_PRINTCLIENT";
		case 0x0319: return L"WM_APPCOMMAND";
		case 0x031A: return L"WM_THEMECHANGED";
		case 0x031D: return L"WM_CLIPBOARDUPDATE";
		case 0x031E: return L"WM_DWMCOMPOSITIONCHANGED";
		case 0x031F: return L"WM_DWMNCRENDERINGCHANGED";
		case 0x0320: return L"WM_DWMCOLORIZATIONCOLORCHANGED";
		case 0x0321: return L"WM_DWMWINDOWMAXIMIZEDCHANGE";
		case 0x0323: return L"WM_DWMSENDICONICTHUMBNAIL";
		case 0x0326: return L"WM_DWMSENDICONICLIVEPREVIEWBITMAP";
		case 0x033F: return L"WM_GETTITLEBARINFOEX";

		case 0x00F0: return L"BM_GETCHECK";
		case 0x00F1: return L"BM_SETCHECK";
		case 0x00F2: return L"BM_GETSTATE";
		case 0x00F3: return L"BM_SETSTATE";
		case 0x00F4: return L"BM_SETSTYLE";
		case 0x00F5: return L"BM_CLICK";
		case 0x00F6: return L"BM_GETIMAGE";
		case 0x00F7: return L"BM_SETIMAGE";
		case 0x00F8: return L"BM_SETDONTCLICK";

		case 0x0140: return L"CB_GETEDITSEL";
		case 0x0141: return L"CB_LIMITTEXT";
		case 0x0142: return L"CB_SETEDITSEL";
		case 0x0143: return L"CB_ADDSTRING";
		case 0x0144: return L"CB_DELETESTRING";
		case 0x0145: return L"CB_DIR";
		case 0x0146: return L"CB_GETCOUNT";
		case 0x0147: return L"CB_GETCURSEL";
		case 0x0148: return L"CB_GETLBTEXT";
		case 0x0149: return L"CB_GETLBTEXTLEN";
		case 0x014A: return L"CB_INSERTSTRING";
		case 0x014B: return L"CB_RESETCONTENT";
		case 0x014C: return L"CB_FINDSTRING";
		case 0x014D: return L"CB_SELECTSTRING";
		case 0x014E: return L"CB_SETCURSEL";
		case 0x014F: return L"CB_SHOWDROPDOWN";
		case 0x0150: return L"CB_GETITEMDATA";
		case 0x0151: return L"CB_SETITEMDATA";
		case 0x0152: return L"CB_GETDROPPEDCONTROLRECT";
		case 0x0153: return L"CB_SETITEMHEIGHT";
		case 0x0154: return L"CB_GETITEMHEIGHT";
		case 0x0155: return L"CB_SETEXTENDEDUI";
		case 0x0156: return L"CB_GETEXTENDEDUI";
		case 0x0157: return L"CB_GETDROPPEDSTATE";
		case 0x0158: return L"CB_FINDSTRINGEXACT";
		case 0x0159: return L"CB_SETLOCALE";
		case 0x015A: return L"CB_GETLOCALE";
		case 0x015b: return L"CB_GETTOPINDEX";
		case 0x015c: return L"CB_SETTOPINDEX";
		case 0x015d: return L"CB_GETHORIZONTALEXTENT";
		case 0x015e: return L"CB_SETHORIZONTALEXTENT";
		case 0x015f: return L"CB_GETDROPPEDWIDTH";
		case 0x0160: return L"CB_SETDROPPEDWIDTH";
		case 0x0161: return L"CB_INITSTORAGE";
		case 0x0163: return L"CB_MULTIPLEADDSTRING";
		case 0x0164: return L"CB_GETCOMBOBOXINFO";
		case 0x0165: return L"CB_MSGMAX";

		case 0x0180: return L"LB_ADDSTRING";
		case 0x0181: return L"LB_INSERTSTRING";
		case 0x0182: return L"LB_DELETESTRING";
		case 0x0183: return L"LB_SELITEMRANGEEX";
		case 0x0184: return L"LB_RESETCONTENT";
		case 0x0185: return L"LB_SETSEL";
		case 0x0186: return L"LB_SETCURSEL";
		case 0x0187: return L"LB_GETSEL";
		case 0x0188: return L"LB_GETCURSEL";
		case 0x0189: return L"LB_GETTEXT";
		case 0x018A: return L"LB_GETTEXTLEN";
		case 0x018B: return L"LB_GETCOUNT";
		case 0x018C: return L"LB_SELECTSTRING";
		case 0x018D: return L"LB_DIR";
		case 0x018E: return L"LB_GETTOPINDEX";
		case 0x018F: return L"LB_FINDSTRING";
		case 0x0190: return L"LB_GETSELCOUNT";
		case 0x0191: return L"LB_GETSELITEMS";
		case 0x0192: return L"LB_SETTABSTOPS";
		case 0x0193: return L"LB_GETHORIZONTALEXTENT";
		case 0x0194: return L"LB_SETHORIZONTALEXTENT";
		case 0x0195: return L"LB_SETCOLUMNWIDTH";
		case 0x0196: return L"LB_ADDFILE";
		case 0x0197: return L"LB_SETTOPINDEX";
		case 0x0198: return L"LB_GETITEMRECT";
		case 0x0199: return L"LB_GETITEMDATA";
		case 0x019A: return L"LB_SETITEMDATA";
		case 0x019B: return L"LB_SELITEMRANGE";
		case 0x019C: return L"LB_SETANCHORINDEX";
		case 0x019D: return L"LB_GETANCHORINDEX";
		case 0x019E: return L"LB_SETCARETINDEX";
		case 0x019F: return L"LB_GETCARETINDEX";
		case 0x01A0: return L"LB_SETITEMHEIGHT";
		case 0x01A1: return L"LB_GETITEMHEIGHT";
		case 0x01A2: return L"LB_FINDSTRINGEXACT";
		case 0x01A5: return L"LB_SETLOCALE";
		case 0x01A6: return L"LB_GETLOCALE";
		case 0x01A7: return L"LB_SETCOUNT";
		case 0x01A8: return L"LB_INITSTORAGE";
		case 0x01A9: return L"LB_ITEMFROMPOINT";
		case 0x01B1: return L"LB_MULTIPLEADDSTRING";
		case 0x01B2: return L"LB_GETLISTBOXINFO";

		case (WM_USER+0): return L"TBM_GETPOS";
		case (WM_USER+1): return L"TBM_GETRANGEMIN";
		case (WM_USER+2): return L"TBM_GETRANGEMAX";
		case (WM_USER+3): return L"TBM_GETTIC";
		case (WM_USER+4): return L"TBM_SETTIC";
		case (WM_USER+5): return L"TBM_SETPOS";
		case (WM_USER+6): return L"TBM_SETRANGE";
		case (WM_USER+7): return L"TBM_SETRANGEMIN";
		case (WM_USER+8): return L"TBM_SETRANGEMAX";
		case (WM_USER+9): return L"TBM_CLEARTICS";
		case (WM_USER+10): return L"TBM_SETSEL";
		case (WM_USER+11): return L"TBM_SETSELSTART";
		case (WM_USER+12): return L"TBM_SETSELEND";
		case (WM_USER+14): return L"TBM_GETPTICS";
		case (WM_USER+15): return L"TBM_GETTICPOS";
		case (WM_USER+16): return L"TBM_GETNUMTICS";
		case (WM_USER+17): return L"TBM_GETSELSTART";
		case (WM_USER+18): return L"TBM_GETSELEND";
		case (WM_USER+19): return L"TBM_CLEARSEL";
		case (WM_USER+20): return L"TBM_SETTICFREQ";
		case (WM_USER+21): return L"TBM_SETPAGESIZE";
		case (WM_USER+22): return L"TBM_GETPAGESIZE";
		case (WM_USER+23): return L"TBM_SETLINESIZE";
		case (WM_USER+24): return L"TBM_GETLINESIZE";
		case (WM_USER+25): return L"TBM_GETTHUMBRECT";
		case (WM_USER+26): return L"TBM_GETCHANNELRECT";
		case (WM_USER+27): return L"TBM_SETTHUMBLENGTH";
		case (WM_USER+28): return L"TBM_GETTHUMBLENGTH";
		case (WM_USER+29): return L"TBM_SETTOOLTIPS";
		case (WM_USER+30): return L"TBM_GETTOOLTIPS";
		case (WM_USER+31): return L"TBM_SETTIPSIDE";
		}

		return my::format(L"{/hex}", message);
	}

	//
	// 文字列を追加します。
	// 内部的に使用されます。
	//
	inline void add(std::wstring& result, const std::wstring& s)
	{
		if (result.length()) result += L" | ";

		result += s;
	}

	//
	// ウィンドウハンドルを文字列化して返します。
	//
	inline std::wstring hwnd_to_string(HWND hwnd)
	{
#if 1
		return my::format(L"({/hex}, {/})", hwnd, my::get_class_name(hwnd));
#else
		return my::format(L"({/hex}, {/}, {/})", hwnd,
			my::get_window_text(hwnd), my::get_class_name(hwnd));
#endif
	}

	//
	// ウィンドウスタイルを文字列に変換して返します。
	//
	inline std::wstring style_to_string(DWORD style, const std::wstring& class_name)
	{
		auto result = std::wstring {};

		if (style & WS_OVERLAPPED) add(result, L"WS_OVERLAPPED");
		if (style & WS_POPUP) add(result, L"WS_POPUP");
		if (style & WS_CHILD) add(result, L"WS_CHILD");
		if (style & WS_MINIMIZE) add(result, L"WS_MINIMIZE");
		if (style & WS_VISIBLE) add(result, L"WS_VISIBLE");
		if (style & WS_DISABLED) add(result, L"WS_DISABLED");
		if (style & WS_CLIPSIBLINGS) add(result, L"WS_CLIPSIBLINGS");
		if (style & WS_CLIPCHILDREN) add(result, L"WS_CLIPCHILDREN");
		if (style & WS_MAXIMIZE) add(result, L"WS_MAXIMIZE");
		if (style & WS_VSCROLL) add(result, L"WS_VSCROLL");
		if (style & WS_HSCROLL) add(result, L"WS_HSCROLL");
		if (style & WS_SYSMENU) add(result, L"WS_SYSMENU");
		if (style & WS_THICKFRAME) add(result, L"WS_THICKFRAME");

		if ((style & WS_CAPTION) == WS_CAPTION)
		{
			if (style & WS_CAPTION) add(result, L"WS_CAPTION");     /* WS_BORDER | WS_DLGFRAME  */
			if (style & WS_MINIMIZEBOX) add(result, L"WS_MINIMIZEBOX");
			if (style & WS_MAXIMIZEBOX) add(result, L"WS_MAXIMIZEBOX");
		}
		else
		{
			if (style & WS_BORDER) add(result, L"WS_BORDER");
			if (style & WS_DLGFRAME) add(result, L"WS_DLGFRAME");
			if (style & WS_GROUP) add(result, L"WS_GROUP");
			if (style & WS_TABSTOP) add(result, L"WS_TABSTOP");
		}

		if (::lstrcmpiW(class_name.c_str(), WC_BUTTONW) == 0)
		{
			switch (style & BS_TYPEMASK)
			{
			case BS_PUSHBUTTON: add(result, L"BS_PUSHBUTTON"); break;
			case BS_DEFPUSHBUTTON: add(result, L"BS_DEFPUSHBUTTON"); break;
			case BS_CHECKBOX: add(result, L"BS_CHECKBOX"); break;
			case BS_AUTOCHECKBOX: add(result, L"BS_AUTOCHECKBOX"); break;
			case BS_RADIOBUTTON: add(result, L"BS_RADIOBUTTON"); break;
			case BS_3STATE: add(result, L"BS_3STATE"); break;
			case BS_AUTO3STATE: add(result, L"BS_AUTO3STATE"); break;
			case BS_GROUPBOX: add(result, L"BS_GROUPBOX"); break;
			case BS_USERBUTTON: add(result, L"BS_USERBUTTON"); break;
			case BS_AUTORADIOBUTTON: add(result, L"BS_AUTORADIOBUTTON"); break;
			case BS_PUSHBOX: add(result, L"BS_PUSHBOX"); break;
			case BS_OWNERDRAW: add(result, L"BS_OWNERDRAW"); break;
			}

			switch (style & BS_CENTER)
			{
			case BS_LEFT: add(result, L"BS_LEFT"); break;
			case BS_RIGHT: add(result, L"BS_RIGHT"); break;
			case BS_CENTER: add(result, L"BS_CENTER"); break;
			}

			switch (style & BS_VCENTER)
			{
			case BS_TOP: add(result, L"BS_TOP"); break;
			case BS_BOTTOM: add(result, L"BS_BOTTOM"); break;
			case BS_VCENTER: add(result, L"BS_VCENTER"); break;
			}

			if (style & BS_LEFTTEXT) add(result, L"BS_LEFTTEXT");
			if (style & BS_TEXT) add(result, L"BS_TEXT");
			if (style & BS_ICON) add(result, L"BS_ICON");
			if (style & BS_BITMAP) add(result, L"BS_BITMAP");
			if (style & BS_PUSHLIKE) add(result, L"BS_PUSHLIKE");
			if (style & BS_MULTILINE) add(result, L"BS_MULTILINE");
			if (style & BS_NOTIFY) add(result, L"BS_NOTIFY");
			if (style & BS_FLAT) add(result, L"BS_FLAT");
		}
		else if (::lstrcmpiW(class_name.c_str(), WC_COMBOBOXW) == 0)
		{
			switch (style & 0x03)
			{
			case CBS_SIMPLE: add(result, L"CBS_SIMPLE"); break;
			case CBS_DROPDOWN: add(result, L"CBS_DROPDOWN"); break;
			case CBS_DROPDOWNLIST: add(result, L"CBS_DROPDOWNLIST"); break;
			}

			if (style & CBS_OWNERDRAWFIXED) add(result, L"CBS_OWNERDRAWFIXED");
			if (style & CBS_OWNERDRAWVARIABLE) add(result, L"CBS_OWNERDRAWVARIABLE");
			if (style & CBS_AUTOHSCROLL) add(result, L"CBS_AUTOHSCROLL");
			if (style & CBS_OEMCONVERT) add(result, L"CBS_OEMCONVERT");
			if (style & CBS_SORT) add(result, L"CBS_SORT");
			if (style & CBS_HASSTRINGS) add(result, L"CBS_HASSTRINGS");
			if (style & CBS_NOINTEGRALHEIGHT) add(result, L"CBS_NOINTEGRALHEIGHT");
			if (style & CBS_DISABLENOSCROLL) add(result, L"CBS_DISABLENOSCROLL");
			if (style & CBS_UPPERCASE) add(result, L"CBS_UPPERCASE");
			if (style & CBS_LOWERCASE) add(result, L"CBS_LOWERCASE");
		}

		return result;
	}

	//
	// 拡張ウィンドウスタイルを文字列に変換して返します。
	//
	inline std::wstring ex_style_to_string(DWORD style)
	{
		auto result = std::wstring {};

		if (style & WS_EX_DLGMODALFRAME) add(result, L"WS_EX_DLGMODALFRAME");
		if (style & WS_EX_NOPARENTNOTIFY) add(result, L"WS_EX_NOPARENTNOTIFY");
		if (style & WS_EX_TOPMOST) add(result, L"WS_EX_TOPMOST");
		if (style & WS_EX_ACCEPTFILES) add(result, L"WS_EX_ACCEPTFILES");
		if (style & WS_EX_TRANSPARENT) add(result, L"WS_EX_TRANSPARENT");
		if (style & WS_EX_MDICHILD) add(result, L"WS_EX_MDICHILD");
		if (style & WS_EX_TOOLWINDOW) add(result, L"WS_EX_TOOLWINDOW");
		if (style & WS_EX_WINDOWEDGE) add(result, L"WS_EX_WINDOWEDGE");
		if (style & WS_EX_CLIENTEDGE) add(result, L"WS_EX_CLIENTEDGE");
		if (style & WS_EX_CONTEXTHELP) add(result, L"WS_EX_CONTEXTHELP");

		if (style & WS_EX_RIGHT) add(result, L"WS_EX_RIGHT");
		if (style & WS_EX_LEFT) add(result, L"WS_EX_LEFT");
		if (style & WS_EX_RTLREADING) add(result, L"WS_EX_RTLREADING");
		if (style & WS_EX_LTRREADING) add(result, L"WS_EX_LTRREADING");
		if (style & WS_EX_LEFTSCROLLBAR) add(result, L"WS_EX_LEFTSCROLLBAR");
		if (style & WS_EX_RIGHTSCROLLBAR) add(result, L"WS_EX_RIGHTSCROLLBAR");

		if (style & WS_EX_CONTROLPARENT) add(result, L"WS_EX_CONTROLPARENT");
		if (style & WS_EX_STATICEDGE) add(result, L"WS_EX_STATICEDGE");
		if (style & WS_EX_APPWINDOW) add(result, L"WS_EX_APPWINDOW");

		if (style & WS_EX_LAYERED) add(result, L"WS_EX_LAYERED");
		if (style & WS_EX_NOINHERITLAYOUT) add(result, L"WS_EX_NOINHERITLAYOUT"); // Disable inheritence of mirroring by children
		if (style & WS_EX_NOREDIRECTIONBITMAP) add(result, L"WS_EX_NOREDIRECTIONBITMAP");
		if (style & WS_EX_LAYOUTRTL) add(result, L"WS_EX_LAYOUTRTL"); // Right to left mirroring
		if (style & WS_EX_COMPOSITED) add(result, L"WS_EX_COMPOSITED");
		if (style & WS_EX_NOACTIVATE) add(result, L"WS_EX_NOACTIVATE");

		return result;
	}

	//
	// クラススタイルを文字列に変換して返します。
	//
	inline std::wstring class_style_to_string(DWORD style)
	{
		auto result = std::wstring {};

		if (style & CS_VREDRAW) add(result, L"CS_VREDRAW");
		if (style & CS_HREDRAW) add(result, L"CS_HREDRAW");
		if (style & CS_DBLCLKS) add(result, L"CS_DBLCLKS");
		if (style & CS_OWNDC) add(result, L"CS_OWNDC");
		if (style & CS_CLASSDC) add(result, L"CS_CLASSDC");
		if (style & CS_PARENTDC) add(result, L"CS_PARENTDC");
		if (style & CS_NOCLOSE) add(result, L"CS_NOCLOSE");
		if (style & CS_SAVEBITS) add(result, L"CS_SAVEBITS");
		if (style & CS_BYTEALIGNCLIENT) add(result, L"CS_BYTEALIGNCLIENT");
		if (style & CS_BYTEALIGNWINDOW) add(result, L"CS_BYTEALIGNWINDOW");
		if (style & CS_GLOBALCLASS) add(result, L"CS_GLOBALCLASS");

		if (style & CS_IME) add(result, L"CS_IME");
		if (style & CS_DROPSHADOW) add(result, L"CS_DROPSHADOW");

		return result;
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
