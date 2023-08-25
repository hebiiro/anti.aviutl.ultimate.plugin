#pragma once

//---------------------------------------------------------------------
// Get

inline HRESULT WINAPI getPrivateProfileBSTR(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, _bstr_t& outValue)
{
	WCHAR buffer[MAX_PATH] = {};
	DWORD result = ::GetPrivateProfileStringW(appName, keyName, L"", buffer, MAX_PATH, fileName);
	if (!result) return S_FALSE;
	outValue = buffer;
	return S_OK;
}

template<class T>
inline HRESULT WINAPI getPrivateProfileString(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, T& outValue)
{
	DWORD result = ::GetPrivateProfileStringW(appName, keyName, outValue, outValue, std::size(outValue), fileName);
	if (!result) return S_FALSE;
	return S_OK;
}

template<class T>
inline HRESULT WINAPI getPrivateProfileInt(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, T& outValue, int radix = 0)
{
	_bstr_t value = L"";
	HRESULT hr = getPrivateProfileBSTR(fileName, appName, keyName, value);
	if (hr != S_OK) return hr;
	if (!(BSTR)value) return S_FALSE;
	outValue = (T)wcstol(value, 0, radix);
	return S_OK;
}

template<class T>
inline HRESULT WINAPI getPrivateProfileReal(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, T& outValue)
{
	_bstr_t value = L"";
	HRESULT hr = getPrivateProfileBSTR(fileName, appName, keyName, value);
	if (hr != S_OK) return hr;
	if (!(BSTR)value) return S_FALSE;
	outValue = (T)wcstod(value, 0);
	return S_OK;
}

