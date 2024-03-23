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
			auto half = std::make_shared<AbsolutePos>(1, 2);
			auto full = std::make_shared<AbsolutePos>(2, 2);

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_SHIFT));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_MOVE_TO_BEGIN));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_MOVE_TO_END));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_MOVE_BEGIN));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_MOVE_END));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_STRETCH_BEGIN));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_STRETCH_END));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_DELETE_MIDPT));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_FIX_BPM));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, half, margin, ctrl(IDC_FRAME_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_FRAME));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_USE_CURRENT_FRAME));
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
			case IDC_SHIFT: hive.app->shift(); break;
			case IDC_MOVE_TO_BEGIN: hive.app->move_to_begin(); break;
			case IDC_MOVE_TO_END: hive.app->move_to_end(); break;
			case IDC_MOVE_BEGIN: hive.app->move_begin(); break;
			case IDC_MOVE_END: hive.app->move_end(); break;
			case IDC_STRETCH_BEGIN: hive.app->stretch_begin(); break;
			case IDC_STRETCH_END: hive.app->stretch_end(); break;
			case IDC_DELETE_MIDPT: hive.app->delete_midpt(); break;
			case IDC_FIX_BPM: hive.app->fix_bpm(); break;

			// エディットボックス
			case IDC_FRAME: if (code == EN_CHANGE) update_config(); break;

			// その他
			case IDC_USE_CURRENT_FRAME: update_config(); break;
			}
		}
	} addin_dialog;
}
