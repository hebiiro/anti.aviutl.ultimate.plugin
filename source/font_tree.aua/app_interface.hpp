#pragma once

namespace apn::font_tree
{
	//
	// このクラスはアプリケーションのインターフェイスです。
	//
	struct AppInterface {
		virtual BOOL show_config_dialog() = 0;
		virtual BOOL read_xml() = 0;
	} *app = nullptr;
}
