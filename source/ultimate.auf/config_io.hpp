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
			register_addin(L"L", L"item_wave", L"アイテム波形", TRUE, L"", { L"namecage.aua", L"NoScrollText.auf", L"ShowWaveform.auf" });
			register_addin(L"L", L"audio_visualizer", L"音声視覚化", TRUE, L"", { L"LevelMeter.auf" });
			register_addin(L"L", L"settings_browser", L"設定ブラウザ", TRUE, L"", {});

			register_addin(L"M", L"filter_copy", L"フィルタコピー", TRUE, L"", { L"CopyFilter.auf" });
			register_addin(L"M", L"filter_drag", L"フィルタドラッグ", TRUE, L"", { L"DragFilter.auf" });
			register_addin(L"M", L"dirty_check", L"終了確認", TRUE, L"", { L"auls_confirmclose.auf", L"DirtyCheck.auf" });
			register_addin(L"M", L"output_check", L"出力確認", TRUE, L"", { L"ConfigChecker.auf" });
			register_addin(L"M", L"text_split", L"テキスト分解", TRUE, L"", { L"SplitText.auf" });
			register_addin(L"M", L"item_align", L"アイテム整列", TRUE, L"", { L"BuildStairs.auf" });
			register_addin(L"M", L"image_export", L"画像エクスポート", TRUE, L"", { L"SaveImage.auf" });
			register_addin(L"M", L"scene_select", L"シーン選択", TRUE, L"", { L"SelectScene.auf" });
			register_addin(L"M", L"ease_select", L"イージング選択", TRUE, L"", { L"SelectEasing.auf" });
			register_addin(L"M", L"zoom_select", L"ズーム選択", TRUE, L"", { L"SetZoom.auf" });
			register_addin(L"M", L"font_tree", L"フォントツリー", TRUE, L"", { L"SelectFavoriteFont.auf" });
			register_addin(L"M", L"font_preview", L"フォントプレビュー", TRUE, L"", {});
			register_addin(L"M", L"font_select", L"フォント選択", TRUE, L"", {});
			register_addin(L"M", L"note", L"メモ帳", TRUE, L"", {});
			register_addin(L"M", L"clipboard_viewer", L"クリップボード画像表示", TRUE, L"", {});

			register_addin(L"S", L"namecage", L"アイテム名常時表示", FALSE, L"", { L"NoScrollText.auf", L"ShowWaveform.auf" });
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
		// コンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			get_string(root, "python_file_name", hive.python_file_name);

			// アドイン情報を読み込みます。
			get_child_nodes(root, "addin",
				[&](const n_json& addin_node)
			{
				// アドインの名前を取得します。
				std::wstring name;
				get_string(addin_node, "name", name);
				MY_TRACE_STR(name);

				// アドインの名前から登録されているアドインを取得します。
				auto it = std::find_if(hive.addins.begin(), hive.addins.end(),
					[&name](const auto& addin) { return addin->name == name; });
				if (it == hive.addins.end()) return TRUE;
				auto& addin = *it;

				// アドインの状態を取得し、登録されているアドインに格納します。
				get_bool(addin_node, "active", addin->active);
				MY_TRACE_INT(addin->active);

				// アドインの引数を取得し、登録されているアドインに格納します。
				get_string(addin_node, "args", addin->args);
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

			set_string(root, "python_file_name", hive.python_file_name);

			set_child_nodes(root, "addin", hive.addins,
				[&](n_json& addin_node, const auto& addin)
			{
				set_string(addin_node, "name", addin->name);
				set_bool(addin_node, "active", addin->active);
				set_string(addin_node, "args", addin->args);

				return TRUE;
			});

			return TRUE;
		}
	} config_io;
}
