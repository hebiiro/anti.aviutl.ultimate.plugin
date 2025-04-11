#pragma once

namespace apn::dark
{
	//
	// このクラスはアセットの入出力を担当します。
	//
	inline struct AssetsIO : StdConfigIO
	{
		struct SkinIO : StdConfigIO
		{
			//
			// ノードからアセットを読み込みます。
			//
			virtual BOOL read_node(n_json& root) override
			{
				MY_TRACE_FUNC("");

				auto skin = hive.skin_list.emplace_back(std::make_shared<Hive::Skin>());

				read_string(root, "module_name", skin->module_name);
				read_string(root, "display_name", skin->display_name);

				return TRUE;
			}
		} skin_io;

		struct SchemeIO : StdConfigIO
		{
			//
			// ノードからアセットを読み込みます。
			//
			virtual BOOL read_node(n_json& root) override
			{
				MY_TRACE_FUNC("");

				auto scheme = hive.scheme_list.emplace_back(std::make_shared<Hive::Scheme>());

				read_string(root, "module_name", scheme->module_name);
				read_string(root, "display_name", scheme->display_name);

				return TRUE;
			}
		} scheme_io;

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			hive.assets_folder_name = magi.get_assets_file_name(hive.c_name);
			MY_TRACE_STR(hive.assets_folder_name);

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
		// アセットを読み込みます。
		//
		BOOL read()
		{
			MY_TRACE_FUNC("");

			read_list(L"skin", skin_io, hive.skin_list, hive.skin_name);
			read_list(L"scheme", scheme_io, hive.scheme_list, hive.scheme_name);

			return TRUE;
		}

		//
		// リストを読み込みます。
		//
		BOOL read_list(LPCWSTR stem, StdConfigIO& io, auto& list, auto& name)
		{
			MY_TRACE_FUNC("");

			try
			{
				// まず、一旦リストを消去します。
				list.clear();

				// フォルダ名を取得します。
				auto folder_name = std::filesystem::path(hive.assets_folder_name) / stem;
				MY_TRACE_STR(folder_name);

				for (const auto& path : std::filesystem::directory_iterator(folder_name))
				{
					auto file_name = path.path().wstring();

					if (file_name.ends_with(L".json"))
						io.read_file(file_name, hive);
				}

				// 表示名順に並べ替えます。
				std::sort(list.begin(), list.end(),
					[](const auto& a, const auto& b)
					{ return a->display_name < b->display_name; });

				// 選択が存在しない場合は
				// リストの一番先頭のモジュールを選択します。
				if (name.empty() && !list.empty())
					name = list.front()->module_name;

				return TRUE;
			}
			catch (const std::exception& error)
			{
				MY_TRACE_STR(error.what());

				hive.message_box(my::format(L"{/}の読み込みに失敗しました\n{/}", stem, error.what()));

				return FALSE;
			}
		}
	} assets_io;
}
