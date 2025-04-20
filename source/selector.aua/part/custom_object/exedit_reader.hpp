#pragma once

namespace apn::selector::custom_object
{
	//
	// このクラスは拡張編集で読み込み済みのカスタムオブジェクトの入出力を担当します。
	//
	inline struct ExEditReader
	{
		//
		// 内蔵スクリプト名を読み込みます。
		//
		BOOL read_embedded_scripts()
		{
			MY_TRACE_FUNC("");

			constexpr LPCWSTR names[] = {
				L"集中線",
				L"走査線",
				L"カウンター",
				L"レンズフレア",
				L"雲",
				L"星",
				L"雪",
				L"雨",
				L"ランダム小物配置(カメラ制御)",
				L"ライン(移動軌跡)",
				L"扇型",
				L"多角形",
				L"周辺ボケ光量",
				L"フレア",
				L"水面",
			};

			for (auto i = 0; i < std::size(names); i++)
				agit.embedded.map[names[i]] = { i, names[i] };

			return TRUE;
		}

		//
		// 拡張編集で読み込み済みのカスタムオブジェクトを読み込みます。
		//
		BOOL read()
		{
			MY_TRACE_FUNC("");

			// 内蔵スクリプト名を読み込みます。
			read_embedded_scripts();

			auto exedit = magi.exin.get_exedit();
			auto table = (LPCSTR)(exedit + 0xCE090);
			auto strings = split_by_zero(table);
			auto index = 0;
			for (const auto& s : strings)
			{
				// s == "スクリプト名@ファイル名\1フォルダ名\0"

				auto pair1 = split(s, '\1');
				auto pair2 = split(pair1.first, '@');

				auto script_name = my::ws(pair2.first);
				auto folder_name = my::ws(pair1.second);
				auto file_name = my::ws(pair2.second);

				if (file_name.length())
				{
					file_name = L"@" + file_name;
				}
				else if (!agit.embedded.map.contains(script_name))
				{
					file_name = script_name;
				}

				auto custom_object = std::make_shared<Agit::List::Node>();
				custom_object->script_name = script_name;
				custom_object->folder_name = folder_name;
				custom_object->file_name = file_name;
				agit.list.add(custom_object);

				index++;
			}

			return TRUE;
		}
	} exedit_reader;
}
