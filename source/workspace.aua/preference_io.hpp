#pragma once

namespace apn::workspace
{
	//
	// このクラスはプリファレンスの入出力を担当します。
	//
	inline struct PreferenceIO : StdConfigIO
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			hive.preference_file_name = magi.get_config_file_name(hive.c_name + L".preference.json"s);
			MY_TRACE_STR(hive.preference_file_name);

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
		// コンフィグを読み込みます。
		//
		BOOL read(uint32_t flags = hive.c_preference_flag.c_default)
		{
			MY_TRACE_FUNC("{/hex}", flags);

			return PreferenceLoader(flags).read_file(hive.preference_file_name, hive);
		}

		//
		// コンフィグを書き込みます。
		//
		BOOL write(uint32_t flags = hive.c_preference_flag.c_default)
		{
			MY_TRACE_FUNC("{/hex}", flags);

			return PreferenceSaver(flags).write_file(hive.preference_file_name, hive);
		}
	} preference_io;
}
