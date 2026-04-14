#pragma once

namespace apn::volume_meter::sub_thread
{
	//
	// このクラスはサブスレッドのコントローラーです。
	//
	inline struct controller_t
	{
		//
		// サブスレッドのIDです。
		//
		DWORD tid = 0;

		//
		// サブスレッドです。
		//
		std::thread cpp_thread;

		//
		// 初期化処理を実行します。
		//
		BOOL init(HWND parent)
		{
			MY_TRACE_FUNC("");

			try
			{
				std::mutex mutex;
				std::condition_variable cv;
				auto is_ready = FALSE;

				// サブスレッドを開始します。
				cpp_thread = std::thread(
					[&]
				{
					// (win32の)スレッドIDを取得します。
					tid = ::GetCurrentThreadId();

					// メッセージループを作成します。
					{
						MSG msg = {};
						::PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE);
					}

					// 準備完了のフラグをセットします。
					{
						std::lock_guard<std::mutex> lock(mutex);

						is_ready = true;
					}

					// 準備が完了したことを待機スレッドに通知します。
					{
						cv.notify_one();
					}

					// メッセージループを開始します。
					return message_loop();
				});

				// サブスレッドの準備が完了するまで待機します。
				{
					std::unique_lock lock(mutex);

					cv.wait(lock, [&]{ return is_ready; });
				}

				// 遅延初期化処理を予約します。
				auto result = ::PostThreadMessage(
					tid, hive.c_message.c_post_init, 0, (LPARAM)parent);

				// このあと使用することはないのでデタッチします。
				cpp_thread.detach();

				return TRUE;
			}
			catch (...)
			{
			}

			return FALSE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			// サブスレッドを終了します。
			::PostThreadMessage(tid, WM_QUIT, 0, 0);

			return TRUE;
		}

		//
		// 遅延初期化処理を実行します。
		//
		BOOL on_post_init(HWND parent)
		{
			MY_TRACE_FUNC("");

			return view.init(parent);
		}

		//
		// 遅延後始末処理を実行します。
		//
		BOOL on_post_exit()
		{
			MY_TRACE_FUNC("");

			return view.exit();
		}

		//
		// サブスレッド内で描画設定をセットします。
		//
		BOOL on_set_paint_option(const std::unique_ptr<paint_option_t>& paint_option)
		{
			MY_TRACE_FUNC("");

			// 描画設定が更新された場合は
			if (model.set_paint_option(paint_option))
				return view.redraw(); // ビューを再描画します。

			return FALSE;
		}

		//
		// サブスレッド内で生の音声データをセットします。
		//
		BOOL on_set_raw_audio_data(const std::unique_ptr<raw_audio_data_t>& raw_audio_data)
		{
			MY_TRACE_FUNC("");

			return view.add_volume(raw_audio_data);
		}

		//
		// サブスレッド内で映像タイミングをセットします。
		//
		BOOL on_set_video_timing(const std::unique_ptr<timing_t>& timing)
		{
			MY_TRACE_FUNC("");

			// 映像タイミングをセットします。
			return view.set_video_timing(timing);
		}

		//
		// サブスレッドのメッセージループです。
		//
		DWORD message_loop()
		{
			MSG msg = {};
			while (::GetMessage(&msg, 0, 0, 0) > 0)
			{
				if (!msg.hwnd)
				{
					switch (msg.message)
					{
					case hive.c_message.c_post_init:
						{
//							MY_TRACE_FUNC("c_post_init");

							// 遅延初期化処理を実行します。
							on_post_init((HWND)msg.lParam);

							break;
						}
					case hive.c_message.c_set_paint_option:
						{
//							MY_TRACE_FUNC("c_set_paint_option");

							// 描画設定をセットします。
							on_set_paint_option(to_unique((paint_option_t*)msg.lParam));

							break;
						}
					case hive.c_message.c_set_raw_audio_data:
						{
//							MY_TRACE_FUNC("c_set_raw_audio_data");

							// 生の音声データをセットします。
							on_set_raw_audio_data(to_unique((raw_audio_data_t*)msg.lParam));

							break;
						}
					case hive.c_message.c_set_video_timing:
						{
//							MY_TRACE_FUNC("c_set_video_timing");

							// 映像信号タイミングをセットします。
							on_set_video_timing(to_unique((timing_t*)msg.lParam));

							break;
						}
					case WM_QUIT:
						{
							// 遅延後始末処理を実行します。
							on_post_exit();

							// スレッドを終了します。
							return 0;
						}
					}
				}

				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}

			return 0;
		}
	} controller;
}
