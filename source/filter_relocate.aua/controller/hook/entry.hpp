#pragma once

namespace apn::filter_relocate::controller::hook
{
	//
	// このクラスはフックにエントリします。
	//
	struct entry_t
	{
		//
		// エントリのコレクションです。
		//
		inline static std::vector<entry_t*> entries;

		//
		// コンストラクタです。
		//
		entry_t() { entries.emplace_back(this); }

		//
		// 初期化処理を実行します。
		//
		inline static BOOL init()
		{
			MY_TRACE_FUNC("");

			for (auto entry : entries)
				if (!entry->on_init()) return FALSE;

			return TRUE;
		}

		//
		// 終了処理を実行します。
		//
		inline static BOOL exit()
		{
			MY_TRACE_FUNC("");

			for (auto entry : entries)
				entry->on_exit();

			return TRUE;
		}

		virtual BOOL on_init() = 0;
		virtual BOOL on_exit() = 0;
	};
}
