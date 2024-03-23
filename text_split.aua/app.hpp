#pragma once

namespace apn::text_split
{
	//
	// このクラスはHive::Appの実装です。
	//
	inline struct App : Hive::App
	{
		//
		// コンストラクタです。
		//
		App()
		{
			hive.app = this;
		}

		virtual BOOL split_text() override
		{
			return Executor().split_text();
		}
	} app;
}
