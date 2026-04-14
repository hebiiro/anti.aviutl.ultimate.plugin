#pragma once

namespace apn::volume_meter::sub_thread
{
	//
	// このクラスはサブスレッドのビューです。
	// メインスレッドのビューの子ウィンドウとして作成されます。
	//
	inline struct view_t : my::Window
	{
		//
		// タイマーの初期IDです。
		//
		inline static constexpr UINT_PTR c_timer_id = 2026;

		// タイマーの間隔です。
		inline static constexpr int32_t c_timer_elapse = 16;

		//
		// 現在のタイマーIDです。
		//
		UINT_PTR timer_id = {};

		//
		// 現在の音量です。
		//
		volume_t current_volume;

		//
		// 再生中の音量です。
		//
		std::list<volume_t> playlist;

		//
		// 映像が作成されたタイミングです。
		//
		timing_t video_timing {};

		//
		// 初期化処理を実行します。
		//
		BOOL init(HWND parent)
		{
			MY_TRACE_FUNC("");

			WNDCLASSEX wc = { sizeof(wc) };
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = hive.instance;
			wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
			wc.lpszClassName = _T("volume_meter.sub_thread.view");
			::RegisterClassEx(&wc);

			return __super::create(
				WS_EX_NOPARENTNOTIFY,
				wc.lpszClassName,
				wc.lpszClassName,
				WS_VISIBLE | WS_CHILD |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				0, 0, 0, 0,
				parent, nullptr, hive.instance, nullptr);
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
		// 音量を再描画します。
		//
		BOOL redraw()
		{
			MY_TRACE_FUNC("");

			// ウィンドウを再描画します。
//			return ::InvalidateRect(*this, nullptr, FALSE);
			return ::InvalidateRect(*this, nullptr, FALSE), ::UpdateWindow(*this);
		}

		//
		// 描画する音量を追加します。
		//
		BOOL add_volume(const std::unique_ptr<raw_audio_data_t>& raw_audio_data)
		{
			MY_TRACE_FUNC("{/}, {/}", raw_audio_data->audio_timing.frame, raw_audio_data->current_frame);

			// 再生中の場合は
			if (raw_audio_data->flag_is_playing)
			{
				MY_TRACE("再生中です\n");

				// タイマーIDが無効の場合は
				if (!timer_id)
				{
					MY_TRACE("タイマーを開始します\n");

					// タイマーを開始します。
					timer_id = ::SetTimer(*this, c_timer_id, c_timer_elapse, nullptr);

					// 再生初回の音声タイミングを
					// 仮の映像タイミングとしてセットします。
					video_timing = raw_audio_data->audio_timing;
				}

				// プレイリストに追加します。
				playlist.emplace_back(raw_audio_data);
			}
			// 再生中ではない場合は
			else
			{
				MY_TRACE("再生中ではありません\n");

				// 音声信号のフレームと現在フレームが異なる場合は
				if (raw_audio_data->audio_timing.frame != raw_audio_data->current_frame)
				{
					MY_TRACE("何もしません\n");

					// 他のプラグインから呼ばれている可能性があるので何もしません。
					return FALSE;
				}

				// タイマーIDが有効の場合は
				if (timer_id)
				{
					MY_TRACE("タイマーを終了します\n");

					// タイマーを終了します。
					::KillTimer(*this, timer_id), timer_id = 0;
				}

				// 現在の音量にセットします。
				current_volume = { raw_audio_data };

				// プレイリストをリセットします。
				playlist.clear();

				// ビューを再描画します。
				redraw();
			}

			return TRUE;
		}

		//
		// 映像タイミングをセットします。
		//
		BOOL set_video_timing(const std::unique_ptr<timing_t>& timing)
		{
			MY_TRACE_FUNC("{/}, {/}", timing->frame, timing->time);

			// 映像タイミングをセットします。
			video_timing = *timing;

			return TRUE;
		}

		//
		// WM_PAINTを処理します。
		//
		LRESULT on_paint(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			// ペインターに描画処理を任せて、0を返します。
			return painter_t(hwnd, current_volume)(), 0;
		}

		//
		// WM_TIMERを処理します。
		//
		LRESULT on_timer(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
//			MY_TRACE_FUNC("WM_TIMER, {/}, {/}, {/}, {/}", message, w_param, l_param, timer_id);

			//
			// この関数はフレーム数をミリ秒に変換して返します。
			//
			const auto frame_to_ms = [&fi = video_timing.fi](int32_t frame) -> int32_t
			{
				return (fi.video_rate != 0 && fi.video_scale != 0) ?
					(frame * (1000 * fi.video_scale) / fi.video_rate) : 0;
//					::MulDiv(frame, 1000 * fi.video_scale, fi.video_rate) : 0;
			};

			//
			// この関数はミリ秒をフレーム数に変換して返します。
			//
			const auto ms_to_frame = [&fi = video_timing.fi](int32_t ms) -> int32_t
			{
				return (fi.video_rate != 0 && fi.video_scale != 0) ?
					(ms * fi.video_rate / (1000 * fi.video_scale)) : 0;
//					::MulDiv(ms, fi.video_rate, 1000 * fi.video_scale) : 0;
			};

			// タイマーIDが有効の場合は
			if (timer_id && w_param == c_timer_id)
			{
				MY_TRACE_FUNC("");

				MY_TRACE_INT(playlist.size());

				// 現在の時間を取得します。
				auto now_time = ::timeGetTime();

				// 映像タイミングからの経過時間を算出します。
				auto elapsed_time = now_time - video_timing.time;

				// 経過時間から経過フレームを算出します。
				auto elapsed_frame = ms_to_frame(elapsed_time);

				// 現在のフレームを算出します。
				auto now_frame = video_timing.frame + elapsed_frame;

//				MY_TRACE_INT(video_timing.time);
//				MY_TRACE_INT(now_time);
//				MY_TRACE_INT(elapsed_time);

				MY_TRACE_INT(video_timing.frame);
				MY_TRACE_INT(now_frame);
				MY_TRACE_INT(elapsed_frame);

				std::vector<volume_t> volumes;

				// プレイリストが空ではない場合はループします。
				while (!playlist.empty())
				{
					// プレイリスト先頭の音量を取得します。
					const auto& volume = playlist.front();

					MY_TRACE_INT(volume.frame);

					// 音量タイミングが現在タイミングより許容範囲以上に前の場合は
					if ((volume.frame + c_timer_elapse) < now_frame)
					{
						MY_TRACE("現在タイミングよりかなり遅れているのでプレイリストから除外します\n");

						// 音量を配列に追加します。
						volumes.emplace_back(volume);

						// プレイリストから除外します。
						playlist.pop_front();
					}
					// 音量タイミングと現在タイミングの差がタイマー間隔以内の場合は
					else if (abs(volume.frame - now_frame) <= c_timer_elapse)
					{
						MY_TRACE("この音量を描画します\n");

						// 音量を配列に追加します。
						volumes.emplace_back(volume);
#if 1
						// 現在の音量にピーク音量をセットします。
						current_volume = {};
						{
							for (size_t i = 0; i < std::size(current_volume.channels); i++)
							{
								current_volume.channels[i].level = -1000.0f;
								current_volume.channels[i].peak = -1000.0f;
							}

							for (const auto& volume : volumes)
							{
								current_volume.frame = std::max(current_volume.frame, volume.frame);

								for (size_t i = 0; i < std::size(current_volume.channels); i++)
								{
									current_volume.channels[i].level = std::max(current_volume.channels[i].level, volume.channels[i].level);
									current_volume.channels[i].peak = std::max(current_volume.channels[i].peak, volume.channels[i].peak);
								}
							}
						}
#else
						// 単一の音量をセットします。
						current_volume = volume;
#endif
						// プレイリストから除外します。
						playlist.pop_front();

						// 音量を描画します。
						redraw();

						break;
					}
					// それ以外の場合は
					else
					{
						MY_TRACE("処理を保留します\n");

						// ここでの処理を保留して、次回のWM_TIMERで処理します。
						break;
					}
				}

				// プレイリストが空になった場合は
				if (playlist.empty())
				{
					MY_TRACE("プレイリストが空になったので、タイマーを終了します\n");

					// プレイリストが空になったので、タイマーを終了します。
					::KillTimer(*this, timer_id), timer_id = 0;
				}
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
			switch (message)
			{
			case WM_CREATE:
				{
					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					// ウィンドウが作成されたことをメインスレッドのビューに通知します。
					::PostMessage(::GetParent(hwnd), hive.c_message.c_post_init, 0, (LPARAM)hwnd);

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					break;
				}
			case WM_PAINT:
				{
//					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					return on_paint(hwnd, message, w_param, l_param);
				}
			case WM_TIMER:
				{
//					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					return on_timer(hwnd, message, w_param, l_param);
				}
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}
	} view;
}
