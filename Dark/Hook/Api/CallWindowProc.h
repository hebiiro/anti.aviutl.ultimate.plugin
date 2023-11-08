#pragma once

namespace fgo::dark::hook
{
	//
	// このクラスは::CallWindowProcInternal()をフックします。
	//
	inline struct
	{
		inline static LRESULT WINAPI hook(WNDPROC wndProc, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			return gdi::client.onWndProc(wndProc, hwnd, message, wParam, lParam);
		}
		inline static decltype(&hook) orig = 0;
	} CallWindowProcInternal;
}
