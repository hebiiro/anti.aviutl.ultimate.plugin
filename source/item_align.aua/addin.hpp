#pragma once

namespace apn::item_align
{
	//
	// このクラスはタイムラインアイテムの位置を変化させます。
	//
	inline struct item_align_t : Addin
	{
		//
		// この仮想関数は、このアドインの識別名が必要なときに呼ばれます。
		//
		virtual LPCWSTR get_addin_name() override
		{
			return hive.c_name;
		}

		//
		// この仮想関数は、このアドインの表示名が必要なときに呼ばれます。
		//
		virtual LPCWSTR get_addin_display_name() override
		{
			return hive.c_display_name;
		}

		//
		// AviUtlにメニューアイテムを追加します。
		//
		BOOL add_menu_item(AviUtl::FilterPlugin* fp, LPCWSTR text, UINT id)
		{
			auto str = my::format(L"[{/}]{/}", hive.c_display_name, text);

			return fp->exfunc->add_menu_item(fp, my::hs(str).c_str(),
				addin_dialog, id, 0, AviUtl::ExFunc::AddMenuItemFlag::None);
		}

		//
		// この仮想関数は、ウィンドウの初期化を実行するときに呼ばれます。
		//
		virtual BOOL on_window_init(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			if (!addin_window.init()) return FALSE;
			if (!config_io.init()) return FALSE;

			// AviUtlにメニューアイテムを追加します。
			add_menu_item(fp, L"ずらす▼", idc_shift_down);
			add_menu_item(fp, L"ずらす▲", idc_shift_up);
			add_menu_item(fp, L"隙間(相対)←", idc_relative_space_left);
			add_menu_item(fp, L"隙間(相対)→", idc_relative_space_right);
			add_menu_item(fp, L"隙間(絶対)←", idc_absolute_space_left);
			add_menu_item(fp, L"隙間(絶対)→", idc_absolute_space_right);
			add_menu_item(fp, L"伸ばす←", idc_stretch_left);
			add_menu_item(fp, L"伸ばす→", idc_stretch_right);
			add_menu_item(fp, L"詰める←", idc_align_left);
			add_menu_item(fp, L"詰める→", idc_align_right);
			add_menu_item(fp, L"詰める▼", idc_move_vert_down);
			add_menu_item(fp, L"詰める▲", idc_move_vert_up);
			add_menu_item(fp, L"BPMズレを修正", idc_fix_bpm);
			add_menu_item(fp, L"現在位置の近くにある中間点を削除", idc_erase_midpt);

			if (!config_io.read()) MY_TRACE("コンフィグの読み込みに失敗しました\n");

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウの後始末を実行するときに呼ばれます。
		//
		virtual BOOL on_window_exit(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			config_io.write();

			config_io.exit();
			addin_window.exit();

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウコマンドを実行するときに呼ばれます。
		//
		virtual BOOL on_window_command(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			switch (w_param)
			{
			case magi.c_command_id.c_addin.c_command:
				{
					MY_TRACE_FUNC("magi.c_command_id.c_addin.c_command");

					// アドインウィンドウを表示します。
					if (::IsWindow(addin_window)) addin_window.show();

					break;
				}
			}

			return FALSE;
		}
	} addin;
}
