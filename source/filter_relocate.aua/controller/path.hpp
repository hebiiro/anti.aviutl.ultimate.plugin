#pragma once

namespace apn::filter_relocate::controller
{
	//
	// このクラスはファイルパスを管理します。
	//
	struct path_t
	{
		std::string before_file_name;
		std::string after_file_name;

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
			before_file_name = my::format("{/}\\filter_relocate_{/}_before.exo", temp_dir, pid);
			MY_TRACE_STR(before_file_name);

			// テンポラリファイル名を取得します。(文字列分割後のexoファイル)
			after_file_name = my::format("{/}\\filter_relocate_{/}_after.exo", temp_dir, pid);
			MY_TRACE_STR(after_file_name);
		}

		//
		// デストラクタです。
		//
		~path_t()
		{
		}
	};
}
