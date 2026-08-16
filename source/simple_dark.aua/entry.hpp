#pragma once

namespace apn::dark
{
	//
	// このクラスはエントリです。
	//
	struct entry_t
	{
		//
		// このクラスはエントリノードです。
		//
		struct node_t
		{
			virtual BOOL on_init() { return TRUE; }
			virtual BOOL on_exit() { return TRUE; }
		};

		//
		// エントリノードのコレクションです。
		//
		std::vector<node_t*> nodes;

		//
		// エントリノードをコレクションに追加します。
		//
		void add(node_t& node)
		{
			nodes.emplace_back(&node);
		}

		//
		// 全てのエントリノードの初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			for (auto node : nodes)
				if (!node->on_init()) return FALSE;

			return TRUE;
		}

		//
		// 全てのエントリノードの後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			for (auto it = nodes.rbegin(); it != nodes.rend(); it++)
				(*it)->on_exit();

			return TRUE;
		}

		virtual BOOL on_init() { return TRUE; }
		virtual BOOL on_exit() { return TRUE; }
	};

	//
	// このクラスは優先エントリです。
	// dllがロードされた(かなり早い)タイミングでエントリします。
	//
	inline entry_t prior_entry;

	//
	// このクラスは共用エントリです。
	// アドイン初期化関数またはエクスポート関数が呼ばれたタイミングでエントリします。
	//
	inline entry_t common_entry;

	//
	// このクラスはホストプロセス専用エントリです。
	// アドイン初期化関数が呼ばれたタイミングでエントリします。
	//
	inline entry_t host_process_entry;
}
