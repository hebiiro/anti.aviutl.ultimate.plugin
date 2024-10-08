#pragma once

namespace apn::item_wave
{
	//
	// このクラスはアプリケーションのインターフェイスです。
	//
	inline struct AppInterface {
		virtual BOOL create_cache(LPCSTR file_name) = 0;
		virtual BOOL clear_caches() = 0;
		virtual BOOL update_item_caches(BOOL send) = 0;
		virtual BOOL on_cache_result() = 0;
		virtual BOOL on_audio_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip, const Addin::ProcState& proc_state) = 0;
	} *app = nullptr;
}
