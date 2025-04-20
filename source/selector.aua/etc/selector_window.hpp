#pragma once

namespace apn::selector
{
	//
	// このクラスはセレクタカテゴリのウィンドウです。
	//
	struct SelectorWindow : StdAddinWindow
	{
		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_CREATE:
				{
					MY_TRACE_FUNC("WM_CREATE, {/hex}, {/hex}", wParam, lParam);

					// カテゴリ名を設定します。
					::SetProp(hwnd, L"aviutl.plugin.category_name", (HANDLE)hive.c_display_name);

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {/hex}, {/hex}", wParam, lParam);

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	};
}
