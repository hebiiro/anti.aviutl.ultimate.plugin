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

			set_combobox_index(IDC_SHIFT_MODE, hive.shift_mode);
			set_int(IDC_SHIFT_OFFSET, hive.shift_offset);
			set_int(IDC_SHIFT_RANGE, hive.shift_range);
			set_combobox_index(IDC_EACH_MODE, hive.each_mode);
			set_combobox_index(IDC_FIT_MODE, hive.fit_mode);
			set_int(IDC_FIT_WIDTH, hive.fit_width);
			set_check(IDC_ERASE_ORIG_ITEM, hive.erase_orig_item);
		}

		//
		// コンフィグを更新します。
		//
		virtual void on_update_config() override
		{
			MY_TRACE_FUNC("");

			get_combobox_index(IDC_SHIFT_MODE, hive.shift_mode);
			get_int(IDC_SHIFT_OFFSET, hive.shift_offset);
			get_int(IDC_SHIFT_RANGE, hive.shift_range);
			get_combobox_index(IDC_EACH_MODE, hive.each_mode);
			get_combobox_index(IDC_FIT_MODE, hive.fit_mode);
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
			auto base_size = get_base_size();
			auto row = std::make_shared<RelativePos>(base_size + margin_value * 2);
			auto stat1 = std::make_shared<RelativePos>(::MulDiv(base_size, 5, 2));
			auto stat2 = std::make_shared<RelativePos>(base_size * 2);
			auto button = std::make_shared<RelativePos>(base_size * 4);
			auto editbox = std::make_shared<RelativePos>(base_size * 3);
			auto checkbox = std::make_shared<RelativePos>(base_size * 7);
			auto combobox = std::make_shared<RelativePos>(base_size * 4);
			auto rest = std::make_shared<AbsolutePos>(1, 1, 1);

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_SPLIT_TEXT));
				node->add_pane(c_axis.c_horz, c_align.c_left, stat1, margin, ctrl(IDC_EACH_MODE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, combobox, margin, ctrl(IDC_EACH_MODE));
				node->add_pane(c_axis.c_horz, c_align.c_left, checkbox, margin, ctrl(IDC_ERASE_ORIG_ITEM));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, rest, margin, ctrl(IDC_SHIFT_STAT));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stat2, margin, ctrl(IDC_SHIFT_MODE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, combobox, margin, ctrl(IDC_SHIFT_MODE));
				node->add_pane(c_axis.c_horz, c_align.c_left, stat1, margin, ctrl(IDC_SHIFT_OFFSET_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, editbox, margin, ctrl(IDC_SHIFT_OFFSET));
				node->add_pane(c_axis.c_horz, c_align.c_left, stat2, margin, ctrl(IDC_SHIFT_RANGE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, editbox, margin, ctrl(IDC_SHIFT_RANGE));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, rest, margin, ctrl(IDC_FIT_STAT));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stat2, margin, ctrl(IDC_FIT_MODE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, combobox, margin, ctrl(IDC_FIT_MODE));
				node->add_pane(c_axis.c_horz, c_align.c_left, stat2, margin, ctrl(IDC_FIT_WIDTH_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, editbox, margin, ctrl(IDC_FIT_WIDTH));
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
