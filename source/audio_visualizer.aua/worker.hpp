#pragma once

namespace apn::audio_visualizer
{
	//
	// このクラスはワーカーです。
	// サブスレッドで音声データを解析します。
	// アーティファクトを作成します。
	// アーティファクトを外部プロセスに送信します。
	//
	inline struct Worker
	{
		//
		// このクラスは範囲です。
		//
		template <typename T>
		struct Range
		{
			T start, end;

			Range() {}
			Range(T start, T end) : start(start), end(end) {}
		};

		using Band = Range<double>;

		//
		// サブスレッドのIDです。
		//
		DWORD tid = 0;

		//
		// サブスレッドのハンドルです。
		//
		my::handle::unique_ptr<> thread;

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
		BOOL init()
		{
			MY_TRACE_FUNC("");

			thread.reset(::CreateThread(nullptr, 0,
				[](LPVOID param)
				{
					SubThread sub_thread;
					return sub_thread.message_loop();
				},
				this, 0, &tid));

			return !!thread;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

//			::PostThreadMessage(tid, WM_QUIT, 0, 0);
			::TerminateThread(thread.get(), 0);

			thread.reset();

			return TRUE;
		}

		//
		// オプションを処理します。
		//
		BOOL process_option(HWND visual, const share::Option& option)
		{
			MY_TRACE_FUNC("");

			// サブスレッドにオプションを送信します。
			return ::PostThreadMessage(tid, hive.c_message.c_receive_option,
				(WPARAM)visual, (LPARAM)new share::Option(option));
		}

		//
		// 音声データを処理します。
		//
		BOOL process_audio(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip, const Addin::ProcState& proc_state)
		{
			MY_TRACE_FUNC("");

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

					auto offset = frame_to_ms(fp, fpip, fpip->frame - start_frame);
					MY_TRACE_INT(offset);

					time = start_time + offset;
					MY_TRACE_INT(time);
				}
			}

