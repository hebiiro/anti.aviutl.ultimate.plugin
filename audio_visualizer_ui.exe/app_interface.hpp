#pragma once

namespace apn::audio_visualizer::ui
{
	//
	// このクラスはアプリケーションのインターフェイスです。
	//
	inline struct AppInterface
	{
		virtual BOOL init() = 0;
		virtual BOOL exit() = 0;
		virtual BOOL run() = 0;
		virtual BOOL read_config() = 0;
		virtual BOOL write_config() = 0;
	} *app = nullptr;
}
