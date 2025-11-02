#pragma once

namespace apn::image_export
{
	//
	// このクラスはアプリケーションのインターフェイスです。
	//
	struct app_interface_t
	{
		//
		// コンストラクタです。
		//
		app_interface_t() { app = this; }
	} *app = nullptr;
}
