#pragma once

namespace apn
{
	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct ConfigIO : StdConfigIO
	{
		//
		// 初期化を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			hive.config_file_name = magi.get_config_file_name(hive.instance);

			// アドインを登録します。
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"L", L"dark", L"ダークモード化", TRUE, L"", std::vector<std::wstring>{ L"DarkenWindow.aul" }));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"L", L"workspace", L"ワークスペース化", TRUE, L"", std::vector<std::wstring>{ L"SplitWindow.aul" }));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"L", L"filer", L"ファイラ", TRUE, L"", std::vector<std::wstring>{ L"ObjectExplorer.auf" }));
//			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"L", L"audio_graph", L"音声グラフ", TRUE, L"", std::vector<std::wstring>{ L"LevelMeter.auf" }));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"L", L"item_wave", L"アイテム波形", TRUE, L"", std::vector<std::wstring>{ L"namecage.aua", L"NoScrollText.auf", L"ShowWaveform.auf" }));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"L", L"settings_browser", L"設定ブラウザ", TRUE, L"debug", std::vector<std::wstring>{}));

			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"filter_copy", L"フィルタコピー", TRUE, L"", std::vector<std::wstring>({ L"CopyFilter.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"filter_drag", L"フィルタドラッグ", TRUE, L"", std::vector<std::wstring>({ L"DragFilter.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"color_code", L"カラーコード追加", TRUE, L"", std::vector<std::wstring>({ L"AddColorCode.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"last_frame", L"最終フレーム自動設定", TRUE, L"", std::vector<std::wstring>({ L"AdjustLastFrame.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"output_check", L"出力確認", TRUE, L"", std::vector<std::wstring>({ L"ConfigChecker.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"image_export", L"画像エクスポート", TRUE, L"", std::vector<std::wstring>({ L"SaveImage.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"text_split", L"テキスト分解", TRUE, L"", std::vector<std::wstring>({ L"SplitText.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"item_align", L"アイテム整列", TRUE, L"", std::vector<std::wstring>({ L"BuildStairs.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"font_preview", L"フォントプレビュー", TRUE, L"", std::vector<std::wstring>({ L"SelectFavoriteFont.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"font_select", L"フォント選択", TRUE, L"", std::vector<std::wstring>({ L"SelectFavoriteFont.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"scene_select", L"シーン選択", TRUE, L"", std::vector<std::wstring>({ L"SelectScene.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"ease_select", L"イージング選択", TRUE, L"", std::vector<std::wstring>({ L"SelectEasing.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"zoom_select", L"ズーム選択", TRUE, L"", std::vector<std::wstring>({ L"SetZoom.auf" })));

			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"S", L"dirty_check", L"終了確認", TRUE, L"", std::vector<std::wstring>({ L"auls_confirmclose.auf", L"DirtyCheck.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"S", L"namecage", L"アイテム名常時表示", FALSE, L"", std::vector<std::wstring>({ L"NoScrollText.auf", L"ShowWaveform.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"S", L"editbox_tweaker", L"エディットボックス微調整", TRUE, L"", std::vector<std::wstring>({ L"OptimizeEditBox.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"S", L"exedit_tweaker", L"拡張編集微調整", FALSE, L"", std::vector<std::wstring>({ L"ExEditTweaker.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"S", L"dialog_position", L"ダイアログ位置調整", TRUE, L"", std::vector<std::wstring>({ L"AdjustDialogPosition.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"S", L"dialog_size", L"ダイアログサイズ調整", TRUE, L"", std::vector<std::wstring>({ L"BigDialog.aul" })));

			read_file(hive.config_file_name, hive);

			return TRUE;
		}

		//
		// 後始末を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return write_file(hive.config_file_name, hive);
		}

		//
		// コンフィグを読み込みます。
		//
		virtual BOOL read_node(ptree& root) override
		{
			MY_TRACE_FUNC("");

			using namespace my::json;

			get_string(root, "python_file_name", hive.python_file_name);

			// ファイル内のアドイン情報を読み込みます。
			if (auto addin_nodes_op = root.get_child_optional("addin"))
			{
				for (const auto& pair : addin_nodes_op.value())
				{
					const auto& addin_node = pair.second;

					// アドインの名前を取得します。
					std::wstring name;
					get_string(addin_node, "name", name);
					MY_TRACE_STR(name);

					// アドインの名前から登録されているアドインを取得します。
					auto it = std::find_if(hive.addins.begin(), hive.addins.end(),
						[&name](const auto& addin) { return addin->name == name; });
					if (it == hive.addins.end()) continue;
					auto& addin = *it;

					// アドインの状態を取得し、登録されているアドインに格納します。
					get_bool(addin_node, "active", addin->active);
					MY_TRACE_INT(addin->active);

					// アドインの引数を取得し、登録されているアドインに格納します。
					get_string(addin_node, "args", addin->args);
					MY_TRACE_STR(addin->args);
				}
			}

			return TRUE;
		}

		//
		// コンフィグを保存します。
		//
		virtual BOOL write_stream(std::ofstream& ofs) override
		{
			MY_TRACE_FUNC("");

			using namespace my::json;

			size_t indent = 0;

			ofs << with_eol(indent++, R"({)"s);
			{
				ofs << with_eol(indent, std::format(R"("python_file_name": "{}",)", my::wide_to_cp(hive.python_file_name, CP_UTF8)));
				ofs << with_eol(0, "");
				ofs << with_eol(indent++, R"("addin": [)"s);
				for (const auto& addin : hive.addins)
				{
					auto str = std::format(R"({{ "active": {}, "name": {}, "args": {} }})",
						from_bool(addin->active), from_string(addin->name), from_string(addin->args));

					auto joint = (addin != hive.addins.back()) ? ","s : ""s;

					ofs << with_eol(indent, str + joint);
				}
				ofs << with_eol(--indent, R"(])"s);
			}
			ofs << with_eol(--indent, R"(})"s);

			return TRUE;
		}
	} config_io;
}
