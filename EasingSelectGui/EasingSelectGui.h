#pragma once
#include "Resource.h"

const UINT_PTR TIMER_ID = 1;

inline HRESULT WINAPI getPrivateProfileColor(
	const MSXML2::IXMLDOMElementPtr& element, Color& outValue)
{
	BYTE A = outValue.GetA();
	BYTE R = outValue.GetR();
	BYTE G = outValue.GetG();
	BYTE B = outValue.GetB();
	getPrivateProfileInt(element, L"A", A);
	getPrivateProfileInt(element, L"R", R);
	getPrivateProfileInt(element, L"G", G);
	getPrivateProfileInt(element, L"B", B);
	outValue = Color(A, R, G, B);

	return S_OK;
}

inline HRESULT WINAPI setPrivateProfileColor(
	const MSXML2::IXMLDOMElementPtr& element, const Color& value)
{
	setPrivateProfileInt(element, L"A", value.GetA());
	setPrivateProfileInt(element, L"R", value.GetR());
	setPrivateProfileInt(element, L"G", value.GetG());
	setPrivateProfileInt(element, L"B", value.GetB());

	return S_OK;
}

inline class CEasingSelectGuiApp : public CWinApp
{
public:

	HWND m_mainProcessWindow;
	DWORD m_mainProcessId;

public:

	CEasingSelectGuiApp() noexcept;
	virtual ~CEasingSelectGuiApp();

	CString getFullPath(LPCTSTR fileSpec)
	{
		TCHAR temp[MAX_PATH] = {};
		::GetModuleFileName(AfxGetInstanceHandle(), temp, std::size(temp));
		::PathAppend(temp, _T("..\\..\\UltimateConfig"));
		MY_TRACE_TSTR(temp);

		TCHAR fileName[MAX_PATH] = {};
		::PathCanonicalize(fileName, temp);
		::PathAppend(fileName, fileSpec);
		MY_TRACE_TSTR(fileName);

		return fileName;
	}

	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();

	afx_msg void OnSelectEasingNotify(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
} theApp;
