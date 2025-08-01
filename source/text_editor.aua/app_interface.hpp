#pragma once

namespace apn::text_editor
{
	//
	// このクラスはアプリケーションのインターフェイスです。
	//
	inline struct AppInterface {
		//
		// コンストラクタです。
		//
		AppInterface() { app = this; }

		virtual BOOL update() = 0;
		virtual BOOL to_edits() = 0;
		virtual BOOL from_edits() = 0;
	} *app = nullptr;
}
