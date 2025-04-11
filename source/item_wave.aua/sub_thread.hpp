#pragma once

namespace apn::item_wave
{
	//
	// このクラスはキャッシュリクエストです。
	//
	struct CacheRequest
	{
		//
		// キャッシュの作成をリクエストされたファイルです。
		//
		std::string file_name;

		//
		// コンストラクタです。
		// これはメインスレッド側の処理です。
		//
		CacheRequest(LPCSTR file_name)
			: file_name(file_name)
		{
		}
	};

	//
	// このクラスはサブスレッドです。
	// このクラスの処理はすべてサブスレッド側の処理です。
	//
	struct SubThread
	{
		inline static constexpr struct Message {
			inline static constexpr uint32_t c_create_cache = WM_APP + 10;
			inline static constexpr uint32_t c_create_cache_internal = WM_APP + 11;
			inline static constexpr uint32_t c_clear_caches = WM_APP + 12;
		} c_message;

		//
		// キャッシュリクエストのコレクションです。
		//
		std::map<std::string, const CacheRequest*> collection;

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
		// キャッシュを作成します。
		//
		void on_create_cache(const CacheRequest* cache_request)
		{
			MY_TRACE_FUNC("");

			// キャッシュリクエストを検索します。
			auto it = collection.find(cache_request->file_name);

			// 既存のキャッシュリクエストが存在する場合は
			if (it != collection.end())
			{
				// このキャッシュリクエストは破棄します。
				delete cache_request;

				MY_TRACE("リクエストを破棄しました\n");

				return;
			}

			// キャッシュリクエストをコレクションに追加します。
			collection[cache_request->file_name] = cache_request;

			// キャッシュリクエストを送信します。
			::PostThreadMessage(::GetCurrentThreadId(), c_message.c_create_cache_internal, (WPARAM)cache_request, 0);
		}

		//
		// キャッシュを作成します。
		// 内部的に使用されます。
		//
		void on_create_cache_internal(const CacheRequest* cache_request)
		{
			MY_TRACE_FUNC("");

			// 共有メモリを取得します。
			if (auto shared = share_manager.main_buffer.get_buffer())
			{
				// 共有メモリに書き込みます。
				share::copy(shared->wave_request.file_name, cache_request->file_name);

				// UIプロセスにキャッシュの作成をリクエストします。
				::SendMessage(hive.ui_window, share::c_message.c_create_cache, 0, 0);
			}

			// キャッシュリクエストを検索します。
			auto it = collection.find(cache_request->file_name);

			// キャッシュリクエストがコレクション内に存在する場合は
			if (it != collection.end())
			{
				// 使用済みメモリを開放します。
				delete it->second;

				// このリクエストをコレクションから削除します。
				collection.erase(it);
			}
		}

		//
		// キャッシュを消去します。
		//
		void on_clear_caches()
		{
			MY_TRACE_FUNC("");

			// UIプロセスにキャッシュの消去をリクエストします。
			::SendMessage(hive.ui_window, share::c_message.c_clear_caches, 0, 0);
		}

		//
		// スレッド関数です。
		//
		DWORD proc()
		{
			MSG msg = {};
			while (::GetMessage(&msg, 0, 0, 0))
			{
				if (!msg.hwnd)
				{
					switch (msg.message)
					{
					case c_message.c_create_cache:
						{
							on_create_cache((const CacheRequest*)msg.wParam);
							break;
						}
					case c_message.c_create_cache_internal:
						{
							on_create_cache_internal((const CacheRequest*)msg.wParam);
							break;
						}
					case c_message.c_clear_caches:
						{
							on_clear_caches();
							break;
						}
					case WM_QUIT:
						{
							return 0;
						}
					}
				}

				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}

			return 0;
		}
	};

	//
	// このクラスはサブスレッドを管理します。
	// 基本的にすべてメインスレッド側の処理です。
	//
	inline struct SubThreadManager
	{
		DWORD tid = 0;
		my::handle::unique_ptr<> thread;

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// サブスレッドを開始します。
			thread.reset(::CreateThread(
				nullptr, 0,
				[](LPVOID param)
				{
					// ここはサブスレッド側の処理です。

					SubThread sub_thread;

					return sub_thread.proc();
				},
				nullptr, 0, &tid));

			// サブスレッドが作成できた場合はTRUEを返します。
			return !!thread;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			// 共有メモリを開放します。
			share_manager.exit();

			// サブスレッドを終了します。
#if 0
			return ::PostThreadMessage(tid, WM_QUIT, 0, 0);
#else
			return ::TerminateThread(thread.get(), -1);
#endif
		}

		//
		// サブスレッドにキャッシュの作成をリクエストします。
		//
		BOOL create_cache(LPCSTR file_name)
		{
			MY_TRACE_FUNC("{/}", file_name);

			return ::PostThreadMessage(tid, SubThread::c_message.c_create_cache, (WPARAM)new CacheRequest(file_name), 0);
		}

		//
		// サブスレッドにキャッシュの消去をリクエストします。
		//
		BOOL clear_caches()
		{
			MY_TRACE_FUNC("");

			return ::PostThreadMessage(tid, SubThread::c_message.c_clear_caches, 0, 0);
		}
	} sub_thread_manager;
}
