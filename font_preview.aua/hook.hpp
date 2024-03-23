#pragma once

namespace apn::font_preview
{
	//
	// このクラスはフック処理を担当します。
	//
	inline struct HookManager
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// 拡張編集のベースアドレスを取得します。
			auto exedit = magi.exin.get_exedit();
			MY_TRACE_HEX(exedit);

			{
				// 拡張編集内のフォントのコンボボックスのスタイルを変更します。

				DWORD style = WS_CHILD | WS_VSCROLL | CBS_DROPDOWNLIST |
					CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | CBS_SORT | CBS_NOINTEGRALHEIGHT;

				my::tools::set_abs_addr(exedit + 0x0008C39A + 1, style);
			}

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
	} hook_manager;
}
