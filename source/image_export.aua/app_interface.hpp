#pragma once

namespace apn::image_export
{
	//
	// このクラスはアプリケーションのインターフェイスです。
	//
	struct AppInterface {
		virtual std::wstring browse() = 0;
		virtual BOOL export_image(const std::wstring& file_name, BOOL has_alpha, BOOL sel_item_only) = 0;
		virtual BOOL copy_image(BOOL has_alpha, BOOL sel_item_only) = 0;
	} *app = nullptr;
}
