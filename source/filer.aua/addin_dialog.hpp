#pragma once

namespace apn::filer
{
	//
	// このクラスはホストプロセスのメインダイアログです。
	//
	inline struct AddinDialog : my::Dialog
	{
		//
		// TRUEを指定するとコントロールがリフレッシュされなくなります。
		// FALSEを指定するとコントロールがリフレッシュされるようになります。
		//
		BOOL locked = FALSE;

		//
		// 初期化処理を実行します。
		//
		BOOL init(HWND parent)
		{
			MY_TRACE_FUNC("{:#010x}", parent);

			return __super::create(hive.instance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), parent);
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// 指定されたコントロールを有効または無効にします。
		//
		BOOL enable_control(UINT id, BOOL enable)
		{
			return ::EnableWindow(::GetDlgItem(*this, id), enable);
		}

		//
		// すべてのコントロールをリフレッシュします。
		//
		BOOL refresh()
		{
			MY_TRACE_FUNC("");

			refresh_listbox();
			refresh_button();
			refresh_checkbox();

			return TRUE;
		}

		//
		// リストボックスをリフレッシュします。
		//
		BOOL refresh_listbox()
		{
			MY_TRACE_FUNC("");

			if (locked) return FALSE;

			auto list = ::GetDlgItem(*this, IDC_FILER_LIST);
			::SendMessage(list, WM_SETREDRAW, FALSE, 0);
			::SendMessage(list, LB_RESETCONTENT, 0, 0);
			for (auto& filer_window : FilerWindow::collection)
			{
				auto name = my::get_window_text(*filer_window);

				::SendMessage(list, LB_ADDSTRING, 0, (LPARAM)name.c_str());
			}
			::SendMessage(list, WM_SETREDRAW, TRUE, 0);

			return TRUE;
		}

		//
		// ボタンをリフレッシュします。
		//
		BOOL refresh_button()
		{
			MY_TRACE_FUNC("");

			if (locked) return FALSE;

			auto sel = (size_t)::SendDlgItemMessage(*this, IDC_FILER_LIST, LB_GETCURSEL, 0, 0);
			auto c = (size_t)::SendDlgItemMessage(*this, IDC_FILER_LIST, LB_GETCOUNT, 0, 0);
			if (sel < c)
			{
				enable_control(IDC_CREATE_FILER, TRUE);
				enable_control(IDC_DESTROY_FILER, TRUE);
				enable_control(IDC_EDIT_FILER, TRUE);
			}
			else
			{
				enable_control(IDC_CREATE_FILER, TRUE);
				enable_control(IDC_DESTROY_FILER, FALSE);
				enable_control(IDC_EDIT_FILER, FALSE);
			}

			return TRUE;
		}

		//
		// チェックボックスをリフレッシュします。
		//
		BOOL refresh_checkbox()
		{
			MY_TRACE_FUNC("");

			if (locked) return FALSE;

			::SendDlgItemMessage(*this, IDC_USE_COMMON_DIALOG, BM_SETCHECK,
				hive.use_common_dialog ? BST_CHECKED : BST_UNCHECKED, 0);

			return TRUE;
		}

		//
		// リストボックス内にある指定されたインデックスにあるファイラ名を変更します。
		//
		BOOL set_filer_name(size_t index, LPCTSTR new_name)
		{
			MY_TRACE_FUNC("{}, {}", index, new_name);

			if (locked) return FALSE;

			auto list = ::GetDlgItem(*this, IDC_FILER_LIST);
			::SendMessage(list, LB_DELETESTRING, index, 0); // 一旦テキストを削除します。
			::SendMessage(list, LB_INSERTSTRING, index, (LPARAM)new_name); // 新しいテキストを挿入します。

			return TRUE;
		}

		//
		// 選択されているファイラを表示状態を切り替えます。
		//
		BOOL show_filer()
		{
			MY_TRACE_FUNC("");

			auto index = (size_t)::SendDlgItemMessage(*this, IDC_FILER_LIST, LB_GETCURSEL, 0, 0);
			if (index >= FilerWindow::collection.size()) return FALSE;

			::SendMessage(*FilerWindow::collection[index], WM_CLOSE, 0, 0);

			return TRUE;
		}

		//
		// コントロールの位置を更新します。
		//
		void on_size()
		{
			MY_TRACE_FUNC("");

			auto base = get_base_size();
			MY_TRACE_INT(base);

			auto list = ::GetDlgItem(*this, IDC_FILER_LIST);
			HWND button[] = {
				::GetDlgItem(*this, IDC_CREATE_FILER),
				::GetDlgItem(*this, IDC_DESTROY_FILER),
				::GetDlgItem(*this, IDC_EDIT_FILER),
			};
			HWND checkbox[] = {
				::GetDlgItem(*this, IDC_USE_COMMON_DIALOG),
			};

			auto rc = my::get_client_rect(*this);
			auto cx = rc.left;
			auto cy = rc.top;
			auto cw = my::get_width(rc);
			auto ch = my::get_height(rc);
			int bx[] = {
				::MulDiv(cw, 0, 3),
				::MulDiv(cw, 1, 3),
				::MulDiv(cw, 2, 3),
				::MulDiv(cw, 3, 3),
			};

			auto dwp = ::BeginDeferWindowPos(5);
			::DeferWindowPos(dwp, list, nullptr, cx, cy, cw, ch - base * 2, SWP_NOZORDER | SWP_NOACTIVATE);
			for (size_t i = 0; i < std::size(button); i++) {
				::DeferWindowPos(dwp, button[i], nullptr, bx[i], ch - base * 2, bx[i + 1] - bx[i], base, SWP_NOZORDER | SWP_NOACTIVATE);
			}
			::DeferWindowPos(dwp, checkbox[0], nullptr, cx, ch - base * 1, cw, base, SWP_NOZORDER | SWP_NOACTIVATE);
			::EndDeferWindowPos(dwp);
		}

		//
		// ファイラウィンドウを作成します。
		//
		BOOL on_create_filer()
		{
			MY_TRACE_FUNC("");

			return hive.app->on_create_filer();
		}

		//
		// ファイラウィンドウを削除します。
		//
		BOOL on_destroy_filer()
		{
			MY_TRACE_FUNC("");

			auto index = (size_t)::SendDlgItemMessage(*this, IDC_FILER_LIST, LB_GETCURSEL, 0, 0);

			return hive.app->on_destroy_filer(index);
		}

		//
		// ファイラウィンドウを編集(リネーム)します。
		//
		BOOL on_edit_filer()
		{
			MY_TRACE_FUNC("");

			auto index = (size_t)::SendDlgItemMessage(*this, IDC_FILER_LIST, LB_GETCURSEL, 0, 0);

			return hive.app->on_edit_filer(index);
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		//
		// ダイアログプロシージャです。
		//
		virtual INT_PTR on_dlg_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_SIZE: on_size(); break;
			case WM_COMMAND:
				{
					auto code = HIWORD(wParam);
					auto id = LOWORD(wParam);
					auto control = (HWND)lParam;

					switch (id)
					{
					case IDOK:
					case IDCANCEL: return 0;
					case IDC_CREATE_FILER: on_create_filer(); break;
					case IDC_DESTROY_FILER: on_destroy_filer(); break;
					case IDC_EDIT_FILER: on_edit_filer(); break;
					case IDC_USE_COMMON_DIALOG:
						{
							hive.use_common_dialog = ::SendDlgItemMessage(*this,
								IDC_USE_COMMON_DIALOG, BM_GETCHECK, 0, 0) == BST_CHECKED;
							MY_TRACE_INT(hive.use_common_dialog);

							break;
						}
					case IDC_FILER_LIST:
						{
							switch (code)
							{
							case LBN_DBLCLK:
								{
									show_filer();

									break;
								}
							case LBN_SELCHANGE:
								{
									refresh_button();

									break;
								}
							}

							break;
						}
					}

					break;
				}
			}

			return __super::on_dlg_proc(hwnd, message, wParam, lParam);
		}
	} addin_dialog;
}
