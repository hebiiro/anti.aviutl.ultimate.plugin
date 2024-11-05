#pragma once

namespace apn::note
{
	//
	// このクラスは共通ノートウィンドウです。
	// このウィンドウはプラグインウィンドウのように振る舞います。
	//
	inline struct SharingNote : RichEditWindow
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			return __super::init(L"共通ノート");
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return __super::exit();
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_CREATE:
				{
					MY_TRACE_FUNC("WM_CREATE, {:#010x}, {:#010x}", wParam, lParam);

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {:#010x}, {:#010x}", wParam, lParam);

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} sharing_note;
}
