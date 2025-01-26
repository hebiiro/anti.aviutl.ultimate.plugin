#pragma once

namespace apn::reboot::outputs
{
	//
	// このクラスは動画出力のパートです。
	//
	inline struct Outputs : Part
	{
		//
		// このパートの名前を返します。
		//
		virtual std::wstring on_get_name()
		{
			return L"outputs";
		}

		//
		// 初期化処理を実行します。
		//
		virtual BOOL on_init() override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL on_exit() override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// コンフィグを読み込みます。
		//
		virtual BOOL on_read(n_json& root) override
		{
			MY_TRACE_FUNC("");

			read_string(root, "plugin_name", agit.plugin_name);

			return TRUE;
		}

		//
		// コンフィグを書き込みます。
		//
		virtual BOOL on_write(n_json& root) override
		{
			MY_TRACE_FUNC("");

			write_string(root, "plugin_name", agit.plugin_name);

			return TRUE;
		}

		//
		// パートの状態を更新します。
		//
		virtual BOOL on_update(AddinDialogInterface* addin_dialog) override
		{
			MY_TRACE_FUNC("");

			// ダイアログの出力プラグイン名を更新します。
			if (agit.plugin_name.length())
				addin_dialog->set_text(IDC_OUTPUTS, agit.plugin_name);

			return TRUE;
		}

		//
		// コマンドを実行します。
		//
		virtual BOOL on_command(AddinDialogInterface* addin_dialog, uint32_t code, uint32_t control_id, HWND control) override
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}, {:#010x}", code, control_id, control);

			// このパート用のコントロールではない場合は何もしません。
			if (control_id != IDC_OUTPUTS) return FALSE;

			// プロジェクトを開いていない場合は何もしません。
			if (!project::is_editing()) return FALSE;

			auto aviutl_window = hive.aviutl_window;
			auto menu = ::GetMenu(aviutl_window);
			auto file_menu = find_menu(menu, _T("ファイル"));
			auto output_menu = find_menu(file_menu, _T("プラグイン出力"));
			auto id = find_menu_item(output_menu, agit.plugin_name.c_str());
			if (id)
			{
				// 動画を出力します。
				::PostMessage(aviutl_window, WM_COMMAND, id, 0);
			}
			else
			{
				hive.message_box(my::format(
					_T("出力プラグインが見つかりませんでした") _T("\n")
					_T("現在選択されているプラグイン = 【{}】"),
					agit.plugin_name.empty() ? _T("なし") : agit.plugin_name));
			}

			return TRUE;
		}

		//
		// オプションを変更します。
		//
		virtual BOOL on_option(AddinDialogInterface* addin_dialog, uint32_t control_id, HWND control, const POINT& point) override
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}", control_id, control);

			// このパート用のコントロールではない場合は何もしません。
			if (control_id != IDC_OUTPUTS) return FALSE;

			// ポップアップメニューを取得します。
			auto aviutl_window = hive.aviutl_window;
			auto menu = ::GetMenu(aviutl_window);
			auto file_menu = find_menu(menu, _T("ファイル"));
			auto output_menu = find_menu(file_menu, _T("プラグイン出力"));

			// ポップアップメニューを表示します。
			auto id = ::TrackPopupMenuEx(output_menu,
				TPM_NONOTIFY | TPM_RETURNCMD, point.x, point.y, control, nullptr);
			if (!id) return FALSE;

			// コンフィグを更新します。
			agit.plugin_name = my::get_menu_item_text(output_menu, id, MF_BYCOMMAND);

			// コンフィグが更新されたので
			// それに合わせてパートの状態も更新します。
			on_update(addin_dialog);

			return TRUE;
		}
	} part;
}
