#pragma once

namespace apn::timeline_map::view
{
	//
	// このクラスは描画設定ダイアログです。
	//
	inline struct loupe_option_dialog_t : StdAddinDialog<idd_loupe_option>
	{
		//
		// コンフィグを更新します。
		//
		virtual void on_update_config() override
		{
			MY_TRACE_FUNC("");

			get_check(idc_loupe_flag_timeline, model::property.loupe.flag_timeline);
			get_check(idc_loupe_flag_overview, model::property.loupe.flag_overview);
			get_int(idc_loupe_viewport_nb_frames, model::property.loupe.viewport.nb_frames);
			get_int(idc_loupe_viewport_nb_layers, model::property.loupe.viewport.nb_layers);
		}

		//
		// コントロールを更新します。
		//
		virtual void on_update_controls() override
		{
			MY_TRACE_FUNC("");

			set_check(idc_loupe_flag_timeline, model::property.loupe.flag_timeline);
			set_check(idc_loupe_flag_overview, model::property.loupe.flag_overview);
			set_int(idc_loupe_viewport_nb_frames, model::property.loupe.viewport.nb_frames);
			set_int(idc_loupe_viewport_nb_layers, model::property.loupe.viewport.nb_layers);
		}

		//
		// コントロールの値でコンフィグを更新します。
		//
		void from_ui(BOOL recreate_resources)
		{
			MY_TRACE_FUNC("");

			// コンフィグを更新します。
			update_config();

			// リスナーに通知します。
			if (listener) listener->on_from_ui(recreate_resources);
		}

		//
		// ダイアログの初期化処理です。
		//
		virtual void on_init_dialog() override
		{
			MY_TRACE_FUNC("");
		}

		//
		// ダイアログのコマンド処理です。
		//
		virtual void on_command(UINT code, UINT control_id, HWND control) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}", code, control_id, control);

			switch (control_id)
			{
			// ボタンの場合の処理です。
			case IDCANCEL:
				{
					// 描画設定ダイアログを非表示にします。
					::ShowWindow(*this, SW_HIDE); break;
				}

			// チェックボックスの場合の処理です。
			case idc_loupe_flag_timeline:
			case idc_loupe_flag_overview:
				{
					// コントロールの値でコンフィグを更新します。
					from_ui(FALSE); break;
				}

			// エディットボックスの場合の処理です。
			case idc_loupe_viewport_nb_frames:
			case idc_loupe_viewport_nb_layers:
				{
					// コントロールの値でコンフィグを更新します。
					if (code == EN_CHANGE) from_ui(TRUE); break;
				}
			}
		}

		//
		// ダイアログプロシージャです。
		//
		virtual INT_PTR on_dlg_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
			return __super::on_dlg_proc(hwnd, message, w_param, l_param);
		}
	} loupe_option_dialog;
}
