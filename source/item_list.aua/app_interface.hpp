#pragma once

namespace apn::item_list
{
	//
	// このクラスはアプリケーションのインターフェイスです。
	//
	inline struct app_interface_t {
		//
		// コンストラクタです。
		//
		app_interface_t() { app = this; }
		virtual BOOL apply_finder() = 0;
		virtual BOOL refresh_list() = 0;
		virtual BOOL reset_list() = 0;
		virtual BOOL reset_sort() = 0;
		virtual BOOL reset_finder() = 0;
	} *app = nullptr;
}
