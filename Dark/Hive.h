#pragma once

namespace fgo::dark
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
			static const UINT WM_INIT_SHUTTLE = WM_APP + 4;
			static const UINT WM_REFRESH_TITLE = WM_APP + 5;
		};

		inline static const LPCWSTR Name = L"Dark";
		inline static const LPCWSTR DisplayName = L"ダークモード";

		//
		// このDLLのインスタンスハンドルです。
		//
		HINSTANCE instance = 0;

		//
		// AviUtlウィンドウです。
		//
		HWND aviutlWindow = 0;

		//
		// レンダラを使用するかどうかのフラグです。
		// レンダラの使用を停止したい場合はTRUEに設定します。
		//
		thread_local inline static BOOL lockRenderer = FALSE;

		//
		// ::ExtTextOutW()をフックするかどうかのフラグです。
		// フックをブロックしたい場合はTRUEに設定します。
		//
		thread_local inline static BOOL lockExtTextOut = FALSE;

		struct ExtTextOutLocker
		{
			ExtTextOutLocker()
			{
				hive.lockExtTextOut = TRUE;
			}

			~ExtTextOutLocker()
			{
				hive.lockExtTextOut = FALSE;
			}
		};

		//
		// このクラスはフックする前のオリジナルのAPIを保持します。
		//
		struct {
			int (WINAPI *FillRect)(HDC dc, LPCRECT rc, HBRUSH brush) = 0;
			BOOL (WINAPI *DrawFrame)(HDC dc, LPRECT rc, UINT width, UINT type) = 0;
			BOOL (WINAPI *DrawFrameControl)(HDC dc, LPRECT rc, UINT type, UINT state) = 0;
			BOOL (WINAPI *FrameRect)(HDC dc, LPCRECT rc, HBRUSH brush) = 0;
			BOOL (WINAPI *DrawEdge)(HDC dc, LPRECT rc, UINT edge, UINT flags) = 0;
			BOOL (WINAPI *DrawFocusRect)(HDC dc, LPCRECT rc) = 0;
			BOOL (WINAPI *DrawStateW)(HDC dc, HBRUSH fore, DRAWSTATEPROC cb, LPARAM lData, WPARAM wData, int x, int y, int cx, int cy, UINT flags) = 0;
			BOOL (WINAPI *ExtTextOutW)(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx) = 0;
			BOOL (WINAPI *PatBlt)(HDC dc, int x, int y, int w, int h, DWORD rop) = 0;

			HRESULT (WINAPI *DrawThemeParentBackground)(HWND hwnd, HDC dc, LPCRECT rc) = 0;
			HRESULT (WINAPI *DrawThemeBackground)(HTHEME theme, HDC dc, int partId, int stateId, LPCRECT rc, LPCRECT rcClip) = 0;
			HRESULT (WINAPI *DrawThemeBackgroundEx)(HTHEME theme, HDC dc, int partId, int stateId, LPCRECT rc, const DTBGOPTS* options) = 0;
			HRESULT (WINAPI *DrawThemeText)(HTHEME theme, HDC dc, int partId, int stateId, LPCWSTR text, int c, DWORD textFlags, DWORD textFlags2, LPCRECT rc) = 0;
			HRESULT (WINAPI *DrawThemeTextEx)(HTHEME theme, HDC dc, int partId, int stateId, LPCWSTR text, int c, DWORD textFlags, LPRECT rc, const DTTOPTS* options) = 0;
			HRESULT (WINAPI *DrawThemeIcon)(HTHEME theme, HDC dc, int partId, int stateId, LPCRECT rc, HIMAGELIST imageList, int iImageIndex) = 0;
			HRESULT (WINAPI *DrawThemeEdge)(HTHEME theme, HDC dc, int partId, int stateId, LPCRECT destRect, UINT edge, UINT flags, LPRECT contentRect) = 0;
		} orig;

		// プロセス内のすべてのウィンドウを再描画します。
		void redraw()
		{
			::EnumWindows(enumWindowsProc, 0);
		}

		inline static BOOL CALLBACK enumWindowsProc(HWND hwnd, LPARAM lParam)
		{
			DWORD pid = 0;
			DWORD tid = ::GetWindowThreadProcessId(hwnd, &pid);

			if (pid == ::GetCurrentProcessId())
			{
				MY_TRACE(_T("Skin::enumWindowsProc(0x%08X)\n"), hwnd);

				::RedrawWindow(hwnd, 0, 0,
					RDW_ERASE | RDW_FRAME | RDW_INTERNALPAINT |
					RDW_INVALIDATE | RDW_ALLCHILDREN);
				::EnumChildWindows(hwnd, enumChildWindowsProc, lParam);
			}

			return TRUE;
		}

		inline static BOOL CALLBACK enumChildWindowsProc(HWND hwnd, LPARAM lParam)
		{
			TCHAR className[MAX_PATH] = {};
			::GetClassName(hwnd, className, std::size(className));

			if (::lstrcmpi(className, TRACKBAR_CLASS) == 0)
			{
				// トラックバー用。
				::SendMessage(hwnd, WM_SETFOCUS, 0, 0);
			}
			else if (::lstrcmpi(className, WC_BUTTON) == 0)
			{
				// ボタン用。
				HICON icon = (HICON)::SendMessage(hwnd, BM_GETIMAGE, IMAGE_ICON, 0);
				::SendMessage(hwnd, BM_SETIMAGE, IMAGE_ICON, (LPARAM)icon);
			}
			else
			{
				::RedrawWindow(hwnd, 0, 0,
					RDW_ERASE | RDW_FRAME | RDW_INTERNALPAINT |
					RDW_INVALIDATE | RDW_ALLCHILDREN);
				::EnumChildWindows(hwnd, enumChildWindowsProc, lParam);
			}

			return TRUE;
		}

		// hwndからステートIDを取得します。スキン関係のクラスで実装したほうがいいかも。
		inline static int getStateId(HWND hwnd) {
			return (int)(uintptr_t)::GetProp(hwnd, _T("fgo::dark::gdi::State"));
		}
		inline static void setStateId(HWND hwnd, int stateId) {
			::SetProp(hwnd, _T("fgo::dark::gdi::State"), (HANDLE)stateId);
		}

		inline static std::string safe_string(LPCSTR name) {
			if ((ULONG_PTR)name > 0x0000FFFF)
				return name;
			else
				return Tools::FormatString<char>(MAX_PATH, "0x%08X", name);
		}

		inline static std::wstring safe_string(LPCWSTR name) {
			if ((ULONG_PTR)name > 0x0000FFFF)
				return name;
			else
				return Tools::FormatString<WCHAR>(MAX_PATH, L"0x%08X", name);
		}

		inline static std::wstring safe_string(LPCRECT rc) {
			if (rc)
				return Tools::FormatString<WCHAR>(MAX_PATH, _T("%d, %d, %d, %d"), rc->left, rc->top, rc->right, rc->bottom);
			else
				return _T("Null");
		}

		inline static std::wstring safe_string(LPCWSTR str, int c) {
			if (c < 0)
				return str;
			else
				return std::wstring(str, c);
		}
	} hive;
}
