#pragma once

namespace apn::item_wave
{
	//
	// フォルダ選択ダイアログを表示します。
	//
	std::wstring pick_folder(HWND owner, const std::wstring& title, const std::wstring& start_folder)
	{
		auto dialog = wil::CoCreateInstance<IFileOpenDialog>(__uuidof(FileOpenDialog));
		if (FAILED(dialog->SetTitle(title.c_str()))) return {};
		FILEOPENDIALOGOPTIONS options;
		if (FAILED(dialog->GetOptions(&options))) return {};
		if (FAILED(dialog->SetOptions(options | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM))) return {};
//		if (FAILED(dialog->SetOptions(options | FOS_PATHMUSTEXIST | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM))) return {};
		wil::com_ptr<IShellItem> start_folder_item;
		if (FAILED(::SHCreateItemFromParsingName(start_folder.c_str(), nullptr, IID_PPV_ARGS(&start_folder_item)))) return {};
		if (FAILED(dialog->SetFolder(start_folder_item.get()))) return {};
		if (FAILED(dialog->Show(owner))) return {};
		wil::com_ptr<IShellItem> result;
		if (FAILED(dialog->GetResult(&result))) return {};
		wil::unique_cotaskmem_string selected_item;
		if (FAILED(result->GetDisplayName(SIGDN_FILESYSPATH, &selected_item))) return {};
		return selected_item.get();
	}
 
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

			auto include_layers = layers_to_string(hive.include_layers);
			auto exclude_layers = layers_to_string(hive.exclude_layers);

