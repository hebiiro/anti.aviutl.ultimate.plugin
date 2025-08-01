#pragma once

namespace apn::text_editor
{
	//
	// このクラスは設定ダイアログのサブクラスです。
	//
	inline struct SettingDialog : my::Window
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// 設定ダイアログをサブクラス化します。
			subclass(magi.exin.get_setting_dialog());

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			// 設定ダイアログのサブクラス化を解除します。
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
			case WM_COMMAND:
				{
					auto code = HIWORD(wParam);
					auto control_id = LOWORD(wParam);
					auto control = (HWND)lParam;

					MY_TRACE_FUNC("WM_COMMAND, {/hex16}, {/hex16}, {/hex}", code, control_id, control);

					// テキスト編集エディットボックスに変更があった場合は
					if (control == magi.exin.get_text_editbox() && code == EN_UPDATE)
					{
						MY_TRACE("テキスト編集エディットボックスのテキストが変更されました\n");

						// editsにも反映します。
						app->to_edits();
					}

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} setting_dialog;
}
