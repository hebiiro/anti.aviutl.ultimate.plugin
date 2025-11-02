#pragma once

namespace apn::item_wave
{
	//
	// このクラスはアプリケーションのインターフェイスです。
	//
	inline struct app_interface_t
	{
		//
		// コンストラクタです。
		//
		app_interface_t() { app = this; }

		virtual BOOL init() = 0;
		virtual BOOL exit() = 0;
		virtual BOOL clear_caches() = 0;
		virtual BOOL on_audio_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip, const Addin::ProcState& proc_state) = 0;
	} *app = nullptr;
}
