#pragma once

namespace apn::one_for_text
{
	//
	// このクラスはアドインダイアログです。
	//
	inline struct AddinDialog : AddinDialogInterface
	{
		//
		// コンフィグを更新します。
		//
		virtual void on_update_config() override
		{
			MY_TRACE_FUNC("");

			get_text(IDC_TEXT_FILE_NAME, hive.text_file_name);
			get_text(IDC_TEXT_EDITOR_PATH, hive.text_editor_path);
			get_int(IDC_LAYER_FRONT, hive.layer_front);
			get_int(IDC_LAYER_BACK, hive.layer_back);
		}

		//
		// コントロールを更新します。
		//
		virtual void on_update_controls() override
		{
			MY_TRACE_FUNC("");

			set_text(IDC_TEXT_FILE_NAME, hive.text_file_name);
			set_text(IDC_TEXT_EDITOR_PATH, hive.text_editor_path);
			set_int(IDC_LAYER_FRONT, hive.layer_front);
			set_int(IDC_LAYER_BACK, hive.layer_back);
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
			auto base_size = get_base_size();
			auto row_size = base_size + margin_value * 2;
			auto row = std::make_shared<RelativePos>(row_size);
			auto button = std::make_shared<RelativePos>(base_size * 5);
			auto stat = std::make_shared<RelativePos>(base_size * 3);
			auto editbox = std::make_shared<RelativePos>(base_size * 2);
			auto half = std::make_shared<AbsolutePos>(1, 2);
			auto full = std::make_shared<AbsolutePos>(2, 2);

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_WRITE_FILE));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_SHOW_FILE));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_READ_FILE));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_LAYER_FRONT_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, editbox, margin, ctrl(IDC_LAYER_FRONT));
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_LAYER_BACK_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, editbox, margin, ctrl(IDC_LAYER_BACK));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_TEXT_FILE_NAME_STAT));
			}

			{
				auto full = std::make_shared<RelativePos>(1, 1, -row_size);

				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row, margin);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, {}, ctrl(IDC_TEXT_FILE_NAME));
				node->add_pane(c_axis.c_horz, c_align.c_left, row, {}, ctrl(IDC_TEXT_FILE_NAME_REF));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_TEXT_EDITOR_PATH_STAT));
			}

			{
				auto full = std::make_shared<RelativePos>(1, 1, -row_size);

				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row, margin);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, {}, ctrl(IDC_TEXT_EDITOR_PATH));
				node->add_pane(c_axis.c_horz, c_align.c_left, row, {}, ctrl(IDC_TEXT_EDITOR_PATH_REF));
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
			case IDC_WRITE_FILE: app->write_file(); break;
			case IDC_SHOW_FILE: app->show_file(); break;
			case IDC_READ_FILE: app->read_file(); break;
			case IDC_TEXT_FILE_NAME_REF:
				{
					auto file_name = get_save_file_name(
						*this,
						L"保存先テキストファイルを選択",
						L"テキストファイル (*.txt)\0*.txt\0"
						L"すべてのファイル (*.*)\0*.*\0",
						hive.text_file_name.c_str(),
						L"txt");

					if (file_name.length())
					{
						hive.text_file_name = file_name;

						update_controls();
					}

					break;
				}
			case IDC_TEXT_EDITOR_PATH_REF:
				{
					auto file_name = get_open_file_name(
						*this,
						L"実行ファイルを選択",
						L"実行ファイル (*.exe)\0*.exe\0"
						L"すべてのファイル (*.*)\0*.*\0",
						hive.text_editor_path.c_str());

					if (file_name.length())
					{
						hive.text_editor_path = file_name;

						update_controls();
					}

					break;
				}
			// エディットボックス
			case IDC_TEXT_FILE_NAME:
			case IDC_TEXT_EDITOR_PATH:
			case IDC_LAYER_FRONT:
			case IDC_LAYER_BACK: if (code == EN_UPDATE) update_config(); break;
			}
		}
	} addin_dialog;
}
