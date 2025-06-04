#pragma once

namespace apn::text_split
{
	//
	// このクラスはテキストオブジェクトを分解します。
	//
	inline struct TextSplit : Addin
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
		virtual BOOL on_window_init(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			// 予め使用予定のコンフィグフォルダを作成しておきます。
			magi.create_config_folders(hive.c_name, {});

			if (!addin_window.init()) return FALSE;
			if (!config_io.init()) return FALSE;

			// AviUtlにメニューアイテムを追加します。
			add_menu_item(fp, L"テキストを分解する", IDC_SPLIT_TEXT);

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
