#pragma once

namespace apn::dark
{
	//
	// このクラスはアプリケーションのインターフェイスです。
	//
	struct AppInterface {
		virtual BOOL init() = 0;
		virtual BOOL exit() = 0;
		virtual BOOL init_skin() = 0;
		virtual BOOL update_skin() = 0;
		virtual BOOL on_change_controls() = 0;
		virtual BOOL on_change_config() = 0;
		virtual BOOL on_change_assets() = 0;
		virtual uint32_t get_current_skin_id() = 0;
		virtual BOOL redraw() = 0;
		virtual BOOL deploy_darken_window_settings() = 0;
		virtual BOOL equal(HTHEME theme1, HTHEME theme2) = 0;
	} *app = nullptr;
}
