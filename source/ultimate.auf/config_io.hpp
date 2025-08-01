#pragma once

namespace apn
{
	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct ConfigIO : StdConfigIO
	{
		//
		// アドイン情報をハイブに登録します。
		//
		void register_addin(
			std::wstring size,
			std::wstring name,
			std::wstring display_name,
			BOOL active,
			std::wstring args,
			std::vector<std::wstring> conflicts)
		{
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(
				size, name, display_name, active, args, conflicts));
		}

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			hive.config_file_name = magi.get_config_file_name(hive.instance);

			// アドイン情報を登録します。

			register_addin(L"L", L"dark", L"ダークモード化", TRUE, L"", { L"DarkenWindow.aul" });
			register_addin(L"L", L"workspace", L"ワークスペース化", TRUE, L"", { L"SplitWindow.aul" });
			register_addin(L"L", L"filer", L"ファイラ", TRUE, L"", { L"ObjectExplorer.auf" });
			register_addin(L"L", L"reboot", L"リブート", FALSE, L"", {});
			register_addin(L"L", L"item_copy", L"アイテムコピー", FALSE, L"", {});
			register_addin(L"L", L"item_wave", L"アイテム波形", FALSE, L"", { L"namecage.aua", L"NoScrollText.auf", L"ShowWaveform.auf" });
			register_addin(L"L", L"audio_visualizer", L"音声視覚化", FALSE, L"", { L"LevelMeter.auf" });
			register_addin(L"L", L"settings_browser", L"設定ブラウザ", FALSE, L"", {});
			register_addin(L"L", L"selector", L"セレクタ", FALSE, L"", {});

			register_addin(L"M", L"filter_copy", L"フィルタコピー", TRUE, L"", { L"CopyFilter.auf" });
			register_addin(L"M", L"filter_drag", L"フィルタドラッグ", TRUE, L"", { L"DragFilter.auf" });
			register_addin(L"M", L"dirty_check", L"終了確認", FALSE, L"", { L"auls_confirmclose.auf", L"DirtyCheck.auf" });
			register_addin(L"M", L"output_check", L"出力確認", FALSE, L"", { L"ConfigChecker.auf" });
			register_addin(L"M", L"text_editor", L"テキスト編集", FALSE, L"", {});
			register_addin(L"M", L"text_collect", L"テキスト一括編集", FALSE, L"", {});
			register_addin(L"M", L"text_drop", L"テキストドロップ", FALSE, L"", {});
			register_addin(L"M", L"text_split", L"テキスト分解", FALSE, L"", { L"SplitText.auf" });
			register_addin(L"M", L"item_align", L"アイテム整列", FALSE, L"", { L"BuildStairs.auf" });
			register_addin(L"M", L"image_export", L"画像エクスポート", FALSE, L"", { L"SaveImage.auf" });
			register_addin(L"M", L"scene_select", L"シーン選択", FALSE, L"", { L"SelectScene.auf" });
			register_addin(L"M", L"ease_select", L"イージング選択", FALSE, L"", { L"SelectEasing.auf" });
			register_addin(L"M", L"zoom_select", L"ズーム選択", FALSE, L"", { L"SetZoom.auf" });
			register_addin(L"M", L"font_tree", L"フォントツリー", FALSE, L"", { L"SelectFavoriteFont.auf" });
			register_addin(L"M", L"font_preview", L"フォントプレビュー", TRUE, L"", {});
			register_addin(L"M", L"font_select", L"フォント選択", FALSE, L"", {});
			register_addin(L"M", L"note", L"メモ帳", TRUE, L"", {});
			register_addin(L"M", L"clipboard_viewer", L"クリップボード画像表示", FALSE, L"", {});

			register_addin(L"S", L"namecage", L"アイテム名常時表示", TRUE, L"", { L"NoScrollText.auf", L"ShowWaveform.auf" });
			register_addin(L"S", L"last_frame", L"最終フレーム自動調整", TRUE, L"", { L"AdjustLastFrame.auf" });
			register_addin(L"S", L"color_code", L"カラーコード追加", TRUE, L"", { L"AddColorCode.auf" });
			register_addin(L"S", L"editbox_tweaker", L"エディットボックス微調整", TRUE, L"", { L"OptimizeEditBox.auf" });
			register_addin(L"S", L"exedit_tweaker", L"拡張編集微調整", FALSE, L"", { L"ExEditTweaker.auf" });
			register_addin(L"S", L"dialog_position", L"ダイアログ位置調整", TRUE, L"", { L"AdjustDialogPosition.auf" });
			register_addin(L"S", L"dialog_size", L"ダイアログサイズ調整", TRUE, L"", { L"BigDialog.aul" });
			register_addin(L"S", L"optima", L"最適化", TRUE, L"", { L"combobox_patch.auf" });

			read_file(hive.config_file_name, hive);

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return write_file(hive.config_file_name, hive);
		}

		//
		// カラー配列を読み込みます。
		//
		inline static void read_color_array(const n_json& node, const std::string& name, auto& color_array)
		{
			read_child_nodes(node, name,
				[&](const n_json& color_node, size_t i)
			{
				if (i >= std::size(color_array))
					return FALSE;

				read_color(color_node, color_array[i]);

				return TRUE;
			});
		}

		//
		// カラー配列を書き込みます。
		//
		inline static void write_color_array(n_json& node, const std::string& name, const auto& color_array)
		{
			write_child_nodes(node, name, color_array,
				[&](n_json& color_node, const auto& color, size_t i)
			{
				write_color(color_node, color);

				return TRUE;
			});
		}

		//
		// コンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			read_string(root, "python_file_name", hive.python_file_name);
			read_color_array(root, "custom_color", magi.custom_colors);

			// アドイン情報を読み込みます。
			read_child_nodes(root, "addin",
				[&](const n_json& addin_node, size_t i)
			{
				// アドインの名前を取得します。
				std::wstring name;
				read_string(addin_node, "name", name);
				MY_TRACE_STR(name);

				// アドインの名前から登録されているアドインを取得します。
				auto it = std::find_if(hive.addins.begin(), hive.addins.end(),
					[&name](const auto& addin) { return addin->name == name; });
				if (it == hive.addins.end()) return TRUE;
				auto& addin = *it;

				// アドインの状態を取得し、登録されているアドインに格納します。
				read_bool(addin_node, "active", addin->active);
				MY_TRACE_INT(addin->active);

				// アドインの引数を取得し、登録されているアドインに格納します。
				read_string(addin_node, "args", addin->args);
				MY_TRACE_STR(addin->args);

				return TRUE;
			});

			return TRUE;
		}

		//
		// コンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			write_string(root, "python_file_name", hive.python_file_name);
			write_color_array(root, "custom_color", magi.custom_colors);

			write_child_nodes(root, "addin", hive.addins,
				[&](n_json& addin_node, const auto& addin, size_t i)
			{
				write_string(addin_node, "name", addin->name);
				write_bool(addin_node, "active", addin->active);
				write_string(addin_node, "args", addin->args);

				return TRUE;
			});

			return TRUE;
		}
	} config_io;
}
