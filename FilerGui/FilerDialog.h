#pragma once
#include "resource.h"
#include "Hive.h"
#include "ExplorerDialog.h"

class FilerDialog : public ExplorerDialog
{
public:

	inline static std::vector<std::shared_ptr<FilerDialog>> collection;

	HWND filerWindow = 0;
	CToolTipCtrl tooltip;

public:

	//
	// コンストラクタです。
	// 内部的に使用されます。create()から呼び出されます。
	//
	FilerDialog(LPCTSTR name, HWND filerWindow, BOOL full)
		: filerWindow(filerWindow)
	{
		MY_TRACE(_T("FilerDialog::FilerDialog(%s, 0x%08X)\n"), name, filerWindow);

		if (!__super::Create(IDD_FILER_DIALOG, AfxGetMainWnd()))
			throw _T("ファイラダイアログの作成に失敗しました");

		// ウィンドウ名を設定します。
		SetWindowText(name);

		if (full)
		{
			// エクスプローラを作成します。
			initExplorer();
		}

		// ファイラダイアログの作成が完了したことをファイラウィンドウに通知します。
		// これにより、ファイラウィンドウがファイラダイアログを参照することができるようになります。
		fire_PostInitFilerDialog();
	}

	//
	// デストラクタです。
	//
	~FilerDialog()
	{
		MY_TRACE(_T("FilerDialog::~FilerDialog()\n"));
	}

	//
	// ファイラウィンドウに初期化が完了したことを通知します。
	//
	void fire_PostInitFilerDialog()
	{
		::PostMessage(filerWindow, Share::Filer::Message::PostInitFilerDialog, (WPARAM)GetSafeHwnd(), 0);
	}

	//
	// ファイラウィンドウにファイル名を取得する必要があることを通知します。
	//
	void fire_GetFileName()
	{
		::PostMessage(filerWindow, Share::Filer::Message::GetFileName, (WPARAM)GetSafeHwnd(), (LPARAM)folder.GetSafeHwnd());
	}

	//
	// ファイラダイアログを作成して返します。
	//
	static std::shared_ptr<FilerDialog> create(HWND filerWindow, BOOL full)
	{
		MY_TRACE(_T("FilerDialog::create(0x%08X, %d)\n"), filerWindow, full);

		CString name; CWnd::FromHandle(filerWindow)->GetWindowText(name);

		return collection.emplace_back(std::make_shared<FilerDialog>(name, filerWindow, full));
	}

	//
	// 設定を読み込みます。
	//
	HRESULT load(const MSXML2::IXMLDOMElementPtr& element)
	{
		MY_TRACE(_T("FilerDialog::load()\n"));

		folder.ResetContent();

		BOOL hasNavPane = TRUE;
		getPrivateProfileBool(element, L"hasNavPane", hasNavPane);
		this->hasNavPane.SetCheck(hasNavPane ? BST_CHECKED : BST_UNCHECKED);

		_bstr_t path;
		getPrivateProfileString(element, L"path", path);
		MY_TRACE_WSTR((BSTR)path);
		if ((BSTR)path) folder.SetWindowText(path);

		{
			MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"bookmark");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

				_bstr_t path;
				getPrivateProfileString(element, L"path", path);
				MY_TRACE_WSTR((BSTR)path);
				if ((BSTR)path) folder.AddString(path);
			}
		}

		exitExplorer();
		initExplorer();

		return S_OK;
	}

	//
	// 設定を保存します。
	//
	HRESULT save(const MSXML2::IXMLDOMElementPtr& element)
	{
		MY_TRACE(_T("FilerDialog::save()\n"));

		BOOL hasNavPane = this->hasNavPane.GetCheck() == BST_CHECKED;
		setPrivateProfileBool(element, L"hasNavPane", hasNavPane);
		setPrivateProfileString(element, L"path", (LPCTSTR)hotFolder);

		// <bookmark>をすべて削除します。
		removeChildren(element, L"bookmark");

		int c = folder.GetCount();
		for (int i = 0; i < c; i++)
		{
			// <bookmark>を作成します。
			MSXML2::IXMLDOMElementPtr bookmarkElement = appendElement(element, L"bookmark");

			CString path; folder.GetLBText(i, path);
			setPrivateProfileString(bookmarkElement, L"path", (LPCTSTR)path);
		}

		return S_OK;
	}

	//
	// エクスプローラを作成します。
	//
	void initExplorer()
	{
		MY_TRACE(_T("FilerDialog::initExplorer() 開始\n"));

		__super::initExplorer(GetDlgItem(IDC_PLACE_HOLDER));

		MY_TRACE(_T("フィルタウィンドウにブラウザのウィンドウハンドルをセットします\n"));

		IShellBrowserPtr browser = explorer;
		MY_TRACE_HEX(browser.GetInterfacePtr());
		HWND hwnd = 0; browser->GetWindow(&hwnd);
		MY_TRACE_HEX(hwnd);
		Share::Filer::FilerWindow::setBrowser(filerWindow, hwnd);

		MY_TRACE(_T("FilerDialog::initExplorer() 完了\n"));
	}

	//
	// エクスプローラを削除します。
	//
	void exitExplorer()
	{
		MY_TRACE(_T("FilerDialog::exitExplorer() 開始\n"));

		__super::exitExplorer();

		MY_TRACE(_T("FilerDialog::exitExplorer() 完了\n"));
	}

	//
	// エクスプローラをリサイズします。
	//
	void resizeExplorer()
	{
		MY_TRACE(_T("FilerDialog::resizeExplorer() 開始\n"));

		__super::resizeExplorer(GetDlgItem(IDC_PLACE_HOLDER));

		MY_TRACE(_T("FilerDialog::resizeExplorer() 完了\n"));
	}

	//
	// 指定されたコントロールに指定されたツールチップテキストを付与します。
	//
	BOOL addTool(UINT id, LPCTSTR text)
	{
		TOOLINFO ti { sizeof(ti) };
		ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS | TTF_TRANSPARENT;
		ti.hwnd = GetSafeHwnd();
		ti.uId = (UINT)GetDlgItem(id)->GetSafeHwnd();
		ti.lpszText = const_cast<LPTSTR>(text);
		return tooltip.SendMessage(TTM_ADDTOOL, 0, (LPARAM)&ti);
	}

protected:

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILER_DIALOG };
#endif
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	virtual void PostNcDestroy() override;
	virtual BOOL OnInitDialog() override;
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) override;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnAppCommand(CWnd* pWnd, UINT nCmd, UINT nDevice, UINT nKey);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnPrevFolder();
	afx_msg void OnNextFolder();
	afx_msg void OnParentFolder();
	afx_msg void OnGetFileName();
	afx_msg void OnAddBookmark();
	afx_msg void OnRemoveBookmark();
	afx_msg void OnHasNavPane();
	afx_msg void OnSelChangeFolder();
	afx_msg LRESULT OnPostExitFilerWindow(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};
