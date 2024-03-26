#pragma once

namespace apn::filter_copy
{
	using namespace my::json;

	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct ConfigIO : StdConfigIO
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			if (!__super::init(hive.instance)) return FALSE;

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			return __super::exit(hive.instance);
		}

		//
		// コンフィグを読み込みます。
		//
		virtual BOOL on_load(ptree& root) override
		{
			return TRUE;
		}

		//
		// コンフィグをファイルに保存します。
		//
		virtual BOOL on_save(std::ofstream& ofs) override
		{
			ptree root;

			write_json(ofs, root);

			return TRUE;
		}

		virtual int message_box(const std::wstring& text) override
		{
			return hive.message_box(text);
		}
	} config_io;
}
