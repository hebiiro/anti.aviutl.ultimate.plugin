#pragma once
#include "Hive.h"

namespace fgo::font_preview
{
	//
	// このクラスはフック処理を担当します。
	//
	struct Hook
	{
		//
		// 初期化を行います。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// 拡張編集のアドレスを取得します。
			auto exedit = (Tools::addr_t)::GetModuleHandle(_T("exedit.auf"));
			MY_TRACE_HEX(exedit);

			{
				// 拡張編集内のフォントのコンボボックスのスタイルを変更します。

				DWORD style = WS_CHILD | WS_VSCROLL | CBS_DROPDOWNLIST |
					CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | CBS_SORT | CBS_NOINTEGRALHEIGHT;

				Tools::set_abs_addr(exedit + 0x0008C39A + 1, style);
			}

			return TRUE;
		}

		//
		// 後始末を行います。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}
	};
}
