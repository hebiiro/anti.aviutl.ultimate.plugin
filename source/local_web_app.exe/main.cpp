#include "pch.h"
#include "resource.h"
#include "app_interface.hpp"
#include "hive.hpp"
#include "utils.hpp"
#include "browser.hpp"
#include "config_io.hpp"
#include "main_window.hpp"
#include "app.hpp"

namespace apn::local_web_app
{
	int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPTSTR cmd_line, int cmd_show)
	{
		// デバッグトレースとCOMを初期化します。
		struct Initializer
		{
			GdiplusStartupInput gdi_si;
			GdiplusStartupOutput gdi_so;
			ULONG_PTR gdi_token = 0;
			ULONG_PTR gdi_hook_token = 0;

			Initializer()
			{
				_tsetlocale(LC_ALL, _T(""));
				::OleInitialize(nullptr);

				gdi_si.SuppressBackgroundThread = TRUE;
				GdiplusStartup(&gdi_token, &gdi_si, &gdi_so);
				gdi_so.NotificationHook(&gdi_hook_token);
			}

			~Initializer()
			{
				gdi_so.NotificationUnhook(gdi_hook_token);
				GdiplusShutdown(gdi_token);

				::OleUninitialize();
			}
		} initializer;

		MY_TRACE_FUNC("{/}", cmd_line);

		// インスタンスハンドルをハイブに格納します。
		hive.instance = instance;

		// ローカルコンテンツの起点となるフォルダを取得します。
		hive.origin_folder_path = my::get_module_file_name(instance).parent_path().parent_path();

		std::wstring orig_path, dummy_path;

		// コマンドライン引数からhtmlファイルのファイルパスを取得します。
		orig_path = cmd_line;
		orig_path = trim(orig_path, L"\"");
		MY_TRACE_STR(orig_path);

		hive.current_file_path = orig_path;
		if (!hive.current_file_path.empty() &&
			hive.current_file_path.extension() != L".html")
		{
			// 元のファイルは拡張子がhtmlではないのでhtmlファイルとして開けません。
			// なので、拡張子をhtmlに変更したダミーファイルの方をブラウザで開きます。

			try
			{
				// ダミーファイルのパスを取得します。
				dummy_path = orig_path + L".html";
				MY_TRACE_STR(dummy_path);

				try
				{
					// まず、作成先ファイルを削除します。
					std::filesystem::remove(dummy_path);

					// ダミーファイル(ハードリンク)を作成します。
					std::filesystem::create_hard_link(orig_path, dummy_path);
				}
				// ハードリンクを作成できなかった場合は
				catch (...)
				{
					// 通常のダミーファイルを作成します。
					std::filesystem::copy(orig_path, dummy_path,
						std::filesystem::copy_options::overwrite_existing |
						std::filesystem::copy_options::recursive);
				}

				hive.current_file_path = dummy_path;
			}
			catch (const std::exception& error)
			{
				hive.message_box(my::ws(error.what()));

				return -1;
			}
			catch (...)
			{
				hive.message_box(L"例外が発生しました");

				return -1;
			}
		}

		// アプリケーションを初期化します。
		if (!app->init())
		{
			hive.message_box(L"アプリケーションの初期化に失敗しました");

			return -1;
		}

		// メインウィンドウを表示します。
		::ShowWindow(main_window, cmd_show);

		// コンフィグを読み込みます。
		config_io.read();

		// メッセージループを開始します。
		MSG msg = {};
		while (::GetMessage(&msg, 0, 0, 0))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		// アプリケーションを終了します。
		app->exit();

		try
		{
			if (!dummy_path.empty())
			{
				// ダミーファイルを削除します。
				std::filesystem::remove(dummy_path);
			}
		}
		catch (...)
		{
			return -1;
		}

		MY_TRACE("プロセスが正常終了しました\n");

		// プロセスを終了します。
		return 0;
	}
}

int APIENTRY _tWinMain(HINSTANCE instance, HINSTANCE prev_instance, LPTSTR cmd_line, int cmd_show)
{
	return apn::local_web_app::WinMain(instance, prev_instance, cmd_line, cmd_show);
}