			set_int(IDC_SCALE, hive.scale);
			set_combobox_index(IDC_WAVE_TYPE, hive.wave_type);
			set_combobox_index(IDC_UPDATE_MODE, hive.update_mode);
			set_combobox_index(IDC_XOR_MODE, hive.xor_mode);
			set_combobox_index(IDC_VOLUME_MODE, hive.volume_mode);
			set_check(IDC_SHOW_WAVE, hive.show_wave);
			set_check(IDC_SHOW_TEXT, hive.show_text);
			set_check(IDC_NAMECAGE, hive.namecage);
			set_check(IDC_BEHIND, hive.behind);
			set_text(IDC_INCLUDE_LAYERS, include_layers);
			set_text(IDC_EXCLUDE_LAYERS, exclude_layers);
			set_text(IDC_INCLUDE_FOLDER, hive.include_folder);
			set_text(IDC_EXCLUDE_FOLDER, hive.exclude_folder);
			set_int(IDC_NAMECAGE_OFFSET, hive.namecage_offset);
			set_int(IDC_MAX_THREAD_COUNT, hive.max_thread_count);
		}

		//
		// コンフィグを更新します。
		//
		virtual void on_update_config() override
		{
			MY_TRACE_FUNC("");

			std::wstring include_layers;
			std::wstring exclude_layers;

			get_int(IDC_SCALE, hive.scale);
			get_combobox_index(IDC_WAVE_TYPE, hive.wave_type);
			get_combobox_index(IDC_UPDATE_MODE, hive.update_mode);
			get_combobox_index(IDC_XOR_MODE, hive.xor_mode);
			get_combobox_index(IDC_VOLUME_MODE, hive.volume_mode);
			get_check(IDC_SHOW_WAVE, hive.show_wave);
			get_check(IDC_SHOW_TEXT, hive.show_text);
			get_check(IDC_NAMECAGE, hive.namecage);
			get_check(IDC_BEHIND, hive.behind);
			get_text(IDC_INCLUDE_LAYERS, include_layers);
			get_text(IDC_EXCLUDE_LAYERS, exclude_layers);
			get_text(IDC_INCLUDE_FOLDER, hive.include_folder);
			get_text(IDC_EXCLUDE_FOLDER, hive.exclude_folder);
			get_int(IDC_NAMECAGE_OFFSET, hive.namecage_offset);
			get_int(IDC_MAX_THREAD_COUNT, hive.max_thread_count);

			hive.include_layers = string_to_layers(include_layers);
			hive.exclude_layers = string_to_layers(exclude_layers);

			magi.exin.invalidate();
		}

		//
		// ダイアログの初期化処理です。
		//
		virtual void on_init_dialog() override
		{
			MY_TRACE_FUNC("");

			init_combobox(IDC_WAVE_TYPE, _T("中央"), _T("下"), _T("上"));
			init_combobox(IDC_UPDATE_MODE, _T("更新しない"), _T("更新する"));
			init_combobox(IDC_XOR_MODE, _T("通常"), _T("XOR"), _T("NotXOR"), _T("Not"));
			init_combobox(IDC_VOLUME_MODE, _T("ピーク"), _T("RMS"));

			using namespace my::layout;

			auto margin_value = 2;
			auto margin = RECT { margin_value, margin_value, margin_value, margin_value };
			auto stat_margin = RECT { 0, 0, margin_value, 0 };
			auto base_size = get_base_size();
			auto row = std::make_shared<RelativePos>(base_size + margin_value * 2);
			auto rest = std::make_shared<AbsolutePos>(1, 1, 1);
			auto size_s = std::make_shared<RelativePos>(base_size * 3 + margin_value * 2);
			auto size_m = std::make_shared<RelativePos>(base_size * 5 + margin_value * 2);
			auto size_l = std::make_shared<RelativePos>(base_size * 6 + margin_value * 2);
			auto spin = std::make_shared<RelativePos>(base_size);

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, size_s, margin, ctrl(IDC_INCLUDE_LAYERS_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, rest, margin, ctrl(IDC_INCLUDE_LAYERS));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, size_s, margin, ctrl(IDC_EXCLUDE_LAYERS_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, rest, margin, ctrl(IDC_EXCLUDE_LAYERS));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, size_s, margin, ctrl(IDC_INCLUDE_FOLDER_STAT));
				{
					auto sub_node = node->add_pane(c_axis.c_horz, c_align.c_left, rest, margin);
					sub_node->add_pane(c_axis.c_horz, c_align.c_right, spin, {}, ctrl(IDC_INCLUDE_FOLDER_DIR));
					sub_node->add_pane(c_axis.c_horz, c_align.c_right, rest, {}, ctrl(IDC_INCLUDE_FOLDER));
				}
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, size_s, margin, ctrl(IDC_EXCLUDE_FOLDER_STAT));
				{
					auto sub_node = node->add_pane(c_axis.c_horz, c_align.c_left, rest, margin);
					sub_node->add_pane(c_axis.c_horz, c_align.c_right, spin, {}, ctrl(IDC_EXCLUDE_FOLDER_DIR));
					sub_node->add_pane(c_axis.c_horz, c_align.c_right, rest, {}, ctrl(IDC_EXCLUDE_FOLDER));
				}
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, size_s, margin, ctrl(IDC_SCALE_STAT));
				{
					auto sub_node = node->add_pane(c_axis.c_horz, c_align.c_left, size_s, margin);
					sub_node->add_pane(c_axis.c_horz, c_align.c_right, spin, {}, ctrl(IDC_SCALE_SPIN));
					sub_node->add_pane(c_axis.c_horz, c_align.c_right, rest, {}, ctrl(IDC_SCALE));
				}
				node->add_pane(c_axis.c_horz, c_align.c_left, size_s, margin, ctrl(IDC_UPDATE_MODE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, size_l, margin, ctrl(IDC_UPDATE_MODE));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, size_s, margin, ctrl(IDC_WAVE_TYPE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, size_s, margin, ctrl(IDC_WAVE_TYPE));
				node->add_pane(c_axis.c_horz, c_align.c_left, size_s, margin, ctrl(IDC_XOR_MODE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, size_s, margin, ctrl(IDC_XOR_MODE));
				node->add_pane(c_axis.c_horz, c_align.c_left, size_s, margin, ctrl(IDC_VOLUME_MODE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, size_s, margin, ctrl(IDC_VOLUME_MODE));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, size_l, margin, ctrl(IDC_SHOW_WAVE));
				node->add_pane(c_axis.c_horz, c_align.c_left, size_l, margin, ctrl(IDC_SHOW_TEXT));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, size_l, margin, ctrl(IDC_BEHIND));
				node->add_pane(c_axis.c_horz, c_align.c_left, size_l, margin, ctrl(IDC_NAMECAGE));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, size_m, margin, ctrl(IDC_NAMECAGE_OFFSET_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, size_s, margin, ctrl(IDC_NAMECAGE_OFFSET));
				node->add_pane(c_axis.c_horz, c_align.c_left, size_m, margin, ctrl(IDC_MAX_THREAD_COUNT_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, size_s, margin, ctrl(IDC_MAX_THREAD_COUNT));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, size_m, margin, ctrl(IDC_CLEAR_CACHES));
				node->add_pane(c_axis.c_horz, c_align.c_left, size_m, margin, ctrl(IDC_SELECT_PEN_COLOR));
				node->add_pane(c_axis.c_horz, c_align.c_left, size_m, margin, ctrl(IDC_SELECT_BRUSH_COLOR));
			}
		}

		//
		// ダイアログのコマンド処理です。
		//
		virtual void on_command(UINT code, UINT control_id, HWND control) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}", code, control_id, control);

			switch (control_id)
			{
			// エディットボックス
			case IDC_SCALE:
			case IDC_INCLUDE_LAYERS:
			case IDC_EXCLUDE_LAYERS:
			case IDC_INCLUDE_FOLDER:
			case IDC_EXCLUDE_FOLDER:
			case IDC_NAMECAGE_OFFSET:
			case IDC_MAX_THREAD_COUNT:
				{
					if (code == EN_UPDATE)
						update_config();

					break;
				}
			// コンボボックス
			case IDC_UPDATE_MODE:
			case IDC_WAVE_TYPE:
			case IDC_XOR_MODE:
			case IDC_VOLUME_MODE:
				{
					if (code == CBN_SELCHANGE)
						update_config();

					break;
				}
			// ボタン
			case IDC_SELECT_PEN_COLOR:
				{
					if (IDOK == magi.exin.show_color_dialog(0, &hive.pen_color, 2))
						magi.exin.invalidate();

					break;
				}
			case IDC_SELECT_BRUSH_COLOR:
				{
					if (IDOK == magi.exin.show_color_dialog(0, &hive.brush_color, 2))
						magi.exin.invalidate();

					break;
				}
			case IDC_CLEAR_CACHES:
				{
					app->clear_caches();

					break;
				}
			case IDC_INCLUDE_FOLDER_DIR:
				{
					auto folder = pick_folder(*this, L"対象フォルダを選択", hive.include_folder);
					if (folder.length())
					{
						set_text(IDC_INCLUDE_FOLDER, folder);
						update_config();
					}

					break;
				}
			case IDC_EXCLUDE_FOLDER_DIR:
				{
					auto folder = pick_folder(*this, L"除外フォルダを選択", hive.exclude_folder);
					if (folder.length())
					{
						set_text(IDC_EXCLUDE_FOLDER, folder);
						update_config();
					}

					break;
				}
			// チェックボックス
			case IDC_SHOW_WAVE:
			case IDC_SHOW_TEXT:
			case IDC_NAMECAGE:
			case IDC_BEHIND:
				{
					update_config();

					break;
				}
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
					MY_TRACE_FUNC("WM_NOTIFY, {/hex}, {/hex}", wParam, lParam);

					switch (wParam)
					{
					case IDC_SCALE_SPIN:
						{
							auto nmh = (NMHDR*)lParam;
							if (nmh->code == UDN_DELTAPOS)
							{
								auto scale = get_int(IDC_SCALE);

								auto nm = (NM_UPDOWN*)nmh;
								if (nm->iDelta < 0)
								{
									scale += 10;
								}
								else
								{
									scale -= 10;
								}

								scale = std::max<int>(scale, 0);
								scale = std::min<int>(scale, 2000);

								set_int(IDC_SCALE, scale);
							}

							break;
						}
					}

					break;
				}
			}

			return __super::on_dlg_proc(hwnd, message, wParam, lParam);
		}
	} addin_dialog;
}
