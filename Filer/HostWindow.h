#pragma once
#include "Hook.h"
#include "Gui.h"
#include "MainDialog.h"
#include "FilerWindow.h"

namespace fgo::filer
{
	//
	// このクラスはホストプロセスのメインウィンドウです。
	//
	struct HostWindow : Tools::AviUtl::PluginWindow, MainDialog::Listener
	{
		//
		// フックです。
		//
		std::shared_ptr<Hook> hook;

		//
		// GUIを担当するクライアントプロセスです。
		//
		std::shared_ptr<Gui> gui;

		//
		// メインダイアログです。
		//
		std::shared_ptr<MainDialog> mainDialog;

		//
		// コンストラクタです。
		//
		HostWindow()
		{
			MY_TRACE(_T("HostWindow::HostWindow()\n"));

			if (!Tools::AviUtl::PluginWindowExtension::create(
				*this,
				hive.instance,
				magi.auin.GetAviUtlWindow(),
				hive.AppName,
				0,
				WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				100, 100, 300, 300))
			{
				throw _T("ホストウィンドウの失敗しました");
			}

			hook = std::make_shared<Hook>();
			gui = std::make_shared<Gui>(*this);
			mainDialog = std::make_shared<MainDialog>(*this);

			windowHolderExtension->setTarget(*mainDialog);
			windowHolderExtension->resize(*this);
		}

		//
		// デストラクタです。
		//
		~HostWindow()
		{
			MY_TRACE(_T("HostWindow::~HostWindow()\n"));
		}

		//
		// クライアントウィンドウにメッセージをポストします。
		//
		template<class WP, class LP>
		inline BOOL postMessage(UINT message, WP wParam, LP lParam) {
			HWND clientWindow = Share::Filer::HostWindow::getClientWindow(*this);
			return ::PostMessage(clientWindow, message, (WPARAM)wParam, (LPARAM)lParam);
		}

		//
		// クライアントウィンドウにメッセージをセンドします。
		//
		template<class WP, class LP>
		inline LRESULT sendMessage(UINT message, WP wParam, LP lParam) {
			HWND clientWindow = Share::Filer::HostWindow::getClientWindow(*this);
			return ::SendMessage(clientWindow, message, (WPARAM)wParam, (LPARAM)lParam);
		}

		//
		// 指定された名前のファイラウィンドウを作成して返します。
		//
		std::shared_ptr<FilerWindow> createFilerWindow(LPCTSTR name, BOOL full)
		{
			MY_TRACE(_T("HostWindow::createFilerWindow(%s, %d)\n"), name, full);

			// フィルタウィンドウを作成し、コレクションに追加します。
			auto filerWindow = FilerWindow::createFilerWindow(name);

			// フィルタウィンドウの作成が完了したことをクライアントウィンドウに通知します。
			// クライアントウィンドウはこの通知に応答してファイラダイアログを作成します。
			postMessage(Share::Filer::Message::PostInitFilerWindow, (HWND)*filerWindow, full);

			return filerWindow;
		}

		//
		// メインダイアログの作成ボタンを押したときの処理です。
		//
		BOOL onCreateFiler() override
		{
			// まず、ユーザーにファイラ名を入力してもらいます。
			RenameDialog dialog(_T(""));
			if (dialog.doModal(*this) != IDOK) return FALSE;
			MY_TRACE_TSTR(dialog.newName);

			// ファイラを作成します。
			auto filerWindow = createFilerWindow(dialog.newName, TRUE);

			// ファイラを表示します。
			::ShowWindow(*filerWindow, SW_SHOW);

			return TRUE;
		}

		//
		// メインダイアログの削除ボタンを押したときの処理です。
		// 指定されたインデックスのファイラを破壊します。
		//
		BOOL onDestroyFiler(std::size_t index) override
		{
			MY_TRACE(_T("HostWindow::onDestroyFiler(%d)\n"), index);

			// インデックスがコレクションの範囲外の場合は何もしません。
			if (index >= FilerWindow::collection.size()) return FALSE;

			// 指定されたインデックスにあるファイラウィンドウを取得します。
			auto& filerWindow = FilerWindow::collection[index];

			// 現在のファイラ名を取得します。
			TCHAR name[MAX_PATH] = {};
			::GetWindowText(*filerWindow, name, std::size(name));
			MY_TRACE_TSTR(name);

			// メッセージテキストを構築します。
			TCHAR text[MAX_PATH] = {};
			::StringCchPrintf(text, std::size(text),
				_T("ファイラ「%s」を削除しますか？")
				_T("\n※ファイラを削除するとファイラに登録してあるブックマークも一緒に削除されます"), name);
			MY_TRACE_TSTR(text);

			// メッセージボックスを出してユーザーに確認してもらいます。
			if (::MessageBox(*this, text, hive.AppName, MB_YESNO | MB_ICONWARNING) != IDYES) return FALSE;

			// ファイラウィンドウを破壊します。
			// ウィンドウ破壊時の最終処理でコレクションから取り除かれ、thisポインタが消滅します。
			return FilerWindow::collection[index]->destroy();
		}

