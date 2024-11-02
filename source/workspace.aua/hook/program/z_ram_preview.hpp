#pragma once

namespace apn::workspace::hook
{
	//
	// このクラスはZRamPreview.aufをフックします。
	//
	inline struct {
		//
		// このクラスは::GetMenuItemCount()をフックします。
		//
		inline static struct {
			inline static int WINAPI hook_proc(HMENU menu)
			{
				MY_TRACE_FUNC("{:#010x}", menu);

				// 「拡張編集RAMプレビュー」用の処理です。

				if (menu == ::GetMenu(hive.main_window))
					return 7; // メインウィンドウのメニューの場合は固定値を返します。

				return orig_proc(menu);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} GetMenuItemCount;

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			auto z_ram_preview = ::GetModuleHandle(_T("ZRamPreview.auf"));
			MY_TRACE_HEX(z_ram_preview);
			if (!z_ram_preview) return FALSE;

			// ZRamPreview.auf内の::GetMenuItemCount()をフックします。
			my::hook::attach_import(GetMenuItemCount, z_ram_preview, "GetMenuItemCount");
			MY_TRACE_HEX(GetMenuItemCount.orig_proc);

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}
	} z_ram_preview;
}
