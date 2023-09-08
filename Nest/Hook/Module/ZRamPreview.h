#pragma once

namespace fgo::nest::hook
{
	//
	// このクラスはZRamPreview.aufをフックします。
	//
	struct ZRamPreview
	{
		//
		// このクラスは::GetMenuItemCount()をフックします。
		//
		inline static struct
		{
			static int WINAPI hook(HMENU menu)
			{
				MY_TRACE(_T("ZRamPreview::GetMenuItemCount(0x%08X)\n"), menu);

				// 「拡張編集RAMプレビュー」用の処理です。

				if (menu == ::GetMenu(hive.mainWindow))
					return 7; // メインウィンドウのメニューの場合は固定値を返します。

				return orig(menu);
			}
			inline static decltype(&hook) orig = 0;
		} GetMenuItemCount;

		//
		// ZRamPreview.auf用のフックを初期化します。
		//
		static BOOL init()
		{
			HMODULE ZRamPreview = ::GetModuleHandle(_T("ZRamPreview.auf"));
			MY_TRACE_HEX(ZRamPreview);
			if (!ZRamPreview) return FALSE;

			// ZRamPreview.auf内の::GetMenuItemCount()をフックします。
			Tools::attach_import(GetMenuItemCount, ZRamPreview, "GetMenuItemCount");
			MY_TRACE_HEX(GetMenuItemCount.orig);

			return TRUE;
		}
	};
}
