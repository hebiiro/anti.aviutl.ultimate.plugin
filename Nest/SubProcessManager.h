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

		//
		// サブプロセスのメニューです。
		//
		HMENU menu = 0;

		//
		// メニューIDの範囲です。
		//
		UINT begin = 0, end = 0;

		//
		// 初期化処理です。
		//
		BOOL init(UINT begin, UINT end)
		{
			MY_TRACE_FUNC("0x%08X, 0x%08X", begin, end);

			menu = ::CreatePopupMenu();
			this->begin = begin;
			this->end = end;

			return TRUE;
		}

		//
		// 後始末処理です。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			::DestroyMenu(menu), menu = 0;

			return TRUE;
		}

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

		//
		// サブプロセス一覧用メニューを更新します。
		//
		BOOL updateMenu()
		{
			MY_TRACE_FUNC("");

			// 一旦すべての項目を削除します。
			while (::DeleteMenu(menu, 0, MF_BYPOSITION)){}

			size_t c = collection.size();
			for (size_t i = 0; i < c; i++)
			{
				HWND subProcess = collection[i];

				MENUITEMINFO mii = { sizeof(mii) };
				mii.fMask = MIIM_STRING | MIIM_DATA | MIIM_ID | MIIM_STATE;

				TCHAR windowName[MAX_PATH] = {};
				::GetWindowText(subProcess, windowName, std::size(windowName));

				if (::IsWindowVisible(subProcess))
					mii.fState = MFS_CHECKED;
				else
					mii.fState = MFS_UNCHECKED;

				mii.wID = begin + i;
				mii.dwItemData = (DWORD)subProcess;
				mii.dwTypeData = windowName;

				::InsertMenuItem(menu, i, TRUE, &mii);
			}

			return TRUE;
		}

		//
		// サブプロセス一覧用メニューを実行します。
		//
		BOOL executeMenu(UINT id)
		{
			MY_TRACE_FUNC("0x%08X", id);

			// IDが範囲外の場合は何もしません。
			if (id < begin || id >= end) return FALSE;

			// メニューアイテムを取得します。
			MENUITEMINFO mii = { sizeof(mii) };
			mii.fMask = MIIM_DATA;
			::GetMenuItemInfo(menu, id, FALSE, &mii);

			// メニューアイテムからサブプロセスを取得します。
			HWND subProcess = (HWND)mii.dwItemData;
			if (!subProcess) return FALSE;

			// メニューで選択されたサブプロセスの表示/非表示を切り替えます。
			::SendMessage(subProcess, WM_CLOSE, 0, 0);

			return TRUE;
		}

		//
		// WM_INITMENUPOPUPを処理します。
		//
		BOOL onInitMenuPopup(WPARAM wParam, LPARAM lParam)
		{
			if ((HMENU)wParam != menu) return FALSE;
			
			return updateMenu();
		}
	} subProcessManager;
}
