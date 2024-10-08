#pragma once

namespace apn::item_wave::reader
{
	inline struct App
	{
		//
		// このモジュールのインスタンスハンドルです。
		//
		HINSTANCE instance = nullptr;

		//
		// UIプロセスのウィンドウです。
		//
		HWND ui_window = nullptr;

		//
		// UIプロセスと同期を取るためのイベントです。
		//
		my::Event event;

		//
		// UIプロセスとのデータ通信用の共有メモリです。
		//
		my::SimpleFileMappingT<share::ReaderBuffer> shared;

		//
		// AviUtlの入力プラグインです。
		// 音声ファイルを読み込むために使用します。
		//
		std::shared_ptr<InputPlugin> input_plugin;

		//
		// 入力プラグインのハンドルです。
		//
		AviUtl::InputHandle input_handle = nullptr;

		//
		// 初期化処理を実行します。
		//
		BOOL init(HINSTANCE instance)
		{
			MY_TRACE_FUNC("{:#010x}", instance);

			// インスタンスハンドルを取得します。
			this->instance = instance;

			// コマンドライン引数を取得します。
			auto command_line = ::GetCommandLineW();
			MY_TRACE_STR(command_line);

			// ホストプロセスのウィンドウを取得します。
			auto host_window = (HWND)wcstoul(command_line, nullptr, 0);
			MY_TRACE_HWND(host_window);
			if (!host_window) return FALSE;

			// UIプロセスのウィンドウを取得します。
			ui_window = share::host_window::get_ui_window(host_window);
			MY_TRACE_HWND(ui_window);
			if (!ui_window) return FALSE;

			// ホストプロセスのプロセスIDを取得します。
			auto host_process_id = DWORD {};
			::GetWindowThreadProcessId(host_window, &host_process_id);
			if (!host_process_id) return FALSE;

			// ここはUIプロセスより先に処理される可能性があります。
			// その場合、イベントのオープンには失敗しても問題ありません。
			// ただし、共有メモリの確保は成功するのが前提なのでopen()は使用できません。
			auto tid = ::GetCurrentThreadId();
			event.open(EVENT_ALL_ACCESS, FALSE, share::get_reader_event_name(tid).c_str());
			shared.init(share::get_reader_buffer_name(tid).c_str());

			// ホストプロセスを取得します。
			my::handle::unique_ptr<> process(::OpenProcess(
				PROCESS_QUERY_LIMITED_INFORMATION, FALSE, host_process_id));
			MY_TRACE_HEX(process.get());

			// 入力プラグインのファイル名を取得します。
			auto file_name = my::get_module_file_name_ex(process.get(), nullptr);
			file_name = file_name.parent_path() / _T("plugins") / _T("lwinput.aui");
			MY_TRACE_STR(file_name);

			// 入力プラグインをロードします。
			input_plugin = std::make_shared<InputPlugin>(file_name.c_str());

			// 入力プラグインをロードできた場合はTRUEを返します。
			return !!input_plugin->dll;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			input_plugin = nullptr;

			return TRUE;
		}

		//
		// 入力プラグインから音声信号を受け取ります。
		// 同時並行して音量を算出します。
		//
		BOOL receive()
		{
			MY_TRACE_FUNC("");

			// イベントが発生するまで待機します。
			if (event) ::WaitForSingleObject(event, INFINITE);

			// 共有メモリを取得します。
			auto buffer = shared.get_buffer();
			if (!buffer) return FALSE;

			// 入力プラグインのインターフェイスを取得します。
			auto dll = input_plugin->dll;
			MY_TRACE_HEX(dll);
			if (!dll) return FALSE;

			// メディアを開きます。
			input_handle = dll->func_open(shared->file_name);
			MY_TRACE_HEX(input_handle);
			if (!input_handle) return FALSE;

			// メディア情報を取得します。
			AviUtl::InputInfo input_info = {};
			auto result = dll->func_info_get(input_handle, &input_info);

			// バッファのサイズを算出し、バッファを確保します。
			int32_t start = 0;
			int32_t length = input_info.audio_format->nSamplesPerSec / share::Volume::c_resolution;
			int32_t buffer_size = length * input_info.audio_format->nBlockAlign;

			MY_TRACE_INT(input_info.audio_format->wFormatTag);
			MY_TRACE_INT(input_info.audio_format->nChannels);
			MY_TRACE_INT(input_info.audio_format->nSamplesPerSec);
			MY_TRACE_INT(input_info.audio_format->nAvgBytesPerSec);
			MY_TRACE_INT(input_info.audio_format->nBlockAlign);
			MY_TRACE_INT(input_info.audio_format->wBitsPerSample);
			MY_TRACE_INT(length);
			MY_TRACE_INT(buffer_size);

			// 入力プラグインを使用して音声信号を読み込みます。

			auto start_time = ::timeGetTime();

			// 計算に必要なデータをハイブに格納します。
			hive.buffer = buffer;
			hive.audio_format = *input_info.audio_format;

			// ワークの配列です。
			std::vector<PTP_WORK> works(share::Volume::c_max_count);

			// 読み込んだフレーム数が格納されます。
			auto c = share::Volume::c_max_count;

			// 最大フレーム数までループします。
			// ただし、読み込む音声信号がなくなった場合はそこでループは終了します。
			for (auto i = 0; i < share::Volume::c_max_count; i++)
			{
				// 音声信号を受け取るためのバッファを確保すします。
				auto buffer = std::make_shared<std::vector<BYTE>>(buffer_size);

				// AviUtlの入力プラグインを使用して音声信号を取得します。
				auto read = dll->func_read_audio(
					input_handle, start, length, buffer->data());

				// 音声信号を読み込めなかった場合は
				if (read == 0)
				{
					// 読み込んだフレーム数をセットします。
					c = i;

					// ループを終了します。
					break;
				}

				// 読み取った分だけ読み取り開始位置を進めます。
				start += read;

				// バッファのサイズを読み取った分だけに調整します。
				buffer->resize(read * input_info.audio_format->nBlockAlign);

				// ワーカーを作成します。
				auto worker = new Worker(i, buffer);

				// スレッドプールワークを作成する。
				works[i] = ::CreateThreadpoolWork(
					[](PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WORK Work)
					{
						auto worker = (Worker*)Context;
						worker->calc_volume();
						delete worker;
					},
					worker, nullptr);

				// スレッドプールワークを開始します。
				::SubmitThreadpoolWork(works[i]);

				// 音声信号が想定より少なかった場合は
				if (read < length)
				{
					// 読み込んだフレーム数をセットします。
					c = i + 1;

					break; // ループを終了します。
				}
			}

			// 読み込んだフレーム数をセットします。
			shared->volume_count = c;

			// すべてのワークが完了するまで待機します。
			for (auto i = 0; i < c; i++)
			{
				::WaitForThreadpoolWorkCallbacks(works[i], FALSE);
				::CloseThreadpoolWork(works[i]);
			}

			auto end_time = ::timeGetTime();

			MY_TRACE("所要時間 = {}秒\n", (end_time - start_time) / 1000.0);

			return TRUE;
		}

		//
		// UIプロセスに算出した音量を送信します。(受け取るように促します)
		//
		BOOL send()
		{
			MY_TRACE_FUNC("");

			auto tid = ::GetCurrentThreadId(); // 共有メモリを識別するためのIDです。

			return ::PostMessage(ui_window, share::c_message.c_receive_cache, (WPARAM)tid, 0);
		}
	} app;
}
