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
		virtual BOOL drag_exo(int32_t mode) = 0;
		virtual BOOL insert_exo(int32_t mode) = 0;
		virtual BOOL read_file() = 0;
		virtual BOOL write_file() = 0;
	} *app = nullptr;
}
