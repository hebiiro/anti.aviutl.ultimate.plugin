#pragma once

namespace apn::ult_combobox
{
	//
	// 指定されたクライアント座標をスクリーン座標に変換して返します。
	//
	inline POINT client_to_screen(HWND hwnd, POINT pt)
	{
		return my::map_window_points(hwnd, nullptr, &pt), pt;
	}

	//
	// 指定されたスクリーン座標をクライアント座標に変換して返します。
	//
	inline POINT screen_to_client(HWND hwnd, POINT pt)
	{
		return my::map_window_points(nullptr, hwnd, &pt), pt;
	}

	//
	// 指定された親ウィンドウにWM_COMMANDを送信します。
	//
	inline BOOL fire_command(HWND parent, HWND control, UINT code)
	{
		auto control_id = ::GetDlgCtrlID(control);

		return ::SendMessage(parent, WM_COMMAND, MAKEWPARAM(control_id, code), (LPARAM)control);
	}

	//
	// 指定されたコントロールの親ウィンドウにWM_COMMANDを送信します。
	//
	inline BOOL fire_command(HWND control, UINT code)
	{
		return fire_command(::GetParent(control), control, code);
	}

	//
	// 指定された親ウィンドウにWM_CTLCOLORを送信します。
	//
	inline HBRUSH fire_ctl_color(HWND parent, HWND control, HDC dc, UINT message)
	{
		auto control_id = ::GetDlgCtrlID(control);

		return (HBRUSH)::SendMessage(parent, message, (WPARAM)dc, (LPARAM)control);
	}

	//
	// 指定されたコントロールの親ウィンドウにWM_CTLCOLORを送信します。
	//
	inline HBRUSH fire_ctl_color(HWND control, HDC dc, UINT message)
	{
		return fire_ctl_color(::GetParent(control), control, dc, message);
	}
}
