#pragma once

namespace apn::optima::part
{
	//
	// このクラスはパーツを管理します。
	//
	inline struct Manager
	{
		//
		// パーツのコレクションです。
		//
		std::vector<NodeBase*> nodes;

		//
		// パーツの初期化処理を実行します。
		//
		BOOL init()
		{
			for (auto part : nodes)
				part->func_init(magi.fp);

			return TRUE;
		}

		//
		// パーツの後始末処理を実行します。
		//
		BOOL exit()
		{
			for (auto part : nodes)
				part->func_exit(magi.fp);

			return TRUE;
		}

		//
		// パーツのウィンドウ作成時の初期化処理を実行します。
		//
		BOOL window_init()
		{
			for (auto part : nodes)
				part->func_window_init(magi.fp);

			return TRUE;
		}

		//
		// パーツのウィンドウ破壊時の後始末処理を実行します。
		//
		BOOL window_exit()
		{
			for (auto part : nodes)
				part->func_window_exit(magi.fp);

			return TRUE;
		}

		//
		// パーツを追加します。
		//
		void add(NodeBase* part)
		{
			nodes.emplace_back(part);
		}
	} manager;
}
