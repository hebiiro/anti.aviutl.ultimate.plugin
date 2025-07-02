#pragma once

namespace apn::text_collect
{
	//
	// このクラスはアプリケーションのインターフェイスです。
	//
	inline struct AppInterface {
		//
		// コンストラクタです。
		//
		AppInterface() { app = this; }

		virtual BOOL write_file() = 0;
		virtual BOOL show_file() = 0;
		virtual BOOL read_file() = 0;
	} *app = nullptr;
}
