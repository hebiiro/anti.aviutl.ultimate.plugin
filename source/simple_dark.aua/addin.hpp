#pragma once

namespace apn::dark
{
	//
	// このクラスはUIにカスタムテーマを適用できるようにします。
	//
	inline struct simple_dark_addin_t : Addin
	{
		//
		// コンストラクタです。
		//
		simple_dark_addin_t()
		{
			MY_TRACE_FUNC("");

			hive.addin = this;
		}

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
		// この仮想関数は、ウィンドウの初期化を実行するときに呼ばれます。
		//
		virtual BOOL on_window_init(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			// 予め使用予定のコンフィグフォルダを作成しておきます。
			magi.create_config_folders(hive.c_name, {});

			// 内部/外部プロセス共通の初期化処理を実行します。
			app->on_initialize_plugin(110);

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウの後始末を実行するときに呼ばれます。
		//
		virtual BOOL on_window_exit(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			// 内部/外部プロセス共通の後始末処理を実行します。
			app->on_uninitialize_plugin();

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
#if 0
		//
		// この仮想関数は、シンボルをエクスポートします。
		//
		virtual my::addr_t on_get_address(LPCWSTR _name) override
		{
			std::wstring name(_name);

			if (name == L"skin") return (my::addr_t)&exports.skin;
			if (name == L"theme_manager") return (my::addr_t)&exports.theme_manager;
			if (name == L"stuff_manager") return (my::addr_t)&exports.stuff_manager;
			if (name == L"visual_style_manager") return (my::addr_t)&exports.visual_style_manager;
			if (name == L"dwm_manager") return (my::addr_t)&exports.dwm_manager;
			if (name == L"icon_manager") return (my::addr_t)&exports.icon_manager;
			if (name == L"config_manager") return (my::addr_t)&exports.config_manager;
			if (name == L"painter") return (my::addr_t)&exports.painter;
			if (name == L"gdiplus") return (my::addr_t)&exports.gdiplus;

			return 0;
		}
#endif
	} addin;
}
