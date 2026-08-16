#pragma once

namespace apn::dark
{
	//
	// このクラスはアプリケーションの基本クラスです。
	//
	inline struct app_base_t
	{
		//
		// コンストラクタです。
		//
		app_base_t() { app_base = this; }

		virtual BOOL on_initialize_plugin(DWORD version) { return TRUE; }
		virtual BOOL on_uninitialize_plugin() { return TRUE; }
	} *app_base = nullptr;

	//
	// このクラスはアプリケーションのインターフェイスです。
	//
	inline struct app_interface_t : app_base_t
	{
		//
		// コンストラクタです。
		//
		app_interface_t() { app = this; }

		virtual BOOL on_init() = 0;
		virtual BOOL on_exit() = 0;
		virtual BOOL on_post_init() = 0;
		virtual BOOL on_update() = 0;
		virtual BOOL on_reload() = 0;
		virtual BOOL on_set_style_file_name(const std::wstring& style_file_name) = 0;
		virtual BOOL on_set_custom_color_file_name(const std::wstring& custom_color_file_name) = 0;
	} *app = nullptr;
}