		//
		// メインダイアログの編集ボタンを押したときの処理です。
		// 指定されたインデックスのファイラをリネームします。
		//
		BOOL onEditFiler(std::size_t index) override
		{
			MY_TRACE(_T("HostWindow::onEditFiler(%d)\n"), index);

			// インデックスがコレクションの範囲外の場合は何もしません。
			if (index >= FilerWindow::collection.size()) return FALSE;

			// 指定されたインデックスにあるファイラウィンドウを取得します。
			auto& filerWindow = FilerWindow::collection[index];

			// 現在のファイラ名を取得します。
			TCHAR name[MAX_PATH] = {};
			::GetWindowText(*filerWindow, name, std::size(name));
			MY_TRACE_TSTR(name);

			// ダイアログを表示してユーザーにファイラ名を入力してもらいます。
			RenameDialog dialog(name);
			if (dialog.doModal(*this) != IDOK) return FALSE;
			MY_TRACE_TSTR(dialog.newName);

			// ファイラをリネームします。
			filerWindow->updateWindowName(dialog.newName);

			// メインダイアログのファイラ名も変更します。
			mainDialog->setFilerName(index, dialog.newName);

			// Nestが存在する場合はシャトル名もリネームします。
			if (hive.nest.shuttle)
				return hive.nest.shuttle->rename(*filerWindow, (_bstr_t)dialog.newName);

			return TRUE;
		}

		//
		// 設定を読み込みます。
		//
		HRESULT load(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE(_T("HostWindow::load()\n"));

			getPrivateProfileBool(element, L"useCommonDialog", hive.useCommonDialog);
			getPrivateProfileWindow(element, L"placement", *this, SW_SHOW);

			MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"filer");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

				_bstr_t name;
				getPrivateProfileString(element, L"name", name);
				MY_TRACE_WSTR((BSTR)name);
				if (!(BSTR)name) continue;

				try
				{
					auto filerWindow = createFilerWindow(name, FALSE);

					filerWindow->load(element);
				}
				catch (LPCTSTR e)
				{
					::MessageBox(hive.mainWindow, e, hive.AppName, MB_OK);
				}
			}

			return S_OK;
		}

		//
		// 設定を保存します。
		//
		HRESULT save(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE(_T("HostWindow::save()\n"));

			setPrivateProfileBool(element, L"useCommonDialog", hive.useCommonDialog);
			setPrivateProfileWindow(element, L"placement", *this);

			for (auto& filerWindow : FilerWindow::collection)
			{
				MSXML2::IXMLDOMElementPtr filerElement = appendElement(element, L"filer");

				TCHAR name[MAX_PATH] = {};
				::GetWindowText(*filerWindow, name, std::size(name));
				setPrivateProfileString(filerElement, L"name", name);

				filerWindow->save(filerElement);
			}

			return S_OK;
		}

		//
		// ウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			return __super::onWndProc(hwnd, message, wParam, lParam);
		}

		//
		// このクラスはユーザーにファイラ名を入力してもらうためのダイアログです。
		//
		struct RenameDialog : Tools::Dialog2
		{
			//
			// ファイラの新しい名前です。
			//
			TCHAR newName[MAX_PATH] = {};

			//
			// コンストラクタです。
			//
			RenameDialog(LPCTSTR name)
			{
				::StringCchCopy(newName, std::size(newName), name);
			}

			//
			// ダイアログをモーダル表示します。
			//
			int doModal(HWND parent)
			{
				create(hive.instance, MAKEINTRESOURCE(IDD_GET_FILER_NAME), parent);

				::SetDlgItemText(*this, IDC_NEW_NAME, newName);

				int retValue = __super::doModal2(parent);

				if (IDOK != retValue)
					return retValue;

				::GetDlgItemText(*this, IDC_NEW_NAME, newName, std::size(newName));

				return retValue;
			}

			//
			// 指定されたファイラ名が使用可能な場合はTRUEを返します。
			//
			BOOL isEmptyName(LPCTSTR newName)
			{
				if (_tcslen(newName) == 0) return FALSE;

				if (hive.nest.shuttle)
				{
					return hive.nest.shuttle->is_empty_name((_bstr_t)newName);
				}
				else
				{
					auto it = std::find_if(FilerWindow::collection.begin(), FilerWindow::collection.end(),
						[newName](const auto& x) {
							TCHAR name[MAX_PATH] = {};
							::GetWindowText(*x, name, std::size(name));
							return _tcscmp(name, newName) == 0;
						});

					return it == FilerWindow::collection.end();
				}
			}

			//
			// OKボタンが押されたときの処理です。
			// ユーザーが入力した名前が無効の場合はダイアログを表示して拒否します。
			//
			void onOK() override
			{
				::GetDlgItemText(*this, IDC_NEW_NAME, newName, std::size(newName));

				// 新しい名前と同じ名前のファイラがすでに存在する場合は
				if (!isEmptyName(newName))
				{
					// メッセージボックスを表示します。
					::MessageBox(*this, _T("名前が重複しています"), hive.AppName, MB_OK | MB_ICONWARNING);

					return;
				}

				__super::onOK();
			}

			//
			// キャンセルボタンが押されたときの処理です。
			//
			void onCancel() override
			{
				__super::onCancel();
			}
		};
	};
}
