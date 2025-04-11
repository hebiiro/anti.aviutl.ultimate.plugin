#pragma once

namespace apn::reboot
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

			// パートのコンフィグ更新処理を実行します。
			for (auto& part : Part::parts) part->on_update_config(this);
		}

		//
		// コントロールを更新します。
		//
		virtual void on_update_controls() override
		{
			MY_TRACE_FUNC("");

			// パートのコントロール更新処理を実行します。
			for (auto& part : Part::parts) part->on_update_controls(this);
		}

		//
		// ダイアログの初期化処理です。
		//
		virtual void on_init_dialog() override
		{
			MY_TRACE_FUNC("");

			using namespace my::layout;

			auto margin_value = 2;
			auto margin = RECT { margin_value, margin_value, margin_value, margin_value };
			auto row = std::make_shared<RelativePos>(get_base_size() + margin_value * 2);
			auto button = std::make_shared<RelativePos>(get_base_size() * 7);
			auto half = std::make_shared<AbsolutePos>(1, 2);
			auto full = std::make_shared<AbsolutePos>(2, 2);

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_SPREADER_ENABLED));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_RELOADER_REBOOT_AVIUTL));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_LAUNCHER_COMMAND_0));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_OUTPUTS));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_MANIPULATOR_OPEN_AVIUTL_FOLDER));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_LAUNCHER_COMMAND_1));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_MANIPULATOR_WRITE));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_MANIPULATOR_SHOW_PSD_TOOL_KIT));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_LAUNCHER_COMMAND_2));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_MANIPULATOR_WRITE_AS));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_MANIPULATOR_PLAY));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_LAUNCHER_COMMAND_3));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_MANIPULATOR_OPEN));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_MANIPULATOR_OPTIMIZE));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_LAUNCHER_COMMAND_4));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_MANIPULATOR_CLOSE));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, nullptr);
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_LAUNCHER_COMMAND_5));
			}
		}

		//
		// ダイアログのコマンド処理です。
		//
		virtual void on_command(UINT code, UINT id, HWND control) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}", code, id, control);

			// パートのコマンド処理を実行します。
			for (auto& part : Part::parts) part->on_command(this, code, id, control);
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_CONTEXTMENU:
				{
					auto control = (HWND)wParam;
					auto id = ::GetDlgCtrlID(control);
					auto point = my::lp_to_pt(lParam);

					// パートのオプション処理を実行します。
					for (auto& part : Part::parts) part->on_option(this, id, control, point);

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} addin_dialog;
}
