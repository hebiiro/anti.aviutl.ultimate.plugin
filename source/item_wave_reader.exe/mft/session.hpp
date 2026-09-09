#pragma once

namespace mft
{
	//
	// このクラスはセッションです。
	//
	struct session_t
	{
		//
		// 音量を算出する時間間隔(フレームレート)です。
		//
		inline static constexpr auto c_fps = 60;

		//
		// 算出された音量データが格納されます。
		//
		std::vector<uint8_t> volumes;

		//
		// MFTのバイトストリームです。
		//
		ComPtr<IMFByteStream> byte_stream;

		//
		// MFTのソースリーダーです。
		//
		ComPtr<IMFSourceReader> source_reader;

		//
		// 音声ファイルのサンプリングレートです。
		//
		UINT32 sample_rate = {};
#if 0
		//
		// 音声ファイルのチャンネル数です。
		//
		UINT32 nb_channels = {};

		//
		// 音声ファイルのフレーム毎のブロック数です。
		//
		UINT32 nb_blocks_per_frame = {};

		//
		// 音声ファイルのフレーム毎のサンプル数です。
		//
		UINT32 nb_samples_per_frame = {};
#endif
		//
		// コンストラクタです。
		//
		session_t(const std::wstring& file_path)
		{
			MY_TRACE_FUNC("");

			// MFTを開始します。
			auto hr = ::MFStartup(MF_VERSION);
			if (FAILED(hr)) throw L"::MFStartup()が失敗しました";

			// 動画ファイルを読み込むソースリーダーを作成します。
			byte_stream.Attach(new prefetch_byte_stream_t(file_path)); // 既定100MB

			hr = ::MFCreateSourceReaderFromByteStream(
				byte_stream.Get(), nullptr, source_reader.GetAddressOf());
			if (FAILED(hr)) throw L"::MFCreateSourceReaderFromByteStream()が失敗しました";

			// 音声ストリーム以外を選択解除します。
			// こうしないと、未読の映像サンプルが際限なくキューに溜まり
			// メモリを消費し続け、さらに裏で映像の処理が走り続けて遅くなります。
			{
				hr = source_reader->SetStreamSelection(MF_SOURCE_READER_ALL_STREAMS, FALSE);
				if (FAILED(hr)) throw L"SetStreamSelection(ALL, FALSE)が失敗しました";

				hr = source_reader->SetStreamSelection(MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE);
				if (FAILED(hr)) throw L"SetStreamSelection(AUDIO, TRUE)が失敗しました";
			}

			// ネイティブの音声タイプを確認します。(デバッグ用)
#ifdef _DEBUG
			{
				ComPtr<IMFMediaType> native_type;
				source_reader->GetNativeMediaType(
					MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, native_type.GetAddressOf());

				auto native_sample_rate = UINT32 {};
				native_type->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &native_sample_rate);
				MY_TRACE_INT(native_sample_rate);
			}
#endif
			// 出力フォーマットを設定します。
			{
				ComPtr<IMFMediaType> desired_type;
				hr = ::MFCreateMediaType(desired_type.GetAddressOf());
				if (FAILED(hr)) throw L"::MFCreateMediaType()が失敗しました";

				hr = desired_type->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
				if (FAILED(hr)) throw L"desired_type->SetGUID(MF_MT_MAJOR_TYPE)が失敗しました";

				hr = desired_type->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_Float);
				if (FAILED(hr)) throw L"desired_type->SetGUID(MF_MT_SUBTYPE)が失敗しました";

				hr = desired_type->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, 1);
//				hr = desired_type->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, 48000);
				hr = desired_type->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 32);

				hr = source_reader->SetCurrentMediaType(
					MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, desired_type.Get());
				if (FAILED(hr)) throw L"source_reader->SetCurrentMediaType()が失敗しました";
			}

			{
				// 実際の出力フォーマットを取得します。
				ComPtr<IMFMediaType> actual_type;
				hr = source_reader->GetCurrentMediaType(
					MF_SOURCE_READER_FIRST_AUDIO_STREAM, actual_type.GetAddressOf());
				if (FAILED(hr)) throw L"source_reader->GetCurrentMediaType()が失敗しました";

				// サンプルレートを取得します。
				actual_type->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &sample_rate);
				MY_TRACE_INT(sample_rate);
#if 0
				// チャンネル数を取得します。
				actual_type->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &nb_channels);
				MY_TRACE_INT(nb_channels);

				// フレーム毎のブロック数を取得します。
				nb_blocks_per_frame = sample_rate / c_fps;
				MY_TRACE_INT(nb_blocks_per_frame);

				// フレーム毎のサンプル数を取得します。
				nb_samples_per_frame = nb_blocks_per_frame * nb_channels;
				MY_TRACE_INT(nb_samples_per_frame);
