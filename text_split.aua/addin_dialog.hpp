#pragma once

namespace apn::text_split
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

			set_combobox(IDC_SHIFT_MODE, hive.shift_mode);
			set_int(IDC_SHIFT_OFFSET, hive.shift_offset);
			set_int(IDC_SHIFT_RANGE, hive.shift_range);
			set_combobox(IDC_EACH_MODE, hive.each_mode);
			set_combobox(IDC_FIT_MODE, hive.fit_mode);
			set_int(IDC_FIT_WIDTH, hive.fit_width);
			set_check(IDC_ERASE_ORIG_ITEM, hive.erase_orig_item);
		}

		//
		// コンフィグを更新します。
		//
		virtual void on_update_config() override
		{
			MY_TRACE_FUNC("");

			get_combobox(IDC_SHIFT_MODE, hive.shift_mode);
			get_int(IDC_SHIFT_OFFSET, hive.shift_offset);
			get_int(IDC_SHIFT_RANGE, hive.shift_range);
			get_combobox(IDC_EACH_MODE, hive.each_mode);
			get_combobox(IDC_FIT_MODE, hive.fit_mode);
			get_int(IDC_FIT_WIDTH, hive.fit_width);
			get_check(IDC_ERASE_ORIG_ITEM, hive.erase_orig_item);
		}

		//
		// ダイアログの初期化処理です。
		//
		virtual void on_init_dialog() override
		{
			MY_TRACE_FUNC("");

			init_combobox(IDC_SHIFT_MODE, _T("なし"), _T("オフセット"), _T("範囲"));
			init_combobox(IDC_EACH_MODE, _T("文字"), _T("行"));
			init_combobox(IDC_FIT_MODE, _T("なし"), _T("アイテムの幅"), _T("指定された幅"));

			using namespace my::layout;

			auto margin_value = 2;
			auto margin = RECT { margin_value, margin_value, margin_value, margin_value };
			auto row = std::make_shared<RelativePos>(get_base_size() + margin_value * 2);
			auto half = std::make_shared<AbsolutePos>(1, 2);
			auto full = std::make_shared<AbsolutePos>(2, 2);

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_SPLIT_TEXT));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, half, margin, ctrl(IDC_SHIFT_MODE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_SHIFT_MODE));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, half, margin, ctrl(IDC_SHIFT_OFFSET_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_SHIFT_OFFSET));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, half, margin, ctrl(IDC_SHIFT_RANGE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_SHIFT_RANGE));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, half, margin, ctrl(IDC_EACH_MODE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_EACH_MODE));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, half, margin, ctrl(IDC_FIT_MODE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_FIT_MODE));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, half, margin, ctrl(IDC_FIT_WIDTH_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_FIT_WIDTH));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_ERASE_ORIG_ITEM));
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
			case IDC_SPLIT_TEXT: hive.app->split_text(); break;

			// エディットボックス
			case IDC_SHIFT_OFFSET:
			case IDC_SHIFT_RANGE:
			case IDC_FIT_WIDTH: if (code == EN_CHANGE) update_config(); break;

			// コンボボックス
			case IDC_SHIFT_MODE:
			case IDC_EACH_MODE:
			case IDC_FIT_MODE: if (code == CBN_SELCHANGE) update_config(); break;

			// その他
			case IDC_ERASE_ORIG_ITEM: update_config(); break;
			}
		}
	} addin_dialog;
}
