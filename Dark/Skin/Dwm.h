#pragma once
#include "Skin/ColorSet.h"

namespace fgo::dark::skin
{
	inline struct Dwm
	{
		enum MY_DWMWINDOWATTRIBUTE
		{
			DWMWA_USE_IMMERSIVE_DARK_MODE = 20,
			DWMWA_WINDOW_CORNER_PREFERENCE = 33,
			DWMWA_BORDER_COLOR = 34,
			DWMWA_CAPTION_COLOR = 35,
			DWMWA_TEXT_COLOR = 36,
			DWMWA_VISIBLE_FRAME_BORDER_THICKNESS,
			DWMWA_LAST
		};

		BOOL enable = FALSE;
		int darkMode = -1;
		int cornerMode = -1;
		COLORREF activeBorderColor = -1;
		COLORREF activeCaptionColor = -1;
		COLORREF activeTextColor = -1;
		COLORREF inactiveBorderColor = -1;
		COLORREF inactiveCaptionColor = -1;
		COLORREF inactiveTextColor = -1;

		BOOL init(HWND hwnd)
		{
			MY_TRACE_FUNC("0x%08X", hwnd);

			enable = get(hwnd) == S_OK;

			return TRUE;
		}

		HRESULT get(HWND hwnd)
		{
			MY_TRACE_FUNC("0x%08X", hwnd);

			// トップレベルウィンドウを取得します。
			while (HWND parent = ::GetParent(hwnd))
				hwnd = parent;

			int cornerMode = 0;
			return ::DwmGetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE , &cornerMode, sizeof(cornerMode));
		}

		void set(HWND hwnd, BOOL active)
		{
			MY_TRACE_FUNC("0x%08X, %d", hwnd, active);

			if (!enable) return;

			BOOL trace = FALSE;

			if (active)
			{
				if (activeBorderColor != -1)
				{
					HRESULT hr = ::DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR , &activeBorderColor, sizeof(activeBorderColor));
					if (trace) MY_TRACE_COM_ERROR(hr);
				}

				if (activeCaptionColor != -1)
				{
					HRESULT hr = ::DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR , &activeCaptionColor, sizeof(activeCaptionColor));
					if (trace) MY_TRACE_COM_ERROR(hr);
				}

				if (activeTextColor != -1)
				{
					HRESULT hr = ::DwmSetWindowAttribute(hwnd, DWMWA_TEXT_COLOR , &activeTextColor, sizeof(activeTextColor));
					if (trace) MY_TRACE_COM_ERROR(hr);
				}
			}
			else
			{
				if (inactiveBorderColor != -1)
				{
					HRESULT hr = ::DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR , &inactiveBorderColor, sizeof(inactiveBorderColor));
					if (trace) MY_TRACE_COM_ERROR(hr);
				}

				if (inactiveCaptionColor != -1)
				{
					HRESULT hr = ::DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR , &inactiveCaptionColor, sizeof(inactiveCaptionColor));
					if (trace) MY_TRACE_COM_ERROR(hr);
				}

				if (inactiveTextColor != -1)
				{
					HRESULT hr = ::DwmSetWindowAttribute(hwnd, DWMWA_TEXT_COLOR , &inactiveTextColor, sizeof(inactiveTextColor));
					if (trace) MY_TRACE_COM_ERROR(hr);
				}
			}

			if (darkMode != -1)
			{
				HRESULT hr = ::DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE , &darkMode, sizeof(darkMode));
				if (trace) MY_TRACE_COM_ERROR(hr);
			}

			if (cornerMode != -1)
			{
				HRESULT hr = ::DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE , &cornerMode, sizeof(cornerMode));
				if (trace) MY_TRACE_COM_ERROR(hr);
			}
		}

		HRESULT load(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE_FUNC("");

			// <Dwm>を読み込みます。
			MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"Dwm");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

				getPrivateProfileNamedColor(element, L"activeBorderColor", activeBorderColor, ColorSet::Type::edgeColor);
				getPrivateProfileNamedColor(element, L"activeCaptionColor", activeCaptionColor, ColorSet::Type::fillColor);
				getPrivateProfileNamedColor(element, L"activeTextColor", activeTextColor, ColorSet::Type::textForeColor);
				getPrivateProfileNamedColor(element, L"inactiveBorderColor", inactiveBorderColor, ColorSet::Type::edgeColor);
				getPrivateProfileNamedColor(element, L"inactiveCaptionColor", inactiveCaptionColor, ColorSet::Type::fillColor);
				getPrivateProfileNamedColor(element, L"inactiveTextColor", inactiveTextColor, ColorSet::Type::textForeColor);
				getPrivateProfileInt(element, L"darkMode", darkMode);
				getPrivateProfileInt(element, L"cornerMode", cornerMode);
			}

			return S_OK;
		}
	} dwm;
}
