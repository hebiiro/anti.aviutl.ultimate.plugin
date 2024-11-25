#pragma once

namespace apn::font_tree
{
	//
	// このクラスはアプリケーションのインターフェイスです。
	//
	struct AppInterface {
		virtual BOOL read_xml() = 0;
	} *app = nullptr;
}
