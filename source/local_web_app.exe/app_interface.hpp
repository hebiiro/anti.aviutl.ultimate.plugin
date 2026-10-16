#pragma once

namespace apn::local_web_app
{
	//
	// このクラスはアプリケーションのインターフェイスです。
	//
	inline struct app_interface_t {
		virtual BOOL init() = 0;
		virtual BOOL exit() = 0;
	} *app = nullptr;
}
