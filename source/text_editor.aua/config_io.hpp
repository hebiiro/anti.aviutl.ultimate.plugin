#pragma once

namespace apn::text_editor
{
	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct ConfigIO : StdConfigIOUseHive<hive>
	{
		//
		// コンフィグが更新されたのでコントロールに適用します。
		//
		virtual BOOL update() override
		{
			MY_TRACE_FUNC("");

			if (!hive.font.name.empty())
			{
				// HFONTを作成します。
				hive.font.handle.reset(::CreateFontW(
					hive.font.height, 0,
					0, 0, 0, 0, 0, 0, DEFAULT_CHARSET,
					0, 0, 0, hive.font.pitch, hive.font.name.c_str()));
			}

			return app->update();
		}

		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			read_int(root, "delay", hive.delay);
			read_bool(root, "zoomable", hive.zoomable);
			{
				n_json font_node;
				read_child_node(root, "font", font_node);
				read_string(font_node, "name", hive.font.name);
				read_int(font_node, "height", hive.font.height);
				read_int(font_node, "pitch", hive.font.pitch);
			}
			read_window_pos(root, "addin_window", addin_window);

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			write_int(root, "delay", hive.delay);
			write_bool(root, "zoomable", hive.zoomable);
			{
				n_json font_node;
				write_string(font_node, "name", hive.font.name);
				write_int(font_node, "height", hive.font.height);
				write_int(font_node, "pitch", hive.font.pitch);
				write_child_node(root, "font", font_node);
			}
			write_window_pos(root, "addin_window", addin_window);

			return TRUE;
		}
	} config_io;
}
