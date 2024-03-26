#pragma once

namespace apn::filter_drag
{
	using namespace my::json;

	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct ConfigIO : StdConfigIO
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			aim_src.available = TRUE;
			aim_src.alpha = 100;
			aim_src.color = RGB(0x00, 0x00, 0xff);

			aim_dst.available = TRUE;
			aim_dst.alpha = 100;
			aim_dst.color = RGB(0xff, 0x00, 0x00);

			sight.config.available = FALSE;
			sight.config.alpha = 200;
			sight.config.pen_color = Color(192, 0, 0, 0);
			sight.config.pen_width = 2.0f;
			sight.config.brush_color = Color(255, 255, 255, 255);
			sight.config.base = 16;
			sight.config.width = 8;
			sight.config.font_name = L"Segoe UI";
			sight.config.font_size = 32.0f;
			sight.config.rotate = 7.77f;
			sight.config.start_offset = Point(0, 100);

			if (!__super::init(hive.instance)) return FALSE;

			sight.update_context();

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			return __super::exit(hive.instance);
		}

		//
		// コンフィグを読み込みます。
		//
		virtual BOOL on_load(ptree& root) override
		{
			get_bool(root, "use_shift", keyboard_hook.use_shift);
			get_bool(root, "use_ctrl", keyboard_hook.use_ctrl);
			get_bool(root, "use_alt", keyboard_hook.use_alt);
			get_bool(root, "use_win", keyboard_hook.use_win);

			get_int(root, "aim_src.alpha", aim_src.alpha);
			get_color(root, "aim_src.color", aim_src.color);

			get_int(root, "aim_dst.alpha", aim_dst.alpha);
			get_color(root, "aim_dst.color", aim_dst.color);

			get_int(root, "sight.alpha", sight.config.alpha);
			get_color(root, "sight.pen_color", sight.config.pen_color);
			get_float(root, "sight.pen_width", sight.config.pen_width);
			get_color(root, "sight.brush_color", sight.config.brush_color);
			get_int(root, "sight.base", sight.config.base);
			get_int(root, "sight.width", sight.config.width);
			get_string(root, "sight.font_name", sight.config.font_name);
			get_float(root, "sight.font_size", sight.config.font_size);
			get_float(root, "sight.rotate", sight.config.rotate);
			get_point(root, "sight.start_offset", sight.config.start_offset);

			return TRUE;
		}

		//
		// コンフィグをファイルに保存します。
		//
		virtual BOOL on_save(std::ofstream& ofs) override
		{
			ptree root;

			set_bool(root, "use_shift", keyboard_hook.use_shift);
			set_bool(root, "use_ctrl", keyboard_hook.use_ctrl);
			set_bool(root, "use_alt", keyboard_hook.use_alt);
			set_bool(root, "use_win", keyboard_hook.use_win);

			set_bool(root, "aim_src.available", aim_src.available);
			set_int(root, "aim_src.alpha", aim_src.alpha);
			set_color(root, "aim_src.color", aim_src.color);

			set_bool(root, "aim_dst.available", aim_dst.available);
			set_int(root, "aim_dst.alpha", aim_dst.alpha);
			set_color(root, "aim_dst.color", aim_dst.color);

			set_bool(root, "sight.available", sight.config.available);
			set_int(root, "sight.alpha", sight.config.alpha);
			set_color(root, "sight.pen_color", sight.config.pen_color);
			set_float(root, "sight.pen_width", sight.config.pen_width);
			set_color(root, "sight.brush_color", sight.config.brush_color);
			set_int(root, "sight.base", sight.config.base);
			set_int(root, "sight.width", sight.config.width);
			set_string(root, "sight.font_name", sight.config.font_name);
			set_float(root, "sight.font_size", sight.config.font_size);
			set_float(root, "sight.rotate", sight.config.rotate);
			set_point(root, "sight.start_offset", sight.config.start_offset);

			write_json(ofs, root);

			return TRUE;
		}

		virtual int message_box(const std::wstring& text) override
		{
			return hive.message_box(text);
		}
	} config_io;
}
