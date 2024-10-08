#pragma once

namespace apn
{
	//
	// このクラスは標準的なアドインウィンドウです。
	//
	struct StdAddinWindow : my::aviutl::PluginWindow
	{
		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_NCCREATE:
				{
					// このウィンドウにカテゴリ名を設定します。
					::SetProp(hwnd, _T("aviutl.plugin.category_name"), (HANDLE)L"アルティメット");

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	};
}
