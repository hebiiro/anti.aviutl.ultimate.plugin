#pragma once
#include "Skin/Theme/VSClass.h"

namespace fgo::dark::skin::theme
{
	struct StateAttributes
	{
		COLORREF fillColor;
		COLORREF textForeColor;
		COLORREF textBackColor;
	};

	//
	// このクラスはテーマ毎の描画を管理します。
	//
	inline struct Manager
	{
		struct TempRect : RECT
		{
			TempRect(LPCRECT rc)
			{
				if (rc)
				{
					left = rc->left;
					top = rc->top;
					right = rc->right;
					bottom = rc->bottom;
				}
				else
				{
					left = top = right = bottom = 0;
				}
			}

			operator LPRECT() { return this; }
		};

		HTHEME themes[THEME_MAXSIZE] = {};
		std::map<HTHEME, std::shared_ptr<VSClass>> collection;

		void init()
		{
			MY_TRACE_FUNC("");

			themes[THEME_MENU] = ::OpenThemeData(hive.aviutlWindow, VSCLASS_MENU);
			themes[THEME_SCROLLBAR] = ::OpenThemeData(hive.aviutlWindow, VSCLASS_SCROLLBAR);
			themes[THEME_STATIC] = ::OpenThemeData(hive.aviutlWindow, VSCLASS_STATIC);
			themes[THEME_BUTTON] = ::OpenThemeData(hive.aviutlWindow, VSCLASS_BUTTON);
			themes[THEME_EDIT] = ::OpenThemeData(hive.aviutlWindow, VSCLASS_EDIT);
			themes[THEME_LISTBOX] = ::OpenThemeData(hive.aviutlWindow, VSCLASS_LISTBOX);
			themes[THEME_COMBOBOX] = ::OpenThemeData(hive.aviutlWindow, VSCLASS_COMBOBOX);
			themes[THEME_TOOLTIP] = ::OpenThemeData(hive.aviutlWindow, VSCLASS_TOOLTIP);
			themes[THEME_TRACKBAR] = ::OpenThemeData(hive.aviutlWindow, VSCLASS_TRACKBAR);
			themes[THEME_SPIN] = ::OpenThemeData(hive.aviutlWindow, VSCLASS_SPIN);
			themes[THEME_HEADER] = ::OpenThemeData(hive.aviutlWindow, VSCLASS_HEADER);
			themes[THEME_LISTVIEW] = ::OpenThemeData(hive.aviutlWindow, VSCLASS_LISTVIEW);
			themes[THEME_TREEVIEW] = ::OpenThemeData(hive.aviutlWindow, VSCLASS_TREEVIEW);
			themes[THEME_TOOLBAR] = ::OpenThemeData(hive.aviutlWindow, VSCLASS_TOOLBAR);
			themes[THEME_TAB] = ::OpenThemeData(hive.aviutlWindow, VSCLASS_TAB);
			themes[THEME_WINDOW] = ::OpenThemeData(hive.aviutlWindow, VSCLASS_WINDOW);
			themes[THEME_DIALOG] = ::OpenThemeData(hive.aviutlWindow, VSCLASS_WINDOW);
			themes[THEME_CTLCOLOR] = ::OpenThemeData(hive.aviutlWindow, VSCLASS_WINDOW);
			themes[THEME_EXEDIT] = ::OpenThemeData(hive.aviutlWindow, VSCLASS_WINDOW);
		}

		void exit()
		{
			MY_TRACE_FUNC("");

			for (auto theme : themes)
				::CloseThemeData(theme);
		}

		void clear()
		{
			MY_TRACE_FUNC("");

			for (auto pair : collection)
				::CloseThemeData(pair.first);
			collection.clear();
		}

		inline static int getCtlColorPartId(UINT message)
		{
			switch (message)
			{
			case WM_CTLCOLORDLG: return CTLCOLORDLG;
			case WM_CTLCOLORMSGBOX: return CTLCOLORMSGBOX;
			case WM_CTLCOLORBTN: return CTLCOLORBTN;
			case WM_CTLCOLORSTATIC: return CTLCOLORSTATIC;
			case WM_CTLCOLOREDIT: return CTLCOLOREDIT;
			case WM_CTLCOLORLISTBOX: return CTLCOLORLISTBOX;
			case WM_CTLCOLORSCROLLBAR: return CTLCOLORSCROLLBAR;
			}

			return 0;
		}

		HTHEME getTheme(THEMES theme) const
		{
			return themes[theme];
		}

		std::shared_ptr<VSClass> getVSClass(HTHEME theme) const
		{
			auto it = collection.find(theme);
			if (it == collection.end()) return 0;
			return it->second;
		}

		std::shared_ptr<VSClass> getVSClass(HWND hwnd, LPCWSTR vsclass) const
		{
			HTHEME theme = ::OpenThemeData(hwnd, vsclass);
			::CloseThemeData(theme);
			return getVSClass(theme);
		}

		std::shared_ptr<State> getState(HTHEME theme, int partId, int stateId) const
		{
			auto vsclass = getVSClass(theme);
			if (!vsclass) return 0;

			auto part = vsclass->getPart(partId);
			if (!part) return 0;

			return part->getState(stateId);
		}

		void getStateAttributes(HTHEME theme, int partId, int stateId, StateAttributes* stateAttributes) const
		{
			auto state = getState(theme, partId, stateId);
			if (state)
			{
				stateAttributes->fillColor = state->fillColor;
				stateAttributes->textForeColor = state->textForeColor;
				stateAttributes->textBackColor = state->textBackColor;
			}
		}

		BOOL drawBackground(HDC dc, HTHEME theme, int partId, int stateId, LPRECT rc)
		{
			auto state = getState(theme, partId, stateId);
			if (!state) return FALSE;

			BOOL retValue = FALSE;
			for (auto node : state->nodes)
				node->draw(dc, TempRect(rc)), retValue = TRUE;
			return retValue;
		}

		BOOL drawIcon(HDC dc, HTHEME theme, int partId, int stateId, LPRECT rc)
		{
			auto state = getState(theme, partId, stateId);
			if (!state) return FALSE;

			BOOL retValue = FALSE;
			for (auto iconNode : state->iconNodes)
				iconNode->draw(dc, TempRect(rc)), retValue = TRUE;
			return retValue;
		}

		BOOL drawText(HDC dc, HTHEME theme, int partId, int stateId, LPCRECT rc, LPCWSTR text, int c, UINT format)
		{
			auto state = getState(theme, partId, stateId);
			if (!state) return FALSE;

			BOOL retValue = FALSE;
			for (auto textNode : state->textNodes)
				textNode->drawText(dc, TempRect(rc), text, c, format), retValue = TRUE;
			return retValue;
		}

		BOOL textOut(HDC dc, HTHEME theme, int partId, int stateId, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx)
		{
			auto state = getState(theme, partId, stateId);
			if (!state) return FALSE;

			BOOL retValue = FALSE;
			for (auto textNode : state->textNodes)
				textNode->textOut(dc, x, y, options, TempRect(rc), text, c, dx), retValue = TRUE;
			return retValue;
		}

		BOOL onDrawThemeBackground(HTHEME theme, HDC dc, int partId, int stateId, LPCRECT rc)
		{
			Hive::ExtTextOutLocker locker; // このスコープ内での::ExtTextOutW()のフックをブロックします。

			RECT rc2 = *rc;
			BOOL result = FALSE;
			if (drawBackground(dc, theme, partId, stateId, &rc2)) result = TRUE;
			if (drawIcon(dc, theme, partId, stateId, &rc2)) result = TRUE;
			return result;
		}

		BOOL onDrawThemeText(HTHEME theme, HDC dc, int partId, int stateId, LPCWSTR text, int c, DWORD textFlags, LPCRECT rc)
		{
			Hive::ExtTextOutLocker locker; // このスコープ内での::ExtTextOutW()のフックをブロックします。

			return drawText(dc, theme, partId, stateId, rc, text, c, textFlags);
		}

		BOOL onExtTextOut(HTHEME theme, HDC dc, int partId, int stateId, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx)
		{
			return textOut(dc, theme, partId, stateId, x, y, options, rc, text, c, dx);
		}

		//
		// <VSClasses>を読み込みます。
		//
		HRESULT loadVSClasses(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE_FUNC("");

			MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"VSClasses");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

				loadVSClass(element);
			}

