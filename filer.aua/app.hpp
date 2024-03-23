#pragma once

namespace apn::filer
{
	//
	// このクラスはHive::Appの実装です。
	//
	inline struct App : Hive::App
	{
		//
		// コンストラクタです。
		//
		App()
		{
			// ハイブに自身を登録します。
			hive.app = this;
		}

		//
		// 初期化処理を実行します。
		//
		virtual BOOL init() override
		{
			MY_TRACE_FUNC("");

			// コンフィグ入出力を初期化します。
			if (!config_io.init())
			{
				MY_TRACE("コンフィグ入出力の初期化に失敗しました");
			}

			// ワークスペース化アドインから補助関数をインポートします。
			if (!hive.workspace.init())
			{
				MY_TRACE("ワークスペース化アドインの初期化に失敗しました");
			}

			// アドインウィンドウを初期化します。
			if (!addin_window.init())
			{
				hive.message_box(L"アドインウィンドウの初期化に失敗しました");
				return FALSE;
			}

			// アドインウィンドウにダークモード化アドインが
			// 読み込まれているかどうかのフラグをセットしておきます。
			share::host_window::set_dark(hive.addin_window, !!apn::addin_manager.get_addin<>(L"dark"));

			// ファイラウィンドウを初期化します。
			filer_window_manager.init();

			// フックを初期化します。
			if (!hook_manager.init())
			{
				hive.message_box(L"フックの初期化に失敗しました");
				return FALSE;
			}

			// クライアントプロセスを初期化します。
			if (!gui.init(hive.addin_window))
			{
				hive.message_box(L"クライアントプロセスの初期化に失敗しました");
				return FALSE;
			}

			// コンフィグファイルの読み込みが完了する前に
			// コントロールがリフレッシュされるのは無駄なので、
			// 一旦アドインダイアログをロックします。
			addin_dialog.locked = TRUE;

			// コンフィグを読み込みます。
			// この中でファイラウィンドウが作成されます。
			config_io.read();

			// ホストプロセスの初期化(ファイラウィンドウの作成)が
			// 完了したことをクライアントプロセスに通知します。
			hive.post_message(share::message::c_init, 0, 0);

			// メインダイアログのロックを解除します。
			addin_dialog.locked = FALSE;

			// コントロールをリフレッシュします。
			refresh();

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL exit() override
		{
			MY_TRACE_FUNC("");

			// コンフィグを保存します。
			config_io.write();

			// ホストプロセスが終了することを
			// (ファイラウィンドウが終了する前に)
			// クライアントプロセスに通知します。
			hive.send_message(share::message::c_exit, 0, 0);

			// クライアントプロセスを終了します。
			gui.exit();

			// フックを終了します。
			hook_manager.exit();

			// ファイラウィンドウを終了します。
			filer_window_manager.exit();

			// アドインウィンドウを終了します。
			addin_window.exit();

			// コンフィグ入出力を終了します。
			config_io.exit();

			return TRUE;
		}

		//
		// アドインコマンドを実行します。
		//
		virtual BOOL command() override
		{
			MY_TRACE_FUNC("");

			if (!::IsWindow(addin_window)) return FALSE;

			return addin_window.show(), TRUE;
		}

		//
		// アドインダイアログをリフレッシュします。
		// 主にファイラウィンドウが増減したときの処理です。
		//
		virtual BOOL refresh() override
		{
			MY_TRACE_FUNC("");

			return addin_dialog.refresh();
		}

		//
		// アドインダイアログの作成ボタンが押されたときの処理です。
		//
		virtual BOOL on_create_filer() override
		{
			MY_TRACE_FUNC("");

			// まず、ユーザーにファイラ名を入力してもらいます。
			RenameDialog dialog(_T(""));
			if (dialog.do_modal(addin_window) != IDOK) return FALSE;
			MY_TRACE_STR(dialog.new_name);

			// ファイラを作成します。
			auto filer_window = filer_window_manager.create_filer_window(dialog.new_name.c_str(), TRUE);

			// ファイラを表示します。
			::ShowWindow(*filer_window, SW_SHOW);

			return TRUE;
		}

		//
		// アドインダイアログの削除ボタンを押したときの処理です。
		// 指定されたインデックスのファイラを破壊します。
		//
		virtual BOOL on_destroy_filer(size_t index) override
		{
			MY_TRACE_FUNC("{}", index);

			// インデックスがコレクションの範囲外の場合は何もしません。
			if (index >= FilerWindow::collection.size()) return FALSE;

			// 指定されたインデックスにあるファイラウィンドウを取得します。
			auto& filer_window = FilerWindow::collection[index];

			// 現在のファイラ名を取得します。
			auto name = my::get_window_text(*filer_window);
			MY_TRACE_STR(name);

			// メッセージテキストを構築します。
			auto text = std::format(
				_T("ファイラ「{}」を削除しますか？") _T("\n")
				_T("※ファイラを削除するとファイラに登録してあるブックマークも一緒に削除されます"), name);
			MY_TRACE_STR(text);

			// メッセージボックスを出してユーザーに確認してもらいます。
			if (hive.message_box(text, hive.addin_window, MB_YESNO | MB_ICONWARNING) != IDYES) return FALSE;

			// ファイラウィンドウを破壊します。
			// ウィンドウ破壊時の最終処理でコレクションから取り除かれ、thisポインタが消滅します。
			return FilerWindow::collection[index]->destroy();
		}

		//
		// アドインダイアログの編集ボタンを押したときの処理です。
		// 指定されたインデックスのファイラをリネームします。
		//
		virtual BOOL on_edit_filer(size_t index) override
		{
			MY_TRACE_FUNC("{}", index);

			// インデックスがコレクションの範囲外の場合は何もしません。
			if (index >= FilerWindow::collection.size()) return FALSE;

			// 指定されたインデックスにあるファイラウィンドウを取得します。
			auto& filer_window = FilerWindow::collection[index];

			// 現在のファイラ名を取得します。
			auto name = my::get_window_text(*filer_window);
			MY_TRACE_STR(name);

			// ダイアログを表示してユーザーにファイラ名を入力してもらいます。
			RenameDialog dialog(name.c_str());
			if (dialog.do_modal(addin_window) != IDOK) return FALSE;
			MY_TRACE_STR(dialog.new_name);

			// ファイラをリネームします。
			filer_window->update_window_name(dialog.new_name.c_str());

			// アドインダイアログのファイラ名も変更します。
			addin_dialog.set_filer_name(index, dialog.new_name.c_str());

			// ワークスペース化アドインが存在する場合はシャトル名もリネームします。
			if (hive.workspace.shuttle)
				return hive.workspace.shuttle->rename(*filer_window, dialog.new_name.c_str());

			return TRUE;
		}

		//
		// このクラスはユーザーにファイラ名を入力してもらうためのダイアログです。
		//
		struct RenameDialog : my::Dialog2
		{
			//
			// ファイラの新しい名前です。
			//
			std::wstring new_name;

			//
			// コンストラクタです。
			//
			RenameDialog(LPCTSTR name)
				: new_name(name)
			{
			}

			//
			// ダイアログをモーダル表示します。
			//
			int do_modal(HWND parent)
			{
				create(hive.instance, MAKEINTRESOURCE(IDD_GET_FILER_NAME), parent);

				::SetDlgItemText(*this, IDC_NEW_NAME, new_name.c_str());

				auto ret_value = __super::do_modal2(parent);
				if (IDOK != ret_value) return ret_value;

				new_name = my::get_dlg_item_text(*this, IDC_NEW_NAME);

				return ret_value;
			}

			//
			// 指定されたファイラ名が使用可能な場合はTRUEを返します。
			//
			BOOL is_available_name(const std::wstring& new_name)
			{
				if (new_name.empty()) return FALSE;

				if (hive.workspace.shuttle)
				{
					return hive.workspace.shuttle->is_available_name(new_name.c_str());
				}
				else
				{
					auto it = std::find_if(FilerWindow::collection.begin(), FilerWindow::collection.end(),
						[new_name](const auto& x) { return my::get_window_text(*x) == new_name; });

					return it == FilerWindow::collection.end();
				}
			}

			//
			// OKボタンが押されたときの処理です。
			// ユーザーが入力した名前が無効の場合はダイアログを表示して拒否します。
			//
			virtual void on_ok() override
			{
				new_name = my::get_dlg_item_text(*this, IDC_NEW_NAME);

				// 新しい名前が使用不可能な場合は
				if (!is_available_name(new_name))
				{
					// メッセージボックスを表示します。
					hive.message_box(L"名前が重複しています", *this, MB_OK | MB_ICONWARNING);

					return;
				}

				__super::on_ok();
			}

			//
			// キャンセルボタンが押されたときの処理です。
			//
			virtual void on_cancel() override
			{
				__super::on_cancel();
			}
		};
	} app;
}
