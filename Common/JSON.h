#pragma once

//---------------------------------------------------------------------

inline std::string utf16_to_utf8(LPCWSTR utf16)
{
	int bufferSize = ::WideCharToMultiByte(CP_UTF8, 0, utf16, -1, 0, 0, 0, 0);

	if (bufferSize == 0)
		return "";

	std::string text(bufferSize - 1, '\0');
	::WideCharToMultiByte(CP_UTF8, 0, utf16, -1, text.data(), bufferSize, 0, 0);

	return text;
}

inline std::wstring utf8_to_utf16(LPCSTR utf8)
{
	int bufferSize = ::MultiByteToWideChar(CP_UTF8, 0, utf8, -1, 0, 0);

	if (bufferSize == 0)
		return L"";

	std::wstring text(bufferSize - 1, L'\0');
	::MultiByteToWideChar(CP_UTF8, 0, utf8, -1, text.data(), bufferSize);

	return text;
}

//---------------------------------------------------------------------
// Get

template<class T>
inline BOOL getPrivateProfileInt(const nlohmann::json& j, LPCSTR name, T& outValue)
{
	try
	{
		j.at(name).get_to(outValue);

		return TRUE;
	}
	catch (...)
	{
		return FALSE;
	}
}

inline BOOL getPrivateProfileWindow(const nlohmann::json& _j, LPCSTR name, HWND hwnd)
{
	try
	{
		nlohmann::json j = _j.at(name);

		WINDOWPLACEMENT wp = { sizeof(wp) };
		if (!::GetWindowPlacement(hwnd, &wp)) return FALSE;

		if (!::IsWindowVisible(hwnd)) wp.showCmd = SW_HIDE;

		wp.flags = WPF_SETMINPOSITION;

		getPrivateProfileInt(j, "showCmd", wp.showCmd);

		getPrivateProfileInt(j, "left", wp.rcNormalPosition.left);
		getPrivateProfileInt(j, "top", wp.rcNormalPosition.top);
		getPrivateProfileInt(j, "right", wp.rcNormalPosition.right);
		getPrivateProfileInt(j, "bottom", wp.rcNormalPosition.bottom);

		getPrivateProfileInt(j, "minX", wp.ptMinPosition.x);
		getPrivateProfileInt(j, "minY", wp.ptMinPosition.y);
		getPrivateProfileInt(j, "maxX", wp.ptMaxPosition.x);
		getPrivateProfileInt(j, "maxY", wp.ptMaxPosition.y);

		if (!::SetWindowPlacement(hwnd, &wp))
			return FALSE;

		return TRUE;
	}
	catch (...)
	{
		return FALSE;
	}
}

//---------------------------------------------------------------------
// Set

template<class T>
inline BOOL setPrivateProfileInt(nlohmann::json& j, LPCSTR name, const T& outValue)
{
	j[name] = outValue;

	return TRUE;
}

inline BOOL setPrivateProfileWindow(nlohmann::json& _j, LPCSTR name, HWND hwnd)
{
	WINDOWPLACEMENT wp = { sizeof(wp) };
	if (!::GetWindowPlacement(hwnd, &wp)) return FALSE;

	if (::IsIconic(hwnd)) wp.showCmd = SW_SHOW;
	if (wp.flags == WPF_RESTORETOMAXIMIZED) wp.showCmd = SW_SHOWMAXIMIZED;
	if (!::IsWindowVisible(hwnd)) wp.showCmd = SW_HIDE;

	nlohmann::json& j = _j[name];

	j["showCmd"] = wp.showCmd;

	j["left"] = wp.rcNormalPosition.left;
	j["top"] = wp.rcNormalPosition.top;
	j["right"] = wp.rcNormalPosition.right;
	j["bottom"] = wp.rcNormalPosition.bottom;

	j["minX"] = wp.ptMinPosition.x;
	j["minY"] = wp.ptMinPosition.y;
	j["maxX"] = wp.ptMaxPosition.x;
	j["maxY"] = wp.ptMaxPosition.y;

	return TRUE;
}

//---------------------------------------------------------------------
