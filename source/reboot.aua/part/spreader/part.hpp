#pragma once

namespace apn::reboot::spreader
{
	//
	// このクラスはスプレッダーのパートです。
	//
	inline struct Spreader : Part
	{
		//
		// このパートの名前を返します。
		//
		virtual std::wstring on_get_name()
		{
			return L"spreader";
		}

		//
		// 初期化処理を実行します。
		//
		virtual BOOL on_init() override
		{
			MY_TRACE_FUNC("");

			if (!player.init()) return FALSE;

			// AviUtlのiniファイルのパスを取得します。
			auto file_name = my::get_module_file_name(nullptr).parent_path() / _T("aviutl.ini");
			MY_TRACE_STR(file_name);

			// AviUtlの設定を取得します。
			agit.movieplaymain = ::GetPrivateProfileInt(_T("system"), _T("movieplaymain"), FALSE, file_name.c_str());

			// AviUtlのモジュールハンドルを取得します。
			auto aviutl = (my::addr_t)::GetModuleHandle(nullptr);
			MY_TRACE_HEX(aviutl);

			// フック用のアドレスを初期化します。
			play_main::init(aviutl);
			play_sub::init(aviutl);

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL on_exit() override
		{
			MY_TRACE_FUNC("");

			player.exit();

			return TRUE;
		}

		//
		// コンフィグを読み込みます。
		//
		virtual BOOL on_read(n_json& root) override
		{
			MY_TRACE_FUNC("");

			get_bool(root, "enabled", agit.enabled);
			get_bool(root, "show_frame", agit.show_frame);
			get_bool(root, "show_time", agit.show_time);

			return TRUE;
		}

		//
		// コンフィグを書き込みます。
		//
		virtual BOOL on_write(n_json& root) override
		{
			MY_TRACE_FUNC("");

			set_bool(root, "enabled", agit.enabled);
			set_bool(root, "show_frame", agit.show_frame);
			set_bool(root, "show_time", agit.show_time);

			return TRUE;
		}

		//
		// パートの状態を更新します。
		//
		virtual BOOL on_update(AddinDialogInterface* addin_dialog) override
		{
			MY_TRACE_FUNC("{}", agit.enabled);

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			if (agit.enabled)
			{
				play_main::enable_hook();
				play_sub::enable_hook();
			}
			else
			{
				play_main::disable_hook();
				play_sub::disable_hook();
			}

			if (DetourTransactionCommit() == NO_ERROR)
			{
				MY_TRACE("APIフックに成功しました\n");

				return TRUE;
			}
			else
			{
				MY_TRACE("APIフックに失敗しました\n");

				return FALSE;
			}
		}

		//
		// コマンドを実行します。
		//
		virtual BOOL on_command(AddinDialogInterface* addin_dialog, uint32_t code, uint32_t control_id, HWND control) override
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}, {:#010x}", code, control_id, control);

			// このパート用のコントロールではない場合は何もしません。
			if (control_id != IDC_SPREADER_ENABLED) return FALSE;

			// コントロールの値が変更されたので
			// それに合わせてコンフィグを更新します。
			addin_dialog->update_config();

			// コンフィグが更新されたので
			// それに合わせてパートの状態も更新します。
			on_update(addin_dialog);

			return TRUE;
		}

		//
		// オプションを変更します。
		//
		virtual BOOL on_option(AddinDialogInterface* addin_dialog, uint32_t control_id, HWND control, const POINT& point) override
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}", control_id, control);

			// このパート用のコントロールではない場合は何もしません。
			if (control_id != IDC_SPREADER_ENABLED) return FALSE;

			// ポップアップメニューのアイテムIDです。
			constexpr uint32_t c_show_frame = 1000;
			constexpr uint32_t c_show_time = 1001;

			// ポップアップメニューを作成します。
			my::menu::unique_ptr<> menu(::CreatePopupMenu());

			::AppendMenu(menu.get(), MF_STRING, c_show_frame, _T("フレームを表示"));
			if (agit.show_frame) ::CheckMenuItem(menu.get(), c_show_frame, MF_CHECKED);
			::AppendMenu(menu.get(), MF_STRING, c_show_time, _T("時間を表示"));
			if (agit.show_time) ::CheckMenuItem(menu.get(), c_show_time, MF_CHECKED);

			auto id = ::TrackPopupMenuEx(menu.get(),
				TPM_NONOTIFY | TPM_RETURNCMD, point.x, point.y, control, nullptr);
			if (!id) return FALSE;

			switch (id)
			{
			case c_show_frame: agit.show_frame = !agit.show_frame; break;
			case c_show_time: agit.show_time = !agit.show_time; break;
			}

			return TRUE;
		}

		//
		// コンフィグを更新します。
		//
		virtual BOOL on_update_config(AddinDialogInterface* addin_dialog) override
		{
			MY_TRACE_FUNC("");

			addin_dialog->get_check(IDC_SPREADER_ENABLED, agit.enabled);

			return TRUE;
		}

		//
		// コントロールを更新します。
		//
		virtual BOOL on_update_controls(AddinDialogInterface* addin_dialog) override
		{
			MY_TRACE_FUNC("");

			addin_dialog->set_check(IDC_SPREADER_ENABLED, agit.enabled);

			return TRUE;
		}
	} part;
}
