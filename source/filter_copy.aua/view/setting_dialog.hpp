#pragma once

namespace apn::filter_copy::view
{
	//
	// このクラスは設定ダイアログをフックします。
	//
	inline struct setting_dialog_t : my::Window, Magi::CommandID::SettingDialog
	{
		//
		// 初期化処理です。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// 設定ダイアログのコンテキストメニューを拡張します。
			for (auto i = 0; i < magi.exin.get_setting_dialog_menu_count(); i++)
			{
				auto menu = magi.exin.get_setting_dialog_menu(i);
				auto sub_menu = ::GetSubMenu(menu, 0);

				if (i == 2)
				{
					::AppendMenu(sub_menu, MF_SEPARATOR, 0, nullptr);
					::AppendMenu(sub_menu, MF_STRING, c_create_clone, _T("完全な複製を下に作成"));
					::AppendMenu(sub_menu, MF_STRING, c_create_same_above, _T("同じフィルタ効果を上に作成"));
					::AppendMenu(sub_menu, MF_STRING, c_create_same_below, _T("同じフィルタ効果を下に作成"));
				}

				::AppendMenu(sub_menu, MF_SEPARATOR, 0, nullptr);
				::AppendMenu(sub_menu, MF_STRING, c_cut_filter, _T("このフィルタを切り取り"));
				::AppendMenu(sub_menu, MF_STRING, c_cut_filter_above, _T("このフィルタ以上を切り取り"));
				::AppendMenu(sub_menu, MF_STRING, c_cut_filter_below, _T("このフィルタ以下を切り取り"));
				::AppendMenu(sub_menu, MF_STRING, c_copy_filter, _T("このフィルタをコピー"));
				::AppendMenu(sub_menu, MF_STRING, c_copy_filter_above, _T("このフィルタ以上をコピー"));
				::AppendMenu(sub_menu, MF_STRING, c_copy_filter_below, _T("このフィルタ以下をコピー"));
				::AppendMenu(sub_menu, MF_STRING, c_paste_filter, _T("フィルタを貼り付け"));
			}

			return subclass(magi.exin.get_setting_dialog());
		}

		//
		// 後始末処理です。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return unsubclass();
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
			switch (message)
			{
			case WM_COMMAND:
				{
					switch (w_param)
					{
					case c_create_clone:
					case c_create_same_above:
					case c_create_same_below: return listener::create_filter->on_create_filter((uint32_t)w_param);
					case c_cut_filter: return listener::copy_filter->on_copy_filter(0, TRUE);
					case c_cut_filter_above: return listener::copy_filter->on_copy_filter(-1, TRUE);
					case c_cut_filter_below: return listener::copy_filter->on_copy_filter(1, TRUE);
					case c_copy_filter: return listener::copy_filter->on_copy_filter(0, FALSE);
					case c_copy_filter_above: return listener::copy_filter->on_copy_filter(-1, FALSE);
					case c_copy_filter_below: return listener::copy_filter->on_copy_filter(1, FALSE);
					case c_paste_filter: return listener::paste_filter->on_paste_filter();
					}

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}
	} setting_dialog;
}
