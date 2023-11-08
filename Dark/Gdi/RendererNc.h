#pragma once
#include "Skin.h"
#include "Gdi/Renderer.h"

namespace fgo::dark::gdi
{
	struct RendererNc : Renderer
	{
		LRESULT onSubclassProc(Reflector* reflector, State* currentState) override
		{
			switch (currentState->message)
			{
			case WM_NCPAINT:
				{
					__super::onSubclassProc(reflector, currentState);

					HWND hwnd = currentState->hwnd;
					HDC dc = ::GetWindowDC(hwnd);
					RECT rc; ::GetWindowRect(hwnd, &rc);
					POINT origin = { rc.left, rc.top };
					::OffsetRect(&rc, -origin.x, -origin.y);

					DWORD style = GetWindowStyle(hwnd);
					DWORD exStyle = GetWindowExStyle(hwnd);
					HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_WINDOW);

					if (exStyle & WS_EX_WINDOWEDGE)
					{
						int partId = skin::theme::WINDOW_WINDOWEDGE;
						int stateId = 0;
						skin::theme::manager.drawBackground(dc, theme, partId, stateId, &rc);
						::InflateRect(&rc, -2, -2);
					}
					else if (style & WS_BORDER)
					{
						int partId = skin::theme::WINDOW_BORDER;
						int stateId = 0;
						skin::theme::manager.drawBackground(dc, theme, partId, stateId, &rc);
						::InflateRect(&rc, -1, -1);
					}

					if (exStyle & WS_EX_STATICEDGE)
					{
						int partId = skin::theme::WINDOW_STATICEDGE;
						int stateId = 0;
						skin::theme::manager.drawBackground(dc, theme, partId, stateId, &rc);
						::InflateRect(&rc, -1, -1);
					}

					if (exStyle & WS_EX_CLIENTEDGE)
					{
						int partId = skin::theme::WINDOW_CLIENTEDGE;
						int stateId = 0;
						skin::theme::manager.drawBackground(dc, theme, partId, stateId, &rc);
						//my::frameRect(dc, &rc, RGB(255, 0, 0), 2);
						::InflateRect(&rc, -2, -2);
					}

					::ReleaseDC(hwnd, dc);

					return 0;
				}
			}

			return __super::onSubclassProc(reflector, currentState);
		}
	};
}
