#include "pch.h"
#include "EasingSelectGui.h"
#include "MainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HRESULT CMainFrame::loadSettings()
{
	MY_TRACE_FUNC("");

	try
	{
		m_isSettingsFileLoaded = FALSE;

		LPCWSTR fileName = m_fileUpdateChecker.getFilePath();
		MY_TRACE_WSTR(fileName);

		// MSXML を使用する。
		MSXML2::IXMLDOMDocumentPtr document(__uuidof(MSXML2::DOMDocument));

		// 設定ファイルを開く。
		if (document->load(fileName) == VARIANT_FALSE)
		{
			MY_TRACE(_T("%s を開けませんでした\n"), fileName);

			return S_FALSE;
		}

		MSXML2::IXMLDOMElementPtr element = document->documentElement;

		// <easing> を読み込む。
		loadEasing(element);

		// <ease> を読み込む。
		m_easeWindow.loadEase(element);

		MY_TRACE(_T("設定ファイルの読み込みに成功しました\n"));

		// 設定ファイルを読み込めたのでフラグを立てておく。
		m_isSettingsFileLoaded = TRUE;

		loadImage();

		return S_OK;
	}
	catch (_com_error& e)
	{
		MY_TRACE(_T("設定ファイルの読み込みに失敗しました\n"));
		MY_TRACE(_T("%s\n"), e.ErrorMessage());
		return e.Error();
	}
}

// <easing> を読み込む。
HRESULT CMainFrame::loadEasing(const MSXML2::IXMLDOMElementPtr& element)
{
	MY_TRACE_FUNC("");

	// <easing> を取得する。
	MSXML2::IXMLDOMNodeListPtr nodeList = element->getElementsByTagName(L"easing");
	int c = nodeList->length;
	for (int i = 0; i < c; i++)
	{
		MSXML2::IXMLDOMElementPtr easingElement = nodeList->item[i];

		// <easing> のアトリビュートを取得する。
		getPrivateProfileInt(easingElement, L"imageVersion", m_imageVersion);
		getPrivateProfileBool(easingElement, L"clamp", m_clamp);
		getPrivateProfileString(easingElement, L"horz", m_horz);
		getPrivateProfileString(easingElement, L"vert", m_vert);
		getPrivateProfileInt(easingElement, L"alpha", m_alpha);
		getPrivateProfileInt(easingElement, L"scale", m_scale);

		{
			// <selected> を取得する。
			MSXML2::IXMLDOMNodeListPtr nodeList = easingElement->getElementsByTagName(L"selected");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr selectedElement = nodeList->item[i];

				getPrivateProfileColor(selectedElement, m_selectedColor);
			}
		}

		{
			// <hot> を取得する。
			MSXML2::IXMLDOMNodeListPtr nodeList = easingElement->getElementsByTagName(L"hot");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr hotElement = nodeList->item[i];

				getPrivateProfileColor(hotElement, m_hotColor);
			}
		}
	}

	return S_OK;
}
