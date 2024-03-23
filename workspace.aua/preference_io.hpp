#pragma once

namespace apn::workspace
{
	//
	// このクラスはプリファレンスの入出力を担当します。
	//
	inline struct PreferenceIO : StdConfigIO
	{
		//
		// TRUEの場合はレイアウトのみを入出力します。
		//
		BOOL layout_only = FALSE;

		//
		// コンストラクタです。
		//
		explicit PreferenceIO(BOOL layout_only = FALSE)
			: layout_only(layout_only)
		{
		}

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
		BOOL read()
		{
			MY_TRACE_FUNC("");

			return read_file(hive.preference_file_name, hive);
		}

		//
		// コンフィグを保存します。
		//
		BOOL write()
		{
			MY_TRACE_FUNC("");

			return write_file(hive.preference_file_name, hive);
		}

		//
		// ノードからプリファレンスを読み込みます。
		//
		virtual BOOL read_node(ptree& root) override
		{
			MY_TRACE_FUNC("");

			return preference_loader.load(root, layout_only);
		}

		//
		// ノードにプリファレンスを保存します。
		//
		virtual BOOL write_node(ptree& root) override
		{
			MY_TRACE_FUNC("");

			return preference_saver.save(root, layout_only);
		}
	} preference_io;
}
