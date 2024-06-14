#pragma once

namespace apn::item_wave::ui
{
	//
	// このクラスはアプリケーションのインターフェイスです。
	//
	inline struct AppInterface {
		virtual BOOL init() = 0;
		virtual BOOL exit() = 0;
		virtual BOOL read_config() = 0;
		virtual BOOL write_config() = 0;
		virtual BOOL clear_caches() = 0;
		virtual BOOL create_cache() = 0;
		virtual BOOL receive_cache(DWORD id) = 0;
	} *app = nullptr;
}
