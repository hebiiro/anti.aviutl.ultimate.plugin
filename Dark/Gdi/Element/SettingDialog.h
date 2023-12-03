#pragma once
#include "Skin.h"
#include "Gdi/Renderer.h"

namespace fgo::dark::gdi
{
	inline struct SettingDialogRenderer : Renderer
	{
		HWND getComboBox(HWND settingDialog)
		{
			for (UINT i = 8147; i >= 8100; i--)
			{
				// ウィンドウハンドルを取得する。
				HWND hwnd = ::GetDlgItem(settingDialog, i);

				// コンボボックスかどうかクラス名で調べる。
				TCHAR className[MAX_PATH] = {};
				::GetClassName(hwnd, className, std::size(className));
				if (::lstrcmpi(className, WC_COMBOBOX) != 0) continue;

				if (::IsWindowVisible(hwnd)) // ウィンドウが可視なら
				{
					// ID - 2 のウィンドウを返す。
					return ::GetDlgItem(settingDialog, i - 2);
				}
			}

			return 0;
		}

		LRESULT onSubclassProc(Reflector* reflector, State* currentState) override
		{
			HWND hwnd = currentState->hwnd;
			UINT message = currentState->message;
			WPARAM wParam = currentState->wParam;
			LPARAM lParam = currentState->lParam;

//			MY_TRACE_FUNC("0x%08X, 0x%08X, 0x%08X, 0x%08X", hwnd, message, wParam, lParam);

			switch (message)
			{
			case WM_ERASEBKGND:
				{
					MY_TRACE_FUNC("WM_ERASEBKGND, 0x%08X, 0x%08X", wParam, lParam);

					// 「設定ダイアログ画面サイズ固定化プラグイン」用の処理です。

					HDC dc = (HDC)wParam;
					RECT rc; ::GetClientRect(hwnd, &rc);

					HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_WINDOW);
					if (skin::theme::manager.onDrawThemeBackground(theme, dc, skin::theme::WINDOW_DIALOGFACE, 0, &rc))
						return TRUE;

					break;
				}
			case WM_COMMAND:
				{
					UINT code = HIWORD(wParam);
					UINT id = LOWORD(wParam);
					HWND sender = (HWND)lParam;

					MY_TRACE_FUNC("WM_COMMAND, 0x%04X, 0x%04X, 0x%08X", code, id, sender);

					// 「スクリプト並び替えプラグイン」用の処理です。

					if (id == 2079)
					{
						MY_TRACE(_T("アニメーション効果が追加されました\n"));

						LRESULT result = __super::onSubclassProc(reflector, currentState);
						HWND combobox = getComboBox(hwnd);
						MY_TRACE_HEX(combobox);
						::SendMessage(hwnd, WM_CTLCOLOREDIT, 0, (LPARAM)combobox);
						return result;
					}

					break;
				}
			}

			return __super::onSubclassProc(reflector, currentState);
		}

		int onFillRect(State* currentState, HDC dc, LPCRECT rc, HBRUSH brush) override
		{
			MY_TRACE_FUNC("0x%08X, (%ws), 0x%08X", dc, hive.safe_string(rc), brush);

			if (brush == (HBRUSH)(COLOR_BTNFACE + 1))
			{
				HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_WINDOW);
				if (skin::theme::manager.onDrawThemeBackground(theme, dc, skin::theme::WINDOW_DIALOGFACE, 0, rc))
					return TRUE;
			}

			return hive.orig.FillRect(dc, rc, brush);
		}

		BOOL onDrawFrame(State* currentState, HDC dc, LPRECT rc, UINT width, UINT type) override
		{
			MY_TRACE_FUNC("0x%08X, (%ws), %d, 0x%08X", dc, hive.safe_string(rc), width, type);

			return hive.orig.DrawFrame(dc, rc, width, type);
		}

		BOOL onDrawFrameControl(State* currentState, HDC dc, LPRECT rc, UINT type, UINT state) override
		{
			MY_TRACE_FUNC("0x%08X, (%ws), 0x%08X, 0x%08X", dc, hive.safe_string(rc), type, state);

			return hive.orig.DrawFrameControl(dc, rc, type, state);
		}

		BOOL onFrameRect(State* currentState, HDC dc, LPCRECT rc, HBRUSH brush) override
		{
			MY_TRACE_FUNC("0x%08X, (%ws), 0x%08X", dc, hive.safe_string(rc), brush);

			return hive.orig.FrameRect(dc, rc, brush);
		}

		BOOL onDrawEdge(State* currentState, HDC dc, LPRECT rc, UINT edge, UINT flags) override
		{
			MY_TRACE_FUNC("0x%08X, (%ws), 0x%08X, 0x%08X", dc, hive.safe_string(rc), edge, flags);

			return hive.orig.DrawEdge(dc, rc, edge, flags);
		}

		BOOL onDrawFocusRect(State* currentState, HDC dc, LPCRECT rc) override
		{
			MY_TRACE_FUNC("0x%08X, 0x%08X, (%ws)", dc, hive.safe_string(rc));

			return hive.orig.DrawFocusRect( dc, rc);
		}

		BOOL onDrawStateW(State* currentState, HDC dc, HBRUSH fore, DRAWSTATEPROC cb, LPARAM lData, WPARAM wData, int x, int y, int cx, int cy, UINT flags) override
		{
			MY_TRACE_FUNC("0x%08X, 0x%08X, %d, %d, %d, %d, 0x%08X", dc, fore, x, y, cx, cy, flags);

			return hive.orig.DrawStateW(dc, fore, cb, lData, wData, x, y, cx, cy, flags);
		}

		BOOL onExtTextOutW(State* currentState, HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx) override
		{
			MY_TRACE_FUNC("%d, %d, 0x%08X, 0x%08X, 0x%08X, %d, 0x%08X, 0x%08X, 0x%08X", x, y, options, rc, text, c, dx, ::GetBkColor(dc), ::GetTextColor(dc));

			// 「設定ダイアログ画面サイズ固定化プラグイン」用の処理です。
			// ::ExtTextOut()を使用した塗りつぶし処理をフックします。

			if (options == ETO_OPAQUE)
			{
				COLORREF color = ::GetBkColor(dc);
				if (color == ::GetSysColor(COLOR_BTNFACE))
				{
					HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_WINDOW);
					if (skin::theme::manager.onDrawThemeBackground(theme, dc, skin::theme::WINDOW_DIALOGFACE, 0, rc))
						return TRUE;
				}
			}

			return hive.orig.ExtTextOutW(dc, x, y, options, rc, text, c, dx);
		}

		BOOL onPatBlt(State* currentState, HDC dc, int x, int y, int w, int h, DWORD rop) override
		{
			MY_TRACE_FUNC("0x%08X, %d, %d, %d, %d, 0x%08X", dc, x, y, w, h, rop);

			return hive.orig.PatBlt(dc, x, y, w, h, rop);
		}
	} settingDialogRenderer;
}
