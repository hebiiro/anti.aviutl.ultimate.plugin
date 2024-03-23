#pragma once

namespace apn::image_export
{
	//
	// このクラスはアドインダイアログです。
	//
	inline struct AddinDialog : StdAddinDialog<IDD_MAIN_DIALOG>
	{
		my::aviutl::SliderPanel<> quality;

		//
		// コントロールを更新します。
		//
		virtual void on_update_controls() override
		{
			MY_TRACE_FUNC("");

			quality.set_value(hive.quality);
			set_combobox(IDC_MODE, hive.mode);
			set_text(IDC_FILE_NAME, hive.file_name);
			set_int(IDC_INDEX, hive.index);
			set_int(IDC_NUMBER_WIDTH, hive.number_width);
		}

		//
		// コンフィグを更新します。
		//
		virtual void on_update_config() override
		{
			MY_TRACE_FUNC("");

			quality.get_value(hive.quality);
			get_combobox(IDC_MODE, hive.mode);
			get_text(IDC_FILE_NAME, hive.file_name);
			get_int(IDC_INDEX, hive.index);
			get_int(IDC_NUMBER_WIDTH, hive.number_width);
		}

		//
		// ダイアログの初期化処理です。
		//
		virtual void on_init_dialog() override
		{
			MY_TRACE_FUNC("");

			quality.init(0, 100, *this, IDC_QUALITY_SLIDER, IDC_QUALITY);

			init_combobox(IDC_MODE,
				_T("保存の度に選択"),
				_T("予め指定されたファイルに保存"),
				_T("予め指定されたファイルに上書き保存"));

			using namespace my::layout;

			auto margin_value = 2;
			auto margin = RECT { margin_value, margin_value, margin_value, margin_value };
			auto base_size = get_base_size();
			auto row_size = base_size + margin_value * 2;
			auto fixed_size = base_size * 3 + margin_value * 2;
			auto row = std::make_shared<RelativePos>(row_size);
			auto fixed = std::make_shared<RelativePos>(fixed_size);
			auto quarter = std::make_shared<AbsolutePos>(1, 4);
			auto half = std::make_shared<AbsolutePos>(1, 2);
			auto full = std::make_shared<AbsolutePos>(2, 2);
			auto center_left = std::make_shared<AbsolutePos>(1, 4);
			auto center = std::make_shared<AbsolutePos>(2, 4);
			auto center_right = std::make_shared<AbsolutePos>(3, 4);
			auto right = std::make_shared<AbsolutePos>(4, 4);

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, fixed, margin, ctrl(IDC_EXPORT_FRAME_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, fixed, margin, ctrl(IDC_EXPORT_FRAME_RGBA));
				node->add_pane(c_axis.c_horz, c_align.c_left, fixed, margin, ctrl(IDC_EXPORT_FRAME_RGB));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, fixed, margin, ctrl(IDC_EXPORT_ITEM_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, fixed, margin, ctrl(IDC_EXPORT_ITEM_RGBA));
				node->add_pane(c_axis.c_horz, c_align.c_left, fixed, margin, ctrl(IDC_EXPORT_ITEM_RGB));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, fixed, margin, ctrl(IDC_MODE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_MODE));
			}

