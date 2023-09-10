#pragma once

namespace fgo::nest
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		struct Label { LPCWSTR label; int value; };

		struct WindowMessage
		{
			static const UINT WM_POST_INIT = WM_APP + 1;
			static const UINT WM_LOAD_CONFIG = WM_APP + 2;
			static const UINT WM_SAVE_CONFIG = WM_APP + 3;
		};

		inline static const LPCTSTR AppName = _T("ネスト");
		inline static const LPCTSTR PSDToolKitName = _T("PSDToolKit(外部)");
		inline static const LPCTSTR BouyomisanName = _T("棒読みさん(外部)");

		HINSTANCE instance = 0; // このDLLのインスタンスハンドルです。
		HTHEME theme = 0;
		HWND mainWindow = 0;
		HWND aviutlWindow = 0;
		HWND exeditWindow = 0;
		HWND settingDialog = 0;
		HWND scriptParamDialog = 0;

		int menuBreak = 30;
		COLORREF fillColor = RGB(0x99, 0x99, 0x99);
		COLORREF borderColor = RGB(0xcc, 0xcc, 0xcc);
		COLORREF hotBorderColor = RGB(0x00, 0x00, 0x00);
		COLORREF activeCaptionColor = ::GetSysColor(COLOR_HIGHLIGHT);
		COLORREF activeCaptionTextColor = RGB(0xff, 0xff, 0xff);
		COLORREF inactiveCaptionColor = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
		COLORREF inactiveCaptionTextColor = RGB(0x00, 0x00, 0x00);
		BOOL useTheme = FALSE;
		BOOL forceScroll = FALSE;
		BOOL showPlayer = FALSE;
		BOOL dockPSDToolKit = FALSE;
		BOOL dockBouyomisan = FALSE;

		//
		// 矩形を塗りつぶします。
		//
		void fillBackground(HDC dc, LPCRECT rc)
		{
			fillBackground(dc, rc, fillColor, WP_DIALOG, 0);
		}

		//
		// 矩形を塗りつぶします。
		//
		void fillBackground(HDC dc, LPCRECT rc, COLORREF color, int partId, int stateId)
		{
			// テーマを使用するなら
			if (useTheme)
			{
				// テーマ API を使用してボーダーを描画する。
				::DrawThemeBackground(theme, dc, partId, stateId, rc, 0);
			}
			// テーマを使用しないなら
			else
			{
				// ブラシで塗りつぶす。
				HBRUSH brush = ::CreateSolidBrush(color);
				::FillRect(dc, rc, brush);
				::DeleteObject(brush);
			}
		}

		//
		// 他のポップアップウィンドウの土台となるポップアップウィンドウを作成します。
		//
		HWND createPopupWindow(HWND parent)
		{
			MY_TRACE(_T("createPopupWindow(0x%08X)\n"), parent);

			WNDCLASS wc = {};
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
			wc.hCursor = ::LoadCursor(0, IDC_ARROW);
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = instance;
			wc.lpszClassName = _T("PopupWindow");
			::RegisterClass(&wc);

			HWND hwnd = ::CreateWindowEx(
				0,
				_T("PopupWindow"),
				_T("PopupWindow"),
				WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				parent, 0, instance, 0);

			return hwnd;
		}

		//
		// フックする前のオリジナルの ::CreateWindowExA() です。
		//
		inline static decltype(&::CreateWindowExA) true_CreateWindowExA = 0;

		//
		// フックする前のオリジナルの ::SetWindowPos() です。
		//
		inline static decltype(&::SetWindowPos) true_SetWindowPos = 0;

		//
		// 強制的に WM_SIZE を送信する ::SetWindowPos() です。
		//
		inline static void forceSetWindowPos(HWND hwnd, HWND insertAfter, int x, int y, int w, int h, UINT flags)
		{
			RECT rcOld; ::GetWindowRect(hwnd, &rcOld);
			true_SetWindowPos(hwnd, insertAfter, x, y, w, h, flags);

			if (!(flags & SWP_NOSIZE))
			{
				RECT rcNew; ::GetWindowRect(hwnd, &rcNew);

				if (getWidth(rcOld) == getWidth(rcNew) && getHeight(rcOld) == getHeight(rcNew))
				{
					// ウィンドウサイズは変更されなかったが、強制的に WM_SIZE を送信する。
					::SendMessage(hwnd, WM_SIZE, 0, 0);
				}
			}
		}
	} hive;
}
