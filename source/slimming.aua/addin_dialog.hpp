#pragma once

namespace apn::slimming
{
	//
	// このクラスはアドインダイアログです。
	//
	inline struct AddinDialog : AddinDialogInterface
	{
		//
		// コンフィグを更新します。
		//
		virtual void on_update_config() override
		{
			MY_TRACE_FUNC("");

			get_check(IDC_SLIMBAR_FLAG_USE, hive.slimbar.config.flag_use);
			get_check(IDC_SLIMBAR_FLAG_WHOLE_TITLE, hive.slimbar.config.flag_whole_title);
			get_int(IDC_SLIMBAR_BUTTON_WIDTH, hive.slimbar.config.button_width);
			get_text(IDC_SLIMBAR_TITLE_FORMAT, hive.slimbar.config.title_format);
		}

		//
		// コントロールを更新します。
		//
		virtual void on_update_controls() override
		{
			MY_TRACE_FUNC("");

			set_check(IDC_SLIMBAR_FLAG_USE, hive.slimbar.config.flag_use);
			set_check(IDC_SLIMBAR_FLAG_WHOLE_TITLE, hive.slimbar.config.flag_whole_title);
			set_int(IDC_SLIMBAR_BUTTON_WIDTH, hive.slimbar.config.button_width);
			set_text(IDC_SLIMBAR_TITLE_FORMAT, hive.slimbar.config.title_format);
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
			case IDC_SLIMBAR_FLAG_USE:
				{
					update_config();

					hive.slimbar.apply_config();

					break;
				}
			case IDC_SLIMBAR_BUTTON_WIDTH:
			case IDC_SLIMBAR_TITLE_FORMAT:
				{
					if (code != EN_UPDATE) break;

					[[fallthrough]]; // このまま処理を続けます。
				}
			case IDC_SLIMBAR_FLAG_WHOLE_TITLE:
				{
					update_config();

					if (!hive.slimbar.config.flag_use) break;

					hive.slimbar.update_layout();

					break;
				}
			}
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_NOTIFY:
				{
					auto header = (NMHDR*)lParam;
					if (header->code == UDN_DELTAPOS)
					{
						auto nm = (NMUPDOWN*)header;
						auto edit_id = (UINT)header->idFrom - 1;

						switch (edit_id)
						{
						case IDC_SLIMBAR_BUTTON_WIDTH:
							{
								auto value = get_int(edit_id);
								value += (nm->iDelta > 0) ? -10 : +10;
								value = std::clamp(value, 0, +1000);
								set_int(edit_id, value);
								break;
							}
						}
					}

					return FALSE;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} addin_dialog;
}
