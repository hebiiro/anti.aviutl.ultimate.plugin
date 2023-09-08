#pragma once

namespace fgo::nest
{
	//
	// このクラスはサブプロセスを管理します。
	//
	inline static struct SubProcessManager
	{
		//
		// サブプロセスのコレクションです。
		//
		std::vector<HWND> collection;

		void add(HWND subProcess)
		{
			// コレクションに追加します。
			collection.push_back(subProcess);
		}

		void erase(HWND subProcess)
		{
			// サブプロセスを削除します。
			::DestroyWindow(subProcess);

			// コレクションから削除します。
			std::erase(collection, subProcess);
		}

		void clear()
		{
			// 全てのサブプロセスを削除します。
			for (auto subProcess : collection)
				::DestroyWindow(subProcess);

			// コレクションをクリアします。
			collection.clear();
		}
	} subProcessManager;
}
