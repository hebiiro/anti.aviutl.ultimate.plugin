#include "pch.h"
#include "Nest.h"

//---------------------------------------------------------------------

class ColonyShuttle : public Shuttle
{
public:

	virtual LRESULT onTargetWndProc(Container* container, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_NCDESTROY:
			{
				// ターゲットウィンドウが削除されたので、シャトルも削除する。

				MY_TRACE(_T("ColonyShuttle::onTargetWndProc(0x%08X, WM_NCDESTROY)\n"), hwnd);

				ShuttlePtr shuttle(shared_from_this());

				g_shuttleManager.removeShuttle(this);

				return Shuttle::onTargetWndProc(container, hwnd, message, wParam, lParam);
			}
		}

		return Shuttle::onTargetWndProc(container, hwnd, message, wParam, lParam);
	}
};

//---------------------------------------------------------------------

// 他のウィンドウの土台となるベースウィンドウを作成する。
HWND createColony(LPCTSTR name)
{
	MY_TRACE(_T("createColony(%s)\n"), name);

	WNDCLASS wc = {};
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.hCursor = ::LoadCursor(0, IDC_ARROW);
	wc.lpfnWndProc = colonyProc;
	wc.hInstance = g_instance;
	wc.lpszClassName = _T("Nest.Colony");
	::RegisterClass(&wc);

	if (!name)
	{
		for (int i = 0; TRUE; i++)
		{
			static TCHAR windowName[MAX_PATH] = {};
			::StringCbPrintf(windowName, sizeof(windowName), _T("Colony.%d"), i);
			MY_TRACE_TSTR(windowName);

			if (g_shuttleManager.getShuttle(windowName))
				continue;

			name = windowName;

			break;
		}
	}

	HWND hwnd = ::CreateWindowEx(
		0,
		_T("Nest.Colony"),
		name,
		WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_THICKFRAME |
		WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		g_hub, 0, g_instance, 0);

	{
		// コロニーをシャトルの中に入れる。
		ShuttlePtr shuttle = std::make_shared<ColonyShuttle>();
		g_shuttleManager.addShuttle(shuttle, name, hwnd);
	}

	return hwnd;
}

//---------------------------------------------------------------------

