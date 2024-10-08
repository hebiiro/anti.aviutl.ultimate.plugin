#pragma once

namespace apn::item_wave
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
		// キャッシュを作成します。
		//
		virtual BOOL create_cache(LPCSTR file_name) override
		{
			MY_TRACE_FUNC("");

			return sub_thread_manager.create_cache(file_name);
		}

		//
		// キャッシュを消去します。
		//
		virtual BOOL clear_caches() override
		{
			MY_TRACE_FUNC("");

			// 全てのキャッシュを消去します。
			sub_thread_manager.clear_caches();
			file_cache_manager.clear_caches();
			item_cache_manager.clear_caches();

			magi.exin.invalidate();

			return TRUE;
		}

		virtual BOOL update_item_caches(BOOL send) override
		{
			MY_TRACE_FUNC("{}", send);

			return item_cache_manager.update(send);
		}

		virtual BOOL on_cache_result() override
		{
			MY_TRACE_FUNC("");

			file_cache_manager.receive_cache();
			item_cache_manager.update(FALSE);

			return TRUE;
		}

		virtual BOOL on_audio_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip, const Addin::ProcState& proc_state) override
		{
			MY_TRACE_FUNC("");

			if (!fp->exfunc->is_editing(fpip->editp))
				return FALSE; // 編集中ではないときは何もしません。

			if (fp->exfunc->is_saving(fpip->editp))
				return FALSE; // 音声を保存するときは何もしません。

			// ファイル情報を取得しておきます。
			fp->exfunc->get_file_info(fpip->editp, &hive.fi);

			switch (hive.update_mode)
			{
			case hive.c_update_mode.c_off:
				{
					return FALSE; // 無効化されているときは何もしません。
				}
			case hive.c_update_mode.c_on_without_playing:
				{
					if (proc_state.is_playing)
						return FALSE; // 再生中のときは何もしません。

					break;
				}
			}

			// UIプロセスのウィンドウが作成済みの場合は
			if (hive.ui_window)
			{
				// アイテムキャッシュを更新します。
				update_item_caches(TRUE);
			}

			return FALSE;
		}
	} app_impl;
}
