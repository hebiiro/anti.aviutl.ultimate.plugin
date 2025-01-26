#pragma once

namespace apn::item_wave
{
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
			MY_TRACE_FUNC("");

			hive.config_file_name = magi.get_config_file_name(hive.instance);
			MY_TRACE_STR(hive.config_file_name);

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// コンフィグを読み込みます。
		//
		BOOL read()
		{
			MY_TRACE_FUNC("");

			return read_file(hive.config_file_name, hive);
		}

		//
		// コンフィグを書き込みます。
		//
		BOOL write()
		{
			MY_TRACE_FUNC("");

			return write_file(hive.config_file_name, hive);
		}

		//
		// コンフィグが更新されたのでコントロールに適用します。
		//
		virtual BOOL update() override
		{
			MY_TRACE_FUNC("");

			return addin_dialog.update_controls();
		}

		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			std::wstring include_layers;
			std::wstring exclude_layers;

			read_int(root, "scale", hive.scale);
			read_label(root, "wave_type", hive.wave_type, hive.c_wave_type.labels);
			read_label(root, "update_mode", hive.update_mode, hive.c_update_mode.labels);
			read_label(root, "xor_mode", hive.xor_mode, hive.c_xor_mode.labels);
			read_color(root, "pen_color", hive.pen_color);
			read_color(root, "brush_color", hive.brush_color);
			read_bool(root, "show_wave", hive.show_wave);
			read_bool(root, "show_text", hive.show_text);
			read_bool(root, "namecage", hive.namecage);
			read_bool(root, "behind", hive.behind);
			read_string(root, "include_layers", include_layers);
			read_string(root, "exclude_layers", exclude_layers);
			read_string(root, "include_folder", hive.include_folder);
			read_string(root, "exclude_folder", hive.exclude_folder);
			read_window_pos(root, "addin_window", addin_window);

			hive.include_layers = string_to_layers(include_layers);
			hive.exclude_layers = string_to_layers(exclude_layers);

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			auto include_layers = layers_to_string(hive.include_layers);
			auto exclude_layers = layers_to_string(hive.exclude_layers);

			write_int(root, "scale", hive.scale);
			write_label(root, "wave_type", hive.wave_type, hive.c_wave_type.labels);
			write_label(root, "update_mode", hive.update_mode, hive.c_update_mode.labels);
			write_label(root, "xor_mode", hive.xor_mode, hive.c_xor_mode.labels);
			write_color(root, "pen_color", hive.pen_color);
			write_color(root, "brush_color", hive.brush_color);
			write_bool(root, "show_wave", hive.show_wave);
			write_bool(root, "show_text", hive.show_text);
			write_bool(root, "namecage", hive.namecage);
			write_bool(root, "behind", hive.behind);
			write_string(root, "include_layers", include_layers);
			write_string(root, "exclude_layers", exclude_layers);
			write_string(root, "include_folder", hive.include_folder);
			write_string(root, "exclude_folder", hive.exclude_folder);
			write_window_pos(root, "addin_window", addin_window);

			return TRUE;
		}
	} config_io;
}
