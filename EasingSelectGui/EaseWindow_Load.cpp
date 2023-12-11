#include "pch.h"
#include "EasingSelectGui.h"
#include "EaseWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// <ease>を読み込みます。
HRESULT CEaseWindow::loadEase(const MSXML2::IXMLDOMElementPtr& element)
{
	MY_TRACE_FUNC("");

	// <ease>を取得します。
	MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"ease");
	int c = nodeList->length;
	for (int i = 0; i < c; i++)
	{
		MSXML2::IXMLDOMElementPtr easeElement = nodeList->item[i];

		// <ease>のアトリビュートを取得します。
		getPrivateProfileBool(easeElement, L"enable", m_enable);
		getPrivateProfileString(easeElement, L"origin", m_origin);
		getPrivateProfileBool(easeElement, L"clamp", m_clamp);
		getPrivateProfileString(easeElement, L"horz", m_horz);
		getPrivateProfileString(easeElement, L"vert", m_vert);
		getPrivateProfileInt(easeElement, L"alpha", m_alpha);
		getPrivateProfileInt(easeElement, L"segmentCount", m_segmentCount);
		getPrivateProfileBool(easeElement, L"hideCursor", m_hideCursor);
		getPrivateProfileBool(easeElement, L"immediately", m_immediately);

		{
			// <window>を取得します。
			MSXML2::IXMLDOMNodeListPtr nodeList = easeElement->selectNodes(L"window");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr windowElement = nodeList->item[i];

				getPrivateProfileInt(windowElement, L"w", m_windowSize.cx);
				getPrivateProfileInt(windowElement, L"h", m_windowSize.cy);
			}
		}

		{
			// <background>を取得します。
			MSXML2::IXMLDOMNodeListPtr nodeList = easeElement->selectNodes(L"background");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr backgroundElement = nodeList->item[i];

				getPrivateProfileColor(backgroundElement, m_backgroundColor);
			}
		}

		{
			// <border>を取得します。
			MSXML2::IXMLDOMNodeListPtr nodeList = easeElement->selectNodes(L"border");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr borderElement = nodeList->item[i];

				getPrivateProfileColor(borderElement, m_borderColor);
				getPrivateProfileInt(borderElement, L"width", m_borderWidth);
			}
		}

		{
			// <curve>を取得します。
			MSXML2::IXMLDOMNodeListPtr nodeList = easeElement->selectNodes(L"curve");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr curveElement = nodeList->item[i];

				getPrivateProfileColor(curveElement, m_curveColor);
				getPrivateProfileInt(curveElement, L"width", m_curveWidth);
			}
		}

		{
			// <invalidCurve>を取得します。
			MSXML2::IXMLDOMNodeListPtr nodeList = easeElement->selectNodes(L"invalidCurve");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr invalidCurveElement = nodeList->item[i];

				getPrivateProfileColor(invalidCurveElement, m_invalidCurveColor);
				getPrivateProfileInt(invalidCurveElement, L"width", m_invalidCurveWidth);
			}
		}

		{
			// <handle>を取得します。
			MSXML2::IXMLDOMNodeListPtr nodeList = easeElement->selectNodes(L"handle");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr handleElement = nodeList->item[i];

				getPrivateProfileColor(handleElement, m_handleColor);
				getPrivateProfileInt(handleElement, L"width", m_handleWidth);
			}
		}

		{
			// <point>を取得します。
			MSXML2::IXMLDOMNodeListPtr nodeList = easeElement->selectNodes(L"point");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr pointElement = nodeList->item[i];

				getPrivateProfileColor(pointElement, m_pointColor);
				getPrivateProfileInt(pointElement, L"radius", m_pointRadius);
			}
		}

		{
			// <hotPoint>を取得します。
			MSXML2::IXMLDOMNodeListPtr nodeList = easeElement->selectNodes(L"hotPoint");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr hotPointElement = nodeList->item[i];

				getPrivateProfileColor(hotPointElement, m_hotPointColor);
			}
		}
	}

	return S_OK;
}
