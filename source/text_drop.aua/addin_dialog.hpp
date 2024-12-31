#pragma once

namespace apn::text_drop
{
	//
	// このクラスはアドインダイアログです。
	//
	inline struct AddinDialog : AddinDialogInterface
	{
		//
		// 指定された座標からモードを取得して返します。
		//
		int32_t get_mode_from_point(const POINT& point)
		{
			auto control = ::ChildWindowFromPointEx(*this, point, CWP_ALL);
			auto control_id = ::GetDlgCtrlID(control);

			switch (control_id)
			{
			case IDC_DO_DRAG_WHOLE: return hive.c_mode.c_whole;
			case IDC_DO_DRAG_LINES: return hive.c_mode.c_lines;
			case IDC_DO_DRAG_CHARS: return hive.c_mode.c_chars;
			case IDC_DO_DRAG_BLOCKS: return hive.c_mode.c_blocks;
			}

			return hive.c_mode.c_none;
		}

		//
		// コンフィグを更新します。
		//
		virtual void on_update_config() override
		{
			MY_TRACE_FUNC("");

			get_int(IDC_ITEM_RANGE, hive.item_range);
			get_int(IDC_SPACE_RANGE, hive.space_range);
			get_int(IDC_CHAR_RANGE, hive.char_range);
			get_combobox_index(IDC_ITEM_DIRECTION, hive.item_direction);
			get_text(IDC_EXO_FILE_NAME, hive.exo_file_name);
			get_text(IDC_DROP_TEXT, hive.drop_text, 2048 + 2);
			get_int(IDC_INSERT_LAYER, hive.insert_layer);
			get_check(IDC_SELECTED_TEXT_ONLY, hive.selected_text_only);
		}

		//
		// コントロールを更新します。
		//
		virtual void on_update_controls() override
		{
			MY_TRACE_FUNC("");

			set_int(IDC_ITEM_RANGE, hive.item_range);
			set_int(IDC_SPACE_RANGE, hive.space_range);
			set_int(IDC_CHAR_RANGE, hive.char_range);
			set_combobox_index(IDC_ITEM_DIRECTION, hive.item_direction);
			set_text(IDC_EXO_FILE_NAME, hive.exo_file_name);
			set_text(IDC_DROP_TEXT, hive.drop_text);
			set_int(IDC_INSERT_LAYER, hive.insert_layer);
			set_check(IDC_SELECTED_TEXT_ONLY, hive.selected_text_only);
		}

		//
		// ダイアログの初期化処理です。
		//
		virtual void on_init_dialog() override
		{
			MY_TRACE_FUNC("");

			init_combobox(IDC_ITEM_DIRECTION, _T("横方向"), _T("縦方向"));

			using namespace my::layout;

			auto margin_value = 2;
			auto margin = RECT { margin_value, margin_value, margin_value, margin_value };
			auto base_size = get_base_size();
			auto row_size = base_size + margin_value * 2;
			auto row = std::make_shared<RelativePos>(row_size);
			auto button = std::make_shared<RelativePos>(base_size * 4);
			auto stat = std::make_shared<RelativePos>(base_size * 3 + margin_value * 4);
			auto editbox = std::make_shared<RelativePos>(base_size * 3);
			auto half = std::make_shared<AbsolutePos>(1, 2);
			auto full = std::make_shared<AbsolutePos>(2, 2);

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_DO_DRAG_WHOLE));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_DO_DRAG_LINES));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_DO_DRAG_CHARS));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_DO_DRAG_BLOCKS));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_ITEM_RANGE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, editbox, margin, ctrl(IDC_ITEM_RANGE));
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_SPACE_RANGE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, editbox, margin, ctrl(IDC_SPACE_RANGE));
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_CHAR_RANGE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, editbox, margin, ctrl(IDC_CHAR_RANGE));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_ITEM_DIRECTION_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, editbox, margin, ctrl(IDC_ITEM_DIRECTION));
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_EXO_FILE_NAME_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_EXO_FILE_NAME));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_INSERT_LAYER_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, editbox, margin, ctrl(IDC_INSERT_LAYER));
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_SELECTED_TEXT_ONLY));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_INSERT_WHOLE));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_INSERT_LINES));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_INSERT_CHARS));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_INSERT_BLOCKS));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_READ_FILE));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_WRITE_FILE));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, full);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_DROP_TEXT));
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
			case IDC_INSERT_WHOLE: app->insert_exo(hive.c_mode.c_whole); break;
			case IDC_INSERT_LINES: app->insert_exo(hive.c_mode.c_lines); break;
			case IDC_INSERT_CHARS: app->insert_exo(hive.c_mode.c_chars); break;
			case IDC_INSERT_BLOCKS: app->insert_exo(hive.c_mode.c_blocks); break;
			case IDC_READ_FILE: app->read_file(); break;
			case IDC_WRITE_FILE: app->write_file(); break;

			// チェックボックス
			case IDC_SELECTED_TEXT_ONLY:
				update_config(); break;

			// エディットボックス
			case IDC_ITEM_RANGE:
			case IDC_SPACE_RANGE:
			case IDC_CHAR_RANGE:
			case IDC_EXO_FILE_NAME:
			case IDC_DROP_TEXT:
			case IDC_INSERT_LAYER:
				if (code == EN_UPDATE) update_config(); break;

			// コンボボックス
			case IDC_ITEM_DIRECTION:
				if (code == CBN_SELCHANGE) update_config(); break;
			}
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_SETCURSOR:
				{
//					MY_TRACE_FUNC("WM_SETCURSOR, {:#010x}, {:#010x}", wParam, lParam);

					// マウスカーソルの座標からモードを取得します。
					auto point = my::get_cursor_pos();
					my::map_window_points(nullptr, hwnd, &point);
					auto mode = get_mode_from_point(point);
					if (mode == hive.c_mode.c_none) break;

					return ::SetCursor(::LoadCursor(nullptr, IDC_HAND)), TRUE;
				}
			case WM_LBUTTONDOWN:
				{
					MY_TRACE_FUNC("WM_LBUTTONDOWN, {:#010x}, {:#010x}", wParam, lParam);

					// マウスカーソルの座標からモードを取得します。
					auto point = my::lp_to_pt(lParam);
					auto mode = get_mode_from_point(point);
					if (mode == hive.c_mode.c_none) break;

					app->drag_exo(mode);

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} addin_dialog;
}
