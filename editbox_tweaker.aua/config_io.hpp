#pragma once

namespace apn::editbox_tweaker
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

			if (!hive.font.name.empty())
			{
				// HFONTを作成します。
				// 複数行エディットボックスのWM_SETFONTでこのハンドルが渡されます。
				hive.font.handle.reset(::CreateFontW(
					hive.font.height, 0,
					0, 0, 0, 0, 0, 0, DEFAULT_CHARSET,
					0, 0, 0, hive.font.pitch, hive.font.name.c_str()));
			}

			return TRUE;
		}

		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			get_bool(root, "unicode_input", hive.unicode_input);
			get_bool(root, "ctrl_a", hive.ctrl_a);
			get_bool(root, "zoomable", hive.zoomable);
			get_int(root, "delta", hive.delta);
			{
				n_json font_node;
				get_child_node(root, "font", font_node);
				get_string(font_node, "name", hive.font.name);
				get_int(font_node, "height", hive.font.height);
				get_int(font_node, "pitch", hive.font.pitch);
			}

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			set_bool(root, "unicode_input", hive.unicode_input);
			set_bool(root, "ctrl_a", hive.ctrl_a);
			set_bool(root, "zoomable", hive.zoomable);
			set_int(root, "delta", hive.delta);
			{
				n_json font_node;
				set_string(font_node, "name", hive.font.name);
				set_int(font_node, "height", hive.font.height);
				set_int(font_node, "pitch", hive.font.pitch);
				set_child_node(root, "font", font_node);
			}

			return TRUE;
		}
	} config_io;
}
