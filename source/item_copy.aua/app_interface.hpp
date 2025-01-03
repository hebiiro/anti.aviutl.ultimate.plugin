#pragma once

namespace apn::item_copy
{
	//
	// このクラスはアプリケーションのインターフェイスです。
	//
	inline struct AppInterface {
		//
		// コンストラクタです。
		//
		AppInterface() { app = this; }

		virtual BOOL paste_item() = 0;
		virtual BOOL copy_item() = 0;
		virtual BOOL read_file() = 0;
		virtual BOOL write_file() = 0;
	} *app = nullptr;
}
