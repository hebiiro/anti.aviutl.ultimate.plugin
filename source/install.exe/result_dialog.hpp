#pragma once

class ResultDialog : public CDialogEx
{
public:

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RESULT_DIALOG };
#endif

	CEdit result;

	//
	// 標準コンストラクタです。
	//
	ResultDialog(CWnd* parent = nullptr)
		: CDialogEx(IDD_RESULT_DIALOG, parent)
	{
	}

	//
	// デストラクタです。
	//
	virtual ~ResultDialog() override
	{
		// メモリリーク対策の処理です。
		CMFCVisualManager::DestroyInstance();
	}

	//
	// メンバ変数をコントロールにバインドします。
	//
	virtual void DoDataExchange(CDataExchange* dx) override
	{
		__super::DoDataExchange(dx);

		DDX_Control(dx, IDC_RESULT, result);
	}

	//
	// ダイアログを初期化します。
	//
	virtual BOOL OnInitDialog() override
	{
		__super::OnInitDialog();

		std::wstring str;

		for (const auto& result : hive.results)
		{
			std::format_to(std::back_inserter(str), L"{} {} => {}" L"\r\n",
				result.succeeded ? L"(成功)" : L"(失敗)", result.from, result.to);
		}

		result.SetWindowText(str.c_str());

		UpdateData(FALSE);

		return TRUE;
	}

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(ResultDialog, CDialogEx)
END_MESSAGE_MAP()
