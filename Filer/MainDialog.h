#pragma once
#include "Resource.h"
#include "Hive.h"
#include "FilerWindow.h"

namespace fgo::filer
{
	//
	// このクラスはホストプロセスのメインダイアログです。
	//
	struct MainDialog : Tools::Dialog, FilerWindow::Listener
	{
		struct Listener {
			virtual BOOL onCreateFiler() = 0;
			virtual BOOL onDestroyFiler(size_t index) = 0;
			virtual BOOL onEditFiler(size_t index) = 0;
		};

		//
		// メインダイアログのリスナーです。
		//
		inline static std::weak_ptr<Listener> listener;

		BOOL lock = FALSE;

		//
		// コンストラクタです。
		//
		MainDialog(HWND parent)
		{
			MY_TRACE_FUNC("0x%08X", parent);

			if (!create(hive.instance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), parent))
			{
				throw _T("メインダイアログの失敗しました");
			}
		}

		//
		// デストラクタです。
		//
		~MainDialog()
		{
			MY_TRACE_FUNC("");
		}

		//
		// ロック状態を返します。
		//
		BOOL getLock() const
		{
			return lock;
		}

		//
		// TRUEを指定するとコントロールがリフレッシュされなくなります。
		// FALSEを指定するとコントロールがリフレッシュされるようになります。
		//
		void setLock(BOOL newLock)
		{
			lock = newLock;
		}

		//
		// 指定されたコントロールを有効または無効にします。
		//
		BOOL enableControl(UINT id, BOOL enable)
		{
			return ::EnableWindow(::GetDlgItem(*this, id), enable);
		}

		//
		// すべてのコントロールをリフレッシュします。
		//
		BOOL refresh()
		{
			MY_TRACE_FUNC("");

			refreshListBox();
			refreshButton();
			refreshCheckBox();

			return TRUE;
		}

		//
		// リストボックスをリフレッシュします。
		//
		BOOL refreshListBox()
		{
			MY_TRACE_FUNC("");

			if (lock) return FALSE;

			HWND list = ::GetDlgItem(*this, IDC_FILER_LIST);
			::SendMessage(list, WM_SETREDRAW, FALSE, 0);
			::SendMessage(list, LB_RESETCONTENT, 0, 0);
			for (auto& filerWindow : FilerWindow::collection)
			{
				TCHAR name[MAX_PATH] = {};
				::GetWindowText(*filerWindow, name, std::size(name));
				::SendMessage(list, LB_ADDSTRING, 0, (LPARAM)name);
			}
			::SendMessage(list, WM_SETREDRAW, TRUE, 0);

			return TRUE;
		}

		//
		// ボタンをリフレッシュします。
		//
		BOOL refreshButton()
		{
			MY_TRACE_FUNC("");

			if (lock) return FALSE;

			size_t index = (size_t)::SendDlgItemMessage(*this, IDC_FILER_LIST, LB_GETCURSEL, 0, 0);
			if (index >= (size_t)::SendDlgItemMessage(*this, IDC_FILER_LIST, LB_GETCOUNT, 0, 0))
			{
				enableControl(IDC_CREATE_FILER, TRUE);
				enableControl(IDC_DESTROY_FILER, FALSE);
				enableControl(IDC_EDIT_FILER, FALSE);
			}
			else
			{
				enableControl(IDC_CREATE_FILER, TRUE);
				enableControl(IDC_DESTROY_FILER, TRUE);
				enableControl(IDC_EDIT_FILER, TRUE);
			}

			return TRUE;
		}

		//
		// チェックボックスをリフレッシュします。
		//
		BOOL refreshCheckBox()
		{
			MY_TRACE_FUNC("");

			if (lock) return FALSE;

			::SendDlgItemMessage(*this, IDC_USE_COMMON_DIALOG, BM_SETCHECK,
				hive.useCommonDialog ? BST_CHECKED : BST_UNCHECKED, 0);

			return TRUE;
		}

		//
		// リストボックス内にある指定されたインデックスにあるファイラ名を変更します。
		//
		BOOL setFilerName(size_t index, LPCTSTR newName)
		{
			MY_TRACE_FUNC("%d, %s", index, newName);

			if (lock) return FALSE;

			HWND list = ::GetDlgItem(*this, IDC_FILER_LIST);
			::SendMessage(list, LB_DELETESTRING, index, 0); // 一旦テキストを削除します。
			::SendMessage(list, LB_INSERTSTRING, index, (LPARAM)newName); // 新しいテキストを挿入します。

			return TRUE;
		}

		//
		// 選択されているファイラを表示状態を切り替えます。
		//
		BOOL showFiler()
		{
			MY_TRACE_FUNC("");

			size_t index = (size_t)::SendDlgItemMessage(*this, IDC_FILER_LIST, LB_GETCURSEL, 0, 0);
			if (index >= FilerWindow::collection.size()) return FALSE;

			::SendMessage(*FilerWindow::collection[index], WM_CLOSE, 0, 0);

			return TRUE;
		}

