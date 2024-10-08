#pragma once

namespace apn::output_check
{
	//
	// このクラスはアドインダイアログです。
	//
	inline struct AddinDialog : StdAddinDialog<IDD_MAIN_DIALOG>
	{
		//
		// コントロールを更新します。
		//
		virtual void on_update_controls() override
		{
			MY_TRACE_FUNC("");

			set_check(IDC_CHECK_RANGE, hive.check_range);
			set_check(IDC_CHECK_LAST_FRAME, hive.check_last_frame);
			set_check(IDC_CHECK_FRAME_RATE, hive.check_frame_rate);
		}

		//
		// コンフィグを更新します。
		//
		virtual void on_update_config() override
		{
			MY_TRACE_FUNC("");

			get_check(IDC_CHECK_RANGE, hive.check_range);
			get_check(IDC_CHECK_LAST_FRAME, hive.check_last_frame);
			get_check(IDC_CHECK_FRAME_RATE, hive.check_frame_rate);
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
			auto half = std::make_shared<AbsolutePos>(1, 2);
			auto full = std::make_shared<AbsolutePos>(2, 2);

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_CHECK_RANGE));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_CHECK_LAST_FRAME));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_CHECK_FRAME_RATE));
			}
		}

		//
		// ダイアログのコマンド処理です。
		//
		virtual void on_command(UINT code, UINT id, HWND control) override
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}, {:#010x}", code, id, control);

			switch (id)
			{
			// その他
			case IDC_CHECK_RANGE: update_config(); break;
			case IDC_CHECK_LAST_FRAME: update_config(); break;
			case IDC_CHECK_FRAME_RATE: update_config(); break;
			}
		}
	} addin_dialog;
}
