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

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			// AviUtlのモジュールハンドルを取得します。
			auto aviutl = (my::addr_t)::GetModuleHandle(nullptr);
			MY_TRACE_HEX(aviutl);

			// フックを初期化します。
			main_play_start::init(aviutl);
			sub_play_start::init(aviutl);
			main_play_stop::init(aviutl);

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

			read_bool(root, "enabled", agit.enabled);
			read_bool(root, "play_select_frame", agit.play_select_frame);
			read_bool(root, "play_loop", agit.play_loop);
			read_label(root, "stretch_mode", agit.stretch_mode, agit.c_stretch_mode.labels);
			read_label(root, "size_mode", agit.size_mode, agit.c_size_mode.labels);
			read_bool(root, "show_frame", agit.show_frame);
			read_bool(root, "show_time", agit.show_time);
			read_bool(root, "show_thumbnail", agit.show_thumbnail);
			read_size(root, "absolute_size", agit.absolute_size);
			read_int(root, "relative_size", agit.relative_size);
			read_child_nodes(root, "thumbnail_scale",
				[&](const n_json& node, size_t i)
			{
				if (i >= std::size(agit.thumbnail_scale))
					return FALSE;

				read_int(node, agit.thumbnail_scale[i]);

				return TRUE;
			});

			return TRUE;
		}

		//
		// コンフィグを書き込みます。
		//
		virtual BOOL on_write(n_json& root) override
		{
			MY_TRACE_FUNC("");

			write_bool(root, "enabled", agit.enabled);
			write_bool(root, "play_select_frame", agit.play_select_frame);
			write_bool(root, "play_loop", agit.play_loop);
			write_label(root, "stretch_mode", agit.stretch_mode, agit.c_stretch_mode.labels);
			write_label(root, "size_mode", agit.size_mode, agit.c_size_mode.labels);
			write_bool(root, "show_frame", agit.show_frame);
			write_bool(root, "show_time", agit.show_time);
			write_bool(root, "show_thumbnail", agit.show_thumbnail);
			write_size(root, "absolute_size", agit.absolute_size);
			write_int(root, "relative_size", agit.relative_size);
			write_child_nodes(root, "thumbnail_scale", agit.thumbnail_scale,
				[&](n_json& node, int32_t thumbnail_scale, size_t i)
			{
				write_int(node, thumbnail_scale);

				return TRUE;
			});

			return TRUE;
		}

		//
		// パートの状態を更新します。
		//
		virtual BOOL on_update(AddinDialogInterface* addin_dialog) override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// コマンドを実行します。
		//
		virtual BOOL on_command(AddinDialogInterface* addin_dialog, uint32_t code, uint32_t control_id, HWND control) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}", code, control_id, control);

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
			MY_TRACE_FUNC("{/hex}, {/hex}", control_id, control);

			// このパート用のコントロールではない場合は何もしません。
			if (control_id != IDC_SPREADER_ENABLED) return FALSE;

			constexpr LPCTSTR stretch_mode_texts[] = {
				_T("伸縮せずに表示"),
				_T("フィットして表示"),
				_T("クロップして表示"),
				_T("引き伸ばして表示"),
			};

			// ポップアップメニューのアイテムIDです。
			constexpr uint32_t c_play_select_frame = 1000;
			constexpr uint32_t c_play_loop = 1001;
			constexpr uint32_t c_stretch_mode_first = 2000;
			constexpr uint32_t c_stretch_mode_last = c_stretch_mode_first + std::size(stretch_mode_texts);
			constexpr uint32_t c_show_frame = 3000;
			constexpr uint32_t c_show_time = 3001;
			constexpr uint32_t c_show_thumbnail = 3002;
			constexpr uint32_t c_show_option_dialog = 4000;

			// ポップアップメニューを作成します。
			my::menu::unique_ptr<> menu(::CreatePopupMenu());

			// play_select_frameを表示を追加します。
			::AppendMenu(menu.get(), MF_STRING, c_play_select_frame, _T("選択範囲を再生"));
			if (agit.play_select_frame) ::CheckMenuItem(menu.get(), c_play_select_frame, MF_CHECKED);

			// play_loopを表示を追加します。
			::AppendMenu(menu.get(), MF_STRING, c_play_loop, _T("繰り返し"));
			if (agit.play_loop) ::CheckMenuItem(menu.get(), c_play_loop, MF_CHECKED);

			// セパレータを追加します。
			::AppendMenu(menu.get(), MF_SEPARATOR, 0, nullptr);

			// stretch_modeを追加します。
			for (size_t i = 0; i < std::size(stretch_mode_texts); i++)
				::AppendMenu(menu.get(), MF_STRING, c_stretch_mode_first + i, stretch_mode_texts[i]);
			::CheckMenuRadioItem(menu.get(), c_stretch_mode_first, c_stretch_mode_last,
				c_stretch_mode_first + agit.stretch_mode, MF_BYCOMMAND);

			// セパレータを追加します。
			::AppendMenu(menu.get(), MF_SEPARATOR, 0, nullptr);

			// show_frameを表示を追加します。
			::AppendMenu(menu.get(), MF_STRING, c_show_frame, _T("フレームを表示"));
			if (agit.show_frame) ::CheckMenuItem(menu.get(), c_show_frame, MF_CHECKED);

			// show_timeを表示を追加します。
			::AppendMenu(menu.get(), MF_STRING, c_show_time, _T("時間を表示"));
			if (agit.show_time) ::CheckMenuItem(menu.get(), c_show_time, MF_CHECKED);

			// show_thumbnailを表示を追加します。
			::AppendMenu(menu.get(), MF_STRING, c_show_thumbnail, _T("サムネイルを表示"));
			if (agit.show_thumbnail) ::CheckMenuItem(menu.get(), c_show_thumbnail, MF_CHECKED);

			// セパレータを追加します。
			::AppendMenu(menu.get(), MF_SEPARATOR, 0, nullptr);

			// オプションダイアログを表示を追加します。
			::AppendMenu(menu.get(), MF_STRING, c_show_option_dialog, _T("詳細オプション"));

			// ポップアップメニューを表示します。
			auto id = ::TrackPopupMenuEx(menu.get(),
				TPM_NONOTIFY | TPM_RETURNCMD, point.x, point.y, control, nullptr);
			if (!id) return FALSE;

			if (id >= c_stretch_mode_first && id <= c_stretch_mode_last)
				agit.stretch_mode = id - c_stretch_mode_first;

			switch (id)
			{
			case c_play_select_frame: agit.play_select_frame = !agit.play_select_frame; break;
			case c_play_loop: agit.play_loop = !agit.play_loop; break;
			case c_show_frame: agit.show_frame = !agit.show_frame; break;
			case c_show_time: agit.show_time = !agit.show_time; break;
			case c_show_thumbnail: agit.show_thumbnail = !agit.show_thumbnail; break;
			case c_show_option_dialog: return show_option_dialog(addin_dialog);
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

		//
		// オプションダイアログを表示します。
		//
		BOOL show_option_dialog(AddinDialogInterface* addin_dialog)
		{
			MY_TRACE_FUNC("");

			OptionDialog option_dialog;

			if (IDOK != option_dialog.do_modal(*addin_dialog))
				return FALSE;

			return TRUE;
		}
	} part;
}
