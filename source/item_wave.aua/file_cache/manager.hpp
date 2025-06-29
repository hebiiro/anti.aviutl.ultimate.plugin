#pragma once

namespace apn::item_wave::file_cache
{
	//
	// このクラスはファイル毎のキャッシュを管理します。
	//
	inline struct Manager : my::Window
	{
		inline static constexpr struct Message {
			inline static constexpr uint32_t c_receive = WM_APP + 2025;
		} c_message;

		struct Listener {
			virtual BOOL on_file_cache_changed(const std::filesystem::path& file_name) = 0;
		} *listener = {};

		//
		// 予約されているリクエストのコレクションです。
		//
		std::deque<std::filesystem::path> requests;

		//
		// ファイルキャッシュのコレクションです。
		//
		std::unordered_map<std::filesystem::path, std::shared_ptr<Node>> caches;

		//
		// 稼働中のスレッドのコレクションです。
		//
		std::unordered_map<size_t, std::thread> threads;

		//
		// レスポンスをロックするためのミューテックスです。
		//
		std::mutex response_mutex;

		//
		// レスポンスのコレクションです。
		//
		std::unordered_map<std::filesystem::path, std::shared_ptr<Node>> responses;

		//
		// コレクション(マップ)のキーを返します。
		//
		inline static size_t get_thread_key(const std::thread::id& thread_id)
		{
			return std::hash<std::thread::id>()(thread_id);
		}

		//
		// 初期化処理を実行します。
		//
		BOOL init(Listener* listener)
		{
			MY_TRACE_FUNC("");

			// リスナーをセットします。
			this->listener = listener;

			// ウィンドウラクス名です。
			const auto class_name = _T("apn::item_wave::file_cache::manager");

			// ウィンドウクラスを登録します。
			WNDCLASS wc = {};
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = hive.instance;
			wc.lpszClassName = class_name;
			::RegisterClass(&wc);

			// メッセージのみのウィンドウを作成します。
			return __super::create(
				DWORD {},
				class_name,
				class_name,
				WS_POPUP,
				0, 0, 0, 0,
				HWND_MESSAGE,
				HMENU {},
				hive.instance,
				LPVOID {});
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return destroy();
		}

		//
		// ファイルキャッシュを返します。
		//
		std::shared_ptr<Node> get(const std::filesystem::path& file_name)
		{
			MY_TRACE_FUNC("{/}", file_name);

			auto it = caches.find(file_name);
			if (it == caches.end()) return nullptr;
			return it->second;
		}

		//
		// ファイルキャッシュの作成をリクエストします。
		//
		BOOL reserve(const std::filesystem::path& file_name)
		{
			MY_TRACE_FUNC("{/}", file_name);

			// 存在しないファイルの場合はFALSEを返します。
			if (!std::filesystem::exists(file_name)) return FALSE;

			// ファイルキャッシュが作成済みまたは作成中の場合はFALSEを返します。
			if (caches.contains(file_name)) return FALSE;

			// ファイルキャッシュがリクエスト済みの場合はFALSEを返します。
			if (std::find(requests.begin(), requests.end(), file_name) != requests.end()) return FALSE;

			// リクエストをコレクションに追加します。
			requests.emplace_back(file_name);

			return TRUE;
		}

		//
		// ファイルキャッシュの作成を促します。
		//
		BOOL yield()
		{
			MY_TRACE_FUNC("");

			// リクエストの数または最大スレッド数までループします。
			while (requests.size() && threads.size() < hive.max_thread_count)
			{
				// 先頭のリクエストを取得します。
				auto file_name = requests.front();

				// 先頭のリクエストをコレクションから削除します。
				requests.pop_front();

				// キャッシュを作成中としてマークします。
				caches[file_name] = nullptr;

				// サブスレッドを作成します。
				auto thread = std::thread([this](std::filesystem::path file_name)
				{
					// ファイルキャッシュを作成します。
					if (auto cache = create(file_name))
					{
						// レスポンスをロックします。
						std::lock_guard lock(response_mutex);

						// レスポンスを追加します。
						responses[file_name] = cache;
					}

					// メインスレッドに通知します。
					::PostMessage(hwnd, c_message.c_receive,
						get_thread_key(std::this_thread::get_id()), 0);
				},
				file_name);

				// このままスレッドを稼働させるのでデタッチします。
				thread.detach();

				// スレッドをコレクションに追加します。
				threads[get_thread_key(thread.get_id())] = std::move(thread);
			}

			return TRUE;
		}

		//
		// ファイルキャッシュを作成します。
		//
		std::shared_ptr<Node> create(const std::filesystem::path& file_name)
		{
			MY_TRACE_FUNC("{/}", file_name);

			// パイプ名を作成します。
			auto pipe_name = my::format(L"\\\\.\\pipe\\item_wave\\{/}", ::GetCurrentThreadId());

			// パイプを作成します。
			my::handle::unique_ptr<> pipe(::CreateNamedPipeW(pipe_name.c_str(),
				PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_WAIT, 1, 0, 0, 0, nullptr));

			// リーダープロセスのファイルパスを取得します。
			auto reader_process_file_path = magi.get_module_file_name(L"x64\\item_wave_reader.exe");

			// リーダープロセスのフォルダパスを取得します。
			auto reader_process_folder_path = my::get_module_file_name(nullptr).parent_path();

			// 音量モードを文字列で取得します。
			auto volume_mode = hive.c_volume_mode.labels[hive.volume_mode].text;

			// 起動コマンドを作成します。
			auto command_line = my::format(L"\"{/}\" \"{/}\" \"{/}\" \"{/}\"",
				reader_process_file_path, volume_mode, file_name, pipe_name);

			// リーダープロセスを作成します。
			SubProcess reader_process(reader_process_folder_path, command_line, FALSE);

			// リーダープロセスを開始します。
			// リーダープロセスを開始できなかった場合はnullptrを返します。
			if (!reader_process.start()) return nullptr;

			// リーダープロセスがパイプに接続するまで待機します。
			::ConnectNamedPipe(pipe.get(), nullptr);

			// ファイルキャッシュを作成します。
			auto cache = std::make_shared<Node>();

			// パイプから計算結果を読み込みます。
			{
				// 音量の数を取得します。
				auto length = uint32_t {};
				::ReadFile(pipe.get(), &length, sizeof(length), nullptr, nullptr);

				// 音量を格納できるようにバッファをリサイズします。
				cache->volumes.resize(length);

				// 読み込み先のインデックスです。
				auto index = uint32_t {};

				// 音量の数までループします。
				while (index < length)
				{
					// 一度に読み込むバイト数です。
					constexpr auto c_read_bytes = DWORD { 4096 };

					// 読み込み用の変数です。
					auto read = DWORD {};
					auto nb_buffer = std::min<DWORD>(c_read_bytes, length - index);

					// 音量をパイプから読み込みます。
					if (!::ReadFile(pipe.get(), &cache->volumes[index], nb_buffer, &read, nullptr))
						break; // 読み込みに失敗した場合はループを終了します。

					// インデックスを進めます。
					index += read;
				}
			}

			// パイプとリーダープロセスの接続を切断します。
			::DisconnectNamedPipe(pipe.get());

			// ファイルキャッシュを返します。
			return cache;
		}

		//
		// ファイルキャッシュを削除します。
		//
		BOOL erase(const std::filesystem::path& file_name)
		{
			MY_TRACE_FUNC("{/}", file_name);

			caches.erase(file_name);

			return TRUE;
		}

		//
		// すべてのファイルキャッシュを消去します。
		//
		BOOL clear()
		{
			MY_TRACE_FUNC("");

			caches.clear();

			return TRUE;
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_CREATE:
				{
					MY_TRACE_FUNC("WM_CREATE, {/hex}, {/hex}", wParam, lParam);

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {/hex}, {/hex}", wParam, lParam);

					break;
				}
			case c_message.c_receive:
				{
					MY_TRACE_FUNC("c_receive, {/hex}, {/hex}", wParam, lParam);

					// 処理が終了したスレッドを削除します。
					threads.erase(wParam);

					// レスポンスです。
					// ロックを最小化するための一時変数です。
					decltype(this->responses) responses;

					{
						// レスポンスをロックします。
						std::lock_guard lock(response_mutex);

						// レスポンスをムーブします。
						responses = std::move(this->responses);
					}

					// レスポンスが存在する場合は
					if (responses.size())
					{
						// レスポンスを走査します。
						for (auto response : responses)
						{
							// レスポンスをキャッシュに変換します。
							caches[response.first] = response.second;

							// キャッシュの変更をリスナーに通知します。
							listener->on_file_cache_changed(response.first);
						}
					}

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} manager;
}
