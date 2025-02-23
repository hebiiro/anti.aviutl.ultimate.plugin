#pragma once

namespace apn::item_wave::ui
{
	//
	// このクラスはアプリケーションです。
	//
	inline struct App : AppInterface
	{
		//
		// コンストラクタです。
		//
		App() { app = this; }

		//
		// 初期化処理を実行します。
		//
		virtual BOOL init() override
		{
			MY_TRACE_FUNC("");

			// コマンドライン引数からホストプロセスのウィンドウを取得します。
			hive.host_window = (HWND)wcstoull(::GetCommandLineW(), nullptr, 0);
			MY_TRACE_HEX(hive.host_window);
			if (!hive.host_window) return FALSE;

			// コンフィグ入出力を初期化します。
			config_io.init();

			if (!ui_window.init())
			{
				hive.message_box(L"UIウィンドウの初期化に失敗しました");

				return FALSE;
			}

			// 共有メモリを初期化します。
			if (!share_manager.init())
			{
				hive.message_box(L"共有メモリの初期化に失敗しました");

				return FALSE;
			}

			// ホストプロセスチェッカーを初期化します。
			host_process_checker.init();

			// コンフィグ更新チェッカーを初期化します。
			config_update_checker.init();

			// 初期化完了のイベントを作成します。
			my::Event completion_event(EVENT_ALL_ACCESS, FALSE,
				share::get_completion_event_name(hive.host_window).c_str());

			// 待機しているホストプロセスに初期化が完了したことを知らせます。
			::SetEvent(completion_event);

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL exit() override
		{
			MY_TRACE_FUNC("");

			// 各種後始末処理を実行します。
			config_update_checker.exit();
			host_process_checker.exit();
			share_manager.exit();
			ui_window.exit();
			config_io.exit();

			return TRUE;
		}

		//
		// コンフィグを読み込みます。
		//
		virtual BOOL read_config() override
		{
			MY_TRACE_FUNC("");

			return config_io.read();
		}

		//
		// コンフィグを書き込みます。
		//
		virtual BOOL write_config() override
		{
			MY_TRACE_FUNC("");

			return config_io.write();
		}

		//
		// すべてのキャッシュを消去します。
		//
		virtual BOOL clear_caches() override
		{
			MY_TRACE_FUNC("");

			cache_manager.clear();

			return TRUE;
		}

		//
		// キャッシュを作成します。
		//
		virtual BOOL create_cache() override
		{
			MY_TRACE_FUNC("");

			auto shared = share_manager.main_buffer.get_buffer();
			if (!shared) return FALSE;
			auto& request = shared->wave_request;

			// リクエストされたキャッシュを取得します。
			if (auto cache = cache_manager.get_cache(request.file_name))
			{
				// キャッシュが作成済みならホストプロセスに通知します。
				send_cache(cache);
			}
			else
			{
				// キャッシュが存在しないので新規作成します。
				// (1) ファイル名からウェイターを作成します。
				// (2) ウェイターからリーダーを作成します。
				// (3) リーダーからキャッシュを作成します。

				// ウェイターを作成します。
				auto waiter = waiter_manager.create_waiter(request.file_name);

				// 可能であれば、リクエストされたファイルを読み込みます。
				read_files();
			}

			return TRUE;
		}

		//
		// リーダーからキャッシュを受け取ります。
		//
		virtual BOOL receive_cache(DWORD id) override
		{
			MY_TRACE_FUNC("{}", id);

			// リーダーを取得します。
			if (auto reader = reader_manager.get_reader(id))
			{
				// キャッシュを作成します。
				if (auto cache = cache_manager.create_cache(reader))
				{
					// キャッシュを作成できた場合はメインプロセスに通知します。
					send_cache(cache);

					// このリーダーは不要になったので削除します。
					reader_manager.erase_reader(id);

					// リーダーの空きができたので次の読み込みを開始します。
					read_files();
				}
			}

			return TRUE;
		}

		//
		// 作成したキャッシュをホストプロセスに送信します。(受け取るように促します)
		//
		BOOL send_cache(const std::shared_ptr<Cache>& cache)
		{
			MY_TRACE_FUNC("{}", cache->file_name);

			// 共有メモリを取得します。
			auto shared = share_manager.main_buffer.get_buffer();
			MY_TRACE_HEX(shared);
			if (!shared) return FALSE;
			auto& result = shared->wave_result;

			// ファイル名を共有メモリにコピーします。
			share::copy(result.file_name, cache->file_name);

			// 音量を共有メモリにコピーします。
			result.volume_count = (int32_t)cache->volumes.size();
			MY_TRACE_INT(result.volume_count);
			memcpy(result.volumes, cache->volumes.data(), sizeof(share::Volume) * cache->volumes.size());

			// キャッシュを受け取るようにホストプロセスに合図します。
			MY_TRACE_HEX(hive.host_window);
			::SendMessage(hive.host_window, share::c_message.c_cache_result, 0, 0);

			return TRUE;
		}

		//
		// 待機中のウェイターが保有するファイルを読み込みます。
		//
		void read_files()
		{
			MY_TRACE_FUNC("");

			// ウェイターが存在し、なおかつリーダーを作成可能な場合は
			while (waiter_manager.available() && reader_manager.available())
			{
				// 先頭のウェイターを取得します。
				auto waiter = waiter_manager.pop_front();

				// リーダーを作成し、ファイルの読み込みを開始します。
				auto reader = reader_manager.create_reader(waiter->file_name);
			}

			MY_TRACE_INT(waiter_manager.collection.size());
			MY_TRACE_INT(reader_manager.collection.size());
		}
	} app_impl;
}
