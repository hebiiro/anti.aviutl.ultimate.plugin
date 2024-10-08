#pragma once

namespace apn::workspace
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

		void add(HWND sub_process)
		{
			// コレクションに追加します。
			collection.push_back(sub_process);
		}

		void erase(HWND sub_process)
		{
			// サブプロセスを削除します。
			::DestroyWindow(sub_process);

			// コレクションから削除します。
			std::erase(collection, sub_process);
		}

		void clear()
		{
			// 全てのサブプロセスを削除します。
			for (auto sub_process : collection)
				::DestroyWindow(sub_process);

			// コレクションをクリアします。
			collection.clear();
		}
	} subProcessManager;
}
