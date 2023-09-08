#pragma once

namespace fgo::filer
{
	//
	// このクラスはファイラウィンドウです。
	// ファイラダイアログを保持します。
	//
	struct FilerWindow : Tools::AviUtl::PluginWindow
	{
		//
		// コンストラクタです。
		//
		FilerWindow(LPCTSTR name, HWND clientWindow)
		{
			MY_TRACE(_T("FilerWindow::FilerWindow(%s, 0x%08X)\n"), name, clientWindow);

			if (!Tools::AviUtl::PluginWindowExtension::create(
				*this, hive.instance, magi.auin.GetAviUtlWindow(), name))
			{
				throw _T("ファイラウィンドウの作成に失敗しました");
			}

			// フィルタウィンドウの作成が完了したことをクライアントウィンドウに通知します。
			// クライアントウィンドウはこの通知に反応してファイラを作成します。
			::PostMessage(clientWindow, Share::Filer::Message::FilerWindowCreated, (WPARAM)(HWND)*this, 0);
		}

		//
		// デストラクタです。
		//
		~FilerWindow()
		{
			MY_TRACE(_T("FilerWindow::~FilerWindow()\n"));
		}

		//
		// 設定を読み込みます。
		//
		BOOL load(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE(_T("FilerWindow::load()\n"));

			getPrivateProfileWindow(element, L"placement", *this);

			::ShowWindow(*this, SW_SHOW);

			return TRUE;
		}

		//
		// ウィンドウ作成時のスタイルやウィンドウサイズを指定します。
		//
		BOOL create(DWORD exStyle, LPCTSTR className, LPCTSTR windowName, DWORD style,
			int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param) override
		{
			return __super::create(
				exStyle,
				className,
				windowName,
				style,
				100, 100, 800, 800,
				parent, menu, instance, param);
		}

		//
		// ウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch (message)
			{
			case Share::Filer::Message::FilerDialogCreated: // このメッセージはファイラダイアログの作成が完了したときに通知されます。
				{
					MY_TRACE(_T("WM_FILER_DIALOG_CREATED, 0x%08X, 0x%08X\n"), wParam, lParam);

					HWND filerDialog = (HWND)wParam;
					HWND browser = (HWND)lParam;

					// ファイラダイアログの作成が完了したので、ターゲットに指定し、リサイズします。
					Tools::AviUtl::WindowHolderExtension::setTarget(*this, filerDialog);

					break;
				}
			case Share::Filer::Message::GetUrl: // このメッセージはファイラダイアログで「取得」ボタンが押されたときに通知されます。
				{
					MY_TRACE(_T("WM_FILER_GET_URL, 0x%08X, 0x%08X\n"), wParam, lParam);

					HWND filerDialog = (HWND)wParam;
					HWND url = (HWND)lParam;

					break;
				}
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}
	};
}
