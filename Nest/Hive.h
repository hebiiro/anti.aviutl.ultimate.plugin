#pragma once

namespace fgo::nest
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static const LPCWSTR Name = L"Nest";
		inline static const LPCWSTR DisplayName = L"ネスト";
		inline static const LPCTSTR SubWindowClassName = _T("Nest.SubWindow");
		inline static const LPCTSTR SubProcessClassName = _T("Nest.SubProcess");

		struct WindowMessage
		{
			static const UINT WM_POST_INIT = WM_APP + 1;
			static const UINT WM_LOAD_CONFIG = WM_APP + 2;
			static const UINT WM_SAVE_CONFIG = WM_APP + 3;
			static const UINT WM_INIT_SHUTTLE = WM_APP + 4;
			static const UINT WM_REFRESH_TITLE = WM_APP + 5;
		};

		struct OrigWindow
		{
			DWORD exStyle = 0;
			TCHAR windowName[MAX_PATH] = {};
		};

		struct SubProcess {
			BOOL dock = FALSE;
			TCHAR run[MAX_PATH] = {};
			BOOL load(LPCWSTR path, LPCWSTR appName) {
				getPrivateProfileInt(path, appName, L"dock", dock);
				getPrivateProfileString(path, appName, L"run", run);
				if (_tcslen(run)) {
					TCHAR directory[MAX_PATH] = {};
					::StringCchCopy(directory, std::size(directory), run);
					::PathRemoveFileSpec(directory);
					::ShellExecute(hive.mainWindow, 0, run, 0, directory, SW_SHOW);
				}
				return TRUE;
			}
			BOOL save(LPCWSTR path, LPCWSTR appName) {
				setPrivateProfileBool(path, appName, L"dock", dock);
				setPrivateProfileString(path, appName, L"run", run);
				return TRUE;
			}
		};

		struct PSDToolKit : SubProcess {
			inline static const LPCTSTR Name = _T("PSDToolKit(外部)");
		} psdtoolkit;

		struct Bouyomisan : SubProcess {
			inline static const LPCTSTR Name = _T("棒読みさん(外部)");
		} bouyomisan;

		struct Console : SubProcess {
			inline static const LPCTSTR Name = _T("コンソール");
		} console;

		struct ShortcutKey {
			Tools::ShortcutKey showCaption = { L"Z", L"SHIFT WIN" };
		} shortcutKey;

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
		BOOL maximumPlay = FALSE;
		BOOL useMaximumPlay = FALSE;

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
			MY_TRACE_FUNC("0x%08X", parent);

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
	} hive;
}
