#pragma once

namespace mft
{
	constexpr auto c_fps = 60;

	//
	// 指定されたファイルの総フレーム数(60FPS換算)を取得します。
	// (サンプル取得は行わず、再生時間の取得だけの軽量な処理です)
	//
	inline uint32_t probe_nb_frames(const std::wstring& file_path)
	{
		MY_TRACE_FUNC("{/}", file_path);

		ComPtr<IMFByteStream> byte_stream;
		byte_stream.Attach(new prefetch_byte_stream_t(file_path, 4 * 1024 * 1024));

		ComPtr<IMFSourceReader> source_reader;
		auto hr = ::MFCreateSourceReaderFromByteStream(
			byte_stream.Get(), nullptr, source_reader.GetAddressOf());
		if (FAILED(hr)) throw L"::MFCreateSourceReaderFromByteStream()が失敗しました";

		auto nb_frames = uint32_t {};

		auto var = PROPVARIANT {};
		::PropVariantInit(&var);
		hr = source_reader->GetPresentationAttribute(
			MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &var);
		if (SUCCEEDED(hr) && var.vt == VT_UI8)
		{
			auto duration_100ns = (UINT64)var.uhVal.QuadPart;
			nb_frames = (uint32_t)((duration_100ns * c_fps + 10'000'000ULL - 1) / 10'000'000ULL);
		}
		::PropVariantClear(&var);

		MY_TRACE_INT(nb_frames);
		return nb_frames;
	}

	//
	// このクラスはセッションです。
	// 指定されたフレーム範囲 [range_start, range_end) だけを処理します。
	// (プロセスを分けて並列化する前提のため、内部でのスレッド分割は行いません)
	//
	struct session_t
	{
		//
		// 算出された音量データです。(サイズは range_end - range_start です)
		//
		std::vector<uint8_t> volumes;

		std::wstring file_path;
		uint32_t range_start = {};
		uint32_t range_end = {};
		UINT32 sample_rate = {};

		session_t(const std::wstring& file_path, uint32_t range_start, uint32_t range_end)
			: file_path(file_path), range_start(range_start), range_end(range_end)
		{
			MY_TRACE_FUNC("");

			auto hr = ::MFStartup(MF_VERSION);
			if (FAILED(hr)) throw L"::MFStartup()が失敗しました";

			volumes.resize(range_end > range_start ? range_end - range_start : 0);

			MY_TRACE("初期化が完了しました\n");
		}

		~session_t()
		{
			MY_TRACE_FUNC("");
			::MFShutdown();
		}

		inline uint64_t get_block_num_at(uint32_t frame_num)
		{
			return (uint64_t)frame_num * sample_rate / c_fps;
		};

		inline size_t get_nb_blocks_at(uint32_t current_frame_num)
		{
			auto current_block_num = get_block_num_at(current_frame_num);
			auto next_block_num = get_block_num_at(current_frame_num + 1);
			return (size_t)(next_block_num - current_block_num);
		};

		//
		// 割り当てられた範囲の音量を算出し、その総数を返します。
		//
		uint32_t extract_volumes(const std::wstring& compute_mode)
		{
			MY_TRACE_FUNC("");

			mft::counter_base_t counter(my::format(L"{/}~{/}の音量算出", range_start, range_end));

			if (range_start >= range_end) return 0;

			auto compute_func = compute_peak;
			if (compute_mode == L"rms") compute_func = compute_rms;

			ComPtr<IMFByteStream> byte_stream;
			byte_stream.Attach(new prefetch_byte_stream_t(file_path));

			ComPtr<IMFSourceReader> source_reader;
			auto hr = ::MFCreateSourceReaderFromByteStream(
				byte_stream.Get(), nullptr, source_reader.GetAddressOf());
			if (FAILED(hr)) throw L"::MFCreateSourceReaderFromByteStream()が失敗しました";

			hr = source_reader->SetStreamSelection(MF_SOURCE_READER_ALL_STREAMS, FALSE);
			if (FAILED(hr)) throw L"SetStreamSelection(ALL, FALSE)が失敗しました";
			hr = source_reader->SetStreamSelection(MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE);
			if (FAILED(hr)) throw L"SetStreamSelection(AUDIO, TRUE)が失敗しました";

			{
				ComPtr<IMFMediaType> desired_type;
				hr = ::MFCreateMediaType(desired_type.GetAddressOf());
				if (FAILED(hr)) throw L"::MFCreateMediaType()が失敗しました";

				hr = desired_type->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
				if (FAILED(hr)) throw L"desired_type->SetGUID(MF_MT_MAJOR_TYPE)が失敗しました";
				hr = desired_type->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_Float);
				if (FAILED(hr)) throw L"desired_type->SetGUID(MF_MT_SUBTYPE)が失敗しました";
				hr = desired_type->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, 1);
				hr = desired_type->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 32);

