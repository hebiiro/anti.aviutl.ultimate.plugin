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
	//
	// エントリポイントです。
	//
	EXTERN_C int APIENTRY _tWinMain(HINSTANCE instance, HINSTANCE prev_instance, LPTSTR cmd_line, int cmd_show)
	{
		// デバッグトレースとCOMを初期化します。
		struct initializer_t
		{
			GdiplusStartupInput gdi_si;
			GdiplusStartupOutput gdi_so;
			ULONG_PTR gdi_token = 0;
			ULONG_PTR gdi_hook_token = 0;

			initializer_t()
			{
				_tsetlocale(LC_ALL, _T(""));
				::OleInitialize(nullptr);

				gdi_si.SuppressBackgroundThread = TRUE;
				GdiplusStartup(&gdi_token, &gdi_si, &gdi_so);
				gdi_so.NotificationHook(&gdi_hook_token);

				my::tracer_to_file::init(nullptr);

				MY_TRACE_FUNC("");
			}

			~initializer_t()
			{
				MY_TRACE_FUNC("");

				my::tracer_to_file::exit();

				gdi_so.NotificationUnhook(gdi_hook_token);
				GdiplusShutdown(gdi_token);

				::OleUninitialize();
			}
		} initializer;

		MY_TRACE_FUNC("{/}", cmd_line);

		// インスタンスハンドルをハイブに格納します。
		hive.instance = instance;
		MY_TRACE_HEX(hive.instance);

		// ローカルコンテンツの起点となるフォルダを取得します。
		hive.origin_folder_path = my::get_module_file_name(instance).parent_path().parent_path();
		MY_TRACE_STR(hive.origin_folder_path);

		// コマンドライン文字列を取得します。
		auto command_line = ::GetCommandLineW();

		// コマンドライン引数を取得します。
		auto argc = int {};
		auto args = ::CommandLineToArgvW(command_line, &argc);
		MY_TRACE_INT(argc);
#ifdef _DEBUG
		// デバッグ用の処理です。
		for (int i = 0; i < argc; i++) MY_TRACE_STR(args[i]);
#endif
		// 引数の数が無効の場合は
		if (argc < 2)
		{
			// メッセージボックスを表示します。
			hive.message_box(L"引数の数が無効です");

			// プロセスを異常終了します。
			return -1;
		}

		// コマンドライン引数からhtmlファイルのファイルパスを取得します。
		std::wstring orig_path = args[1], dummy_path;

		// カレントファイルのパスをセットします。
		hive.current_file_path = orig_path;

		// カレントファイルのパスが無効の場合は
		if (hive.current_file_path.empty())
		{
			// メッセージボックスを表示します。
			hive.message_box(L"カレントファイルのパスが無効です");

			// プロセスを異常終了します。
			return -1;
		}

		// カレントファイルがhtmlではない場合は
		if (hive.current_file_path.extension() != L".html")
		{
			// 拡張子がhtmlではないので、htmlファイルとして開けません。
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

					// ハードリンクを作成できなかった場合は
					if (!std::filesystem::exists(dummy_path))
					{
						// 例外を投げます。
						throw std::filesystem::filesystem_error(
							"ハードリンクの作成に失敗しました",
							dummy_path,
							std::make_error_code(std::errc::no_such_file_or_directory));
					}
				}
				// ハードリンクを作成できなかった場合は
				catch (...)
				{
					// 通常のダミーファイルを作成します。
					std::filesystem::copy(orig_path, dummy_path,
						std::filesystem::copy_options::overwrite_existing |
						std::filesystem::copy_options::recursive);
				}

				// ファイルを作成できなかった場合は
				if (!std::filesystem::exists(dummy_path))
				{
					// 例外を投げます。
					throw std::filesystem::filesystem_error(
						"ダミーファイルの作成に失敗しました",
						dummy_path,
						std::make_error_code(std::errc::no_such_file_or_directory));
				}

				// カレントファイルのパスを再セットします。
				hive.current_file_path = dummy_path;
				MY_TRACE_STR(hive.current_file_path);
			}
			catch (const std::exception& error)
			{
				// メッセージボックスを表示します。
				hive.message_box(my::ws(error.what()));

				// プロセスを異常終了します。
				return -1;
			}
			catch (...)
			{
				// メッセージボックスを表示します。
				hive.message_box(L"例外が発生しました");

				// プロセスを異常終了します。
				return -1;
			}
		}

		// アプリケーションを初期化します。
		if (!app->init())
		{
			// メッセージボックスを表示します。
			hive.message_box(L"アプリケーションの初期化に失敗しました");

			// プロセスを異常終了します。
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
			// メッセージをディスパッチします。
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		// アプリケーションを終了します。
		app->exit();

		try
		{
			// ダミーファイルが有効の場合は
			if (dummy_path.length())
			{
				// ダミーファイルを削除します。
				std::filesystem::remove(dummy_path);
			}
		}
		catch (...)
		{
			// プロセスを異常終了します。
			return -1;
		}

		MY_TRACE("プロセスが正常終了しました\n");

		// プロセスを終了します。
		return 0;
	}
}