			return S_OK;
		}

		//
		// <VSClass>を読み込みます。
		//
		HRESULT loadVSClass(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE_FUNC("");

			MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"VSClass");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

				auto vsclass = createVSClass(element);

				vsclass->load(element);
			}

			return S_OK;
		}

		//
		// VSClassを作成して返します。
		// ただし、すでに存在する場合はそのVSClassを返します。
		//
		std::shared_ptr<VSClass> createVSClass(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE_FUNC("");

			_bstr_t id = L"";
			getPrivateProfileBSTR(element, L"id", id);
			auto vsclass = std::make_shared<VSClass>(id);

			// <Name>を読み込みます。
			MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"Name");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

				HTHEME theme = getTheme(element);
//				MY_TRACE_HEX(theme);

				if (collection[theme])
				{
					// themeはすでに存在するので閉じます。
					::CloseThemeData(theme);

					// 既存のVSClassを取得します。
					vsclass = collection[theme];
				}
				else
				{
					// themeとvsclassをコレクションに追加します。
					collection[theme] = vsclass;
				}
			}

			return vsclass;
		}

		//
		// elementに対応したHTHEMEを返します。
		//
		inline static HTHEME getTheme(const MSXML2::IXMLDOMElementPtr& element)
		{
			BOOL dpi = FALSE;
			getPrivateProfileBool(element, L"dpi", dpi);
			_bstr_t name = element->text;

			if (dpi)
			{
				UINT dpi = ::GetDpiForWindow(hive.aviutlWindow);
				return ::OpenThemeDataForDpi(hive.aviutlWindow, name, dpi);
			}
			else
			{
				return ::OpenThemeData(hive.aviutlWindow, name);
			}
		}
	} manager;
}
