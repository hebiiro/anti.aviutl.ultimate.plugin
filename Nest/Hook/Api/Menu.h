#pragma once

namespace fgo::nest::hook
{
	//
	// ::GetMenu()、::SetMenu()、::DrawMenuBar() で
	// AviUtlウィンドウのハンドルが渡されたとき、メインウィンドウのハンドルに取り替えて偽装します。
	// これにより、AviUtlウィンドウのメニュー処理がメインウィンドウに対して行われるようになります。
	//
	inline HWND getMenuOwner(HWND hwnd)
	{
		if (hwnd == hive.aviutlWindow)
			return hive.mainWindow;

		Shuttle* shuttle = Shuttle::getPointer(hwnd);
		if (shuttle && shuttle->floatContainer)
			return *shuttle->floatContainer;

		return hwnd;
	}

	//
	// このクラスは::GetMenu()をフックします。
	//
	inline struct
	{
		inline static HMENU WINAPI hook(HWND hwnd)
		{
//			MY_TRACE_FUNC("0x%08X", hwnd);

			hwnd = getMenuOwner(hwnd);

			return orig(hwnd);
		}
		inline static decltype(&hook) orig = ::GetMenu;
	} GetMenu;

	//
	// このクラスは::SetMenu()をフックします。
	//
	inline struct
	{
		inline static BOOL WINAPI hook(HWND hwnd, HMENU menu)
		{
//			MY_TRACE_FUNC("0x%08X, 0x%08X", hwnd, menu);

			hwnd = getMenuOwner(hwnd);

			if (hwnd == hive.mainWindow)
			{
				if (::GetMenuState(menu, MainWindow::CommandID::MAXIMIZE_PLAY, MF_BYCOMMAND) == -1)
					::AppendMenu(menu, MF_STRING, MainWindow::CommandID::MAXIMIZE_PLAY, _T(""));
			}

			return orig(hwnd, menu);
		}
		inline static decltype(&hook) orig = ::SetMenu;
	} SetMenu;

	//
	// このクラスは::DrawMenuBar()をフックします。
	//
	inline struct
	{
		inline static BOOL WINAPI hook(HWND hwnd)
		{
//			MY_TRACE_FUNC("0x%08X", hwnd);

			hwnd = getMenuOwner(hwnd);

			return orig(hwnd);
		}
		inline static decltype(&hook) orig = ::DrawMenuBar;
	} DrawMenuBar;
}
