#pragma once

//---------------------------------------------------------------------
// Macro

#define COM_TRY(expression) \
 \
do \
{ \
	HRESULT hr = (expression); \
 \
	if (FAILED(hr)) \
	{ \
		MY_TRACE(_T("failed : %s\n"), _T(#expression)); \
 \
		return hr; \
	} \
} \
while (0)

//---------------------------------------------------------------------
// Save

inline HRESULT WINAPI saveXMLDocument(
	const MSXML2::IXMLDOMDocumentPtr& document,
	const _bstr_t& path, _bstr_t encoding = L"UTF-16")
{
	IStreamPtr stream;
	COM_TRY(::SHCreateStreamOnFileW(
		path,
		STGM_WRITE | STGM_SHARE_DENY_WRITE |
		STGM_CREATE | STGM_DIRECT,
		&stream));

	MSXML2::IMXWriterPtr writer(__uuidof(MSXML2::MXXMLWriter));
	writer->indent = VARIANT_TRUE;
	writer->byteOrderMark = VARIANT_TRUE;
//	writer->omitXMLDeclaration = VARIANT_TRUE;
//	writer->version = L"1.0";
//	writer->encoding = encoding;
	writer->output = stream.GetInterfacePtr();

	MSXML2::ISAXXMLReaderPtr reader(__uuidof(MSXML2::SAXXMLReader));
	reader->putContentHandler(MSXML2::ISAXContentHandlerPtr(writer));

	return reader->parse(document.GetInterfacePtr());
}

//---------------------------------------------------------------------
// Append

inline MSXML2::IXMLDOMElementPtr appendElement(
	const MSXML2::IXMLDOMDocumentPtr& document, const MSXML2::IXMLDOMNodePtr& node, LPCWSTR tag)
{
	MSXML2::IXMLDOMElementPtr newElement = document->createElement(tag);
	return node->appendChild(newElement);
}

inline MSXML2::IXMLDOMElementPtr appendElement(
	const MSXML2::IXMLDOMNodePtr& node, LPCWSTR tag)
{
	return appendElement(node->ownerDocument, node, tag);
}

inline MSXML2::IXMLDOMTextPtr appendText(
	const MSXML2::IXMLDOMDocumentPtr& document, const MSXML2::IXMLDOMNodePtr& node, BSTR text)
{
	MSXML2::IXMLDOMTextPtr newText = document->createTextNode(text);
	return node->appendChild(newText);
}

inline MSXML2::IXMLDOMTextPtr appendText(
	const MSXML2::IXMLDOMNodePtr& node, BSTR text)
{
	return appendText(node->ownerDocument, node, text);
}

//---------------------------------------------------------------------
// Remove

//
// 指定された名前の子要素をすべて削除します。
//
inline int removeChildren(const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name)
{
	MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(name);
	int c = nodeList->length;
	for (int i = 0; i < c; i++)
	{
		MSXML2::IXMLDOMElementPtr child = nodeList->item[i];

		element->removeChild(child);
	}

	return c;
}

//---------------------------------------------------------------------
// Get

inline HRESULT WINAPI getPrivateProfileVARIANT(
	const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, _variant_t& outValue)
{
	outValue = element->getAttribute(name);

	return S_OK;
}

inline HRESULT WINAPI getPrivateProfileBSTR(
	const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, _bstr_t& outValue)
{
	try
	{
		_variant_t var = element->getAttribute(name);

		if (var.vt == VT_NULL)
			return S_FALSE;

		outValue = var;

		return S_OK;
	}
	catch (_com_error& e)
	{
		return e.Error();
	}
}

template<class T>
inline HRESULT WINAPI getPrivateProfileInt(
	const _variant_t& var, T& outValue, int radix = 0)
{
	try
	{
		if (var.vt == VT_NULL)
			return S_FALSE;

		_bstr_t value = (_bstr_t)var;

		if (!(BSTR)value)
			return S_FALSE;

		outValue = (T)wcstol(value, 0, radix);

		return S_OK;
	}
	catch (_com_error& e)
	{
		return e.Error();
	}
}

template<class T>
inline HRESULT WINAPI getPrivateProfileInt(
	const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, T& outValue, int radix = 0)
{
	return getPrivateProfileInt(element->getAttribute(name), outValue, radix);
}

template<class T>
inline HRESULT WINAPI getPrivateProfileReal(const variant_t& var, T& outValue)
{
	try
	{
		if (var.vt == VT_NULL)
			return S_FALSE;

		_bstr_t value = (_bstr_t)var;

		if (!(BSTR)value)
			return S_FALSE;

		outValue = (T)wcstod(value, 0);

		return S_OK;
	}
	catch (_com_error& e)
	{
		return e.Error();
	}
}

template<class T>
inline HRESULT WINAPI getPrivateProfileReal(
	const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, T& outValue)
{
	return getPrivateProfileReal(element->getAttribute(name), outValue);
}

template<class T>
inline HRESULT WINAPI getPrivateProfileString(const _variant_t& var, T& outValue)
{
	try
	{
		if (var.vt == VT_NULL)
			return S_FALSE;

		_bstr_t value = (_bstr_t)var;

		if (!(BSTR)value)
			return S_FALSE;

		outValue = (BSTR)value;

		return S_OK;
	}
	catch (_com_error& e)
	{
		return e.Error();
	}
}

template<class T>
inline HRESULT WINAPI getPrivateProfileString(
	const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, T& outValue)
{
	return getPrivateProfileString(element->getAttribute(name), outValue);
}

template<class T>
inline HRESULT WINAPI getPrivateProfileString(const _variant_t& var, T& outValue, int cch)
{
	try
	{
		if (var.vt == VT_NULL)
			return S_FALSE;

		_bstr_t value = (_bstr_t)var;

		if (!(BSTR)value)
			return S_FALSE;

		::StringCchCopy(outValue, cch, value);

		return S_OK;
	}
	catch (_com_error& e)
	{
		return e.Error();
	}
}

template<class T>
inline HRESULT WINAPI getPrivateProfileString(
	const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, T& outValue, int cch)
{
	return getPrivateProfileString(element->getAttribute(name), outValue, cch);
}

template<class T>
inline HRESULT WINAPI getPrivateProfileBool(const _variant_t& var, T& outValue)
{
	try
	{
		if (var.vt == VT_NULL)
			return S_FALSE;

		_bstr_t value = (_bstr_t)var;

		if (!(BSTR)value || ::lstrlenW(value) == 0)
			return S_FALSE;

		if (::StrCmpIW(value, L"ON") == 0)
		{
			outValue = true;
		}
		else if (::StrCmpIW(value, L"OFF") == 0)
		{
			outValue = false;
		}
		else if (::StrCmpIW(value, L"YES") == 0)
		{
			outValue = true;
		}
		else if (::StrCmpIW(value, L"NO") == 0)
		{
			outValue = false;
		}
		else
		{
			outValue = !!wcstol(value, 0, 0);
		}

		return S_OK;
	}
	catch (_com_error& e)
	{
		return e.Error();
	}
}

template<class T>
inline HRESULT WINAPI getPrivateProfileBool(
	const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, T& outValue)
{
	return getPrivateProfileBool(element->getAttribute(name), outValue);
}

inline HRESULT WINAPI getPrivateProfileColor(const _variant_t& var, COLORREF& outValue)
{
	try
	{
		if (var.vt == VT_NULL)
			return S_FALSE;

		_bstr_t value = (_bstr_t)var;

		if (!(BSTR)value)
			return S_FALSE;

		int c = ::lstrlenW(value);

		if (c == 0)
			return S_FALSE;

		BSTR bstrValue = value;

		if (*bstrValue == L'#')
		{
			DWORD temp = wcstoul(bstrValue + 1, 0, 16);

			if (c == 4)
			{
				// #fc8

				DWORD r = (temp & 0x0F00) >> 8;
				DWORD g = (temp & 0x00F0) >> 4;
				DWORD b = (temp & 0x000F) >> 0;

				r |= r << 4;
				g |= g << 4;
				b |= b << 4;

				outValue = RGB(r, g, b);
			}
			else
			{
				// #ffcc88

				DWORD r = (temp & 0x00FF0000) >> 16;
				DWORD g = (temp & 0x0000FF00) >> 8;
				DWORD b = (temp & 0x000000FF) >> 0;

				outValue = RGB(r, g, b);
			}
		}
		else
		{
			BSTR sep1 = ::StrChrW(bstrValue, L',');

			if (!sep1)
				return S_FALSE;

			BSTR sep2 = ::StrChrW(sep1 + 1, L',');

			if (!sep2)
				return S_FALSE;

			*sep1 = L'\0';
			*sep2 = L'\0';

			DWORD r = wcstoul(bstrValue, 0, 0);
			DWORD g = wcstoul(sep1 + 1, 0, 0);
			DWORD b = wcstoul(sep2 + 1, 0, 0);

			outValue = RGB(r, g, b);
		}

		return S_OK;
	}
	catch (_com_error& e)
	{
		return e.Error();
	}
}

inline HRESULT WINAPI getPrivateProfileColor(
	const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, COLORREF& outValue)
{
	return getPrivateProfileColor(element->getAttribute(name), outValue);
}
#ifdef _GDIPLUS_H
inline HRESULT WINAPI getPrivateProfileColor(const _variant_t& var, Gdiplus::Color& outValue)
{
	try
	{
		if (var.vt == VT_NULL)
			return S_FALSE;

		_bstr_t value = (_bstr_t)var;

		if (!(BSTR)value)
			return S_FALSE;

		int c = ::lstrlenW(value);

		if (c == 0)
			return S_FALSE;

		BSTR bstrValue = value;

		if (*bstrValue == L'#')
		{
			DWORD temp = wcstoul(bstrValue + 1, 0, 16);

			if (c == 4)
			{
				// #fc8

				DWORD r = (temp & 0x0F00) >> 8;
				DWORD g = (temp & 0x00F0) >> 4;
				DWORD b = (temp & 0x000F) >> 0;

				r |= r << 4;
				g |= g << 4;
				b |= b << 4;

				outValue.SetFromCOLORREF(RGB(r, g, b));
			}
			else
			{
				// #ffcc88

				DWORD r = (temp & 0x00FF0000) >> 16;
				DWORD g = (temp & 0x0000FF00) >> 8;
				DWORD b = (temp & 0x000000FF) >> 0;

				outValue.SetFromCOLORREF(RGB(r, g, b));
			}
		}
		else
		{
			BSTR sep1 = ::StrChrW(bstrValue, L',');

			if (!sep1)
			{
				outValue.SetValue(wcstoul(bstrValue, 0, 16));
			}
			else
			{
				BSTR sep2 = ::StrChrW(sep1 + 1, L',');

				if (!sep2)
					return S_FALSE;

				BSTR sep3 = ::StrChrW(sep2 + 1, L',');

				if (!sep3)
				{
					*sep1 = L'\0';
					*sep2 = L'\0';

					DWORD r = wcstoul(bstrValue, 0, 0);
					DWORD g = wcstoul(sep1 + 1, 0, 0);
					DWORD b = wcstoul(sep2 + 1, 0, 0);

					outValue.SetFromCOLORREF(RGB(r, g, b));
				}
				else
				{
					*sep1 = L'\0';
					*sep2 = L'\0';
					*sep3 = L'\0';

					BYTE a = (BYTE)wcstoul(bstrValue, 0, 0);
					BYTE r = (BYTE)wcstoul(sep1 + 1, 0, 0);
					BYTE g = (BYTE)wcstoul(sep2 + 1, 0, 0);
					BYTE b = (BYTE)wcstoul(sep3 + 1, 0, 0);

					outValue.SetValue(outValue.MakeARGB(a, r, g, b));
				}
			}
		}

		return S_OK;
	}
	catch (_com_error& e)
	{
		return e.Error();
	}
}

inline HRESULT WINAPI getPrivateProfileColor(
	const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, Gdiplus::Color& outValue)
{
	return getPrivateProfileColor(element->getAttribute(name), outValue);
}
#endif
template<class T, class S>
inline HRESULT WINAPI getPrivateProfilePercent(const _variant_t& var, T& outValue, S& outValuePercent)
{
	try
	{
		if (var.vt == VT_NULL)
			return S_FALSE;

		_bstr_t value = (_bstr_t)var;

		if (!(BSTR)value || ::lstrlenW(value) == 0)
			return S_FALSE;

		if (::StrChrW(value, L'%'))
			outValuePercent = wcstol(value, 0, 0);
		else
			outValue = wcstol(value, 0, 0);

		return S_OK;
	}
	catch (_com_error& e)
	{
		return e.Error();
	}
}

template<class T, class S>
inline HRESULT WINAPI getPrivateProfilePercent(
	const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, T& outValue, S& outValuePercent)
{
	return getPrivateProfilePercent(element->getAttribute(name), outValue, outValuePercent);
}

template<class T>
inline HRESULT WINAPI getPrivateProfileTime(const _variant_t& var, T& outValue)
{
	try
	{
		if (var.vt == VT_NULL)
			return S_FALSE;

		_bstr_t value = (_bstr_t)var;

		if (!(BSTR)value || ::lstrlenW(value) == 0)
			return S_FALSE;

		LPWSTR sep = ::StrChrW(value, L':');

		if (sep)
		{
			*sep = L'\0';
			outValue = _wtof(value) * 60 + _wtof(sep + 1);
		}
		else
		{
			outValue = _wtof(value);
		}

		return S_OK;
	}
	catch (_com_error& e)
	{
		return e.Error();
	}
}

template<class T>
inline HRESULT WINAPI getPrivateProfileTime(
	const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, T& outValue)
{
	return getPrivateProfileTime(element->getAttribute(name), outValue);
}

template<class T>
inline HRESULT WINAPI getPrivateProfileFileName(
	const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, T& outValue)
{
	try
	{
		// 属性を取得する。
		_variant_t var = element->getAttribute(name);

		if (var.vt == VT_NULL)
			return S_FALSE; // 属性が無効だった。

		// 文字列に変換する。
		_bstr_t value = (_bstr_t)var;

		if (!(BSTR)value || ::lstrlenW(value) == 0)
			return S_FALSE; // 文字列が無効だった。

		// 元のドキュメントの URL を取得する。
		_bstr_t url = element->ownerDocument->url;
//		MY_TRACE_WSTR((BSTR)url);

		// 元のドキュメントのフォルダを取得する。
		DWORD c = MAX_PATH;
		WCHAR folder[MAX_PATH] = {};
		::PathCreateFromUrlW(url, folder, &c, 0);
		::PathRemoveFileSpecW(folder);
//		MY_TRACE_WSTR(folder);

		WCHAR fileName[MAX_PATH] = {};
		::StringCchCopyW(fileName, std::size(fileName), folder);
		::PathAppendW(fileName, value);
//		MY_TRACE_WSTR(fileName);

		outValue = fileName;

		return S_OK;
	}
	catch (_com_error& e)
	{
		return e.Error();
	}
}

template<class T, class Labels>
inline HRESULT WINAPI getPrivateProfileLabel(
	const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, T& outValue, const Labels& labels)
{
	_bstr_t text;
	HRESULT hr = getPrivateProfileBSTR(element, name, text);
	if (hr != S_OK) return hr;

	for (auto label : labels)
	{
		if (wcscmp(label.text, text) == 0)
		{
			outValue = label.value;
			return S_OK;
		}
	}

	return S_FALSE;
}

inline HRESULT getPrivateProfileWindow(const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, HWND hwnd, DWORD cmdShow = -1)
{
	WINDOWPLACEMENT wp = { sizeof(wp) };
	if (!::GetWindowPlacement(hwnd, &wp)) return S_FALSE;
	if (!::IsWindowVisible(hwnd)) wp.showCmd = SW_HIDE;
	wp.flags = WPF_ASYNCWINDOWPLACEMENT | WPF_SETMINPOSITION;

	MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(name);
	int c = nodeList->length;
	for (int i = 0; i < c; i++)
	{
		MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

		if (cmdShow == -1)
			getPrivateProfileInt(element, L"flags", wp.showCmd);
		else
			wp.showCmd = cmdShow;

		getPrivateProfileInt(element, L"left", wp.rcNormalPosition.left);
		getPrivateProfileInt(element, L"top", wp.rcNormalPosition.top);
		getPrivateProfileInt(element, L"right", wp.rcNormalPosition.right);
		getPrivateProfileInt(element, L"bottom", wp.rcNormalPosition.bottom);

		getPrivateProfileInt(element, L"minX", wp.ptMinPosition.x);
		getPrivateProfileInt(element, L"minY", wp.ptMinPosition.y);
		getPrivateProfileInt(element, L"maxX", wp.ptMaxPosition.x);
		getPrivateProfileInt(element, L"maxY", wp.ptMaxPosition.y);
	}

	if (!::SetWindowPlacement(hwnd, &wp))
		return S_FALSE;

	return S_OK;
}

inline HRESULT getPrivateProfileWindowRect(const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, HWND hwnd)
{
	RECT rc = {};

	MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(name);
	int c = nodeList->length;
	for (int i = 0; i < c; i++)
	{
		MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

		getPrivateProfileInt(element, L"left", rc.left);
		getPrivateProfileInt(element, L"top", rc.top);
		getPrivateProfileInt(element, L"right", rc.right);
		getPrivateProfileInt(element, L"bottom", rc.bottom);
	}

	if (!::SetWindowPos(hwnd, 0, rc.left, rc.top,
		rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOACTIVATE))
	{
		return S_FALSE;
	}

	return S_OK;
}

inline HRESULT getPrivateProfileLOGFONT(const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, LOGFONT& outValue)
{
	MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(name);
	int c = nodeList->length;
	for (int i = 0; i < c; i++)
	{
		MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

		getPrivateProfileInt(element, L"lfHeight", outValue.lfHeight);
		getPrivateProfileInt(element, L"lfWidth", outValue.lfWidth);
		getPrivateProfileInt(element, L"lfEscapement", outValue.lfEscapement);
		getPrivateProfileInt(element, L"lfOrientation", outValue.lfOrientation);
		getPrivateProfileInt(element, L"lfWeight", outValue.lfWeight);

		getPrivateProfileInt(element, L"lfItalic", outValue.lfItalic);
		getPrivateProfileInt(element, L"lfUnderline", outValue.lfUnderline);
		getPrivateProfileInt(element, L"lfStrikeOut", outValue.lfStrikeOut);
		getPrivateProfileInt(element, L"lfCharSet", outValue.lfCharSet);
		getPrivateProfileInt(element, L"lfOutPrecision", outValue.lfOutPrecision);
		getPrivateProfileInt(element, L"lfClipPrecision", outValue.lfClipPrecision);
		getPrivateProfileInt(element, L"lfQuality", outValue.lfQuality);
		getPrivateProfileInt(element, L"lfPitchAndFamily", outValue.lfPitchAndFamily);
		getPrivateProfileString(element, L"lfFaceName", outValue.lfFaceName, std::size(outValue.lfFaceName));
	}

	return S_OK;
}

//---------------------------------------------------------------------
// Set

template<class T>
inline HRESULT WINAPI setPrivateProfileInt(
	const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, const T& value)
{
	return element->setAttribute(name, value);
}

template<class T>
inline HRESULT WINAPI setPrivateProfileReal(
	const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, const T& value)
{
	return element->setAttribute(name, value);
}

template<class T>
inline HRESULT WINAPI setPrivateProfileString(
	const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, const T& value)
{
	return element->setAttribute(name, value);
}

template<class T>
inline HRESULT WINAPI setPrivateProfileBool(
	const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, const T& value)
{
	return element->setAttribute(name, value ? L"YES" : L"NO");
}

inline HRESULT WINAPI setPrivateProfileColor(
	const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, COLORREF value)
{
	WCHAR text[MAX_PATH] = {};
	::StringCchPrintfW(text, std::size(text), L"#%02x%02x%02x", GetRValue(value), GetGValue(value), GetBValue(value));

	return element->setAttribute(name, text);
}
#ifdef _GDIPLUS_H
inline HRESULT WINAPI setPrivateProfileColor(
	const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, const Gdiplus::Color& value)
{
	WCHAR text[MAX_PATH] = {};
	::StringCchPrintfW(text, std::size(text), L"%08X", value.GetValue());

	return element->setAttribute(name, text);
}
#endif
template<class T, class Labels>
inline HRESULT WINAPI setPrivateProfileLabel(
	const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, const T& value, const Labels& labels)
{
	for (auto label : labels)
	{
		if (label.value == value)
			return element->setAttribute(name, label.text);
	}

	return S_FALSE;
}

inline HRESULT setPrivateProfileWindow(const MSXML2::IXMLDOMElementPtr& _element, LPCWSTR name, HWND hwnd, DWORD cmdShow = -1)
{
	WINDOWPLACEMENT wp = { sizeof(wp) };
	if (!::GetWindowPlacement(hwnd, &wp)) return E_FAIL;

	if (::IsIconic(hwnd)) wp.showCmd = SW_SHOW;
	if (wp.flags == WPF_RESTORETOMAXIMIZED) wp.showCmd = SW_SHOWMAXIMIZED;
	if (!::IsWindowVisible(hwnd)) wp.showCmd = SW_HIDE;

	MSXML2::IXMLDOMElementPtr element = appendElement(_element, name);

	setPrivateProfileInt(element, L"flags", (cmdShow == -1) ? wp.showCmd : cmdShow);

	setPrivateProfileInt(element, L"left", wp.rcNormalPosition.left);
	setPrivateProfileInt(element, L"top", wp.rcNormalPosition.top);
	setPrivateProfileInt(element, L"right", wp.rcNormalPosition.right);
	setPrivateProfileInt(element, L"bottom", wp.rcNormalPosition.bottom);

	setPrivateProfileInt(element, L"minX", wp.ptMinPosition.x);
	setPrivateProfileInt(element, L"minY", wp.ptMinPosition.y);
	setPrivateProfileInt(element, L"maxX", wp.ptMaxPosition.x);
	setPrivateProfileInt(element, L"maxY", wp.ptMaxPosition.y);

	return S_OK;
}

inline HRESULT setPrivateProfileWindowRect(const MSXML2::IXMLDOMElementPtr& _element, LPCWSTR name, HWND hwnd)
{
	RECT rc; ::GetWindowRect(hwnd, &rc);

	MSXML2::IXMLDOMElementPtr element = appendElement(_element, name);

	setPrivateProfileInt(element, L"left", rc.left);
	setPrivateProfileInt(element, L"top", rc.top);
	setPrivateProfileInt(element, L"right", rc.right);
	setPrivateProfileInt(element, L"bottom", rc.bottom);

	return S_OK;
}

inline HRESULT setPrivateProfileLOGFONT(const MSXML2::IXMLDOMElementPtr& _element, LPCWSTR name, const LOGFONT& value)
{
	MSXML2::IXMLDOMElementPtr element = appendElement(_element, name);

	setPrivateProfileInt(element, L"lfHeight", value.lfHeight);
	setPrivateProfileInt(element, L"lfWidth", value.lfWidth);
	setPrivateProfileInt(element, L"lfEscapement", value.lfEscapement);
	setPrivateProfileInt(element, L"lfOrientation", value.lfOrientation);
	setPrivateProfileInt(element, L"lfWeight", value.lfWeight);

	setPrivateProfileInt(element, L"lfItalic", value.lfItalic);
	setPrivateProfileInt(element, L"lfUnderline", value.lfUnderline);
	setPrivateProfileInt(element, L"lfStrikeOut", value.lfStrikeOut);
	setPrivateProfileInt(element, L"lfCharSet", value.lfCharSet);
	setPrivateProfileInt(element, L"lfOutPrecision", value.lfOutPrecision);
	setPrivateProfileInt(element, L"lfClipPrecision", value.lfClipPrecision);
	setPrivateProfileInt(element, L"lfQuality", value.lfQuality);
	setPrivateProfileInt(element, L"lfPitchAndFamily", value.lfPitchAndFamily);
	setPrivateProfileString(element, L"lfFaceName", value.lfFaceName);

	return S_OK;
}

//---------------------------------------------------------------------
