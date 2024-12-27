#pragma once

namespace apn::audio_visualizer::ui
{
	//
	// このクラスはメインウィンドウです。
	//
	inline struct MainWindow : my::Window
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// ウィンドウクラスを登録します。
			WNDCLASSEXW wc = { sizeof(wc) };
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = hive.instance;
			wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
			wc.lpszClassName = _T("audio_visualizer.ui_window");
			::RegisterClassExW(&wc);

			// 非表示のオーバラップウィンドウを作成します。
			return __super::create(
				WS_EX_NOPARENTNOTIFY,
				wc.lpszClassName,
				hive.c_display_name,
				WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				100, 100, 600, 600,
				nullptr, nullptr, hive.instance, nullptr);
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			// ウィンドウを破壊します。
			return destroy();
		}

		//
		// 音声を表示するまでの待機時間を返します。
		//
		BOOL wait(const share::AudioArtifact* audio_artifact)
		{
			// 音声アーティファクトの時間が有効の場合は
			if (audio_artifact->time)
			{
				// 現在時間を取得します。
				auto time = ::timeGetTime();

				if (audio_artifact->time > time)
				{
					auto due_time = audio_artifact->time - time;
					MY_TRACE("{}, {:.3f}, {:.3f}, {:.3f}秒後に表示します\n",
						audio_artifact->frame, time / 1000.0, audio_artifact->time / 1000.0, due_time / 1000.0);

					::Sleep(due_time);
				}
				else if (audio_artifact->time + 5 < time)
				{
					MY_TRACE("{}, {:.3f}, {:.3f}, 表示が間に合っていないのでスキップします\n",
						audio_artifact->frame, time / 1000.0, audio_artifact->time / 1000.0);

					return FALSE;
				}
			}

			return TRUE;
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_CREATE:
				{
					MY_TRACE_FUNC("WM_CREATE, {:#010x}, {:#010x}", wParam, lParam);

					// このウィンドウをメインウィンドウに設定します。
					share::host_window::set_ui_window(hive.host_window, hive.main_window = hwnd);

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {:#010x}, {:#010x}", wParam, lParam);

					break;
				}
			case share::c_message.c_init_process:
				{
					MY_TRACE_FUNC("c_init_process, {:#010x}, {:#010x}", wParam, lParam);

					// 設定を読み込みます。
					app->read_config();

					break;
				}
			case share::c_message.c_exit_process:
				{
					MY_TRACE_FUNC("c_exit_process, {:#010x}, {:#010x}", wParam, lParam);

					// 設定を保存します。
					app->write_config();

					// プロセスを終了します。
					::PostQuitMessage(0);

					break;
				}
			case share::c_message.c_init_co_window:
				{
					MY_TRACE_FUNC("c_init_co_window, {:#010x}, {:#010x}", wParam, lParam);

					// コウィンドウを取得します。
					auto co_window = (HWND)wParam;

					// ビジュアルを作成します。
					if (auto visual = visual_manager.create_visual(co_window))
					{
						// コウィンドウにビジュアルの作成が完了したことを通知します。
						::PostMessage(co_window, share::c_message.c_init_co_window, (WPARAM)co_window, (LPARAM)(HWND)*visual);
					}

					break;
				}
			case share::c_message.c_exit_co_window:
				{
					MY_TRACE_FUNC("c_exit_co_window, {:#010x}, {:#010x}", wParam, lParam);

					// コウィンドウを取得します。
					auto co_window = (HWND)wParam;

					// ビジュアルを取得します。
					auto visual = (HWND)lParam;

					// ビジュアルを削除します。
					visual_manager.erase_visual(visual);

					break;
				}
			case share::c_message.c_update_option:
				{
					MY_TRACE_FUNC("c_update_option, {:#010x}, {:#010x}", wParam, lParam);

					// ビジュアルを取得します。
					auto visual = (HWND)wParam;

					// ファイルマッピングを取得します。
					my::handle::unique_ptr<> file_mapping((HANDLE)lParam);
					MY_TRACE_HEX(file_mapping.get());
					if (!file_mapping) break;

					// オプションを取得します。
					my::SharedMemory<share::Option> option(file_mapping.get());
					MY_TRACE_HEX(option.get());
					if (!option) break;

					// ビジュアルにオプションの更新を通知します。
					visual_manager.notify_update_option(visual, *option);

					break;
				}
			case share::c_message.c_update_audio:
				{
					MY_TRACE_FUNC("c_update_audio, {:#010x}, {:#010x}", wParam, lParam);

					// ファイルマッピングを取得します。
					my::handle::unique_ptr<> file_mapping((HANDLE)lParam);
					MY_TRACE_HEX(file_mapping.get());
					if (!file_mapping) break;

					// 音声アーティファクトを取得します。
					my::SharedMemory<share::AudioArtifact> audio_artifact(file_mapping.get());
					MY_TRACE_HEX(audio_artifact.get());
					if (!audio_artifact) break;

					// 表示タイミングが早すぎる場合は少し待機します。
					if (!wait(audio_artifact))
						break;

					// ビジュアルに音声の更新を通知します。
					visual_manager.notify_update_audio(*audio_artifact);

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} main_window;
}