// ベースウィンドウのウィンドウ関数。
LRESULT CALLBACK colonyProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_NOTIFY:
		{
			NMHDR* header = (NMHDR*)lParam;

			switch (header->code)
			{
			case NM_RCLICK:
				{
					showPaneMenu(hwnd);

					break;
				}
			case TCN_SELCHANGE:
				{
					Pane* pane = TabControl::getPane(header->hwndFrom);
					if (pane)
						pane->m_tab.changeCurrent();

					break;
				}
			}

			break;
		}
	case WM_CREATE:
		{
			MY_TRACE(_T("colonyProc(WM_CREATE, 0x%08X, 0x%08X)\n"), wParam, lParam);

			g_colonyManager.insert(hwnd);

			break;
		}
	case WM_DESTROY:
		{
			MY_TRACE(_T("colonyProc(WM_DESTROY, 0x%08X, 0x%08X)\n"), wParam, lParam);

			break;
		}
	case WM_CLOSE:
		{
			MY_TRACE(_T("colonyProc(WM_CLOSE, 0x%08X, 0x%08X)\n"), wParam, lParam);

			if (::GetKeyState(VK_SHIFT) < 0)
			{
				// Shift キーが押されている場合はコロニーを削除する。

				if (IDYES != ::MessageBox(hwnd, _T("コロニーを削除しますか？"), _T("Nest"), MB_YESNO))
					return 0;

				// 何もしないとハブが他のウィンドウの後ろに隠れてしまうので、
				// 手動でフォアグラウンドにする。
				::SetActiveWindow(g_hub);
			}
			else
			{
				// コロニーの表示状態を切り替える。
				// ::ShowWindow(hwnd, SW_SHOW) ではなぜか WM_SHOWWINDOW が発生しないので、
				// 親ウィンドウの表示状態を直接切り替える。

				if (::IsWindowVisible(hwnd))
				{
					::ShowWindow(::GetParent(hwnd), SW_HIDE);
				}
				else
				{
					::ShowWindow(::GetParent(hwnd), SW_SHOW);
				}

				return 0;
			}

			g_colonyManager.erase(hwnd);
		
			break;
		}
	case WM_SIZE:
		{
			g_colonyManager.calcLayout(hwnd);

			break;
		}
	case WM_SHOWWINDOW:
		{
			MY_TRACE(_T("colonyProc(WM_SHOWWINDOW, 0x%08X, 0x%08X)\n"), wParam, lParam);

			if (wParam)
			{
				PanePtr root = g_colonyManager.getRootPane(hwnd);

				root->changeCurrent();
			}

			break;
		}
	case WM_PAINT:
		{
			PaintDC pdc(hwnd);
			RECT rc = pdc.m_ps.rcPaint;
			UxDC dc(pdc, &rc);

			if (!dc.isValid())
				return 0;

			PanePtr root = g_colonyManager.getRootPane(hwnd);

			{
				// 背景を塗りつぶす。

				fillBackground(dc, &rc);
			}

			{
				// ボーダーを描画する。

				HBRUSH brush = ::CreateSolidBrush(g_borderColor);
				root->drawBorder(dc, brush);
				::DeleteObject(brush);
			}

			{
				// ホットボーダーを描画する。

				// ホットボーダーの矩形を取得できたら
				RECT rcHotBorder;
				if (g_hotBorderPane &&
					g_hotBorderPane->m_owner == hwnd &&
					g_hotBorderPane->getBorderRect(&rcHotBorder))
				{
					// テーマを使用するなら
					if (g_useTheme)
					{
						int partId = WP_BORDER;
						int stateId = CS_ACTIVE;

						// テーマ API を使用してボーダーを描画する。
						::DrawThemeBackground(g_theme, dc, partId, stateId, &rcHotBorder, 0);
					}
					// テーマを使用しないなら
					else
					{
						// ブラシで塗りつぶす。
						HBRUSH brush = ::CreateSolidBrush(g_hotBorderColor);
						::FillRect(dc, &rcHotBorder, brush);
						::DeleteObject(brush);
					}
				}
			}

			{
				// 各ウィンドウのキャプションを描画する。

				LOGFONTW lf = {};
				::GetThemeSysFont(g_theme, TMT_CAPTIONFONT, &lf);
				HFONT font = ::CreateFontIndirectW(&lf);
				HFONT oldFont = (HFONT)::SelectObject(dc, font);

				root->drawCaption(dc);

				::SelectObject(dc, oldFont);
				::DeleteObject(font);
			}

			return 0;
		}
	case WM_SETCURSOR:
		{
			if (hwnd == (HWND)wParam)
			{
				PanePtr root = g_colonyManager.getRootPane(hwnd);

				POINT point; ::GetCursorPos(&point);
				::ScreenToClient(hwnd, &point);

				PanePtr borderPane = root->hitTestBorder(point);
				if (!borderPane) break;

				switch (borderPane->m_splitMode)
				{
				case SplitMode::vert:
					{
						::SetCursor(::LoadCursor(0, IDC_SIZEWE));

						return TRUE;
					}
				case SplitMode::horz:
					{
						::SetCursor(::LoadCursor(0, IDC_SIZENS));

						return TRUE;
					}
				}
			}

			break;
		}
	case WM_LBUTTONDOWN:
		{
			MY_TRACE(_T("colonyProc(WM_LBUTTONDOWN, 0x%08X, 0x%08X)\n"), wParam, lParam);

			// マウス座標を取得する。
			POINT point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

			if (showTargetMenu(hwnd, point))
				break;

			PanePtr root = g_colonyManager.getRootPane(hwnd);

			// マウス座標にあるボーダーを取得する。
			g_hotBorderPane = root->hitTestBorder(point);

			// ボーダーが有効かチェックする。
			if (g_hotBorderPane)
			{
				// オフセットを取得する。
				g_hotBorderPane->m_dragOffset = g_hotBorderPane->getDragOffset(point);

				// マウスキャプチャを開始する。
				::SetCapture(hwnd);

				// 再描画する。
				::InvalidateRect(hwnd, &g_hotBorderPane->m_position, FALSE);

				break;
			}

			{
				// マウス座標にあるペインを取得できたら
				PanePtr pane = root->hitTestPane(point);
				if (pane)
				{
					// クリックされたペインがウィンドウを持っているなら
					Shuttle* shuttle = pane->getActiveShuttle();
					if (shuttle)
						::SetFocus(shuttle->m_hwnd); // そのウィンドウにフォーカスを当てる。
				}
			}

			break;
		}
	case WM_LBUTTONUP:
		{
			MY_TRACE(_T("colonyProc(WM_LBUTTONUP, 0x%08X, 0x%08X)\n"), wParam, lParam);

			// マウス座標を取得する。
			POINT point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

			// マウスをキャプチャ中かチェックする。
			if (::GetCapture() == hwnd)
			{
				// マウスキャプチャを終了する。
				::ReleaseCapture();

				if (g_hotBorderPane)
				{
					// ボーダーを動かす。
					g_hotBorderPane->dragBorder(point);

					// レイアウトを再計算する。
					g_hotBorderPane->recalcLayout();

					// 再描画する。
					::InvalidateRect(hwnd, &g_hotBorderPane->m_position, FALSE);
				}
			}

			break;
		}
	case WM_RBUTTONDOWN:
		{
			MY_TRACE(_T("colonyProc(WM_RBUTTONDOWN, 0x%08X, 0x%08X)\n"), wParam, lParam);

			// レイアウト編集メニューを表示する。
			showPaneMenu(hwnd);

			break;
		}
	case WM_MOUSEMOVE:
		{
			// マウス座標を取得する。
			POINT point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

			// マウスをキャプチャ中かチェックする。
			if (::GetCapture() == hwnd)
			{
				if (g_hotBorderPane)
				{
					// ボーダーを動かす。
					g_hotBorderPane->dragBorder(point);

					// レイアウトを再計算する。
					g_hotBorderPane->recalcLayout();

					// 再描画する。
					::InvalidateRect(hwnd, &g_hotBorderPane->m_position, FALSE);
				}
			}
			else
			{
				PanePtr root = g_colonyManager.getRootPane(hwnd);

				// マウス座標にあるボーダーを取得する。
				PanePtr hotBorderPane = root->hitTestBorder(point);

				// 現在のホットボーダーと違う場合は
				if (g_hotBorderPane != hotBorderPane)
				{
					// ホットボーダーを更新する。
					g_hotBorderPane = hotBorderPane;

					// 再描画する。
					::InvalidateRect(hwnd, 0, FALSE);
				}

				// マウスリーブイベントをトラックする。
				TRACKMOUSEEVENT tme = { sizeof(tme) };
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = hwnd;
				::TrackMouseEvent(&tme);
			}

			break;
		}
	case WM_MOUSELEAVE:
		{
			MY_TRACE(_T("colonyProc(WM_MOUSELEAVE, 0x%08X, 0x%08X)\n"), wParam, lParam);

			// 再描画用のペイン。
			PanePtr pane = g_hotBorderPane;

			// ホットボーダーが存在すれば
			if (g_hotBorderPane)
			{
				// ホットボーダーを無効にする。
				g_hotBorderPane = 0;

				// 再描画する。
				::InvalidateRect(hwnd, &pane->m_position, FALSE);
			}

			break;
		}
	case WM_MOUSEWHEEL:
		{
			MY_TRACE(_T("colonyProc(WM_MOUSEWHEEL, 0x%08X, 0x%08X)\n"), wParam, lParam);

			int delta = (short)HIWORD(wParam);
			POINT point = LP2PT(lParam);
			::MapWindowPoints(0, hwnd, &point, 1);
			HWND child = ::ChildWindowFromPointEx(hwnd, point,
				CWP_SKIPDISABLED | CWP_SKIPINVISIBLE | CWP_SKIPTRANSPARENT);

			Pane* pane = TabControl::getPane(child);
			if (pane)
			{
				int c = pane->m_tab.getTabCount();
				int current = pane->m_tab.getCurrentIndex();

				if (delta > 0)
				{
					current--;
				}
				else
				{
					current++;
				}

				current = std::max<int>(current, 0);
				current = std::min<int>(current, c - 1);

				pane->m_tab.setCurrentIndex(current);
				pane->m_tab.changeCurrent();
			}

			break;
		}
	}

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

//---------------------------------------------------------------------
