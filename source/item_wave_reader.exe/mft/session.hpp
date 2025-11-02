#pragma once

namespace mft
{
	//
	// このクラスはセッションです。
	//
	struct session_t
	{
		inline static constexpr auto c_fps = 60;

		ComPtr<IMFSourceReader> source_reader;

		UINT32 sample_rate = {};
		UINT32 nb_channels = {};
		UINT32 nb_blocks_per_frame = {};
		UINT32 nb_samples_per_frame = {};

		//
		// 初期化処理を実行します。
		//
		bool init(const std::wstring& file_path)
		{
			MY_TRACE_FUNC("");

			// MFTを開始します。
			auto hr = ::MFStartup(MF_VERSION);
			if (FAILED(hr)) return false;


			// 動画ファイルを読み込むソースリーダーを作成します。
			hr = ::MFCreateSourceReaderFromURL(
				file_path.c_str(), nullptr, source_reader.GetAddressOf());
			if (FAILED(hr)) return false;

			// 出力フォーマットを設定します。
			{
				ComPtr<IMFMediaType> desired_type;
				hr = ::MFCreateMediaType(desired_type.GetAddressOf());
				if (FAILED(hr)) return false;

				hr = desired_type->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
				if (FAILED(hr)) return false;

				hr = desired_type->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_Float);
				if (FAILED(hr)) return false;

				hr = MFSetAttributeSize(desired_type.Get(), MF_MT_FRAME_SIZE, 0, 0);
				hr = desired_type->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, 1);
				hr = desired_type->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, 48000);
				hr = desired_type->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 32);

				hr = source_reader->SetCurrentMediaType(
					MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, desired_type.Get());
				if (FAILED(hr)) return false;
			}

			{
				// 実際の出力フォーマットを取得します。
				ComPtr<IMFMediaType> actual_type;
				hr = source_reader->GetCurrentMediaType(
					MF_SOURCE_READER_FIRST_AUDIO_STREAM, actual_type.GetAddressOf());
				if (FAILED(hr)) return false;

				// サンプルレートを取得します。
				actual_type->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &sample_rate);
				MY_TRACE_INT(sample_rate);

				// チャンネル数を取得します。
				actual_type->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &nb_channels);
				MY_TRACE_INT(nb_channels);

				// フレーム毎のブロック数を取得します。
				nb_blocks_per_frame = sample_rate / c_fps;
				MY_TRACE_INT(nb_blocks_per_frame);

				// フレーム毎のサンプル数を取得します。
				nb_samples_per_frame = nb_blocks_per_frame * nb_channels;
				MY_TRACE_INT(nb_samples_per_frame);
			}
#if 0
			// 先頭にシークします。
			{
				auto pos = PROPVARIANT {};
				::PropVariantInit(&pos);
				pos.vt = VT_I8;
				pos.hVal.QuadPart = 0;
				hr = source_reader->SetCurrentPosition(GUID_NULL, pos);
				::PropVariantClear(&pos);
			}
#endif
			MY_TRACE("初期化が完了しました\n");

			return true;
		}

		//
		// 後始末処理を実行します。
		//
		bool reset()
		{
			MY_TRACE_FUNC("");

			source_reader = nullptr;

			// MFTを終了します。
			::MFShutdown();

			return true;
		}

		//
		// 指定された実数を音量範囲にマップします。
		//
		inline static uint8_t map(float value)
		{
			auto mapped = (int)(value * 255.0f);
			mapped = std::max(mapped, 0);
			mapped = std::min(mapped, 255);

			return (uint8_t)mapped;
		}

		//
		// 指定されたサンプルからRMSを算出して返します。
		//
		inline static uint8_t compute_rms(const float* samples, size_t size)
		{
			auto sum_squares = 0.0f;
			for (size_t i = 0; i < size; i++) {
				auto s = samples[i];
				sum_squares += s * s;
			}
			auto rms = std::sqrt(sum_squares / size);
			return map(rms);
		}

		//
		// 指定されたサンプルからピークを算出して返します。
		//
		inline static uint8_t compute_peak(const float* samples, size_t size)
		{
			auto peak = 0.0f;
			for (size_t i = 0; i < size; i++)
				peak = std::max(peak, std::abs(samples[i]));
			return map(peak);
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
		// 指定された音声ファイルからフレーム(60FPS)毎の音量を算出して返します。
		//
		std::vector<uint8_t> extract_volumes(const std::wstring& compute_mode)
		{
			MY_TRACE_FUNC("");

			// 音量の算出方式を取得します。
			auto compute_func = compute_peak;
			if (compute_mode == L"rms") compute_func = compute_rms;

			// 算出した音量を格納するバッファです。
			auto volumes = std::vector<uint8_t> {};

			// 取得したサンプルを格納するバッファです。
			auto samples = std::vector<float> {};

			// 最初のフレームのブロック数を取得します。
			auto nb_blocks_per_frame = get_nb_blocks_at(0);
			MY_TRACE_INT(nb_blocks_per_frame);

			while (1)
			{
				// サンプルを読み込みます。
				ComPtr<IMFSample> sample; auto flags = DWORD {};
				auto hr = source_reader->ReadSample(
					MF_SOURCE_READER_FIRST_AUDIO_STREAM,
					0, nullptr, &flags, nullptr, sample.GetAddressOf());
				MY_TRACE_HEX(hr);
				MY_TRACE_HEX(sample.Get());
				MY_TRACE_HEX(flags);

				// 読み込みに失敗した場合はループを終了します。
				if (FAILED(hr)) break;

				// サンプルが無効の場合はループを終了します。
				if (!sample) break;

				// ストリームの末尾の場合はループを終了します。
				if (flags & MF_SOURCE_READERF_ENDOFSTREAM) break;

				{
					// メディアバッファを取得します。
					ComPtr<IMFMediaBuffer> media_buffer;
					sample->ConvertToContiguousBuffer(media_buffer.GetAddressOf());

					// メディアバッファをロックします。
					BYTE* data = {}; auto nb_bytes = DWORD {};
					media_buffer->Lock(&data, nullptr, &nb_bytes);
					MY_TRACE_INT(nb_bytes);

					// 取得予定のサンプル数を取得します。
					auto nb_samples = nb_bytes / sizeof(samples[0]);
					MY_TRACE_INT(nb_samples);

					// 蓄積されているサンプル数を取得します。
					auto c = samples.size();
					MY_TRACE_INT(c);

					// 取得予定のサンプル数の分だけ領域を拡張します。
					samples.resize(c + nb_samples);

					// サンプルを取得します。
					memcpy(samples.data() + c, data, nb_bytes);

					// ロックを解除します。
					media_buffer->Unlock();
				}

				// 音量を算出可能な数のサンプルが蓄積されている場合は
				while (samples.size() >= nb_blocks_per_frame)
				{
					// サンプルから音量を算出します。
					volumes.emplace_back(compute_func(samples.data(), nb_blocks_per_frame));
//					MY_TRACE_INT(volumes.size());

					// 計算が終わったサンプルを取り除きます。
					samples.erase(samples.begin(), samples.begin() + nb_blocks_per_frame);

					// フレームあたりのブロック数を更新します。
					nb_blocks_per_frame = get_nb_blocks_at(volumes.size());
//					MY_TRACE_INT(nb_blocks_per_frame);
				}
			}

			// 残りのサンプルから音量を算出します。
			if (samples.size())
				volumes.emplace_back(compute_func(samples.data(), samples.size()));

			MY_TRACE_INT(volumes.size());

			// 算出した音量を返します。
			return volumes;
		}
	};
}
