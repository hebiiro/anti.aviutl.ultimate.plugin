#pragma once

namespace apn::filer
{
	//
	// ウィンドウ位置をノードから読み込みます。
	//
	inline void read_window_pos(const n_json& node, HWND hwnd, UINT flags = 0, UINT show_cmd = -1)
	{
		// ウィンドウプレースメントを取得します。
		WINDOWPLACEMENT wp = { sizeof(wp) };
		::GetWindowPlacement(hwnd, &wp);

		wp.flags = WPF_SETMINPOSITION;
		if (!::IsWindowVisible(hwnd)) wp.showCmd = SW_HIDE;

		if (show_cmd == -1)
			read_int(node, "show_cmd", wp.showCmd);
		else
			wp.showCmd = show_cmd;
/*
		auto normal_rc = RECT {};
		read_rect(node, "normal", normal_rc);

		auto min_pos = POINT {};
		read_point(node, "min", min_pos);

		auto max_pos = POINT {};
		read_point(node, "max", max_pos);
*/
		::SetWindowPlacement(hwnd, &wp);
	}

	//
	// ウィンドウ位置をノードから読み込みます。
	//
	inline void read_window_pos(const n_json& node, const std::string& name, HWND hwnd, UINT flags = 0, UINT show_cmd = -1)
	{
		return read_window_pos(read_child(node, name), hwnd, flags, show_cmd);
	}

	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct ConfigIO : StdConfigIOUseHive<hive>
	{
		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			// プロパティを読み込みます。
			read_bool(root, "use_common_dialog", hive.use_common_dialog);

			auto window_rc1 = my::get_window_rect(addin_window);

			// アドインウィンドウのウィンドウ位置を読み込みます。
			read_window_pos(root, "addin_window", addin_window);

			auto window_rc2 = my::get_window_rect(addin_window);
/*
+		rcNormalPosition	{LT(0, 0) RB(184, 263)  [184 x 263]}	tagRECT
+		window_rc1	{LT(108, 129) RB(292, 392)  [184 x 263]}	tagRECT
+		window_rc2	{LT(0, 0) RB(184, 263)  [184 x 263]}	tagRECT
*/
			read_child_nodes(root, "filer",
				[&](const n_json& filer_node, size_t i)
			{
				// ファイラの名前を読み込みます。
				std::wstring name;
				read_string(filer_node, "name", name);

				// ファイラウィンドウを作成します。
				auto filer_window = filer_window_manager.create_filer_window(name.c_str(), FALSE);
				if (filer_window)
				{
					// ファイラウィンドウのウィンドウ位置を読み込みます。
					read_window_pos(filer_node, "filer_window", *filer_window);
				}

				return TRUE;
			});

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			// プロパティを書き込みます。
			write_bool(root, "use_common_dialog", hive.use_common_dialog);

			// アドインウィンドウのウィンドウ位置を書き込みます。
			write_window_pos(root, "addin_window", addin_window);

			write_child_nodes(root, "filer", FilerWindow::collection,
				[&](n_json& filer_node, const auto& filer_window, size_t i)
			{
				// ファイラの名前を書き込みます。
				auto name = my::get_window_text(*filer_window);
				write_string(filer_node, "name", name);

				// ファイラウィンドウのウィンドウ位置を書き込みます。
				write_window_pos(filer_node, "filer_window", *filer_window);

				return TRUE;
			});

			return TRUE;
		}
	} config_io;
}
