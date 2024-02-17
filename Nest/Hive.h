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

		struct WindowMessage {
			inline static const UINT PostInit = WM_APP + 1;
			inline static const UINT LoadConfig = WM_APP + 2;
			inline static const UINT SaveConfig = WM_APP + 3;
			inline static const UINT InitShuttle = WM_APP + 4;
			inline static const UINT RefreshTitle = WM_APP + 5;
		};

		struct OrigWindow {
			DWORD exStyle = 0;
			TCHAR windowName[MAX_PATH] = {};
		};

		struct SubWindow {
			inline static const LPCTSTR ClassName = _T("Nest.SubWindow");
		};

		struct SubProcess {
			inline static const LPCTSTR ClassName = _T("Nest.SubProcess");
			BOOL dock = FALSE;
			TCHAR run[MAX_PATH] = {};
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
		BOOL showTabForce = FALSE;

		BOOL useMaximumPlay = FALSE;
	} hive;
}