			// サブスレッドに音声サンプルを送信します。
			return ::PostThreadMessage(tid, hive.c_message.c_receive_audio_sample,
				0, (LPARAM)new AudioSample(fp, fpip, time));
		}

		//
		// フレーム番号をミリ秒に変換して返します。
		//
		inline static int32_t frame_to_ms(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip, int32_t frame)
		{
			AviUtl::FileInfo fi = {};
			fp->exfunc->get_file_info(fpip->editp, &fi);

			if (fi.video_rate != 0 && fi.video_scale != 0)
				return (int32_t)(frame * 1000.0f * fi.video_scale / fi.video_rate);
			else
				return 0;
		}

		//
		// このクラスはサブスレッドです。
		//
		struct SubThread
		{
			//
			// コンストラクタです。
			//
			SubThread()
			{
				MY_TRACE_FUNC("");
			}

			//
			// デストラクタです。
			//
			~SubThread()
			{
				MY_TRACE_FUNC("");
			}

			//
			// ハンドルを複製して返します。
			//
			inline static HANDLE duplicate_handle(HANDLE src)
			{
				auto dst = HANDLE {};
				::DuplicateHandle(
					::GetCurrentProcess(), src,
					outer.pi.hProcess, &dst,
					0, FALSE, DUPLICATE_SAME_ACCESS);
				return dst;
			}

			//
			// メッセージループです。
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
						case hive.c_message.c_receive_option:
							{
								MY_TRACE_FUNC("c_receive_option");

								// ビジュアルを受け取ります。
								auto visual = (HWND)msg.wParam;

								// オプションを受け取ります。
								auto option = (const share::Option*)msg.lParam;

								// 外部プロセスにオプションを送信します。
								post_option(visual, option);

								// 使用済みになったオプションを削除します。
								delete option;

								break;
							}
						case hive.c_message.c_receive_audio_sample:
							{
								MY_TRACE_FUNC("c_receive_audio_sample");

								// 音声サンプルを受け取ります。
								auto audio_sample = (const AudioSample*)msg.lParam;

								// 音声サンプルから音声アーティファクトを作成します。
								// そして、外部プロセスに音声アーティファクトを送信します。
								post_audio_artifact(audio_sample);

								// 使用済みになった音声サンプルを削除します。
								delete audio_sample;

								break;
							}
						case WM_QUIT:
							{
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

			//
			// オプションを構築します。
			//
			BOOL make_option(const share::Option* option, HANDLE file_mapping)
			{
				my::SharedMemory<share::Option> shared(file_mapping);
				if (!shared) return FALSE;
				*shared = *option;
				return TRUE;
			}

			//
			// オプションを作成して返します。
			//
			HANDLE create_option(const share::Option* option)
			{
				auto file_mapping = ::CreateFileMapping(
					INVALID_HANDLE_VALUE,
					nullptr,
					PAGE_READWRITE,
					0,
					sizeof(share::Option),
					nullptr);

				make_option(option, file_mapping);

				return file_mapping;
			}

			//
			// 外部プロセスに音声アーティファクトを送信します。
			//
			BOOL post_option(HWND visual, const share::Option* option)
			{
				MY_TRACE_FUNC("{/hex}", visual);

				// 外部プロスが無効の場合は何もしません。
				if (!hive.ui_window) return FALSE;

				// オプションを作成します。
				my::handle::unique_ptr<> file_mapping(create_option(option));
				if (!file_mapping) return FALSE;

				// オプションを複製します。
				auto handle = duplicate_handle(file_mapping.get());

				// 外部プロセスにオプションの複製を送信します。
				::PostMessage(hive.ui_window, share::c_message.c_update_option, (WPARAM)visual, (LPARAM)handle);

				return TRUE;
			}

			//
			// 音声アーティファクトを構築します。
			//
			BOOL make_audio_artifact(const AudioSample* audio_sample, HANDLE file_mapping)
			{
				my::SharedMemory<share::AudioArtifact> shared(file_mapping);
				if (!shared) return FALSE;
				return calc(audio_sample, shared);
			}

			//
			// 音声アーティファクトを作成して返します。
			//
			HANDLE create_audio_artifact(const AudioSample* audio_sample)
			{
				auto file_mapping = ::CreateFileMapping(
					INVALID_HANDLE_VALUE,
					nullptr,
					PAGE_READWRITE,
					0,
					sizeof(share::AudioArtifact),
					nullptr);

				make_audio_artifact(audio_sample, file_mapping);

				return file_mapping;
			}

			//
			// 音声サンプルの解析処理が遅れている場合はTRUEを返します。
			//
			BOOL is_late(const AudioSample* audio_sample)
			{
				// 音声サンプルの時間が有効の場合は
				if (audio_sample->time)
				{
					// 現在時間を取得します。
					auto time = ::timeGetTime();

					// 許容範囲以上に遅れている場合は解析をスキップします。
					if (time > audio_sample->time && time - audio_sample->time > 5)
					{
						MY_TRACE("{/}, {/.3f}, {/.3f}, 解析が間に合っていないのでスキップします\n",
							audio_sample->frame, time / 1000.0, audio_sample->time / 1000.0);

						return TRUE;
					}
				}

				return FALSE;
			}

			//
			// 外部プロセスに音声アーティファクトを送信します。
			//
			BOOL post_audio_artifact(const AudioSample* audio_sample)
			{
//				MY_TRACE_FUNC("");

				// 外部プロスが無効の場合は何もしません。
				if (!hive.ui_window) return FALSE;

				// 音声サンプルの解析処理が遅れている場合は何もしません。
				if (is_late(audio_sample)) return FALSE;

				// 音声アーティファクトを作成します。
				my::handle::unique_ptr<> file_mapping(create_audio_artifact(audio_sample));
				if (!file_mapping) return FALSE;

				// 音声アーティファクトを複製します。
				auto handle = duplicate_handle(file_mapping.get());

				// 外部プロセスに音声アーティファクトの複製を送信します。
				::PostMessage(hive.ui_window, share::c_message.c_update_audio, 0, (LPARAM)handle);

				return TRUE;
			}

			//
			// 受け取った音声信号を-1.0～1.0に正規化して返します。
			//
			inline static double normalize(short pcm)
			{
				return pcm / 32768.0;
			}

			//
			// 指定されたデータ列に窓関数を適用します。
			//
			inline static void apply_hann_window(std::vector<double>& data)
			{
				size_t N = data.size();
				for (size_t i = 0; i < N; i++)
					data[i] *= 0.5 * (1 - cos(2 * M_PI * i / (N - 1)));
			}

			//
			// 指定された個数の周波数帯域を返します。
			//
			inline static std::vector<Band> create_frequency_bands(const share::SpectreOption& option)
			{
				std::vector<Band> bands(option.div);
				auto range = Range<double> { option.freq_min, option.freq_max };
				auto step = (range.end - range.start) / option.div;

				for (size_t i = 0; i < option.div; i++)
				{
					bands[i] = {
						range.start + (i + 0) * step,
						range.start + (i + 1) * step,
					};
				}

				return bands;
			}

			//
			// 対数スケールで指定された個数の周波数帯域を返します。
			//
			inline static std::vector<Band> create_log_frequency_bands(const share::SpectreOption& option)
			{
				std::vector<Band> bands(option.div);
				auto range = Range<double> { log10(option.freq_min), log10(option.freq_max) };
				auto step = (range.end - range.start) / option.div;

				for (size_t i = 0; i < option.div; i++)
				{
					bands[i] = {
						pow(10, range.start + (i + 0) * step),
						pow(10, range.start + (i + 1) * step),
					};
				}

				return bands;
			}

			//
			// 指定された周波数帯域の振幅を返します。
			//
			double get_band_amplitude(const std::vector<double>& amplitude, const Band& band, size_t N, size_t audio_rate)
			{
				auto limit = N / 2 - 1;
				auto range = Range<double> { band.start * N / audio_rate, band.end * N / audio_rate };
				auto index_range = Range<size_t> { (size_t)range.start, (size_t)range.end };

				if (index_range.start > limit)
				{
					range.start = limit;
					index_range.start = limit;
				}

				if (index_range.end > limit)
				{
					range.end = limit;
					index_range.end = limit;
				}

				if (index_range.start == index_range.end)
					return amplitude[index_range.start] * (range.end - range.start);

				auto sum_amplitude = 0.0;
				sum_amplitude += amplitude[index_range.start] * (index_range.start + 1 - range.start);
				for (size_t i = index_range.start + 1; i < index_range.end - 1; i++)
					sum_amplitude += amplitude[i];
				sum_amplitude += amplitude[index_range.end - 1] * (range.end - index_range.end);

				return sum_amplitude / (range.end - range.start);
			}

			//
			// 指定されたチャンネルの音声アーティファクトを算出します。
			//
			void calc(const AudioSample* audio_sample, share::AudioArtifact* audio_artifact, int32_t ch)
			{
				size_t audio_n = audio_sample->audio_n;
				size_t audio_ch = audio_sample->audio_ch;
				size_t audio_rate = audio_sample->fi.audio_rate;
				auto& spectre_option = audio_artifact->spectre_option;

				// サンプルの配列を作成します。
				std::vector<double> samples(audio_n);
				for (size_t i = 0; i < audio_n; i++)
					samples[i] = normalize(audio_sample->audiop[i * audio_ch + ch]);

				// 窓関数を適用します。
				apply_hann_window(samples);

				// RMSとピークを算出します。
				auto rms = 0.0;
				auto peak = 0.0;

				for (auto sample : samples)
				{
					auto s = sample * sample;
					rms += s;
					peak = std::max(peak, s);
				}

				rms = sqrt(rms / samples.size());
				peak = sqrt(peak);
				MY_TRACE_REAL(rms);
				MY_TRACE_REAL(peak);

				audio_artifact->channels[ch].level = 20 * log10(rms);
				audio_artifact->channels[ch].peak = 20 * log10(peak);
				MY_TRACE_REAL(audio_artifact->channels[ch].level);
				MY_TRACE_REAL(audio_artifact->channels[ch].peak);

				// 複素数の配列を作成します。
				auto fft_complex = FFT(samples);
				size_t N = fft_complex.size();

				// 振幅スペクトルを算出します。
				std::vector<double> amplitude(N / 2);

				auto total_energy = 0.0;
				for (size_t i = 0; i < amplitude.size(); i++)
				{
					auto n = norm(fft_complex[i + 1]);
					amplitude[i] = sqrt(n);
					total_energy += n;
				}
				MY_TRACE_REAL(total_energy);

				total_energy = sqrt(total_energy);
				MY_TRACE_REAL(total_energy);

				// 振幅をサンプル数で除算するようにします。
				auto mul = 20.0;
				auto scale_factor = 1.0 / samples.size();

				// 分割数を乗算して振幅が分割減少されるのを相殺します。
				scale_factor *= spectre_option.div;

				// RMSスケーリングが有効の場合は
				if (spectre_option.rms_scaling)
				{
					// 合計エネルギーがRMSになるようにスケーリングします。
					mul = 10.0;
					scale_factor = rms / total_energy;
					MY_TRACE_REAL(scale_factor);
				}

				// 指定された周波数範囲を指定された個数の帯域に分割します。
				auto bands = spectre_option.log_freq ?
					create_log_frequency_bands(spectre_option) :
					create_frequency_bands(spectre_option);

				// 各帯域の平均振幅を計算します。
				auto& spectre = audio_artifact->channels[ch].spectre;
				for (size_t i = 0; i < bands.size(); i++)
				{
					// 周波数帯域を取得します。
					const auto& band = bands[i];

					// 周波数帯域の振幅を取得します。
					auto band_amplitude = get_band_amplitude(amplitude, band, N, audio_rate);

					// 振幅をdBに変換します。
					spectre[i].amplitude = mul * log10(band_amplitude * scale_factor + 1e-10);

//					spectre[i].frequency = (band.start + band.end) / 2; // 帯域の平均周波数です。
//					spectre[i].frequency = band.start; // 帯域の最小周波数です。
					spectre[i].frequency = band.end; // 帯域の最大周波数です。
				}
			}

			//
			// 音声サンプルを元に音声アーティファクトを構築します。
			//
			BOOL calc(const AudioSample* audio_sample, share::AudioArtifact* audio_artifact)
			{
				audio_artifact->frame = audio_sample->frame;
				audio_artifact->time = audio_sample->time;
				audio_artifact->spectre_option = audio_sample->option.spectre_option;
				audio_artifact->spectre_option.div = std::min(
					audio_artifact->spectre_option.div,
					std::size(audio_artifact->channels->spectre));

				for (int32_t ch = 0; ch < audio_sample->audio_ch; ch++)
					calc(audio_sample, audio_artifact, ch);

				return TRUE;
			}
		};
	} worker;
}
