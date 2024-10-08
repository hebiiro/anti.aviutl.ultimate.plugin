#pragma once

namespace apn::local_web_app
{
	//
	// このクラスはアプリケーションのインターフェイスです。
	//
	inline struct AppInterface {
		virtual BOOL init() = 0;
		virtual BOOL exit() = 0;
	} *app = nullptr;
}
