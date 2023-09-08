#pragma once

namespace fgo::nest
{
	//
	// このクラスはサブウィンドウを管理します。
	//
	inline static struct SubWindowManager
	{
		//
		// サブウィンドウのコレクションです。
		//
		std::vector<HWND> collection;

		void add(HWND subWindow)
		{
			// コレクションに追加します。
			collection.push_back(subWindow);
		}

		void erase(HWND subWindow)
		{
			// サブウィンドウを削除します。
			::DestroyWindow(subWindow);

			// コレクションから削除します。
			std::erase(collection, subWindow);
		}

		void clear()
		{
			// 全てのサブウィンドウを削除します。
			for (auto subWindow : collection)
				::DestroyWindow(subWindow);

			// コレクションをクリアします。
			collection.clear();
		}
	} subWindowManager;
}
