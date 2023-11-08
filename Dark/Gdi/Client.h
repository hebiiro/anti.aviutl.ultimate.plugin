#pragma once
#include "Gdi/Manager.h"
#include "Gdi/Element/Dialog.h"
#include "Gdi/Element/Static.h"
#include "Gdi/Element/Button.h"
#include "Gdi/Element/EditBox.h"
#include "Gdi/Element/ComboBox.h"
#include "Gdi/Element/ListBox.h"
#include "Gdi/Element/ToolTip.h"
#include "Gdi/Element/TrackBar.h"
#include "Gdi/Element/Spin.h"
#include "Gdi/Element/ListView.h"
#include "Gdi/Element/TreeView.h"
#include "Gdi/Element/ToolBar.h"
#include "Gdi/Element/Tab.h"
#include "Gdi/Element/AviUtl.h"
#include "Gdi/Element/AviUtlButton.h"
#include "Gdi/Element/SettingDialog.h"

namespace fgo::dark::gdi
{
	//
	// このクラスはGDIレンダラの利用者です。
	//
	inline struct Client
	{
		inline static constexpr LPCTSTR PropName = _T("fgo::dark::gdi::Client");

		inline static struct : Reflector
		{
			LRESULT onWndProc(State* currentState) override
			{
				return origProc(currentState);
			}
		} reflector;

		//
		// GDIレンダラをウィンドウにアタッチします。
		//
		inline static void attachWindow(HWND hwnd, LPCTSTR className)
		{
			MY_TRACE_FUNC("0x%08X, %s", hwnd, className);

			Renderer* renderer = 0;

			if (::lstrcmpi(className, _T("#32770")) == 0) renderer = &dialogRenderer;
			else if (::lstrcmpi(className, WC_STATIC) == 0) renderer = &staticRenderer;
			else if (::lstrcmpi(className, WC_BUTTON) == 0) renderer = &buttonRenderer;
			else if (::lstrcmpi(className, WC_EDIT) == 0) renderer = &editboxRenderer;
			else if (::lstrcmpi(className, WC_COMBOBOX) == 0) renderer = &comboboxRenderer;
			else if (::lstrcmpi(className, WC_LISTBOX) == 0) renderer = &listboxRenderer;
			else if (::lstrcmpi(className, _T("ComboLBox")) == 0) renderer = &listboxRenderer;
			else if (::lstrcmpi(className, TOOLTIPS_CLASS) == 0) renderer = &tooltipRenderer;
			else if (::lstrcmpi(className, TRACKBAR_CLASS) == 0) renderer = &trackbarRenderer;
			else if (::lstrcmpi(className, UPDOWN_CLASS) == 0) renderer = &spinRenderer;
			else if (::lstrcmpi(className, WC_LISTVIEW) == 0) renderer = &listviewRenderer;
			else if (::lstrcmpi(className, _T("DirectUIHWND")) == 0) renderer = &listviewRenderer;
			else if (::lstrcmpi(className, WC_TREEVIEW) == 0) renderer = &treeviewRenderer;
			else if (::lstrcmpi(className, TOOLBARCLASSNAME) == 0) renderer = &toolbarRenderer;
			else if (::lstrcmpi(className, WC_TABCONTROL) == 0) renderer = &tabRenderer;
			else if (::lstrcmpi(className, _T("AviUtl")) == 0) renderer = &aviutlRenderer;
			else if (::lstrcmpi(className, _T("AviUtlButton")) == 0) renderer = &aviutlButtonRenderer;
			else if (::lstrcmpi(className, _T("ExtendedFilterClass")) == 0) renderer = &settingDialogRenderer;
#if 0
			else if (::StrStrI(className, _T("WindowsForms10.") _T("Window"))) renderer = &dialogRenderer;
			else if (::StrStrI(className, _T("WindowsForms10.") WC_STATIC)) renderer = &staticRenderer;
			else if (::StrStrI(className, _T("WindowsForms10.") WC_BUTTON)) renderer = &buttonRenderer;
			else if (::StrStrI(className, _T("WindowsForms10.") WC_EDIT)) renderer = &editboxRenderer;
			else if (::StrStrI(className, _T("WindowsForms10.") WC_COMBOBOX)) renderer = &comboboxRenderer;
			else if (::StrStrI(className, _T("WindowsForms10.") WC_LISTBOX)) renderer = &listboxRenderer;
			else if (::StrStrI(className, _T("WindowsForms10.") TOOLTIPS_CLASS)) renderer = &tooltipRenderer;
			else if (::StrStrI(className, _T("WindowsForms10.") TRACKBAR_CLASS)) renderer = &trackbarRenderer;
			else if (::StrStrI(className, _T("WindowsForms10.") UPDOWN_CLASS)) renderer = &spinRenderer;
			else if (::StrStrI(className, _T("WindowsForms10.") WC_LISTVIEW)) renderer = &listviewRenderer;
			else if (::StrStrI(className, _T("WindowsForms10.") WC_TREEVIEW)) renderer = &treeviewRenderer;
			else if (::StrStrI(className, _T("WindowsForms10.") TOOLBARCLASSNAME)) renderer = &toolbarRenderer;
			else if (::StrStrI(className, _T("WindowsForms10.") WC_TABCONTROL)) renderer = &tabRenderer;
#endif
			if (renderer)
				Renderer::attach(hwnd, renderer);
		}

		//
		// ウィンドウを微調整します。
		//
		inline static void tweakWindow(HWND hwnd, LPCTSTR className)
		{
			MY_TRACE_FUNC("0x%08X, %s", hwnd, className);

			if (getStyle(hwnd) & WS_CAPTION)
			{
				if (!hive.lockRenderer)
					skin::dwm.set(hwnd, FALSE);
			}
			else if (::lstrcmpi(className, WC_STATIC) == 0)
			{
				// ラウドネスメーター(全体)のスタティックテキストを微調整します。

				HWND parent = ::GetParent(hwnd);
				TCHAR windowText[MAX_PATH] = {};
				::GetWindowText(parent, windowText, std::size(windowText));

				if (::lstrcmp(windowText, _T("ラウドネスメーター(全体)")) == 0)
					modifyStyle(hwnd, 0, SS_CENTERIMAGE);
			}
		}

		//
		// ウィンドウを初期化します。
		//
		inline static void initWindow(HWND hwnd)
		{
			MY_TRACE_FUNC("0x%08X", hwnd);

			// ウィンドウのクラス名を取得します。
			TCHAR className[MAX_PATH] = {};
			::GetClassName(hwnd, className, std::size(className));
			MY_TRACE_TSTR(className);

			// GDIレンダラをアタッチします。
			attachWindow(hwnd, className);

			// ウィンドウを微調整します。
			tweakWindow(hwnd, className);
		}

		//
		// オリジナルのウィンドウプロシージャを呼び出します。
		//
		inline static LRESULT CALLBACK origProc(State* currentState)
		{
			return hive.orig.CallWindowProcInternal(currentState->wndProc, currentState->hwnd, currentState->message, currentState->wParam, currentState->lParam);
		}

		//
		// ウィンドウプロシージャのハンドラです。
		//
		inline static LRESULT CALLBACK onWndProc(
			WNDPROC wndProc, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			State currentState = { wndProc, hwnd, message, wParam, lParam };

			if (message == WM_NCCREATE && !::GetProp(hwnd, PropName))
			{
				::SetProp(hwnd, PropName, &client);

				initWindow(hwnd);
			}

//			MY_TRACE_FUNC("0x%08X(0x%08X), 0x%08X, 0x%08X, 0x%08X, 0x%08X", wndProc, currentProc, hwnd, message, wParam, lParam);

			// レンダラの使用が抑制されている場合はデフォルト処理のみ行います。
			if (hive.lockRenderer)
				return origProc(&currentState);

			switch (message)
			{
			case WM_ACTIVATE:
				{
					int active = LOWORD(wParam);

					MY_TRACE_FUNC("WM_ACTIVATE, %d", active);

					skin::dwm.set(hwnd, active);

					break;
				}
			case WM_CTLCOLORDLG:
			case WM_CTLCOLORMSGBOX:
			case WM_CTLCOLORBTN:
			case WM_CTLCOLOREDIT:
			case WM_CTLCOLORLISTBOX:
			case WM_CTLCOLORSCROLLBAR:
				{
//					MY_TRACE_FUNC("WM_CTLCOLOR, 0x%08X, 0x%08X, 0x%08X, 0x%08X", hwnd, message, wParam, lParam);

					HDC dc = (HDC)wParam;
					HWND control = (HWND)lParam;
					BOOL enable = ::IsWindowEnabled(control);
					HBRUSH brush = (HBRUSH)origProc(&currentState);
//					HBRUSH dcBrush = (HBRUSH)::GetStockObject(DC_BRUSH);
					COLORREF bkColor = ::GetBkColor(dc);
//					COLORREF brushColor = my::getBrushColor(brush);
//					MY_TRACE(_T("brush = 0x%08X, dcBrush = 0x%08X, bkColor = 0x%08X, brushColor = 0x%08X\n"), brush, dcBrush, bkColor, brushColor);

					HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_CTLCOLOR);
					int partId = skin::theme::manager.getCtlColorPartId(message);
					int stateId = 0;

					if (!enable)
					{
						stateId = skin::theme::CTLCOLOR_DISABLED;
					}
					else if (message == WM_CTLCOLOREDIT)
					{
						if (brush == (HBRUSH)(COLOR_BTNFACE + 1) || bkColor == ::GetSysColor(COLOR_BTNFACE))
						{
							// スタティックコントロールに似たエディットボックスだった。
							stateId = skin::theme::CTLCOLOR_STATICTEXT;
							hive.setStateId(control, skin::theme::ETS_STATICTEXT);
//							MY_TRACE(_T("ETS_STATICTEXT 0x%08X\n"), control);
						}
						else
						{
							// 通常のエディットボックスだった。
							stateId = skin::theme::CTLCOLOR_NORMAL;
							hive.setStateId(control, ETS_NORMAL);
//							MY_TRACE(_T("ETS_NORMAL 0x%08X\n"), control);
						}
					}
					else if (brush != (HBRUSH)::GetStockObject(DC_BRUSH))
					{
						if (brush == (HBRUSH)(COLOR_BTNFACE + 1) || bkColor == ::GetSysColor(COLOR_BTNFACE))
						{
							stateId = skin::theme::CTLCOLOR_NORMAL;
						}
						else if (brush == (HBRUSH)(COLOR_WINDOW + 1) || bkColor == ::GetSysColor(COLOR_WINDOW))
						{
							stateId = skin::theme::CTLCOLOR_NORMAL;
						}
					}

					auto state = skin::theme::manager.getState(theme, partId, stateId);
					if (!state)
						return (LRESULT)brush;

					return (LRESULT)state->getFillBrush();
				}
			case WM_CTLCOLORSTATIC:
				{
//					MY_TRACE_FUNC("WM_CTLCOLORSTATIC, 0x%08X, 0x%08X, 0x%08X, 0x%08X", hwnd, message, wParam, lParam);

					HDC dc = (HDC)wParam;
					HWND control = (HWND)lParam;
					BOOL enable = ::IsWindowEnabled(control);
					HBRUSH brush = (HBRUSH)origProc(&currentState);
//					COLORREF bkColor = ::GetBkColor(dc);
//					COLORREF brushColor = my::getBrushColor(brush);
//					MY_TRACE(_T("brush = 0x%08X, bkColor = 0x%08X, brushColor = 0x%08X\n"), brush, bkColor, brushColor);

					HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_CTLCOLOR);
					int partId = skin::theme::manager.getCtlColorPartId(message);
					int stateId = 0;

					// 読み取り専用または無効状態のエディットボックスの場合は WM_CTLCOLORSTATIC が送られてくる。
					// しかし、エディットボックスのスキンは WM_CTLCOLOREDIT で処理したい。
					// したがって、コントロールがエディットボックスかどうか調べて、
					// エディットボックスだった場合は WM_CTLCOLOREDIT に変更する。

					TCHAR className[MAX_PATH] = {};
					::GetClassName(control, className, std::size(className));
					if (::lstrcmpi(className, WC_EDIT) == 0)
					{
						// エディットボックスだった。
						partId = skin::theme::manager.getCtlColorPartId(WM_CTLCOLOREDIT);

						if (enable)
						{
							// 読み取り専用のエディットボックスだった。
							stateId = skin::theme::CTLCOLOR_READONLY;
							hive.setStateId(control, ETS_READONLY);
//							MY_TRACE(_T("ETS_READONLY 0x%08X\n"), control);
						}
						else
						{
							// 無効状態のエディットボックスだった。
							stateId = skin::theme::CTLCOLOR_DISABLED;
							hive.setStateId(control, ETS_DISABLED);
//							MY_TRACE(_T("ETS_DISABLED 0x%08X\n"), control);
						}
					}
					else if (!enable)
					{
						// 無効状態のスタティックコントロールだった。
						stateId = skin::theme::CTLCOLOR_DISABLED;
					}
					else if (brush != (HBRUSH)::GetStockObject(DC_BRUSH))
					{
						if (::GetWindowLong(control, GWL_STYLE) & SS_SUNKEN && ::GetWindowTextLength(control) == 0)
						{
							// カラーダイアログのスタティックコントロールだった。
						}
						else
						{
							// 通常状態のスタティックコントロールだった。
							stateId = skin::theme::CTLCOLOR_NORMAL;
						}
					}

					auto state = skin::theme::manager.getState(theme, partId, stateId);
					if (!state)
						return (LRESULT)brush;

					return (LRESULT)state->getFillBrush();
				}
			case WM_NOTIFY:
				{
					NMHDR* nm = (NMHDR*)lParam;

					TCHAR className[MAX_PATH] = {};
					::GetClassName(nm->hwndFrom, className, std::size(className));
//					MY_TRACE_TSTR(className);

					if (::lstrcmpi(className, WC_BUTTON) == 0)
						return 0; // ボタンの場合は何もしない。

					switch (nm->code)
					{
					case NM_CUSTOMDRAW:
						{
							auto renderer = Renderer::fromHandle(nm->hwndFrom);
							if (renderer)
								return renderer->onCustomDraw(&reflector, &currentState);

							break;
						}
					}

					break;
				}
			case WM_NCRBUTTONUP:
				{
					MY_TRACE_FUNC("WM_NCRBUTTONUP, 0x%08X, 0x%08X, 0x%08X, 0x%08X", hwnd, message, wParam, lParam);

					if (wParam == HTMENU)
					{
						skin::manager.showMenu(hwnd);

						return 0;
					}

					break;
				}
			}

			LRESULT result = 0;
			auto oldState = manager.getCurrentState();
			manager.setCurrentState(currentState);
			auto renderer = Renderer::fromHandle(hwnd);
			if (renderer)
				result = renderer->onSubclassProc(&reflector, &currentState);
			else
				result = origProc(&currentState);
			manager.setCurrentState(oldState);
			return result;
		}
	} client;
}