template<class T>
inline HRESULT WINAPI getPrivateProfileBool(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, T& outValue)
{
	_bstr_t value = L"";
	HRESULT hr = getPrivateProfileBSTR(fileName, appName, keyName, value);
	if (hr != S_OK) return hr;
	if (!(BSTR)value) return S_FALSE;
	if (::lstrlenW(value) == 0) return S_FALSE;

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

inline HRESULT WINAPI getPrivateProfileColor(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, COLORREF& outValue)
{
	_bstr_t value = L"";
	HRESULT hr = getPrivateProfileBSTR(fileName, appName, keyName, value);
	if (hr != S_OK) return hr;
	if (!(BSTR)value) return S_FALSE;

	int c = ::lstrlenW(value);

	if (c == 0) return S_FALSE;

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
#ifdef _GDIPLUS_H
inline HRESULT WINAPI getPrivateProfileColor(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, Gdiplus::Color& outValue)
{
	_bstr_t value = L"";
	HRESULT hr = getPrivateProfileBSTR(fileName, appName, keyName, value);
	if (hr != S_OK) return hr;
	if (!(BSTR)value) return S_FALSE;

	int c = ::lstrlenW(value);

	if (c == 0) return S_FALSE;

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
#endif
#ifdef NANOVG_H
inline HRESULT WINAPI getPrivateProfileColor(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, NVGcolor& outValue)
{
	_bstr_t value = L"";
	HRESULT hr = getPrivateProfileBSTR(fileName, appName, keyName, value);
	if (hr != S_OK) return hr;
	if (!(BSTR)value) return S_FALSE;

	int c = ::lstrlenW(value);

	if (c == 0) return S_FALSE;

	BSTR bstrValue = value;

	if (*bstrValue == L'#')
	{
		BSTR sep = wcschr(bstrValue, L',');
		BSTR end = 0;
		DWORD temp = wcstoul(bstrValue + 1, &end, 16);

		DWORD r = 0, g = 0, b = 0, a = 255;

		if (end - bstrValue == 4)
		{
			// #fc8

			r = (temp & 0x0F00) >> 8;
			g = (temp & 0x00F0) >> 4;
			b = (temp & 0x000F) >> 0;

			r |= r << 4;
			g |= g << 4;
			b |= b << 4;
		}
		else
		{
			// #ffcc88

			r = (temp & 0x00FF0000) >> 16;
			g = (temp & 0x0000FF00) >> 8;
			b = (temp & 0x000000FF) >> 0;
		}

		if (sep)
		{
			// #..., A

			a = wcstoul(sep + 1, 0, 0);
		}

		outValue = nvgRGBA((BYTE)r, (BYTE)g, (BYTE)b, (BYTE)a);
		return S_OK;
	}

	if (wcschr(bstrValue, L'.'))
	{
		// 小数。

		BSTR sep1 = wcschr(bstrValue, L',');

		if (!sep1)
		{
			// RGB

			float rgb = wcstof(bstrValue, 0);
			outValue = nvgRGBf(rgb, rgb, rgb);
			return S_OK;
		}

		BSTR sep2 = wcschr(sep1 + 1, L',');

		if (!sep2)
		{
			// RGB, A

			float rgb = wcstof(bstrValue, 0);
			float a = wcstof(sep1 + 1, 0);
			outValue = nvgRGBAf(rgb, rgb, rgb, a);
			return S_OK;
		}

		BSTR sep3 = wcschr(sep2 + 1, L',');

		if (!sep3)
		{
			// R, G, B

			float r = wcstof(bstrValue, 0);
			float g = wcstof(sep1 + 1, 0);
			float b = wcstof(sep2 + 1, 0);
			outValue = nvgRGBf(r, g, b);
			return S_OK;
		}

		{
			// R, G, B, A

			float r = wcstof(bstrValue, 0);
			float g = wcstof(sep1 + 1, 0);
			float b = wcstof(sep2 + 1, 0);
			float a = wcstof(sep3 + 1, 0);
			outValue = nvgRGBAf(r, g, b, a);
			return S_OK;
		}
	}
	else
	{
		// 整数。

		BSTR sep1 = wcschr(bstrValue, L',');

		if (!sep1)
		{
			// RGB

			DWORD rgb = wcstoul(bstrValue, 0, 0);
			outValue = nvgRGB((BYTE)rgb, (BYTE)rgb, (BYTE)rgb);
			return S_OK;
		}

		BSTR sep2 = wcschr(sep1 + 1, L',');

		if (!sep2)
		{
			// RGB, A

			DWORD rgb = wcstoul(bstrValue, 0, 0);
			DWORD a = wcstoul(sep1 + 1, 0, 0);
			outValue = nvgRGBA((BYTE)rgb, (BYTE)rgb, (BYTE)rgb, (BYTE)a);
			return S_OK;
		}

		BSTR sep3 = wcschr(sep2 + 1, L',');

		if (!sep3)
		{
			// R, G, B

			DWORD r = wcstoul(bstrValue, 0, 0);
			DWORD g = wcstoul(sep1 + 1, 0, 0);
			DWORD b = wcstoul(sep2 + 1, 0, 0);
			outValue = nvgRGB((BYTE)r, (BYTE)g, (BYTE)b);
			return S_OK;
		}

		{
			// R, G, B, A

			DWORD r = wcstoul(bstrValue, 0, 0);
			DWORD g = wcstoul(sep1 + 1, 0, 0);
			DWORD b = wcstoul(sep2 + 1, 0, 0);
			DWORD a = wcstoul(sep3 + 1, 0, 0);
			outValue = nvgRGBA((BYTE)r, (BYTE)g, (BYTE)b, (BYTE)a);
			return S_OK;
		}
	}

	return S_OK;
}
#endif
template<class T, class S>
inline HRESULT WINAPI getPrivateProfilePercent(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, T& outValue, S& outValuePercent)
{
	_bstr_t value = L"";
	HRESULT hr = getPrivateProfileBSTR(fileName, appName, keyName, value);
	if (hr != S_OK) return hr;
	if (!(BSTR)value) return S_FALSE;
	if (::lstrlenW(value) == 0) return S_FALSE;

	if (::StrChrW(value, L'%'))
		outValuePercent = wcstol(value, 0, 0);
	else
		outValue = wcstol(value, 0, 0);

	return S_OK;
}

template<class T>
inline HRESULT WINAPI getPrivateProfileTime(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, T& outValue)
{
	_bstr_t value = L"";
	HRESULT hr = getPrivateProfileBSTR(fileName, appName, keyName, value);
	if (hr != S_OK) return hr;
	if (!(BSTR)value) return S_FALSE;
	if (::lstrlenW(value) == 0) return S_FALSE;

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

template<class T, class A>
inline HRESULT WINAPI getPrivateProfileLabel(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, T& outValue, const A& array)
{
	_bstr_t value = L"";
	HRESULT hr = getPrivateProfileBSTR(fileName, appName, keyName, value);
	if (hr != S_OK) return hr;
	if (!(BSTR)value) return S_FALSE;

	int c = _countof(array);
	for (int i = 0; i < c; i++)
	{
		if (::lstrcmpW(array[i].label, value) == 0)
		{
			MY_TRACE_WSTR(array[i].label);
			outValue = array[i].value;
			return S_OK;
		}
	}

	return S_FALSE;
}

inline HRESULT getPrivateProfileWindow(LPCWSTR fileName, LPCWSTR name, HWND hwnd, DWORD cmdShow = -1)
{
	WINDOWPLACEMENT wp = { sizeof(wp) };
	if (!::GetWindowPlacement(hwnd, &wp)) return S_FALSE;
	if (!::IsWindowVisible(hwnd)) wp.showCmd = SW_HIDE;
	wp.flags = WPF_SETMINPOSITION;

	if (cmdShow == -1)
		getPrivateProfileInt(fileName, name, L"flags", wp.showCmd);
	else
		wp.showCmd = cmdShow;

	getPrivateProfileInt(fileName, name, L"left", wp.rcNormalPosition.left);
	getPrivateProfileInt(fileName, name, L"top", wp.rcNormalPosition.top);
	getPrivateProfileInt(fileName, name, L"right", wp.rcNormalPosition.right);
	getPrivateProfileInt(fileName, name, L"bottom", wp.rcNormalPosition.bottom);

	getPrivateProfileInt(fileName, name, L"minX", wp.ptMinPosition.x);
	getPrivateProfileInt(fileName, name, L"minY", wp.ptMinPosition.y);
	getPrivateProfileInt(fileName, name, L"maxX", wp.ptMaxPosition.x);
	getPrivateProfileInt(fileName, name, L"maxY", wp.ptMaxPosition.y);

	if (!::SetWindowPlacement(hwnd, &wp))
		return S_FALSE;

	return S_OK;
}

//---------------------------------------------------------------------
// Set

template<class T>
inline HRESULT WINAPI setPrivateProfileBSTR(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, const T& value)
{
	::WritePrivateProfileStringW(appName, keyName, value, fileName);
	return S_OK;
}

template<class T>
inline HRESULT WINAPI setPrivateProfileInt(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, const T& value)
{
	WCHAR text[MAX_PATH] = {};
	::StringCbPrintfW(text, sizeof(text), L"%d", value);
	return setPrivateProfileBSTR(fileName, appName, keyName, text);
}

template<class T>
inline HRESULT WINAPI setPrivateProfileReal(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, const T& value)
{
	WCHAR text[MAX_PATH] = {};
	::StringCbPrintfW(text, sizeof(text), L"%f", value);
	return setPrivateProfileBSTR(fileName, appName, keyName, text);
}

template<class T>
inline HRESULT WINAPI setPrivateProfileBool(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, const T& value)
{
	return setPrivateProfileBSTR(fileName, appName, keyName, value ? L"YES" : L"NO");
}

inline HRESULT WINAPI setPrivateProfileColor(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, COLORREF value)
{
	BYTE r = GetRValue(value);
	BYTE g = GetGValue(value);
	BYTE b = GetBValue(value);
	WCHAR text[MAX_PATH] = {};
	::StringCbPrintfW(text, sizeof(text), L"#%02x%02x%02x", r, g, b);
	return setPrivateProfileBSTR(fileName, appName, keyName, text);
}

#ifdef _GDIPLUS_H
inline HRESULT WINAPI setPrivateProfileColor(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, const Gdiplus::Color& value)
{
	WCHAR text[MAX_PATH] = {};
	::StringCbPrintfW(text, sizeof(text), L"%08X", value.GetValue());
	return setPrivateProfileBSTR(fileName, appName, keyName, text);
}
#endif
#ifdef NANOVG_H
inline HRESULT WINAPI setPrivateProfileColor(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, const NVGcolor& value)
{
	BYTE r = (BYTE)roundf(value.r * 255.0f);
	BYTE g = (BYTE)roundf(value.g * 255.0f);
	BYTE b = (BYTE)roundf(value.b * 255.0f);
	BYTE a = (BYTE)roundf(value.a * 255.0f);
	WCHAR text[MAX_PATH] = {};
	::StringCbPrintfW(text, sizeof(text), L"%d, %d, %d, %d", r, g, b, a);
	return setPrivateProfileBSTR(fileName, appName, keyName, text);
}
#endif
template<class T, class A>
inline HRESULT WINAPI setPrivateProfileLabel(LPCWSTR fileName, LPCWSTR appName, LPCWSTR keyName, const T& value, const A& array)
{
	int c = _countof(array);
	for (int i = 0; i < c; i++)
	{
		if (array[i].value == value)
			return setPrivateProfileBSTR(fileName, appName, keyName, array[i].label);
	}

	return S_FALSE;
}

inline HRESULT setPrivateProfileWindow(LPCWSTR fileName, LPCWSTR name, HWND hwnd, DWORD cmdShow = -1)
{
	WINDOWPLACEMENT wp = { sizeof(wp) };
	if (!::GetWindowPlacement(hwnd, &wp)) return E_FAIL;

	if (::IsIconic(hwnd)) wp.showCmd = SW_SHOW;
	if (wp.flags == WPF_RESTORETOMAXIMIZED) wp.showCmd = SW_SHOWMAXIMIZED;
	if (!::IsWindowVisible(hwnd)) wp.showCmd = SW_HIDE;

	setPrivateProfileInt(fileName, name, L"flags", (cmdShow == -1) ? wp.showCmd : cmdShow);

	setPrivateProfileInt(fileName, name, L"left", wp.rcNormalPosition.left);
	setPrivateProfileInt(fileName, name, L"top", wp.rcNormalPosition.top);
	setPrivateProfileInt(fileName, name, L"right", wp.rcNormalPosition.right);
	setPrivateProfileInt(fileName, name, L"bottom", wp.rcNormalPosition.bottom);

	setPrivateProfileInt(fileName, name, L"minX", wp.ptMinPosition.x);
	setPrivateProfileInt(fileName, name, L"minY", wp.ptMinPosition.y);
	setPrivateProfileInt(fileName, name, L"maxX", wp.ptMaxPosition.x);
	setPrivateProfileInt(fileName, name, L"maxY", wp.ptMaxPosition.y);

	return S_OK;
}

//---------------------------------------------------------------------
