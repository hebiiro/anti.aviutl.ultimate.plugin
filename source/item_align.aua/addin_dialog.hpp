#pragma once

namespace apn::item_align
{
	//
	// このクラスはアドインダイアログです。
	//
	inline struct addin_dialog_t : StdAddinDialog<idd_main>
	{
		void apply_sub_time(uint32_t id, double time)
		{
			auto text = my::format(_T("{/0.4f}"), time);
			set_text(id - 2, text);
		}

		void apply_sub_time(uint32_t id)
		{
			switch (hive.time.mode)
			{
			case hive.time.c_mode.c_frame:
				{
					utils_t utils;

					auto frame = get_int(id);
					auto time = utils.frame_to_time(frame);
					apply_sub_time(id, time);

					break;
				}
			case hive.time.c_mode.c_bpm:
				{
					utils_t utils;
					if (utils.bpm == 0) break;

					auto bpm = get_int(id);
					auto time = utils.bpm_to_time(bpm);
					apply_sub_time(id, time);

					break;
				}
			}
		}

		//
		// コントロールを更新します。
		//
		virtual void on_update_controls() override
		{
			MY_TRACE_FUNC("");

			set_check(idc_flag_use_current_frame, hive.flag_use_current_frame);
			set_combobox_index(idc_time_mode, hive.time.mode);
			set_text(idc_time_align_sec, hive.time.elements[hive.time.c_index.c_align].sec);
			set_text(idc_time_stretch_sec, hive.time.elements[hive.time.c_index.c_stretch].sec);
			set_text(idc_time_relative_space_sec, hive.time.elements[hive.time.c_index.c_relative_space].sec);
			set_text(idc_time_absolute_space_sec, hive.time.elements[hive.time.c_index.c_absolute_space].sec);
			set_text(idc_time_shift_sec, hive.time.elements[hive.time.c_index.c_shift].sec);
			set_int(idc_time_align_frame, hive.time.elements[hive.time.c_index.c_align].frame);
			set_int(idc_time_stretch_frame, hive.time.elements[hive.time.c_index.c_stretch].frame);
			set_int(idc_time_relative_space_frame, hive.time.elements[hive.time.c_index.c_relative_space].frame);
			set_int(idc_time_absolute_space_frame, hive.time.elements[hive.time.c_index.c_absolute_space].frame);
			set_int(idc_time_shift_frame, hive.time.elements[hive.time.c_index.c_shift].frame);
			set_int(idc_move_vert_layer, hive.move_vert_layer);
		}

		//
		// コンフィグを更新します。
		//
		virtual void on_update_config() override
		{
			MY_TRACE_FUNC("");

			get_check(idc_flag_use_current_frame, hive.flag_use_current_frame);
			get_combobox_index(idc_time_mode, hive.time.mode);
			get_text(idc_time_align_sec, hive.time.elements[hive.time.c_index.c_align].sec);
			get_text(idc_time_stretch_sec, hive.time.elements[hive.time.c_index.c_stretch].sec);
			get_text(idc_time_relative_space_sec, hive.time.elements[hive.time.c_index.c_relative_space].sec);
			get_text(idc_time_absolute_space_sec, hive.time.elements[hive.time.c_index.c_absolute_space].sec);
			get_text(idc_time_shift_sec, hive.time.elements[hive.time.c_index.c_shift].sec);
			get_int(idc_time_align_frame, hive.time.elements[hive.time.c_index.c_align].frame);
			get_int(idc_time_stretch_frame, hive.time.elements[hive.time.c_index.c_stretch].frame);
			get_int(idc_time_relative_space_frame, hive.time.elements[hive.time.c_index.c_relative_space].frame);
			get_int(idc_time_absolute_space_frame, hive.time.elements[hive.time.c_index.c_absolute_space].frame);
			get_int(idc_time_shift_frame, hive.time.elements[hive.time.c_index.c_shift].frame);
			get_int(idc_move_vert_layer, hive.move_vert_layer);
		}

		//
		// ダイアログの初期化処理です。
		//
		virtual void on_init_dialog() override
		{
			MY_TRACE_FUNC("");

			init_combobox(idc_time_mode, _T("なし"), _T("フレーム"), _T("BPM"));

			using namespace my::layout;

			auto margin_value = 2;
			auto margin = RECT { margin_value, margin_value, margin_value, margin_value };
			auto base_size = get_base_size();
			auto row = std::make_shared<RelativePos>(base_size + margin_value * 2);
			auto stc = std::make_shared<RelativePos>(base_size * 4);
			auto arrow = std::make_shared<RelativePos>(base_size + margin_value * 2);
			auto stc2 = std::make_shared<RelativePos>(base_size * 2);
			auto time = std::make_shared<RelativePos>(base_size * 3 + margin_value * 2);
			auto time2 = std::make_shared<RelativePos>(base_size * 3 + margin_value * 2);
			auto checkbox = std::make_shared<RelativePos>(base_size * 6);
			auto combobox = std::make_shared<RelativePos>(base_size * 4);
			auto rest = std::make_shared<AbsolutePos>(1, 1, 1);

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, checkbox, margin, ctrl(idc_flag_use_current_frame));
				node->add_pane(c_axis.c_horz, c_align.c_left, combobox, margin, ctrl(idc_time_mode));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stc, margin, ctrl(idc_align_stc));
				node->add_pane(c_axis.c_horz, c_align.c_left, arrow, margin, ctrl(idc_align_left));
				node->add_pane(c_axis.c_horz, c_align.c_left, arrow, margin, ctrl(idc_align_right));
				node->add_pane(c_axis.c_horz, c_align.c_left, stc2, margin, ctrl(idc_time_align_stc));
				node->add_pane(c_axis.c_horz, c_align.c_left, time, margin, ctrl(idc_time_align_sec));
				node->add_pane(c_axis.c_horz, c_align.c_left, time2, margin, ctrl(idc_time_align_frame));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stc, margin, ctrl(idc_stretch_stc));
				node->add_pane(c_axis.c_horz, c_align.c_left, arrow, margin, ctrl(idc_stretch_left));
				node->add_pane(c_axis.c_horz, c_align.c_left, arrow, margin, ctrl(idc_stretch_right));
				node->add_pane(c_axis.c_horz, c_align.c_left, stc2, margin, ctrl(idc_time_stretch_stc));
				node->add_pane(c_axis.c_horz, c_align.c_left, time, margin, ctrl(idc_time_stretch_sec));
				node->add_pane(c_axis.c_horz, c_align.c_left, time2, margin, ctrl(idc_time_stretch_frame));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stc, margin, ctrl(idc_relative_space_stc));
				node->add_pane(c_axis.c_horz, c_align.c_left, arrow, margin, ctrl(idc_relative_space_left));
				node->add_pane(c_axis.c_horz, c_align.c_left, arrow, margin, ctrl(idc_relative_space_right));
				node->add_pane(c_axis.c_horz, c_align.c_left, stc2, margin, ctrl(idc_time_relative_space_stc));
				node->add_pane(c_axis.c_horz, c_align.c_left, time, margin, ctrl(idc_time_relative_space_sec));
				node->add_pane(c_axis.c_horz, c_align.c_left, time2, margin, ctrl(idc_time_relative_space_frame));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stc, margin, ctrl(idc_absolute_space_stc));
				node->add_pane(c_axis.c_horz, c_align.c_left, arrow, margin, ctrl(idc_absolute_space_left));
				node->add_pane(c_axis.c_horz, c_align.c_left, arrow, margin, ctrl(idc_absolute_space_right));
				node->add_pane(c_axis.c_horz, c_align.c_left, stc2, margin, ctrl(idc_time_absolute_space_stc));
				node->add_pane(c_axis.c_horz, c_align.c_left, time, margin, ctrl(idc_time_absolute_space_sec));
				node->add_pane(c_axis.c_horz, c_align.c_left, time2, margin, ctrl(idc_time_absolute_space_frame));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stc, margin, ctrl(idc_shift_stc));
				node->add_pane(c_axis.c_horz, c_align.c_left, arrow, margin, ctrl(idc_shift_down));
				node->add_pane(c_axis.c_horz, c_align.c_left, arrow, margin, ctrl(idc_shift_up));
				node->add_pane(c_axis.c_horz, c_align.c_left, stc2, margin, ctrl(idc_time_shift_stc));
				node->add_pane(c_axis.c_horz, c_align.c_left, time, margin, ctrl(idc_time_shift_sec));
				node->add_pane(c_axis.c_horz, c_align.c_left, time2, margin, ctrl(idc_time_shift_frame));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stc, margin, ctrl(idc_move_vert_stc));
				node->add_pane(c_axis.c_horz, c_align.c_left, arrow, margin, ctrl(idc_move_vert_down));
				node->add_pane(c_axis.c_horz, c_align.c_left, arrow, margin, ctrl(idc_move_vert_up));
				node->add_pane(c_axis.c_horz, c_align.c_left, stc2, margin, ctrl(idc_move_vert_layer_stc));
				node->add_pane(c_axis.c_horz, c_align.c_left, time, margin, ctrl(idc_move_vert_layer));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, rest, margin, ctrl(idc_fix_bpm));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, rest, margin, ctrl(idc_erase_midpt));
			}
		}

		//
		// ダイアログのコマンド処理です。
		//
		virtual void on_command(UINT code, UINT id, HWND control) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}", code, id, control);

			switch (id)
			{
			// ボタン
			case idc_align_left: app->align_left(); break;
			case idc_align_right: app->align_right(); break;
			case idc_stretch_left: app->stretch_left(); break;
			case idc_stretch_right: app->stretch_right(); break;
			case idc_relative_space_left: app->relative_space_left(); break;
			case idc_relative_space_right: app->relative_space_right(); break;
			case idc_absolute_space_left: app->absolute_space_left(); break;
			case idc_absolute_space_right: app->absolute_space_right(); break;
			case idc_shift_up: app->shift_up(); break;
			case idc_shift_down: app->shift_down(); break;
			case idc_move_vert_down: app->move_vert_down(); break;
			case idc_move_vert_up: app->move_vert_up(); break;
			case idc_fix_bpm: app->fix_bpm(); break;
			case idc_erase_midpt: app->erase_midpt(); break;

			// エディットボックス
			case idc_time_align_sec:
			case idc_time_stretch_sec:
			case idc_time_relative_space_sec:
			case idc_time_absolute_space_sec:
			case idc_time_shift_sec:
			case idc_move_vert_layer: if (code == EN_CHANGE) update_config(); break;

			// サブエディットボックス
			case idc_time_align_frame:
			case idc_time_stretch_frame:
			case idc_time_relative_space_frame:
			case idc_time_absolute_space_frame:
			case idc_time_shift_frame: if (code == EN_CHANGE) apply_sub_time(id); break;

			// コンボボックス
			case idc_time_mode: if (code == CBN_SELCHANGE) update_config(); break;

			// その他
			case idc_flag_use_current_frame: update_config(); break;
			}
		}
	} addin_dialog;
}
