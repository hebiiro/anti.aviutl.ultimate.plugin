#pragma once

namespace apn::item_align
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

			set_int(IDC_FRAME, hive.frame);
			set_check(IDC_USE_CURRENT_FRAME, hive.use_current_frame);
		}

		//
		// コンフィグを更新します。
		//
		virtual void on_update_config() override
		{
			MY_TRACE_FUNC("");

			get_int(IDC_FRAME, hive.frame);
			get_check(IDC_USE_CURRENT_FRAME, hive.use_current_frame);
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
			std::shared_ptr<AbsolutePos> q[13];
			for (auto i = 0; i < std::size(q); i++)
				q[i] = std::make_shared<AbsolutePos>(i, std::size(q) - 1);

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, q[3], margin, ctrl(IDC_FRAME_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, q[6], margin, ctrl(IDC_FRAME));
				node->add_pane(c_axis.c_horz, c_align.c_left, q[12], margin, ctrl(IDC_USE_CURRENT_FRAME));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, q[12], margin, ctrl(IDC_SHIFT_SELECTION));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, q[12], margin, ctrl(IDC_FIX_SELECTION_BPM));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, q[12], margin, ctrl(IDC_DELETE_MIDPT));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, q[6], margin, ctrl(IDC_MOVE_CURRENT_FRAME_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, q[9], margin, ctrl(IDC_MOVE_CURRENT_FRAME_TO_START));
				node->add_pane(c_axis.c_horz, c_align.c_left, q[12], margin, ctrl(IDC_MOVE_CURRENT_FRAME_TO_END));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, q[6], margin, ctrl(IDC_MOVE_SELECTION_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, q[9], margin, ctrl(IDC_MOVE_SELECTION_START));
				node->add_pane(c_axis.c_horz, c_align.c_left, q[12], margin, ctrl(IDC_MOVE_SELECTION_END));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, q[6], margin, ctrl(IDC_STRETCH_SELECTION_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, q[9], margin, ctrl(IDC_STRETCH_SELECTION_START));
				node->add_pane(c_axis.c_horz, c_align.c_left, q[12], margin, ctrl(IDC_STRETCH_SELECTION_END));
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
			// ボタン
			case IDC_SHIFT_SELECTION: hive.app->shift_selection(); break;
			case IDC_MOVE_CURRENT_FRAME_TO_START: hive.app->move_current_frame_to_start(); break;
			case IDC_MOVE_CURRENT_FRAME_TO_END: hive.app->move_current_frame_to_end(); break;
			case IDC_MOVE_SELECTION_START: hive.app->move_selection_start(); break;
			case IDC_MOVE_SELECTION_END: hive.app->move_selection_end(); break;
			case IDC_STRETCH_SELECTION_START: hive.app->stretch_selection_start(); break;
			case IDC_STRETCH_SELECTION_END: hive.app->stretch_selection_end(); break;
			case IDC_DELETE_MIDPT: hive.app->delete_midpt(); break;
			case IDC_FIX_SELECTION_BPM: hive.app->fix_selection_bpm(); break;

			// エディットボックス
			case IDC_FRAME: if (code == EN_CHANGE) update_config(); break;

			// その他
			case IDC_USE_CURRENT_FRAME: update_config(); break;
			}
		}
	} addin_dialog;
}
