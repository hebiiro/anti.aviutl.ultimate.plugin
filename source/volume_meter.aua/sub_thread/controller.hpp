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
		// 再生が開始されたフレームです。
		//
		int32_t start_frame = 0;

		//
		// 再生が開始された時間です。
		//
		DWORD start_time = 0;

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
		// 描画設定をセットします。
		//
		BOOL set_paint_option(const paint_option_t& paint_option)
		{
			MY_TRACE_FUNC("");

			// サブスレッドに描画設定を送信します。
			return ::PostThreadMessage(tid, hive.c_message.c_set_paint_option,
				0, (LPARAM)std::make_unique<paint_option_t>(paint_option).release());
		}

		//
		// 生の音声データをセットします。
		//
		BOOL set_raw_audio_data(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip)
		{
			MY_TRACE_FUNC("");

			// ファイル情報を取得します。
			AviUtl::FileInfo fi = {};
			fp->exfunc->get_file_info(fpip->editp, &fi);

			//
			// この関数はフレーム番号をミリ秒に変換して返します。
			//
			const auto frame_to_ms = [&](int32_t frame) -> int32_t
			{
				if (fi.video_rate != 0 && fi.video_scale != 0)
					return (int32_t)(frame * 1000.0f * fi.video_scale / fi.video_rate);
				else
					return 0;
			};

			// 対応していない音声チャンネル数の場合は何もしません。
			if (fpip->audio_ch != 1 && fpip->audio_ch != 2)
				return FALSE;

			// 対応していない音声サンプル数の場合は何もしません。
			if (fpip->audio_n == 0)
				return FALSE;

			auto time = DWORD {};

			// 現在フレームを取得します。
			auto current_frame = fp->exfunc->get_frame(fpip->editp);

			// 処理中フレームを取得します。
			auto processing_frame = fpip->frame;

			// 処理中フレームと現在フレームが同じの場合は
			if (current_frame == processing_frame)
			{
				MY_TRACE("再生中ではありません\n");

				start_time = 0;
			}
			// 処理中フレームと現在フレームが異なる場合は
			else
			{
				// 再生中ではない場合は
				if (!magi.auin.is_playing())
				{
					// 他のプラグインから呼ばれている可能性があるので何もしません。
					return FALSE;
				}

				if (start_time == 0 || start_frame >= fpip->frame)
				{
					MY_TRACE("再生が開始されました\n");

					start_frame = fpip->frame;
					start_time = ::timeGetTime();

					time = start_time;
				}
				else
				{
					MY_TRACE("再生中です\n");

					MY_TRACE_INT(fpip->frame);
					MY_TRACE_INT(start_frame);

					auto offset = frame_to_ms(fpip->frame - start_frame);
					MY_TRACE_INT(offset);

					time = start_time + offset;
					MY_TRACE_INT(time);
				}
			}

			// サブスレッドに生の音声データを送信します。
			return ::PostThreadMessage(tid, hive.c_message.c_set_raw_audio_data,
				0, (LPARAM)std::make_unique<raw_audio_data_t>(fp, fpip, fi, time).release());
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

			//
			// この関数は音量の表示タイミングになるまで待機します。
			//
			const auto wait = [](const volume_t& volume) -> BOOL
			{
				// 音量の時間が有効の場合は
				if (volume.time)
				{
					// 現在時間を取得します。
					auto time = ::timeGetTime();

					if (volume.time > time)
					{
						auto due_time = volume.time - time;
						MY_TRACE("{/}, {/.3f}, {/.3f}, {/.3f}秒後に表示します\n",
							volume.frame, time / 1000.0, volume.time / 1000.0, due_time / 1000.0);

						::Sleep(due_time);
					}
					else if (volume.time + 5 < time)
					{
						MY_TRACE("{/}, {/.3f}, {/.3f}, 表示が間に合っていないのでスキップします\n",
							volume.frame, time / 1000.0, volume.time / 1000.0);

						return FALSE;
					}
				}

				return TRUE;
			};

			// 音量が更新された場合は
			if (model.set_volume(raw_audio_data))
			{
				// 表示タイミングになるまで待機します。
				if (!wait(model.volume))
					return FALSE; // 表示タイミングが間に合っていない場合は何もしません。

				return view.redraw(); // ビューを再描画します。
			}

			return FALSE;
		}

		//
		// サブスレッドのメッセージループです。
		//
		DWORD message_loop()
		{
			MSG msg = {};
			while (::GetMessage(&msg, 0, 0, 0))
			{
				if (!msg.hwnd)
				{
					switch (msg.message)
					{
					case hive.c_message.c_post_init:
						{
							MY_TRACE_FUNC("c_post_init");

							// 遅延初期化処理を実行します。
							on_post_init((HWND)msg.lParam);

							break;
						}
					case hive.c_message.c_set_paint_option:
						{
							MY_TRACE_FUNC("c_set_paint_option");

							// 描画設定をセットします。
							on_set_paint_option(to_unique((paint_option_t*)msg.lParam));

							break;
						}
					case hive.c_message.c_set_raw_audio_data:
						{
							MY_TRACE_FUNC("c_set_raw_audio_data");

							// 生の音声データをセットします。
							on_set_raw_audio_data(to_unique((raw_audio_data_t*)msg.lParam));

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
