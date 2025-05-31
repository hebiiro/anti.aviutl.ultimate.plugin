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
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			if (!config_io.init()) return FALSE;
			if (!addin_window.init()) return FALSE;
			if (!item_cache::manager.init()) return FALSE;
			if (!hook_manager.init()) return FALSE;

			if (!config_io.read()) MY_TRACE("コンフィグの読み込みに失敗しました\n");

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			config_io.write();

			hook_manager.exit();
			item_cache::manager.exit();
			addin_window.exit();
			config_io.exit();

			return TRUE;
		}

		//
		// すべてのキャッシュを消去します。
		//
		virtual BOOL clear_caches() override
		{
			MY_TRACE_FUNC("");

			// すべてのキャッシュを消去します。
			item_cache::manager.clear();

			magi.exin.invalidate();

			return TRUE;
		}

		//
		// この仮想関数は、音声処理を実行するときに呼ばれます。
		//
		virtual BOOL on_audio_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip, const Addin::ProcState& proc_state) override
		{
			MY_TRACE_FUNC("");

			// 編集中ではないときは何もしません。
			if (!fp->exfunc->is_editing(fpip->editp)) return FALSE;

			// 音声を保存するときは何もしません。
			if (fp->exfunc->is_saving(fpip->editp)) return FALSE;

			// 現在のファイル情報を取得します。
			auto fi = AviUtl::FileInfo {};
			fp->exfunc->get_file_info(fpip->editp, &fi);

			// プロジェクトのFPSが異なる場合は
			if (fi.video_rate != hive.current_fi.video_rate ||
				fi.video_scale != hive.current_fi.video_scale)
			{
				// アイテムキャッシュをリセットします。
				item_cache::manager.reset();
			}

			// ファイル情報を更新します。
			hive.current_fi = fi;

			switch (hive.update_mode)
			{
			case hive.c_update_mode.c_off:
				{
					return FALSE; // キャッシュの更新が無効化されている場合は何もしません。
				}
			case hive.c_update_mode.c_on_without_playing:
				{
					// 再生中の場合は何もしません。
					if (proc_state.is_playing) return FALSE;

					break;
				}
			}

			// アイテムキャッシュを更新します。
			item_cache::manager.refresh();

			return FALSE;
		}
	} app_impl;
}
