#pragma once
#include "Hive.h"
#include "View.h"

namespace fgo::audio_graph_gui
{
	//
	// このクラスは音声グラフGUI全体の流れです。
	//
	inline struct App
	{
		//
		// 初期化を行います。
		//
		BOOL init(HINSTANCE instance, HWND hostWindow)
		{
			hive.instance = instance;

			_tsetlocale(LC_CTYPE, _T(""));

			Tools::Trace::File::init(0, 0, TRUE);

			::CoInitialize(0);

			MY_TRACE_FUNC("0x%08X", hostWindow);

			hive.init(hostWindow);
			hive.load();

			host_process_checker.init();
			config_file_checker.init();

			return TRUE;
		}

		//
		// 後始末を行います。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			config_file_checker.exit();
			host_process_checker.exit();

			hive.save();
			hive.exit();

			Tools::Trace::File::exit();

			::CoUninitialize();

			return TRUE;
		}

		//
		// メッセージループです。
		//
		BOOL messageLoop()
		{
			MY_TRACE_FUNC("");

			MSG msg = {};
			while (::GetMessage(&msg, 0, 0, 0) > 0)
			{
				// Share::AudioGraph::Message::Init(スレッドメッセージ)をハンドルします。
				if (msg.hwnd == 0 && msg.message == Share::AudioGraph::Message::Init)
				{
					auto graphContainer = (HWND)msg.wParam;
					auto graphWindowType = (int)msg.lParam;

					// グラフウィンドウを作成します。
					auto graphWindow = view.createGraphWindow(graphContainer, graphWindowType);
					if (graphWindow)
					{
						// グラフコンテナにグラフウィンドウのハンドルを渡します。
						::PostMessage(graphContainer, Share::AudioGraph::Message::Init, (WPARAM)(HWND)*graphWindow, 0);
					}

					continue;
				}

				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}

			return TRUE;
		}

		//
		// エントリポイントです。
		//
		int WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPTSTR cmdLine, int cmdShow)
		{
			MY_TRACE_FUNC("");

			if (!init(instance, (HWND)_tcstoul(::GetCommandLine(), 0, 0))) return 0;
			if (!messageLoop()) return 0;
			if (!exit()) return 0;

			return 0;
		}

		//
		// このクラスはホストプロセスを監視します。
		// ホストプロセスが無効になった場合はカレントプロセスを終了します。
		// これにより、ホストプロセスが強制終了したとしても、
		// クライアントプロセス(カレントプロセス)の残骸が残らないようになります。
		//
		struct HostProcessChecker
		{
			//
			// 初期化を行います。
			//
			BOOL init()
			{
				MY_TRACE_FUNC("");

				// ホストプロセスを監視するスレッドタイマーを作成します。
				// カレントプロセス内のウィンドウが破壊されたあとも
				// チェックしなければならないのでWM_TIMERは使用できません。
				// ホストプロセス(のウィンドウ)はハイブに格納されています。
				::SetTimer(0, 0, 1000, timerProc);

				return TRUE;
			}

			//
			// 後始末を行います。
			//
			BOOL exit()
			{
				MY_TRACE_FUNC("");

				return TRUE;
			}

			//
			// タイマー関数です。
			//
			static void CALLBACK timerProc(HWND hwnd, UINT message, UINT_PTR timerId, DWORD time)
			{
				if (!::IsWindow(hive.hostWindow))
				{
					MY_TRACE(_T("ホストプロセスが無効になりました\n"));

					::PostQuitMessage(0);
				}
			}
		} host_process_checker;

		//
		// このクラスはコンフィグファイルを監視します。
		// これにより、ユーザーがコンフィグファイルを直接編集した場合でも、
		// 変更が即時に反映されるようになります。
		//
		struct ConfigFileChecker : FileUpdateChecker
		{
			//
			// 初期化を行います。
			//
			BOOL init()
			{
				MY_TRACE_FUNC("");

				// ファイルアップデートチェッカー(スーパークラス)を初期化します。
				__super::init(hive.configFileName);

				// コンフィグファイルを監視するスレッドタイマーを作成します。
				::SetTimer(0, 0, 1000, timerProc);

				return TRUE;
			}

			//
			// 後始末を行います。
			//
			BOOL exit()
			{
				MY_TRACE_FUNC("");

				return TRUE;
			}

			//
			// タイマー関数です。
			//
			static void CALLBACK timerProc(HWND hwnd, UINT message, UINT_PTR timerId, DWORD time)
			{
				if (app.config_file_checker.isFileUpdated())
				{
					MY_TRACE(_T("コンフィグファイルが更新されました\n"));

					hive.load(); // ハイブにコンフィグファイルを再ロードさせます。(ドキュメントの再読み込み)
					view.updateDesign(); // ビューのデザインを更新します。(ビューの再描画)
				}
			}
		} config_file_checker;
	} app;
}