		//
		// ファイラウィンドウの初期化時に通知されます。
		//
		BOOL onInitFilerWindow(FilerWindow* filerWindow) override
		{
			MY_TRACE_FUNC("0x%08X", filerWindow);

			return refresh();
		}

		//
		// ファイラウィンドウの後始末時に通知されます。
		//
		BOOL onExitFilerWindow(FilerWindow* filerWindow) override
		{
			MY_TRACE_FUNC("0x%08X", filerWindow);

			return refresh();
		}

		//
		// コントロールの位置を更新します。
		//
		void onSize()
		{
			MY_TRACE_FUNC("");

			int base = getBaseSize();
			MY_TRACE_INT(base);

			HWND list = ::GetDlgItem(*this, IDC_FILER_LIST);
			HWND button[] = {
				::GetDlgItem(*this, IDC_CREATE_FILER),
				::GetDlgItem(*this, IDC_DESTROY_FILER),
				::GetDlgItem(*this, IDC_EDIT_FILER),
			};
			HWND checkbox[] = {
				::GetDlgItem(*this, IDC_USE_COMMON_DIALOG),
			};

			RECT rc; ::GetClientRect(*this, &rc);
			int cx = rc.left;
			int cy = rc.top;
			int cw = getWidth(rc);
			int ch = getHeight(rc);
			int bx[] = {
				::MulDiv(cw, 0, 3),
				::MulDiv(cw, 1, 3),
				::MulDiv(cw, 2, 3),
				::MulDiv(cw, 3, 3),
			};

			HDWP dwp = ::BeginDeferWindowPos(5);
			::DeferWindowPos(dwp, list, 0, cx, cy, cw, ch - base * 2, SWP_NOZORDER | SWP_NOACTIVATE);
			for (size_t i = 0; i < std::size(button); i++) {
				::DeferWindowPos(dwp, button[i], 0, bx[i], ch - base * 2, bx[i + 1] - bx[i], base, SWP_NOZORDER | SWP_NOACTIVATE);
			}
			::DeferWindowPos(dwp, checkbox[0], 0, cx, ch - base * 1, cw, base, SWP_NOZORDER | SWP_NOACTIVATE);
			::EndDeferWindowPos(dwp);
		}

		//
		// ファイラウィンドウを作成します。
		//
		BOOL onCreateFiler()
		{
			MY_TRACE_FUNC("");

			if (auto listener = this->listener.lock())
				return listener->onCreateFiler();
			else
				return FALSE;
		}

		//
		// ファイラウィンドウを削除します。
		//
		BOOL onDestroyFiler()
		{
			MY_TRACE_FUNC("");

			size_t index = (size_t)::SendDlgItemMessage(*this, IDC_FILER_LIST, LB_GETCURSEL, 0, 0);

			if (auto listener = this->listener.lock())
				return listener->onDestroyFiler(index);
			else
				return FALSE;
		}

		//
		// ファイラウィンドウを編集(リネーム)します。
		//
		BOOL onEditFiler()
		{
			MY_TRACE_FUNC("");

			size_t index = (size_t)::SendDlgItemMessage(*this, IDC_FILER_LIST, LB_GETCURSEL, 0, 0);

			if (auto listener = this->listener.lock())
				return listener->onEditFiler(index);
			else
				return FALSE;
		}

		//
		// ウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			return __super::onWndProc(hwnd, message, wParam, lParam);
		}

		//
		// ダイアログプロシージャです。
		//
		INT_PTR onDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_SIZE: onSize(); break;
			case WM_COMMAND:
				{
					UINT code = HIWORD(wParam);
					UINT id = LOWORD(wParam);
					HWND control = (HWND)lParam;

					switch (id)
					{
					case IDOK:
					case IDCANCEL: return 0;
					case IDC_CREATE_FILER: onCreateFiler(); break;
					case IDC_DESTROY_FILER: onDestroyFiler(); break;
					case IDC_EDIT_FILER: onEditFiler(); break;
					case IDC_USE_COMMON_DIALOG:
						{
							hive.useCommonDialog = ::SendDlgItemMessage(*this,
								IDC_USE_COMMON_DIALOG, BM_GETCHECK, 0, 0) == BST_CHECKED;
							MY_TRACE_INT(hive.useCommonDialog);

							break;
						}
					case IDC_FILER_LIST:
						{
							switch (code)
							{
							case LBN_DBLCLK:
								{
									showFiler();

									break;
								}
							case LBN_SELCHANGE:
								{
									refreshButton();

									break;
								}
							}

							break;
						}
					}

					break;
				}
			}

			return __super::onDlgProc(hwnd, message, wParam, lParam);
		}
	};
}
