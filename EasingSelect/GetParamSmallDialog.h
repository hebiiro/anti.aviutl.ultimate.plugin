#pragma once
#include "Hive.h"

namespace fgo::easing_select
{
	//
	// このクラスはフックされたダイアログです。
	//
	struct GetParamSmallDialog : Tools::Window
	{
		//
		// ウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch (message)
			{
			case WM_ACTIVATE:
			case WM_DESTROY:
				{
					::SendMessage(hive.mainWindow, Share::EasingSelect::Message::FireNotify, 0, 0);

					break;
				}
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}
	};
}
