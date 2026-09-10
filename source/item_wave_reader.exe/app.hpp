#pragma once

namespace apn::item_wave::reader
{
	//
	// このクラスはアプリケーションです。
	//
	inline struct app_t
	{
		//
		// 1ワーカーあたりの最低フレーム数です。
		// これを下回る場合はプロセスを分割せず、直接処理します。
		// (60FPS換算で1800フレーム=30秒程度を目安にしています)
		//
		inline static constexpr uint32_t c_min_frames_per_worker = 1800;

		//
		// 指定範囲の音量を算出し、結果をパイプに書き込みます。
		// (直接処理モード・作業員モードの両方から共通で呼び出します)
		//
		BOOL write_volumes_to_pipe(HANDLE pipe, const std::wstring& compute_mode,
			const std::wstring& file_name, uint32_t range_start, uint32_t range_end)
		{
			MY_TRACE_FUNC("");

			mft::session_t session(file_name, range_start, range_end);
			auto length = session.extract_volumes(compute_mode);
			MY_TRACE_INT(length);

			auto nb_written_bytes1 = DWORD {};
			auto r1 = ::WriteFile(pipe, &length, sizeof(length), &nb_written_bytes1, nullptr);
			MY_TRACE_INT(r1);
			MY_TRACE_INT(nb_written_bytes1);

			auto nb_written_bytes2 = DWORD {};
			auto r2 = ::WriteFile(pipe, session.volumes.data(), length, &nb_written_bytes2, nullptr);
			MY_TRACE_INT(r2);
			MY_TRACE_INT(nb_written_bytes2);

			return TRUE;
		}

		//
		// 作業員モードです。指定された範囲だけを処理し、
		// 自分専用のパイプに結果を書き込みます。
		//
		BOOL run_worker(const std::wstring& compute_mode,
			const std::wstring& file_name, const std::wstring& pipe_name,
			uint32_t range_start, uint32_t range_end)
		{
			MY_TRACE_FUNC("");

			my::handle::unique_ptr<> pipe(
				::CreateFileW(pipe_name.c_str(),
				GENERIC_WRITE, 0, nullptr,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
			if (pipe.get() == INVALID_HANDLE_VALUE) return FALSE;

			return write_volumes_to_pipe(pipe.get(), compute_mode, file_name, range_start, range_end);
		}

		//
		// 司令塔モードです。
		// ファイルの総フレーム数に応じて、
		// ・十分に長い場合はファイルを範囲分けして自分自身を複数起動し、並列処理させます。
		// ・短い場合はプロセスを分割せず、この場で直接処理します。
		//
		BOOL run_coordinator(const std::wstring& compute_mode,
			const std::wstring& file_name, const std::wstring& pipe_name)
		{
			MY_TRACE_FUNC("");

			mft::counter_base_t counter(L"MFセッション(全体)");

			// MFTを開始します。(nb_framesの取得のためだけに使います)
			::MFStartup(MF_VERSION);
			auto nb_frames = mft::probe_nb_frames(file_name);
			::MFShutdown();

			// 元のパイプを開きます。
			my::handle::unique_ptr<> pipe(
				::CreateFileW(pipe_name.c_str(),
				GENERIC_READ | GENERIC_WRITE, 0, nullptr,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
			if (pipe.get() == INVALID_HANDLE_VALUE) return FALSE;

			// 並列数を計算します。
			auto max_workers = (uint32_t)std::thread::hardware_concurrency();
			if (!max_workers) max_workers = 4;

			// フレーム数から見て妥当な並列数を求めます。
			auto nb_workers = nb_frames / c_min_frames_per_worker;
			nb_workers = (std::min)(nb_workers, max_workers);

			MY_TRACE_INT(nb_frames);
			MY_TRACE_INT(nb_workers);

			// 分割するまでもない小さなファイルの場合は、
			// プロセスを分割せず、この場で直接処理します。
			// (プロセス起動やパイプ作成のオーバーヘッドの方が
			//  処理本体より高くつくケースを避けるためです)
			if (nb_workers <= 1)
			{
				MY_TRACE("分割せずに直接処理します\n");
				return write_volumes_to_pipe(pipe.get(), compute_mode, file_name, 0, nb_frames);
			}

			auto volumes = std::vector<uint8_t>(nb_frames);

			// 自分自身の実行ファイルパスを取得します。
			auto self_path = my::get_module_file_name(nullptr);

			struct worker_t
			{
				my::handle::unique_ptr<> pipe;      // 司令塔側(サーバー)のパイプハンドルです。
				PROCESS_INFORMATION pi = {};
				uint32_t range_start = {}, range_end = {};
			};
			std::vector<worker_t> workers(nb_workers);

			// 各作業員用のパイプを作成し、子プロセスを起動します。
			for (auto i = uint32_t {}; i < nb_workers; i++)
			{
				auto& w = workers[i];
				w.range_start = (uint32_t)((uint64_t)nb_frames * i / nb_workers);
				w.range_end   = (uint32_t)((uint64_t)nb_frames * (i + 1) / nb_workers);

				auto child_pipe_name = pipe_name + L"_w" + std::to_wstring(i);

				w.pipe.reset(::CreateNamedPipeW(child_pipe_name.c_str(),
					PIPE_ACCESS_INBOUND,
					PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
					1, 0, 4 * 1024 * 1024, 0, nullptr));
				if (w.pipe.get() == INVALID_HANDLE_VALUE) return FALSE;

				auto cmd = my::format(L"\"{/}\" {/} \"{/}\" \"{/}\" {/} {/}",
					self_path, compute_mode, file_name, child_pipe_name, w.range_start, w.range_end);

				STARTUPINFOW si = { sizeof(si) };
				if (!::CreateProcessW(nullptr, cmd.data(), nullptr, nullptr,
					FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &w.pi))
				{
					return FALSE;
				}
			}

			// 各作業員からの結果を受け取ります。(接続待ち→読み込み)
			for (auto i = uint32_t {}; i < nb_workers; i++)
			{
				auto& w = workers[i];

				::ConnectNamedPipe(w.pipe.get(), nullptr);

				auto length = uint32_t {};
				auto nb_read_bytes1 = DWORD {};
				::ReadFile(w.pipe.get(), &length, sizeof(length), &nb_read_bytes1, nullptr);

				auto count = (std::min)(length, w.range_end - w.range_start);
				if (count)
				{
					auto nb_read_bytes2 = DWORD {};
					::ReadFile(w.pipe.get(), volumes.data() + w.range_start,
						count, &nb_read_bytes2, nullptr);
				}
			}

			// 全プロセスの終了を待ちます。
			for (auto& w : workers)
			{
				::WaitForSingleObject(w.pi.hProcess, INFINITE);
				::CloseHandle(w.pi.hProcess);
				::CloseHandle(w.pi.hThread);
			}

			// 結果を元のパイプに書き込みます。
			auto length = (uint32_t)volumes.size();
			auto nb_written_bytes1 = DWORD {};
			::WriteFile(pipe.get(), &length, sizeof(length), &nb_written_bytes1, nullptr);

			auto nb_written_bytes2 = DWORD {};
			::WriteFile(pipe.get(), volumes.data(), length, &nb_written_bytes2, nullptr);

			return TRUE;
		}

		//
		// メイン処理を実行します。
		//
		BOOL main()
		{
			MY_TRACE_FUNC("");

			auto command_line = ::GetCommandLineW();
			MY_TRACE_STR(command_line);

			auto c = int {};
			auto args = ::CommandLineToArgvW(command_line, &c);
			if (!args) return FALSE;
			my::scope_exit scope_exit([args](){ ::LocalFree(args); });
			MY_TRACE_INT(c);
			if (c < 4) return FALSE;

			auto compute_mode = (std::wstring)args[1];
			auto file_name = (std::wstring)args[2];
			auto pipe_name = (std::wstring)args[3];

			// 引数が6個(範囲指定あり)の場合は作業員モードです。
			if (c >= 6)
			{
				auto range_start = (uint32_t)std::stoul(args[4]);
				auto range_end   = (uint32_t)std::stoul(args[5]);
				return run_worker(compute_mode, file_name, pipe_name, range_start, range_end);
			}

			// それ以外(通常起動)は司令塔モードです。
			return run_coordinator(compute_mode, file_name, pipe_name);
		}
	} app;
}
