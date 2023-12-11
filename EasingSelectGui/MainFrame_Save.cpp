#include "pch.h"
#include "EasingSelectGui.h"
#include "MainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HRESULT CMainFrame::saveSettings()
{
	MY_TRACE_FUNC("");

	// 設定ファイルを読み込めていない場合は、保存処理をするとファイルの中の設定値がすべて初期値に戻ってしまいます。
	// これを防ぐために、フラグを確認して設定ファイルを読み込めていない場合は保存処理をしないようにします。
	if (!m_isSettingsFileLoaded)
		return S_FALSE;

	try
	{
		// ドキュメントを作成します。
		MSXML2::IXMLDOMDocumentPtr document(__uuidof(MSXML2::DOMDocument));

		// ドキュメントエレメントを作成します。
		MSXML2::IXMLDOMElementPtr element = appendElement(document, document, L"settings");

		// <easing>を作成します。
		saveEasing(element);

		// <ease>を作成します。
		m_easeWindow.saveEase(element);

		return saveXMLDocument(document, m_fileUpdateChecker.getFilePath(), L"UTF-16");
	}
	catch (_com_error& e)
	{
		MY_TRACE(_T("%s\n"), e.ErrorMessage());
		return e.Error();
	}
}

// <easing>を作成します。
HRESULT CMainFrame::saveEasing(const MSXML2::IXMLDOMElementPtr& element)
{
	MY_TRACE_FUNC("");

	// <easing>を作成します。
	MSXML2::IXMLDOMElementPtr easingElement = appendElement(element, L"easing");

	// <easing>のアトリビュートを作成します。
	setPrivateProfileInt(easingElement, L"imageVersion", m_imageVersion);
	setPrivateProfileBool(easingElement, L"clamp", m_clamp);
	setPrivateProfileString(easingElement, L"horz", m_horz);
	setPrivateProfileString(easingElement, L"vert", m_vert);
	setPrivateProfileInt(easingElement, L"alpha", m_alpha);
	setPrivateProfileInt(easingElement, L"scale", m_scale);

	{
		MSXML2::IXMLDOMElementPtr selectedElement = appendElement(easingElement, L"selected");

		setPrivateProfileColor(selectedElement, m_selectedColor);
	}

	{
		MSXML2::IXMLDOMElementPtr hotElement = appendElement(easingElement, L"hot");

		setPrivateProfileColor(hotElement, m_hotColor);
	}

	return S_OK;
}