#endif
			}

			// 動画の総再生時間(100ナノ秒単位)を取得し、
			// 総フレーム数を事前に算出します。
			{
				auto var = PROPVARIANT {};
				::PropVariantInit(&var);
				auto hr = source_reader->GetPresentationAttribute(
					MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &var);
				if (SUCCEEDED(hr) && var.vt == VT_UI8)
				{
					auto duration_100ns = (UINT64)var.uhVal.QuadPart;

					// 100ナノ秒単位 → フレーム数への変換です。
					// 端数を切り上げて、実際のフレーム数以上を確保しておきます。
					auto nb_frames = (duration_100ns * c_fps + 10'000'000ULL - 1) / 10'000'000ULL;
					MY_TRACE_INT(nb_frames);

					volumes.resize(nb_frames);
				}
				::PropVariantClear(&var);
			}

			MY_TRACE("初期化が完了しました\n");
		}

		//
		// デストラクタです。
		//
		~session_t()
		{
			MY_TRACE_FUNC("");

			source_reader = nullptr;

			// MFTを終了します。
			::MFShutdown();
		}

		//
		// 指定されたタイムスタンプにシークします。
		//
		HRESULT seek(LONGLONG timestamp)
		{
			auto pos = PROPVARIANT {};
			::PropVariantInit(&pos);
			pos.vt = VT_I8;
			pos.hVal.QuadPart = timestamp;
			auto hr = source_reader->SetCurrentPosition(GUID_NULL, pos);
			::PropVariantClear(&pos);
			return hr;
		}

		//
		// 指定されたフレーム番号に対応するブロック番号を返します。
		//
		inline size_t get_block_num_at(size_t frame_num)
		{
			return frame_num * sample_rate / c_fps;
		};

		//
		// 指定されたフレームのブロック数を返します。
		//
		inline size_t get_nb_blocks_at(size_t current_frame_num)
		{
			// 次のフレーム番号を取得します。
			auto next_frame_num = current_frame_num + 1;

			// 現在のブロック番号を取得します。
			auto current_block_num = get_block_num_at(current_frame_num);

			// 次のブロック番号を取得します。
			auto next_block_num = get_block_num_at(next_frame_num);

			// フレーム番号の差分を返します。
			return next_block_num - current_block_num;
		};

		//
		// 音声ファイルからフレーム(60FPS)毎の音量を算出し、その総数を返します。
		//
		uint32_t extract_volumes(const std::wstring& compute_mode)
		{
			MY_TRACE_FUNC("");

			mft::counter_base_t counter(L"音量算出");

			// 音量の総数(フレーム数)です。
			auto nb_volumes = uint32_t {};

			// 音量の算出方式を取得します。
			auto compute_func = compute_peak;
			if (compute_mode == L"rms") compute_func = compute_rms;

			// 取得したサンプルを格納するアライン済みバッファです。
			auto samples = std::vector<float, aligned_allocator<float, 32>> {};

			// 最初のフレームのブロック数を取得します。
			auto nb_blocks_per_frame = get_nb_blocks_at(0);
			MY_TRACE_INT(nb_blocks_per_frame);

			while (1)
			{
				// サンプル読み込み用の変数です。
				ComPtr<IMFSample> sample;
				auto current_flags = DWORD {};
				auto current_timestamp = LONGLONG {};

				{
					counter_t counter(L"サンプル取得");

					// サンプルを読み込みます。
					auto hr = source_reader->ReadSample(
						MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr,
						&current_flags, &current_timestamp, sample.GetAddressOf());
//					MY_TRACE_HEX(hr);
//					MY_TRACE_HEX(sample.Get());
//					MY_TRACE_HEX(current_flags);
//					MY_TRACE_INT(current_timestamp);

					// 読み込みに失敗した場合はループを終了します。
					if (FAILED(hr)) break;

					// サンプルが無効の場合はループを終了します。
					if (!sample) break;

					// ストリームの末尾の場合はループを終了します。
					if (current_flags & MF_SOURCE_READERF_ENDOFSTREAM) break;
				}

				{
					counter_t counter(L"サンプル変換");

					// メディアバッファを取得します。
					ComPtr<IMFMediaBuffer> media_buffer;
					sample->ConvertToContiguousBuffer(media_buffer.GetAddressOf());

					// メディアバッファをロックします。
					BYTE* data = {}; auto nb_bytes = DWORD {};
					media_buffer->Lock(&data, nullptr, &nb_bytes);
//					MY_TRACE_INT(nb_bytes);

					// 取得予定のサンプル数を取得します。
					auto nb_samples = nb_bytes / sizeof(samples[0]);
//					MY_TRACE_INT(nb_samples);

					// 蓄積されているサンプル数を取得します。
					auto c = samples.size();
//					MY_TRACE_INT(c);

					// 取得予定のサンプル数の分だけ領域を拡張します。
					samples.resize(c + nb_samples);

					// サンプルを取得します。
					memcpy(samples.data() + c, data, nb_bytes);

					// ロックを解除します。
					media_buffer->Unlock();
				}

				{
					counter_t counter(L"サンプル処理");

					auto sample = samples.begin();
					auto nb_samples = samples.size();

					// 音量を算出可能な数のサンプルが蓄積されている場合は
					while (nb_samples >= nb_blocks_per_frame)
					{
						// 音量の総数が想定より多い場合はここで算出を終了します。
						if (nb_volumes >= volumes.size()) return (uint32_t)volumes.size();

						// サンプルから音量を算出します。
						volumes[nb_volumes++] = compute_func(&sample[0], nb_blocks_per_frame);

						// サンプルの位置を次に進めます。
						sample += nb_blocks_per_frame;
						nb_samples -= nb_blocks_per_frame;

						// フレームあたりのブロック数を更新します。
						nb_blocks_per_frame = get_nb_blocks_at(nb_volumes);
//						MY_TRACE_INT(nb_blocks_per_frame);
					}

					// 計算が終わったサンプルを取り除きます。
					samples.erase(samples.begin(), sample);
				}
			}

			// サンプルが残っている場合は
			if (samples.size())
			{
				// 音量の総数が想定より多い場合はここで算出を終了します。
				if (nb_volumes >= volumes.size()) return (uint32_t)volumes.size();

				// 残りのサンプルから音量を算出します。
				volumes[nb_volumes++] = compute_func(samples.data(), samples.size());
			}

			// 算出した音量の総数を返します。
			return nb_volumes;
		}
	};
}
