#pragma once

inline class InstallApp : public CWinApp
{
public:

	virtual BOOL InitInstance() override
	{
		INITCOMMONCONTROLSEX icc = { sizeof(icc) };
		icc.dwICC = ICC_WIN95_CLASSES;
		::InitCommonControlsEx(&icc);

		__super::InitInstance();

		SetRegistryKey(_T("aviutl_ultimate_plugin"));

		InstallDialog dialog; m_pMainWnd = &dialog;
		if (IDOK != dialog.DoModal()) return FALSE;

		return FALSE;
	}

	DECLARE_MESSAGE_MAP()
} theApp;

BEGIN_MESSAGE_MAP(InstallApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()