				hr = source_reader->SetCurrentMediaType(
					MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, desired_type.Get());
				if (FAILED(hr)) throw L"source_reader->SetCurrentMediaType()が失敗しました";
			}

			{
				ComPtr<IMFMediaType> actual_type;
				hr = source_reader->GetCurrentMediaType(
					MF_SOURCE_READER_FIRST_AUDIO_STREAM, actual_type.GetAddressOf());
				if (FAILED(hr)) throw L"source_reader->GetCurrentMediaType()が失敗しました";
				actual_type->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &sample_rate);
				MY_TRACE_INT(sample_rate);
			}

			auto block_start = get_block_num_at(range_start);

			if (range_start > 0)
			{
				auto seek_timestamp = (LONGLONG)((uint64_t)block_start * 10'000'000ULL / sample_rate);
				auto pos = PROPVARIANT {};
				::PropVariantInit(&pos);
				pos.vt = VT_I8;
				pos.hVal.QuadPart = seek_timestamp;
				{
					counter_base_t counter(L"SetCurrentPosition");

					hr = source_reader->SetCurrentPosition(GUID_NULL, pos);
				}
				::PropVariantClear(&pos);
				if (FAILED(hr)) throw L"source_reader->SetCurrentPosition()が失敗しました";
			}

			auto samples = std::vector<float, aligned_allocator<float, 32>> {};

			auto block_cursor = uint64_t {};
			auto block_cursor_valid = false;

			auto frame_num = range_start;
			auto nb_blocks_per_frame = get_nb_blocks_at(frame_num);
			auto nb_volumes = uint32_t {};

			while (1)
			{
				ComPtr<IMFSample> sample;
				auto current_flags = DWORD {};
				auto current_timestamp = LONGLONG {};

				auto hr = source_reader->ReadSample(
					MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr,
					&current_flags, &current_timestamp, sample.GetAddressOf());

				if (FAILED(hr)) break;
				if (!sample) break;
				if (current_flags & MF_SOURCE_READERF_ENDOFSTREAM) break;

				ComPtr<IMFMediaBuffer> media_buffer;
				sample->ConvertToContiguousBuffer(media_buffer.GetAddressOf());

				BYTE* data = {}; auto nb_bytes = DWORD {};
				media_buffer->Lock(&data, nullptr, &nb_bytes);
				auto nb_samples = nb_bytes / sizeof(float);

				if (!block_cursor_valid)
				{
					block_cursor = (uint64_t)((double)current_timestamp * sample_rate / 10'000'000.0 + 0.5);
					block_cursor_valid = true;
				}

				auto c = samples.size();
				samples.resize(c + nb_samples);
				memcpy(samples.data() + c, data, nb_bytes);
				media_buffer->Unlock();

				if (block_cursor + samples.size() <= block_start)
				{
					block_cursor += samples.size();
					samples.clear();
					continue;
				}
				if (block_cursor < block_start)
				{
					auto skip = (size_t)(block_start - block_cursor);
					samples.erase(samples.begin(), samples.begin() + skip);
					block_cursor += skip;
				}

				auto it = samples.begin();
				auto remain = samples.size();

				while (remain >= nb_blocks_per_frame && frame_num < range_end)
				{
					volumes[nb_volumes++] = compute_func(&it[0], nb_blocks_per_frame);

					it += nb_blocks_per_frame;
					remain -= nb_blocks_per_frame;
					block_cursor += nb_blocks_per_frame;

					frame_num++;
					if (frame_num < range_end) nb_blocks_per_frame = get_nb_blocks_at(frame_num);
				}

				samples.erase(samples.begin(), it);

				if (frame_num >= range_end) break;
			}

			if (frame_num < range_end && samples.size())
				volumes[nb_volumes++] = compute_func(samples.data(), samples.size());

			return nb_volumes;
		}
	};
}
