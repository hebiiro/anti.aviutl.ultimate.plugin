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
#if 0
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"L", L"filer", L"ファイラ", std::initializer_list<std::wstring>({ L"ObjectExplorer.auf" })));

			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"filter_copy", L"フィルタコピー", std::initializer_list<std::wstring>({ L"CopyFilter.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"filter_drag", L"フィルタドラッグ", std::initializer_list<std::wstring>({ L"DragFilter.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"color_code", L"カラーコード追加", std::initializer_list<std::wstring>({ L"AddColorCode.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"last_frame", L"最終フレーム自動設定", std::initializer_list<std::wstring>({ L"AdjustLastFrame.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"encode_guard", L"エンコードガード", std::initializer_list<std::wstring>({ L"ConfigChecker.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"", L"", std::initializer_list<std::wstring>({ L".auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"", L"", std::initializer_list<std::wstring>({ L".auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"", L"", std::initializer_list<std::wstring>({ L".auf" })));

| [[SingleOut]] | 画像のエクスポート | 現在のフレームまたは選択アイテムを画像としてエクスポートします。 |
| [[TextSplit]] | テキスト分解 | テキストオブジェクトを文字毎に別々のオブジェクトに分解します。 |
| [[ItemAlign]] | アイテム整列 | タイムラインのアイテムを整列します。 |
| [[SceneSelect]] | シーン選択 | シーンをボタンで選択できるようにします。 |
| [[EasingSelect]] | イージング選択 | イージングをGUIで選択できるようにします。 |
| [[FontPreview]] | フォントプレビュー | フォントのプレビューを確認しながら選択できるようにします。 |
| [[FontSelect]] | フォント選択 | カスタムメニューからフォントを選択できるようにします。 |
|---|---|---|
| [[Namecage]] | アイテム名常時表示 | アイテム名がタイムラインから出ないようにします。 |
| [[EditBoxTweaker]] | エディットボックス微調整 | エディットボックスの挙動を微調整します。 |
| [[ExEditTweaker]] | 拡張編集微調整 | 拡張編集の挙動を微調整します。 |
| [[DialogPosition]] | ダイアログ位置調整 | ダイアログがモニター外に表示されないように調整します。 |
| [[DialogSize]] | ダイアログサイズ調整 | DPIに合わせてダイアログのサイズを調整します。 |
#endif
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"M", L"zoom_select", L"ズーム選択", std::initializer_list<std::wstring>({ L"SetZoom.auf" })));
			hive.addins.emplace_back(std::make_unique<Hive::Addin>(L"S", L"editbox_tweaker", L"エディットボックス微調整", std::initializer_list<std::wstring>({ L"OptimizeEditBox.auf" })));

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
