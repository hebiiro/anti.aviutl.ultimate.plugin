#pragma once

namespace apn::text_split
{
	//
	// このクラスはアプリケーションのインターフェイスです。
	//
	inline struct AppInterface {
		//
		// コンストラクタです。
		//
		AppInterface() { app = this; }

		virtual BOOL split_text() = 0;
	} *app = nullptr;
}
