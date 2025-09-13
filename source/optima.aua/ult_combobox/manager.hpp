#pragma once

namespace apn::ult_combobox
{
	//
	// このクラスはノードを管理します。
	//
	inline struct Manager
	{
		//
		// ノードのコレクションです。
		//
		std::unordered_map<HWND, std::shared_ptr<Node>> nodes;

		//
		// ノードをコレクションに追加します。
		//
		void add(const std::shared_ptr<Node>& node)
		{
			nodes[*node] = node;
		}

		//
		// ノードをコレクションから削除します。
		//
		void remove(HWND hwnd)
		{
			nodes.erase(hwnd);
		}
	} manager;
}
