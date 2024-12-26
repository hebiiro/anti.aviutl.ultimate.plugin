#pragma once

namespace apn::one_for_text
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
		virtual BOOL write_file_limited() = 0;
		virtual BOOL read_file_limited() = 0;
	} *app = nullptr;
}
