#pragma once

namespace apn::workspace::hook
{
	inline struct Menu
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			my::hook::attach(GetMenu);
			my::hook::attach(SetMenu);
			my::hook::attach(DrawMenuBar);

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// ::GetMenu()、::SetMenu()、::DrawMenuBar()で
		// AviUtlウィンドウのハンドルが渡されたとき、メインウィンドウのハンドルに取り替えて偽装します。
		// これにより、AviUtlウィンドウのメニュー処理がメインウィンドウに対して行われるようになります。
		//
		inline static HWND get_menu_owner(HWND hwnd)
		{
			if (hwnd == hive.aviutl_window)
				return hive.main_window;

			auto shuttle = Shuttle::get_pointer(hwnd);
			if (shuttle && shuttle->float_container)
				return *shuttle->float_container;

			return hwnd;
		}

		//
		// このクラスは::GetMenu()をフックします。
		//
		struct {
			inline static HMENU WINAPI hook_proc(HWND hwnd)
			{
//				MY_TRACE_FUNC("{:#010x}", hwnd);

				hwnd = get_menu_owner(hwnd);

				return orig_proc(hwnd);
			}
			inline static decltype(&hook_proc) orig_proc = ::GetMenu;
		} GetMenu;

		//
		// このクラスは::SetMenu()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HWND hwnd, HMENU menu)
			{
//				MY_TRACE_FUNC("{:#010x}, {:#010x}", hwnd, menu);

				hwnd = get_menu_owner(hwnd);

				return orig_proc(hwnd, menu);
			}
			inline static decltype(&hook_proc) orig_proc = ::SetMenu;
		} SetMenu;

		//
		// このクラスは::DrawMenuBar()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HWND hwnd)
			{
//				MY_TRACE_FUNC("{:#010x}", hwnd);

				hwnd = get_menu_owner(hwnd);

				return orig_proc(hwnd);
			}
			inline static decltype(&hook_proc) orig_proc = ::DrawMenuBar;
		} DrawMenuBar;
	} menu;
}
