#pragma once

namespace fgo::nest::hook
{
	inline Shuttle* getShuttle(HWND hwnd)
	{
		// WS_CHILDスタイルがあるかチェックします。
		DWORD style = ::GetWindowLong(hwnd, GWL_STYLE);
		if (!(style & WS_CHILD)) return 0;

		return Shuttle::getPointer(hwnd);
	}

	//
	// このクラスは::MoveWindow()をフックします。
	//
	inline struct
	{
		inline static BOOL WINAPI hook(HWND hwnd, int x, int y, int w, int h, BOOL repaint)
		{
			Shuttle* shuttle = getShuttle(hwnd);

			if (!shuttle) // シャトルを取得できない場合はデフォルト処理を行います。
				return orig(hwnd, x, y, w, h, repaint);

			MY_TRACE(_T("::MoveWindow(0x%08X, %d, %d, %d, %d)\n"), hwnd, x, y, w, h);
			MY_TRACE_HWND(hwnd);
			MY_TRACE_WSTR((LPCWSTR)shuttle->name);

			// ターゲットのウィンドウ位置を更新します。
			BOOL result = orig(hwnd, x, y, w, h, repaint);

			// ターゲットの親ウィンドウを取得します。
			HWND parent = ::GetParent(hwnd);

			// 親ウィンドウがドッキングコンテナなら
			if (parent == *shuttle->dockContainer)
			{
				// ドッキングコンテナにターゲットの新しいウィンドウ位置を算出させます。
				shuttle->dockContainer->onWndProc(parent, WM_SIZE, 0, 0);
			}
			// 親ウィンドウがフローティングコンテナなら
			else if (parent == *shuttle->floatContainer)
			{
				WINDOWPOS wp = {};
				wp.x = x; wp.y = y; wp.cx = w; wp.cy = h;

				// フローティングコンテナに自身の新しいウィンドウ位置を算出させます。
				if (shuttle->floatContainer->onSetContainerPos(&wp))
				{
					// コンテナのウィンドウ位置を更新します。
					orig(parent, wp.x, wp.y, wp.cx, wp.cy, repaint);
				}
			}

			return result;
		}
		inline static decltype(&hook) orig = ::MoveWindow;
	} MoveWindow;

	//
	// このクラスは::SetWindowPos()をフックします。
	//
	inline struct
	{
		inline static BOOL WINAPI hook(HWND hwnd, HWND insertAfter, int x, int y, int w, int h, UINT flags)
		{
			Shuttle* shuttle = getShuttle(hwnd);

			if (!shuttle) // シャトルを取得できない場合はデフォルト処理を行います。
				return orig(hwnd, insertAfter, x, y, w, h, flags);

			MY_TRACE_FUNC("%s, 0x%08X, %d, %d, %d, %d", (LPCTSTR)shuttle->name, hwnd, x, y, w, h);

			// ターゲットのウィンドウ位置を更新します。
			BOOL result = orig(hwnd, insertAfter, x, y, w, h, flags);

			// ターゲットの親ウィンドウを取得します。
			HWND parent = ::GetParent(hwnd);

			// 親ウィンドウがドッキングコンテナなら
			if (parent == *shuttle->dockContainer)
			{
				// ドッキングコンテナにターゲットの新しいウィンドウ位置を算出させます。
				shuttle->dockContainer->onWndProc(parent, WM_SIZE, 0, 0);
			}
			// 親ウィンドウがフローティングコンテナなら
			else if (parent == *shuttle->floatContainer)
			{
				WINDOWPOS wp = {};
				wp.x = x; wp.y = y; wp.cx = w; wp.cy = h;

				// フローティングコンテナに自身の新しいウィンドウ位置を算出させます。
				if (shuttle->floatContainer->onSetContainerPos(&wp))
				{
					// フローティングコンテナのウィンドウ位置を更新します。
					orig(parent, 0, wp.x, wp.y, wp.cx, wp.cy, flags);
				}
			}

			return result;
		}
		inline static decltype(&hook) orig = ::SetWindowPos;
	} SetWindowPos;
}
