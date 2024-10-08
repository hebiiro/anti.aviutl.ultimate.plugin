#pragma once

namespace apn::item_align
{
	//
	// このクラスはタイムラインアイテムの位置を変化させます。
	//
	inline struct ItemAlign : Addin
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
			auto str = std::format(L"[{}]{}", hive.c_display_name, text);

			return fp->exfunc->add_menu_item(fp, my::hs(str).c_str(),
				addin_dialog, id, 0, AviUtl::ExFunc::AddMenuItemFlag::None);
		}

		//
		// この仮想関数は、ウィンドウの初期化を実行するときに呼ばれます。
		//
		virtual BOOL on_window_init(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			if (!addin_window.init()) return FALSE;
			if (!config_io.init()) return FALSE;

			// AviUtlにメニューアイテムを追加します。
			add_menu_item(fp, L"ずらす▼", IDC_SHIFT_DOWN);
			add_menu_item(fp, L"ずらす▲", IDC_SHIFT_UP);
			add_menu_item(fp, L"隙間(相対)◀", IDC_REL_SPACE_LEFT);
			add_menu_item(fp, L"隙間(相対)▶", IDC_REL_SPACE_RIGHT);
			add_menu_item(fp, L"隙間(絶対)◀", IDC_ABS_SPACE_LEFT);
			add_menu_item(fp, L"隙間(絶対)▶", IDC_ABS_SPACE_RIGHT);
			add_menu_item(fp, L"伸ばす◀", IDC_STRETCH_LEFT);
			add_menu_item(fp, L"伸ばす▶", IDC_STRETCH_RIGHT);
			add_menu_item(fp, L"詰める◀", IDC_ALIGN_LEFT);
			add_menu_item(fp, L"詰める▶", IDC_ALIGN_RIGHT);
			add_menu_item(fp, L"詰める▼", IDC_MOVE_VERT_DOWN);
			add_menu_item(fp, L"詰める▲", IDC_MOVE_VERT_UP);
			add_menu_item(fp, L"BPMズレを修正", IDC_FIX_BPM);
			add_menu_item(fp, L"現在位置の近くにある中間点を削除", IDC_ERASE_MIDPT);

			if (!config_io.read()) MY_TRACE("コンフィグの読み込みに失敗しました\n");

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウの後始末を実行するときに呼ばれます。
		//
		virtual BOOL on_window_exit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
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
		virtual BOOL on_window_command(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			switch (wParam)
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
