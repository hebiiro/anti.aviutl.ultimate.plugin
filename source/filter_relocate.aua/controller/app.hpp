#pragma once

namespace apn::filter_relocate::controller
{
	//
	// このクラスはコントローラー層のアプリケーションです。
	//
	inline struct app_t : view::main_dialog_t::listener_t
	{
		//
		// aviutlにメニュー項目を追加します。
		//
		inline static BOOL add_menu_item(AviUtl::FilterPlugin* fp, LPCWSTR text, UINT id)
		{
			auto str = my::format(L"[{/}]{/}", model::property.c_display_name, text);

			return fp->exfunc->add_menu_item(fp, my::hs(str).c_str(),
				view::main_dialog, id, 0, AviUtl::ExFunc::AddMenuItemFlag::None);
		}

		//
		// アプリ全体の初期化処理を実行します。
		//
		BOOL init(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
		{
			MY_TRACE_FUNC("");

			// このクラスをメインダイアログのリスナーにセットします。
			view::main_dialog.listener = this;

			// 予め使用予定のコンフィグフォルダを作成しておきます。
			magi.create_config_folders(model::property.c_name, {});

			if (!controller::config_io.init()) return FALSE;
			if (!controller::hook::manager.init()) return FALSE;
			if (!view::main_window.init()) return FALSE;

			// aviutlにメニュー項目を追加します。
			add_menu_item(fp, L"フィルタを分離", idc_split_item_below);
/*
			add_menu_item(fp, L"グループ制御にフィルタを分離", idc_split_item_above);
			add_menu_item(fp, L"フィルタを結合", idc_merge_item_below);
			add_menu_item(fp, L"グループ制御のフィルタを結合", idc_merge_item_above);
*/
			if (!controller::config_io.read()) MY_TRACE("コンフィグの読み込みに失敗しました\n");

			return FALSE;
		}

		//
		// アプリ全体の後始末処理を実行します。
		//
		BOOL exit(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
		{
			MY_TRACE_FUNC("");

			controller::config_io.write();

			view::main_window.exit();
			controller::hook::manager.exit();
			controller::config_io.exit();

			return FALSE;
		}

		//
		// ウィンドウコマンドを実行します。
		//
		BOOL window_command(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
		{
			switch (w_param)
			{
			case magi.c_command_id.c_addin.c_command:
				{
					MY_TRACE_FUNC("magi.c_command_id.c_addin.c_command");

					// メインウィンドウを表示します。
					if (::IsWindow(view::main_window)) view::main_window.show();

					break;
				}
			}

			return FALSE;
		}

		//
		// 選択アイテムのフィルタを下のアイテムに分離します。
		//
		virtual BOOL on_split_item_below() override
		{
			return do_action([](){ action::split_item_below_t().execute(); });
		}

		//
		// 指定されたアクションを実行します。
		//
		BOOL do_action(auto func)
		{
			try
			{
				return func(), TRUE;
			}
			catch (const std::wstring& error)
			{
				view::main_window.message_box(error);
			}
			catch (...)
			{
				view::main_window.message_box(L"未知の例外が発生しました");
			}

			return FALSE;
		}
	} app;
}
