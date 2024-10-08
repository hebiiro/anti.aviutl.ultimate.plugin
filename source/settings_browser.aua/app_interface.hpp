#pragma once

namespace apn::settings_browser
{
	//
	// このクラスはアプリケーションのインターフェイスです。
	//
	inline struct AppInterface {
		virtual BOOL generate_settings_json() = 0;
		virtual BOOL navigate() = 0;
	} *app = nullptr;
}
