#pragma once

namespace apn::volume_meter::main_thread
{
	//
	// このクラスはメインスレッドのコントローラーです。
	//
	inline struct controller_t : paint_option_dialog_t::listener_t
	{
		//
		// この仮想関数は描画設定が変更されたときに呼び出されます。
		//
		virtual void on_change_paint_option(const paint_option_t& paint_option) override
		{
			// 描画設定をセットします。
			set_paint_option(paint_option);
		}

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// コントローラーを描画設定ダイアログのリスナーに登録します。
			paint_option_dialog.listener = this;

			if (!view.init())
			{
				hive.message_box(L"メインスレッドのビューの初期化に失敗しました\n");

				return FALSE;
			}

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			view.exit();

			return TRUE;
		}

		//
		// コンフィグを読み込みます。
		//
		BOOL read_node(n_json& root)
		{
			MY_TRACE_FUNC("");

			read_color(root, "paint_option.background_color", model.paint_option.background_color);
			read_color(root, "paint_option.warning_color", model.paint_option.warning_color);
			read_color(root, "paint_option.level_color", model.paint_option.level_color);

			read_int(root, "paint_option.level_width", model.paint_option.level_width);
			read_int(root, "paint_option.min_db", model.paint_option.min_db);
			read_int(root, "paint_option.max_db", model.paint_option.max_db);
			read_int(root, "paint_option.gamma", model.paint_option.gamma);
			read_int(root, "paint_option.nb_separators", model.paint_option.nb_separators);
			read_int(root, "paint_option.separator_width", model.paint_option.separator_width);
			read_int(root, "paint_option.text_area_height", model.paint_option.text_area_height);

			for (size_t i = 0; i < std::size(model.paint_option.c_zone.c_labels); i++)
			{
				auto name = "paint_option."s + model.paint_option.c_zone.c_labels[i];

				read_int(root, name + ".db", model.paint_option.zones[i].db);
				read_color(root, name + ".color", model.paint_option.zones[i].color);
			}

			for (size_t i = 0; i < std::size(model.paint_option.c_font.c_labels); i++)
			{
				auto name = "paint_option."s + model.paint_option.c_font.c_labels[i];

				read_string(root, name + ".name", model.paint_option.fonts[i].name);
				read_int(root, name + ".width", model.paint_option.fonts[i].width);
				read_int(root, name + ".height", model.paint_option.fonts[i].height);
				read_color(root, name + ".text_color", model.paint_option.fonts[i].text_color);
				read_color(root, name + ".shadow_color", model.paint_option.fonts[i].shadow_color);
				read_point(root, name + ".text_offset", model.paint_option.fonts[i].text_offset);
				read_point(root, name + ".shadow_offset", model.paint_option.fonts[i].shadow_offset);
			}

			read_window_pos(root, "view", view);

			return TRUE;
		}

		//
		// コンフィグを書き込みます。
		//
		BOOL write_node(n_json& root)
		{
			MY_TRACE_FUNC("");

			write_color(root, "paint_option.background_color", model.paint_option.background_color);
			write_color(root, "paint_option.warning_color", model.paint_option.warning_color);
			write_color(root, "paint_option.level_color", model.paint_option.level_color);

			write_int(root, "paint_option.level_width", model.paint_option.level_width);
			write_int(root, "paint_option.min_db", model.paint_option.min_db);
			write_int(root, "paint_option.max_db", model.paint_option.max_db);
			write_int(root, "paint_option.gamma", model.paint_option.gamma);
			write_int(root, "paint_option.nb_separators", model.paint_option.nb_separators);
			write_int(root, "paint_option.separator_width", model.paint_option.separator_width);
			write_int(root, "paint_option.text_area_height", model.paint_option.text_area_height);

			for (size_t i = 0; i < std::size(model.paint_option.c_zone.c_labels); i++)
			{
				auto name = "paint_option."s + model.paint_option.c_zone.c_labels[i];

				write_int(root, name + ".db", model.paint_option.zones[i].db);
				write_color(root, name + ".color", model.paint_option.zones[i].color);
			}

			for (size_t i = 0; i < std::size(model.paint_option.c_font.c_labels); i++)
			{
				auto name = "paint_option."s + model.paint_option.c_font.c_labels[i];

				write_string(root, name + ".name", model.paint_option.fonts[i].name);
				write_int(root, name + ".width", model.paint_option.fonts[i].width);
				write_int(root, name + ".height", model.paint_option.fonts[i].height);
				write_color(root, name + ".text_color", model.paint_option.fonts[i].text_color);
				write_color(root, name + ".shadow_color", model.paint_option.fonts[i].shadow_color);
				write_point(root, name + ".text_offset", model.paint_option.fonts[i].text_offset);
				write_point(root, name + ".shadow_offset", model.paint_option.fonts[i].shadow_offset);
			}

			write_window_pos(root, "view", view);

			return TRUE;
		}

		//
		// 描画設定をセットします。
		//
		BOOL set_paint_option(const paint_option_t& paint_option)
		{
//			MY_TRACE_FUNC("");

			// サブスレッドに描画設定を送信します。
			return ::PostThreadMessage(
				sub_thread::controller.tid, hive.c_message.c_set_paint_option,
				0, (LPARAM)std::make_unique<paint_option_t>(paint_option).release());
		}

		//
		// 生の音声データをセットします。
		//
		BOOL set_raw_audio_data(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip)
		{
//			MY_TRACE_FUNC("");

			// 対応していない音声チャンネル数の場合は何もしません。
			if (fpip->audio_ch != 1 && fpip->audio_ch != 2)
				return FALSE;

			// 対応していない音声サンプル数の場合は何もしません。
			if (fpip->audio_n == 0)
				return FALSE;

			// サブスレッドに生の音声データを送信します。
			return ::PostThreadMessage(
				sub_thread::controller.tid, hive.c_message.c_set_raw_audio_data,
				0, (LPARAM)std::make_unique<raw_audio_data_t>(fp, fpip).release());
		}

		//
		// 映像信号タイミングをセットします。
		//
		BOOL set_video_timing(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip)
		{
//			MY_TRACE_FUNC("");

			// サブスレッドに映像信号タイミングを送信します。
			return ::PostThreadMessage(
				sub_thread::controller.tid, hive.c_message.c_set_video_timing,
				0, (LPARAM)std::make_unique<timing_t>(fp, fpip).release());
		}
	} controller;
}
