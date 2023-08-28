#include "pch.h"
#include "Nest.h"

//---------------------------------------------------------------------

std::wstring getName(LPCWSTR x)
{
	std::wstring name(x);
	std::wstring t = L"* ";  // 検索文字列
	auto pos = name.find(t);  // 検索文字列が見つかった位置
	auto len = t.length(); // 検索文字列の長さ
	if (pos != std::string::npos)
		name.replace(pos, len, L"");

	return name;
}

HRESULT saveConfig()
{
	MY_TRACE(_T("saveConfig()\n"));

	WCHAR fileName[MAX_PATH] = {};
	::GetModuleFileNameW(g_instance, fileName, MAX_PATH);
	::PathRenameExtensionW(fileName, L".xml");

	return saveConfig(fileName, FALSE);
}

HRESULT saveConfig(LPCWSTR fileName, BOOL _export)
{
	MY_TRACE(_T("saveConfig(%ws, %d)\n"), fileName, _export);

	try
	{
		// ドキュメントを作成する。
		MSXML2::IXMLDOMDocumentPtr document(__uuidof(MSXML2::DOMDocument));

		// ドキュメントエレメントを作成する。
		MSXML2::IXMLDOMElementPtr element = appendElement(document, document, L"config");

		if (!_export) // エクスポートのときはこれらの変数は保存しない。
		{
			setPrivateProfileInt(element, L"borderWidth", g_borderWidth);
			setPrivateProfileInt(element, L"captionHeight", g_captionHeight);
			setPrivateProfileInt(element, L"tabHeight", g_tabHeight);
			setPrivateProfileInt(element, L"menuBreak", g_menuBreak);
			setPrivateProfileLabel(element, L"tabMode", g_tabMode, g_tabModeLabel);
			setPrivateProfileColor(element, L"fillColor", g_fillColor);
			setPrivateProfileColor(element, L"borderColor", g_borderColor);
			setPrivateProfileColor(element, L"hotBorderColor", g_hotBorderColor);
			setPrivateProfileColor(element, L"activeCaptionColor", g_activeCaptionColor);
			setPrivateProfileColor(element, L"activeCaptionTextColor", g_activeCaptionTextColor);
			setPrivateProfileColor(element, L"inactiveCaptionColor", g_inactiveCaptionColor);
			setPrivateProfileColor(element, L"inactiveCaptionTextColor", g_inactiveCaptionTextColor);
			setPrivateProfileBool(element, L"useTheme", g_useTheme);
			setPrivateProfileBool(element, L"forceScroll", g_forceScroll);
			setPrivateProfileBool(element, L"showPlayer", g_showPlayer);
		}

		// <hub> を作成する。
		saveHub(element);

		// <colony> を作成する。
		saveColony(element);

		// <floatShuttle> を作成する。
		saveFloatShuttle(element);

		return saveXMLDocument(document, fileName, L"UTF-16");
	}
	catch (_com_error& e)
	{
		MY_TRACE(_T("%s\n"), e.ErrorMessage());
		return e.Error();
	}
}

// <hub> を作成する。
HRESULT saveHub(const MSXML2::IXMLDOMElementPtr& element)
{
	MY_TRACE(_T("saveHub()\n"));

	{
		HWND hwndHub = g_hub;

		// <hub> を作成する。
		MSXML2::IXMLDOMElementPtr hubElement = appendElement(element, L"hub");

		setPrivateProfileWindow(hubElement, L"placement", hwndHub);

		PanePtr root = g_colonyManager.getRootPane(hwndHub);

		// <pane> を作成する。
		savePane(hubElement, root);
	}

	return S_OK;
}

// <colony> を作成する。
HRESULT saveColony(const MSXML2::IXMLDOMElementPtr& element)
{
	MY_TRACE(_T("saveColony()\n"));

	for (auto colony : g_colonyManager.m_array)
	{
		// <colony> を作成する。
		MSXML2::IXMLDOMElementPtr colonyElement = appendElement(element, L"colony");

		TCHAR name[MAX_PATH] = {};
		::GetWindowText(colony, name, MAX_PATH);
		setPrivateProfileString(colonyElement, L"name", name);

		PanePtr root = g_colonyManager.getRootPane(colony);

		// <pane> を作成する。
		savePane(colonyElement, root);
	}

	return S_OK;
}

// <pane> を作成する。
HRESULT savePane(const MSXML2::IXMLDOMElementPtr& element, PanePtr pane)
{
	MY_TRACE(_T("savePane()\n"));

	// <pane> を作成する。
	MSXML2::IXMLDOMElementPtr paneElement = appendElement(element, L"pane");

	int current = pane->m_tab.getCurrentIndex();

	setPrivateProfileLabel(paneElement, L"splitMode", pane->m_splitMode, g_splitModeLabel);
	setPrivateProfileLabel(paneElement, L"origin", pane->m_origin, g_originLabel);
	setPrivateProfileInt(paneElement, L"border", pane->m_border);
	setPrivateProfileInt(paneElement, L"isBorderLocked", pane->m_isBorderLocked);
	setPrivateProfileInt(paneElement, L"current", current);

	int c = pane->m_tab.getTabCount();
	for (int i = 0; i < c; i++)
	{
		Shuttle* shuttle = pane->m_tab.getShuttle(i);

		// <dockShuttle> を作成する。
		MSXML2::IXMLDOMElementPtr dockShuttleElement = appendElement(paneElement, L"dockShuttle");

		for (auto& x : g_shuttleManager.m_map)
		{
			if (shuttle == x.second.get())
			{
				std::wstring name = getName(x.first);

				setPrivateProfileString(dockShuttleElement, L"name", name.c_str());

				break;
			}
		}
	}

	for (auto& child : pane->m_children)
	{
		if (child)
			savePane(paneElement, child);
	}

	return S_OK;
}

// <floatShuttle> を作成する。
HRESULT saveFloatShuttle(const MSXML2::IXMLDOMElementPtr& element)
{
	MY_TRACE(_T("saveFloatShuttle()\n"));

	for (auto x : g_shuttleManager.m_map)
	{
		// <floatShuttle> を作成する。
		MSXML2::IXMLDOMElementPtr floatShuttleElement = appendElement(element, L"floatShuttle");

		std::wstring name = getName(x.first);

		setPrivateProfileString(floatShuttleElement, L"name", name.c_str());
		setPrivateProfileWindow(floatShuttleElement, L"placement", x.second->m_floatContainer->m_hwnd);
	}

	return S_OK;
}

//---------------------------------------------------------------------
