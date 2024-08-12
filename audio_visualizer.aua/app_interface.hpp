#pragma once

namespace apn::audio_visualizer
{
	//
	// このクラスはアプリケーションのインターフェイスです。
	//
	struct AppInterface {
		virtual BOOL read_config() = 0;
		virtual BOOL write_config() = 0;
		virtual BOOL update_config() = 0;
		virtual BOOL create_co_window() = 0;
		virtual BOOL destroy_co_window(size_t index) = 0;
		virtual BOOL edit_co_window(size_t index) = 0;
	} *app = nullptr;
}
