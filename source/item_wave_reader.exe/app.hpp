#pragma once

namespace apn::item_wave::reader
{
	inline struct App
	{
		//
		// メイン処理を実行します。
		//
		BOOL main()
		{
			MY_TRACE_FUNC("");

			// コマンドライン引数を取得します。
			auto command_line = ::GetCommandLineW();
			MY_TRACE_STR(command_line);

			// コマンドライン引数を個別に取得します。
			auto c = int {};
			auto args = ::CommandLineToArgvW(command_line, &c);
			MY_TRACE_INT(c);
			if (c < 4) return FALSE;

			// 音量算出関数です。
			auto compute_func = compute_peak;
			auto compute_func_string = (std::wstring)args[1];
			MY_TRACE_STR(compute_func_string);
			if (compute_func_string == L"rms") compute_func = compute_rms;

			// 音声ファイルのファイル名を取得します。
			auto file_name = (std::wstring)args[2];
			MY_TRACE_STR(file_name);

			// 結果出力用のパイプ名を取得します。
			auto pipe_name = (std::wstring)args[3];
			MY_TRACE_STR(pipe_name);

			// 結果出力用のパイプを作成します。
			my::handle::unique_ptr<> pipe(
				::CreateFileW(pipe_name.c_str(),
				GENERIC_READ | GENERIC_WRITE, 0, nullptr,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
			MY_TRACE_HEX(pipe.get());
			if (pipe.get() == INVALID_HANDLE_VALUE) return FALSE;

			// フレーム毎の音量を算出します。
			{
				auto start_time = ::timeGetTime();

				auto volumes = extract_rms(my::wide_to_cp(file_name, CP_UTF8), compute_func);

				auto end_time = ::timeGetTime();

				MY_TRACE("所要時間 = {/}秒\n", (end_time - start_time) / 1000.0);

				// 結果をパイプに書き込みます。
				{
					auto length = (uint32_t)volumes.size();
					MY_TRACE_INT(length);

					auto r2 = ::WriteFile(pipe.get(), &length, sizeof(length), nullptr, nullptr);
					MY_TRACE_INT(r2);

					auto r3 = ::WriteFile(pipe.get(), volumes.data(), sizeof(volumes[0]) * length, nullptr, nullptr);
					MY_TRACE_INT(r3);
				}
			}

			return TRUE;
		}
	} app;
}
