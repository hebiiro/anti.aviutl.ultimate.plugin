#pragma once

namespace apn::item_affine
{
	//
	// このクラスはアプリケーションのインターフェイスです。
	//
	inline struct app_interface_t {
		//
		// コンストラクタです。
		//
		app_interface_t() { app = this; }

		virtual BOOL pos_execute() = 0;
		virtual BOOL len_execute() = 0;
	} *app = nullptr;
}
