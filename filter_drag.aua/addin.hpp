#pragma once
#include "common/my/file_update_checker.hpp"

namespace apn::filter_drag
{
	//
	// このクラスは拡張編集の設定ダイアログにフィルタのドラッグ機能を追加します。
	//
	inline struct FilterDrag : Addin
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
		// この仮想関数は、ウィンドウの初期化のタイミングで呼び出されます。
		//
		virtual BOOL on_window_init(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
		{
			if (!aim_src.init()) return FALSE;
			if (!aim_dst.init()) return FALSE;
			if (!sight.init()) return FALSE;
			if (!config_io.init()) return FALSE;
			if (!hook_manager.init()) return FALSE;
//			if (!keyboard_hook.init()) return FALSE;
//			if (!config_file_checker.init()) return FALSE;

			return TRUE;
		}

		//
		// この仮想関数は、ウィンドウの後始末のタイミングで呼び出されます。
		//
		virtual BOOL on_window_exit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
		{
//			config_file_checker.exit();
//			keyboard_hook.exit();
			hook_manager.exit();
			config_io.exit();
			sight.exit();
			aim_dst.exit();
			aim_src.exit();

			return TRUE;
		}

		//
		// このクラスはコンフィグファイルの更新を監視します。
		// これにより、ユーザーがコンフィグファイルを直接編集した場合でも
		// 変更が即時に反映されるようになります。
		//
		struct ConfigFileChecker : my::FileUpdateChecker
		{
			//
			// 初期化処理を実行します。
			//
			BOOL init()
			{
				MY_TRACE_FUNC("");

				// ファイルアップデートチェッカー(スーパークラス)を初期化します。
				__super::init(config_io.get_config_file_name(hive.instance).c_str());

				// コンフィグファイルを監視するタイマーを作成します。
				::SetTimer(0, 0, 1000,
					[](HWND hwnd, UINT message, UINT_PTR timerId, DWORD time)
					{
						if (addin.config_file_checker.check_update())
						{
							MY_TRACE("コンフィグファイルが更新されました\n");

							config_io.init(); // コンフィグファイルを再ロードします。
						}
					});

				return TRUE;
			}

			//
			// 後始末処理を実行します。
			//
			BOOL exit()
			{
				MY_TRACE_FUNC("");

				return TRUE;
			}
		} config_file_checker;
	} addin;
}
