#pragma once

namespace apn::audio_visualizer
{
	//
	// このクラスはアプリケーションの実装です。
	//
	inline struct App : AppInterface
	{
		//
		// コンストラクタです。
		//
		App() { app = this; }

		//
		// コンフィグを読み込みます。
		//
		virtual BOOL read_config() override
		{
			MY_TRACE_FUNC("");

			{
				// コンフィグファイルの読み込みが完了する前に
				// コントロールがリフレッシュされるのは無駄なので、
				// 一旦アドインダイアログをロックします。
				AddinDialog::Locker locker(&addin_dialog);

				// コンフィグを読み込みます。
				// この中でコウィンドウが作成されます。
				if (!config_io.read())
				{
					constexpr LPCWSTR names[] = { L"左右Lv", L"左Lv", L"右Lv" };

					for (auto name : names)
					{
						auto co_window = co_window_manager.create_co_window(name);

						::ShowWindow(*co_window, SW_SHOW);
					}
				}
			}

			// アドインダイアログをリフレッシュします。
			addin_dialog.refresh();

			// アドインダイアログのコントロールを更新します。
			addin_dialog.update_controls();

			return TRUE;
		}

		//
		// コンフィグを書き込みます。
		//
		virtual BOOL write_config() override
		{
			MY_TRACE_FUNC("");

			// コンフィグを書き込みます。
			config_io.write();

			return TRUE;
		}

		//
		// コンフィグが更新されたときの処理です。
		//
		virtual BOOL update_config() override
		{
			MY_TRACE_FUNC("");

			return worker.process_option(nullptr, hive.option);
		}

		//
		// アドインダイアログの作成ボタンが押されたときの処理です。
		//
		virtual BOOL create_co_window() override
		{
			MY_TRACE_FUNC("");

			// まず、ユーザーにウィンドウ名を入力してもらいます。
			RenameDialog dialog(_T(""));
			if (dialog.do_modal(hive.main_window) != IDOK) return FALSE;
			MY_TRACE_STR(dialog.new_name);

			// ウィンドウを作成します。
			auto window = co_window_manager.create_co_window(dialog.new_name);

			// ウィンドウを表示します。
			::ShowWindow(*window, SW_SHOW);

			// アドインダイアログをリフレッシュします。
			addin_dialog.refresh();

			return TRUE;
		}

		//
		// アドインダイアログの削除ボタンが押されたときの処理です。
		// 指定されたインデックスのウィンドウを破壊します。
		//
		virtual BOOL destroy_co_window(size_t index) override
		{
			MY_TRACE_FUNC("{/}", index);

			// 指定されたインデックスにあるウィンドウを取得します。
			auto co_window = co_window_manager.get_co_window(index);
			if (!co_window) return FALSE;

			// 現在のウィンドウ名を取得します。
			auto name = co_window->name;
			MY_TRACE_STR(name);

			// メッセージテキストを構築します。
			auto text = my::format(
				_T("ウィンドウ「{/}」を削除しますか？") _T("\n")
				_T("※ウィンドウを削除するとウィンドウの設定も一緒に削除されます"), name);
			MY_TRACE_STR(text);

			// メッセージボックスを出してユーザーに確認します。
			if (hive.message_box(text, hive.main_window, MB_YESNO | MB_ICONWARNING) != IDYES) return FALSE;

			// ウィンドウを削除します。
			co_window_manager.erase_co_window(index);

			// アドインダイアログをリフレッシュします。
			addin_dialog.refresh();

			return TRUE;
		}

		//
		// アドインダイアログの編集ボタンが押されたときの処理です。
		// 指定されたインデックスのウィンドウをリネームします。
		//
		virtual BOOL edit_co_window(size_t index) override
		{
			MY_TRACE_FUNC("{/}", index);

			// 指定されたインデックスにあるウィンドウを取得します。
			auto co_window = co_window_manager.get_co_window(index);
			if (!co_window) return FALSE;

			// 現在のウィンドウ名を取得します。
			auto name = my::get_window_text(*co_window);
			MY_TRACE_STR(name);

			// ダイアログを表示してユーザーにウィンドウ名を入力してもらいます。
			RenameDialog dialog(name.c_str());
			if (dialog.do_modal(addin_window) != IDOK) return FALSE;
			MY_TRACE_STR(dialog.new_name);

			// ウィンドウをリネームします。
			co_window->name = dialog.new_name;
			co_window->update_window_name();

			// アドインダイアログのウィンドウ名も変更します。
			addin_dialog.set_window_name(index, dialog.new_name.c_str());

			// ワークスペース化アドインが存在する場合はシャトル名もリネームします。
			if (hive.workspace.shuttle)
				return hive.workspace.shuttle->rename(*co_window, dialog.new_name.c_str());

			return TRUE;
		}

		//
		// このクラスはユーザーにウィンドウ名を入力してもらうためのダイアログです。
		//
		struct RenameDialog : my::Dialog2
		{
			//
			// ウィンドウの新しい名前です。
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
				create(hive.instance, MAKEINTRESOURCE(IDD_WINDOW_CONFIG), parent);

				bind_text(IDC_NEW_NAME, new_name);

				auto ret_value = __super::do_modal2(parent);
				if (IDOK != ret_value) return ret_value;

				new_name = get_text(IDC_NEW_NAME);

				return ret_value;
			}

			//
			// 指定されたウィンドウ名が使用可能な場合はTRUEを返します。
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
					auto it = std::find_if(
						co_window_manager.collection.begin(),
						co_window_manager.collection.end(),
						[new_name](const auto& x) { return my::get_window_text(*x) == new_name; });

					return it == co_window_manager.collection.end();
				}
			}

			//
			// OKボタンが押されたときの処理です。
			// ユーザーが入力した名前が無効の場合はダイアログを表示して拒否します。
			//
			virtual void on_ok() override
			{
				new_name = get_text(IDC_NEW_NAME);

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
	} app_impl;
}
