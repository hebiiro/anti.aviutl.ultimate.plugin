#pragma once

namespace apn::text_split
{
	//
	// このクラスはファイルパスを管理します。
	//
	struct path_t
	{
		std::string temp_file_name;
		std::string each_temp_file_name;

		//
		// コンストラクタです。
		//
		path_t()
		{
			MY_TRACE_FUNC("");

			// テンポラリフォルダのパスを取得します。
			auto temp_dir = my::hs(my::get_temp_path());
//			auto temp_dir = my::hs(magi.get_config_file_name(hive.c_name));
			MY_TRACE_STR(temp_dir);

			// カレントプロセスのIDを取得します。ファイル名の重複を防ぐのに使用します。
			auto pid = ::GetCurrentProcessId();
			MY_TRACE_INT(pid);

			// テンポラリファイル名を取得します。
			temp_file_name = my::format("{/}\\text_split_{/}.exo", temp_dir, pid);
			MY_TRACE_STR(temp_file_name);

			// テンポラリファイル名を取得します。(文字列分割後のexoファイル)
			each_temp_file_name = my::format("{/}\\text_split_{/}_each.exo", temp_dir, pid);
			MY_TRACE_STR(each_temp_file_name);
		}

		//
		// デストラクタです。
		//
		~path_t()
		{
		}
	};
}
