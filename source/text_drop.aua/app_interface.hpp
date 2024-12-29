#pragma once

namespace apn::text_drop
{
	//
	// このクラスはアプリケーションのインターフェイスです。
	//
	inline struct AppInterface {
		//
		// コンストラクタです。
		//
		AppInterface() { app = this; }

		virtual BOOL write_exo(int32_t mode, const std::wstring& exo_path) = 0;
	} *app = nullptr;
}
