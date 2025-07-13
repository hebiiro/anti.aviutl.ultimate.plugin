#pragma once

namespace apn::audio_visualizer
{
	//
	// このクラスはアドインダイアログです。
	//
	inline struct AddinDialog : StdAddinDialog<IDD_MAIN_DIALOG>
	{
		inline static constexpr auto float_fmt = _T("{/.0f}");

		//
		// 指定されたコントロールを有効または無効にします。
		//
		BOOL enable_control(UINT id, BOOL enable)
		{
			return ::EnableWindow(ctrl(id), enable);
		}

		//
		// すべてのコントロールをリフレッシュします。
		//
		BOOL refresh()
		{
			MY_TRACE_FUNC("");

			refresh_listbox();
			refresh_buttons();

			return TRUE;
		}

		//
		// リストボックスをリフレッシュします。
		//
		BOOL refresh_listbox()
		{
			MY_TRACE_FUNC("");

			if (is_locked()) return FALSE;

			auto list = ctrl(IDC_WINDOW_LIST);
			::SendMessage(list, WM_SETREDRAW, FALSE, 0);
			::SendMessage(list, LB_RESETCONTENT, 0, 0);
			for (auto& co_window : co_window_manager.collection)
			{
				auto name = my::get_window_text(*co_window);

				::SendMessage(list, LB_ADDSTRING, 0, (LPARAM)name.c_str());
			}
			::SendMessage(list, WM_SETREDRAW, TRUE, 0);

			return TRUE;
		}

		//
		// ボタンをリフレッシュします。
		//
		BOOL refresh_buttons()
		{
			MY_TRACE_FUNC("");

			if (is_locked()) return FALSE;

			auto list = ctrl(IDC_WINDOW_LIST);
			auto sel = (size_t)::SendMessage(list, LB_GETCURSEL, 0, 0);
			auto c = (size_t)::SendMessage(list, LB_GETCOUNT, 0, 0);
			if (sel < c)
			{
				enable_control(IDC_CREATE_WINDOW, TRUE);
				enable_control(IDC_DESTROY_WINDOW, TRUE);
				enable_control(IDC_EDIT_WINDOW, TRUE);
			}
			else
			{
				enable_control(IDC_CREATE_WINDOW, TRUE);
				enable_control(IDC_DESTROY_WINDOW, FALSE);
				enable_control(IDC_EDIT_WINDOW, FALSE);
			}

			return TRUE;
		}

		//
		// リストボックス内の指定されたインデックスにあるウィンドウ名を変更します。
		//
		BOOL set_window_name(size_t index, LPCTSTR new_name)
		{
			MY_TRACE_FUNC("{/}, {/}", index, new_name);

			if (is_locked()) return FALSE;

			auto list = ctrl(IDC_WINDOW_LIST);
			::SendMessage(list, LB_DELETESTRING, index, 0); // 一旦テキストを削除します。
			::SendMessage(list, LB_INSERTSTRING, index, (LPARAM)new_name); // 新しいテキストを挿入します。

			return TRUE;
		}

		//
		// 選択されているウィンドウを表示状態を切り替えます。
		//
		BOOL show_window()
		{
			MY_TRACE_FUNC("");

			auto list = ctrl(IDC_WINDOW_LIST);
			auto index = (size_t)::SendMessage(list, LB_GETCURSEL, 0, 0);
			auto co_window = co_window_manager.get_co_window(index);
			if (!co_window) return FALSE;

			::SendMessage(*co_window, WM_CLOSE, 0, 0);

			return TRUE;
		}

		//
		// ウィンドウを作成します。
		//
		BOOL create_window()
		{
			MY_TRACE_FUNC("");

			return app->create_co_window();
		}

		//
		// ウィンドウを削除します。
		//
		BOOL destroy_window()
		{
			MY_TRACE_FUNC("");

			return app->destroy_co_window(
				(size_t)::SendMessage(ctrl(IDC_WINDOW_LIST), LB_GETCURSEL, 0, 0));
		}

		//
		// ウィンドウを編集(リネーム)します。
		//
		BOOL edit_window()
		{
			MY_TRACE_FUNC("");

			return app->edit_co_window(
				(size_t)::SendMessage(ctrl(IDC_WINDOW_LIST), LB_GETCURSEL, 0, 0));
		}

		//
		// コンフィグを更新します。
		//
		virtual void on_update_config() override
		{
			MY_TRACE_FUNC("");

			get_combobox_index(IDC_MODE, hive.mode);
			get_int(IDC_RANGE_MIN, hive.option.range.min);
			get_int(IDC_RANGE_MAX, hive.option.range.max);
			get_int(IDC_MARKER_BASE, hive.option.marker.base);
			get_int(IDC_MARKER_AREA, hive.option.marker.area);
			get_float(IDC_SPECTRE_FREQ_MIN, hive.option.spectre_option.freq_min);
			get_float(IDC_SPECTRE_FREQ_MAX, hive.option.spectre_option.freq_max);
			get_int(IDC_SPECTRE_DIV, hive.option.spectre_option.div);
			get_check(IDC_SPECTRE_RMS_SCALING, hive.option.spectre_option.rms_scaling);
			get_check(IDC_SPECTRE_LOG_FREQ, hive.option.spectre_option.log_freq);

			app->update_config();
		}

		//
		// コントロールを更新します。
		//
		virtual void on_update_controls() override
		{
			MY_TRACE_FUNC("");

			set_combobox_index(IDC_MODE, hive.mode);
			set_int(IDC_RANGE_MIN, hive.option.range.min);
			set_int(IDC_RANGE_MAX, hive.option.range.max);
			set_int(IDC_MARKER_BASE, hive.option.marker.base);
			set_int(IDC_MARKER_AREA, hive.option.marker.area);
			set_float(IDC_SPECTRE_FREQ_MIN, hive.option.spectre_option.freq_min);
			set_float(IDC_SPECTRE_FREQ_MAX, hive.option.spectre_option.freq_max);
			set_int(IDC_SPECTRE_DIV, hive.option.spectre_option.div);
			set_check(IDC_SPECTRE_RMS_SCALING, hive.option.spectre_option.rms_scaling);
			set_check(IDC_SPECTRE_LOG_FREQ, hive.option.spectre_option.log_freq);

			app->update_config();
		}

		//
		// ダイアログの初期化処理です。
		//
		virtual void on_init_dialog() override
		{
			MY_TRACE_FUNC("");

			init_combobox(IDC_MODE, _T("無効"), _T("有効"), _T("有効(再生中以外)"));
			::SendDlgItemMessage(*this, IDC_RANGE_MIN, UDM_SETRANGE32, -100, +100);
			::SendDlgItemMessage(*this, IDC_RANGE_MAX, UDM_SETRANGE32, -100, +100);
			::SendDlgItemMessage(*this, IDC_MARKER_BASE, UDM_SETRANGE32, -100, +100);
			::SendDlgItemMessage(*this, IDC_MARKER_AREA, UDM_SETRANGE32, -100, +100);

			using namespace my::layout;

			auto margin_value = 2;
			auto margin = RECT { margin_value, margin_value, margin_value, margin_value };
			auto base_size = get_base_size();
			auto row = std::make_shared<RelativePos>(base_size + margin_value * 2);
			auto stat = std::make_shared<RelativePos>(base_size * 4);
			auto control = std::make_shared<RelativePos>(base_size * 4);
			auto edit = std::make_shared<RelativePos>(base_size * 2);
			auto spin = std::make_shared<RelativePos>(base_size * 2);
			auto checkbox = std::make_shared<RelativePos>(base_size * 6);
			auto rest = std::make_shared<AbsolutePos>(1, 1, 1);

			::SendDlgItemMessage(*this, IDC_MODE, CB_SETDROPPEDWIDTH, base_size * 6, 0);

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_bottom, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, checkbox, margin, ctrl(IDC_SPECTRE_RMS_SCALING));
				node->add_pane(c_axis.c_horz, c_align.c_left, checkbox, margin, ctrl(IDC_SPECTRE_LOG_FREQ));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_bottom, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_SPECTRE_DIV_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, edit, margin, ctrl(IDC_SPECTRE_DIV));
				node->add_pane(c_axis.c_horz, c_align.c_left, spin, margin, ctrl(IDC_SPECTRE_DIV_SPIN));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_bottom, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_SPECTRE_FREQ_MIN_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, edit, margin, ctrl(IDC_SPECTRE_FREQ_MIN));
				node->add_pane(c_axis.c_horz, c_align.c_left, spin, margin, ctrl(IDC_SPECTRE_FREQ_MIN_SPIN));
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_SPECTRE_FREQ_MAX_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, edit, margin, ctrl(IDC_SPECTRE_FREQ_MAX));
				node->add_pane(c_axis.c_horz, c_align.c_left, spin, margin, ctrl(IDC_SPECTRE_FREQ_MAX_SPIN));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_bottom, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, rest, margin, ctrl(IDC_SPECTRE_STAT));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_bottom, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_MARKER_BASE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, edit, margin, ctrl(IDC_MARKER_BASE));
				node->add_pane(c_axis.c_horz, c_align.c_left, spin, margin, ctrl(IDC_MARKER_BASE_SPIN));
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_MARKER_AREA_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, edit, margin, ctrl(IDC_MARKER_AREA));
				node->add_pane(c_axis.c_horz, c_align.c_left, spin, margin, ctrl(IDC_MARKER_AREA_SPIN));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_bottom, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_RANGE_MIN_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, edit, margin, ctrl(IDC_RANGE_MIN));
				node->add_pane(c_axis.c_horz, c_align.c_left, spin, margin, ctrl(IDC_RANGE_MIN_SPIN));
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_RANGE_MAX_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, edit, margin, ctrl(IDC_RANGE_MAX));
				node->add_pane(c_axis.c_horz, c_align.c_left, spin, margin, ctrl(IDC_RANGE_MAX_SPIN));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_bottom, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_MODE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, control, margin, ctrl(IDC_MODE));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_bottom, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, control, margin, ctrl(IDC_CREATE_WINDOW));
				node->add_pane(c_axis.c_horz, c_align.c_left, control, margin, ctrl(IDC_DESTROY_WINDOW));
				node->add_pane(c_axis.c_horz, c_align.c_left, control, margin, ctrl(IDC_EDIT_WINDOW));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_bottom, rest);
				node->add_pane(c_axis.c_horz, c_align.c_left, rest, margin, ctrl(IDC_WINDOW_LIST));
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
			// リストボックス
			case IDC_WINDOW_LIST:
				{
					switch (code)
					{
					case LBN_DBLCLK:
						{
							show_window();

							break;
						}
					case LBN_SELCHANGE:
						{
							refresh_buttons();

							break;
						}
					}

					break;
				}
			// コンボボックス
			case IDC_MODE:
				{
					if (code == CBN_SELCHANGE) update_config(); break;
				}
			// エディットボックス
			case IDC_RANGE_MIN:
			case IDC_RANGE_MAX:
			case IDC_MARKER_BASE:
			case IDC_MARKER_AREA:
			case IDC_SPECTRE_FREQ_MIN:
			case IDC_SPECTRE_FREQ_MAX:
			case IDC_SPECTRE_DIV:
				{
					if (code == EN_CHANGE) update_config(); break;
				}
			// チェックボックス
			case IDC_SPECTRE_RMS_SCALING:
			case IDC_SPECTRE_LOG_FREQ:
				{
					update_config(); break;
				}
			// ボタン
			case IDC_CREATE_WINDOW: create_window(); break;
			case IDC_DESTROY_WINDOW: destroy_window(); break;
			case IDC_EDIT_WINDOW: edit_window(); break;
			}
		}

		//
		// ダイアログプロシージャです。
		//
		virtual INT_PTR on_dlg_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
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
						case IDC_RANGE_MIN:
						case IDC_RANGE_MAX:
						case IDC_MARKER_BASE:
						case IDC_MARKER_AREA:
							{
								auto value = get_int(edit_id);
								value += (nm->iDelta > 0) ? -1 : +1;
								value = std::clamp(value, -100, +100);
								set_int(edit_id, value);
								break;
							}
						case IDC_SPECTRE_FREQ_MIN:
						case IDC_SPECTRE_FREQ_MAX:
							{
								auto value = (double)get_float(edit_id);
								value += (nm->iDelta > 0) ? -1.0 : +1.0;
								value = std::clamp(value, 100.0, 24000.0);
								set_float(edit_id, value);
								break;
							}
						case IDC_SPECTRE_DIV:
							{
								auto value = get_int(edit_id);
								value += (nm->iDelta > 0) ? -1 : +1;
								value = std::clamp(value, 1, 100);
								set_int(edit_id, value);
								break;
							}
						}
					}

					return FALSE;
				}
			}

			return __super::on_dlg_proc(hwnd, message, wParam, lParam);
		}
	} addin_dialog;
}
