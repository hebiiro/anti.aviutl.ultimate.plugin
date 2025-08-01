#pragma once

namespace apn::text_editor
{
	//
	// このクラスはテキストアイテムのテキストエディットボックスのサブクラスです。
	//
	inline struct TextEditBox : my::Window
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// テキストエディットボックスをサブクラス化します。
			subclass(magi.exin.get_text_editbox());

			// editsのテキストに反映させます。
			app->to_edits();

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			// テキストエディットボックスのサブクラス化を解除します。
			unsubclass();

			return TRUE;
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_SETTEXT:
				{
					if (auto text =(LPCWSTR)lParam)
					{
						MY_TRACE_FUNC("WM_SETTEXT, {/}", text);

						// editsのテキストに反映させます。
						app->to_edits();
					}

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} text_editbox;
}