			{
				auto full = std::make_shared<RelativePos>(1, 1, -row_size);

				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_FILE_NAME));
				node->add_pane(c_axis.c_horz, c_align.c_left, row, margin, ctrl(IDC_FILE_NAME_DIR));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);

				{
					auto node2 = node->add_pane(c_axis.c_horz, c_align.c_left, half);
					node2->add_pane(c_axis.c_horz, c_align.c_left, fixed, margin, ctrl(IDC_INDEX_STAT));
					node2->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_INDEX));
				}

				{
					auto node2 = node->add_pane(c_axis.c_horz, c_align.c_left, full);
					node2->add_pane(c_axis.c_horz, c_align.c_left, fixed, margin, ctrl(IDC_NUMBER_WIDTH_STAT));
					node2->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_NUMBER_WIDTH));
				}
			}

			{
				auto full = std::make_shared<RelativePos>(1, 1, -fixed_size);

				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, fixed, margin, ctrl(IDC_QUALITY_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_QUALITY_SLIDER));
				node->add_pane(c_axis.c_horz, c_align.c_left, fixed, margin, ctrl(IDC_QUALITY));
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
			case IDC_EXPORT_FRAME_RGB: export_image(FALSE, FALSE); break;
			case IDC_EXPORT_FRAME_RGBA: export_image(TRUE, FALSE); break;
			case IDC_EXPORT_ITEM_RGB: export_image(FALSE, TRUE); break;
			case IDC_EXPORT_ITEM_RGBA: export_image(TRUE, TRUE); break;
			case IDC_FILE_NAME_DIR: on_file_name_dir(); break;

			// エディットボックス
			case IDC_FILE_NAME:
			case IDC_INDEX:
			case IDC_NUMBER_WIDTH: if (code == EN_CHANGE) update_config(); break;

			// スライダー
			case IDC_QUALITY: if (code == EN_CHANGE) hive.quality = quality.on_change_text(); break;

			// コンボボックス
			case IDC_MODE: if (code == CBN_SELCHANGE) update_config(); break;
			}
		}

		//
		// 自動モード用のファイル名を返します。
		//
		std::wstring get_file_name_for_auto_mode()
		{
			MY_TRACE_FUNC("");

			// 基本となるファイル名を取得します。
			auto file_name = hive.file_name;
			MY_TRACE_STR(file_name);

			// AviUtlの現在のプロジェクト名を取得します。
			AviUtl::SysInfo si = {};
			magi.fp->exfunc->get_sys_info(magi.exin.get_editp(), &si);
			auto project = std::filesystem::path(si.project_name).stem();
			MY_TRACE_STR(project);

			// AviUtlの現在のフレーム番号を文字列に変換します。
			auto frame = magi.fp->exfunc->get_frame(magi.exin.get_editp());
			std::wstringstream frame_ss;
			frame_ss << std::setw(hive.number_width) << std::setfill(L'0') << frame;
			MY_TRACE_STR(frame_ss.str());

			// インデックスを文字列に変換します。
			auto index = hive.index;
			std::wstringstream index_ss;
			index_ss << std::setw(hive.number_width) << std::setfill(L'0') << index;
			MY_TRACE_STR(index_ss.str());

			// ファイル名を置換します。
			file_name = my::replace(file_name, L"<project>", project);
			file_name = my::replace(file_name, L"<frame>", frame_ss.str());
			file_name = my::replace(file_name, L"<index>", index_ss.str());

			// インデックスをインクリメントします。
			hive.index = index + 1;

			// インデックスを変更したのでコントロールに反映させます。
			update_controls();

			// 置換後のファイル名を返します。
			return file_name.c_str();
		}

		//
		// 出力先ファイル名を返します。
		//
		std::wstring get_file_name()
		{
			MY_TRACE_FUNC("");

			switch (hive.mode)
			{
			case hive.c_mode.c_manual:
				{
					// ユーザーが選択したファイル名を返します。
					return hive.app->browse();
				}
			case hive.c_mode.c_auto:
			case hive.c_mode.c_overwrite:
				{
					// 自動保存用のファイル名を取得します。
					auto file_name = get_file_name_for_auto_mode();
					if (file_name.empty()) break;

					// 上書きモードの場合は
					if (hive.mode == hive.c_mode.c_overwrite)
						return file_name; // 自動保存用ファイル名を返します。

					// ファイルが存在しない場合は
					if (::GetFileAttributesW(file_name.c_str()) == -1)
						return file_name; // 自動保存用ファイル名を返します。

					// ファイルを上書きするかどうかをユーザーに確認します。
					auto str = std::format(L"{}\nはすでに存在します。上書きしますか?", file_name);
					if (IDYES == hive.message_box(str, *this, MB_YESNO))
						return file_name;

					break;
				}
			}

			return std::wstring();
		}

		//
		// アクションに画像をエクスポートさせます。
		//
		BOOL export_image(BOOL has_alpha, BOOL sel_item_only)
		{
			auto file_name = get_file_name();
			if (file_name.empty()) return FALSE;
			return hive.app->export_image(file_name.c_str(), has_alpha, sel_item_only);
		}

		//
		// ファイル選択ダイアログを表示します。
		//
		BOOL on_file_name_dir()
		{
			MY_TRACE_FUNC("");

			auto file_name = hive.app->browse();
			if (file_name.empty()) return FALSE;

			return set_text(IDC_FILE_NAME, file_name.c_str());
		}

		//
		// ダイアログプロシージャです。
		//
		virtual INT_PTR on_dlg_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_HSCROLL:
				{
					MY_TRACE_FUNC("WM_HSCROLL, {:#010x}, {:#010x}", wParam, lParam);

					auto control = (HWND)lParam;

					if (control == quality.slider) hive.quality = quality.on_change_pos();

					break;
				}
			}

			return __super::on_dlg_proc(hwnd, message, wParam, lParam);
		}
	} addin_dialog;
}
