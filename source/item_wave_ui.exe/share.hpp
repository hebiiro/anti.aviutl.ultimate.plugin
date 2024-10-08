#pragma once

namespace apn::item_wave::ui
{
	inline struct ShareManager
	{
		my::Mutex mutex;
		my::SimpleFileMappingT<share::MainBuffer> main_buffer;

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// ホストウィンドウに結び付けられた共有オブジェクトを開きます。
			if (!mutex.open(MUTEX_ALL_ACCESS, FALSE, share::get_mutex_name(hive.host_window).c_str())) return FALSE;
			if (!main_buffer.open(FILE_MAP_ALL_ACCESS, FALSE, share::get_main_buffer_name(hive.host_window).c_str())) return FALSE;

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}
	} share_manager;
}
