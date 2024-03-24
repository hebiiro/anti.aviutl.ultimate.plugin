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

			// アドインを登録します。
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"L", L"dark", L"ダークモード化", std::initializer_list<std::wstring>({ L"DarkenWindow.aul" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"L", L"nest", L"ワークスペース化", std::initializer_list<std::wstring>({ L"SplitWindow.aul" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"L", L"filer", L"ファイラ", std::initializer_list<std::wstring>({ L"ObjectExplorer.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"L", L"audio_graph", L"レベルメーター", std::initializer_list<std::wstring>({ L"LevelMeter.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"L", L"item_wave", L"アイテム波形", std::initializer_list<std::wstring>({ L"NoScrollText.auf", L"ShowWaveform.auf" })));

			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"filter_copy", L"フィルタコピー", std::initializer_list<std::wstring>({ L"CopyFilter.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"filter_drag", L"フィルタドラッグ", std::initializer_list<std::wstring>({ L"DragFilter.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"color_code", L"カラーコード追加", std::initializer_list<std::wstring>({ L"AddColorCode.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"last_frame", L"最終フレーム自動設定", std::initializer_list<std::wstring>({ L"AdjustLastFrame.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"encode_guard", L"エンコードガード", std::initializer_list<std::wstring>({ L"ConfigChecker.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"image_export", L"画像エクスポート", std::initializer_list<std::wstring>({ L"SaveImage.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"text_split", L"テキスト分解", std::initializer_list<std::wstring>({ L"SplitText.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"item_align", L"アイテム整列", std::initializer_list<std::wstring>({ L"BuildStairs.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"font_preview", L"フォントプレビュー", std::initializer_list<std::wstring>({ L"SelectFavoriteFont.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"font_select", L"フォント選択", std::initializer_list<std::wstring>({ L"SelectFavoriteFont.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"scene_select", L"シーン選択", std::initializer_list<std::wstring>({ L"SelectScene.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"ease_select", L"イージング選択", std::initializer_list<std::wstring>({ L"SelectEasing.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"zoom_select", L"ズーム選択", std::initializer_list<std::wstring>({ L"SetZoom.auf" })));

			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"S", L"namecage", L"アイテム名常時表示", std::initializer_list<std::wstring>({ L"item_wave.aua", L"NoScrollText.auf", L"ShowWaveform.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"S", L"editbox_tweaker", L"エディットボックス微調整", std::initializer_list<std::wstring>({ L"OptimizeEditBox.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"S", L"exedit_tweaker", L"拡張編集微調整", std::initializer_list<std::wstring>({ L"ExEditTweaker.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"S", L"dialog_position", L"ダイアログ位置調整", std::initializer_list<std::wstring>({ L"AdjustDialogPosition.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"S", L"dialog_size", L"ダイアログサイズ調整", std::initializer_list<std::wstring>({ L"BigDialog.aul" })));

			return __super::init(hive.instance);
		}

		//
		// 後始末を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return __super::exit(hive.instance);
		}

		//
		// コンフィグを読み込みます。
		//
		virtual BOOL on_load(ptree& root) override
		{
			MY_TRACE_FUNC("");

			// ファイル内のアドイン情報を読み込みます。
			for (auto& addin : root.get_child("addins"))
			{
				// アドインの名前を取得します。
				auto name = my::cp_to_wide(addin.second.get<std::string>("name", ""), CP_UTF8);
				MY_TRACE_STR(name);

				// アドインの名前から登録されているアドインを取得します。
				auto it = std::find_if(hive.addins.begin(), hive.addins.end(),
					[&name](const auto& addin) { return addin->name == name; });
				if (it == hive.addins.end()) continue;

				// アドインの状態を取得し、登録されているアドインに格納します。
				auto active = addin.second.get<bool>("active", false);
				MY_TRACE_INT(active);
				(*it)->active = active;

				// アドインの引数を取得し、登録されているアドインに格納します。
				auto args = my::cp_to_wide(addin.second.get<std::string>("args", ""), CP_UTF8);
				MY_TRACE_STR(args);
				(*it)->args = args;
			}

			return TRUE;
		}

		//
		// コンフィグをファイルに保存します。
		//
		virtual BOOL on_save(std::ofstream& ofs) override
		{
			MY_TRACE_FUNC("");

			using namespace my::json;

			size_t indent = 0;

			ofs << with_eol(indent++, R"({)"s);
			{
				ofs << with_eol(indent++, R"("addins": [)"s);
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

		virtual int message_box(const std::wstring& text) override
		{
			return hive.message_box(text);
		}
	} config_io;
}
