#pragma once
#include "Container.h"

namespace fgo::nest
{
	//
	// このクラスはターゲットウィンドウをクライアント領域全体まで広げるコンテナです。
	//
	struct SpreadContainer : Container
	{
		//
		// コンストラクタです。
		//
		SpreadContainer(Listener* listener, DWORD style)
			: Container(listener, style)
		{
		}

		//
		// デストラクタです。
		//
		~SpreadContainer()
		{
		}

		//
		// コンテナのウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_SIZE:
				{
					// ターゲットウィンドウをコンテナウィンドウのクライアント領域全体まで広げます。

					HWND target = listener->getTarget();

					RECT rc; ::GetClientRect(hwnd, &rc);
					clientToWindow(target, &rc);
					listener->onSetTargetWindowPos(&rc);
					int x = rc.left;
					int y = rc.top;
					int w = rc.right - rc.left;
					int h = rc.bottom - rc.top;

					hive.true_SetWindowPos(target, 0, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

					return 0;
				}
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}
	};
}
