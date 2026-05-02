#pragma once

namespace apn::filter_relocate::controller::hook
{
	//
	// このクラスはフックを管理します。
	//
	inline struct manager_t
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			return entry_t::init();
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return entry_t::exit();
		}
	} manager;
}
