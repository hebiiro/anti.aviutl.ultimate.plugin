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
		// コンフィグを保存します。
		//
		BOOL write()
		{
			MY_TRACE_FUNC("");

			return write_file(hive.config_file_name, hive);
		}

		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(ptree& root) override
		{
			MY_TRACE_FUNC("");

			using namespace my::json;

			get_bool(root, "unicode_input", hive.unicode_input);
			get_bool(root, "ctrl_a", hive.ctrl_a);
			get_bool(root, "zoomable", hive.zoomable);
			get_int(root, "delta", hive.delta);
			get_string(root, "font.name", hive.font.name);
			get_int(root, "font.height", hive.font.height);
			get_int(root, "font.pitch", hive.font.pitch);

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
		// ノードにコンフィグを保存します。
		//
		virtual BOOL write_node(ptree& root) override
		{
			MY_TRACE_FUNC("");

			using namespace my::json;

			set_bool(root, "unicode_input", hive.unicode_input);
			set_bool(root, "ctrl_a", hive.ctrl_a);
			set_bool(root, "zoomable", hive.zoomable);
			set_int(root, "delta", hive.delta);
			set_string(root, "font.name", hive.font.name);
			set_int(root, "font.height", hive.font.height);
			set_int(root, "font.pitch", hive.font.pitch);

			return TRUE;
		}
	} config_io;
}
