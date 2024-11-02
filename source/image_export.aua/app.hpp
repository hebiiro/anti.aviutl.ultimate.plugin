#pragma once

namespace apn::image_export
{
	//
	// このクラスはHive::Appの実装です。
	//
	inline struct App : Hive::App
	{
		//
		// コンストラクタです。
		//
		App()
		{
			hive.app = this;
		}

		virtual std::wstring browse() override
		{
			constexpr LPCSTR filter = "PNG / BMP / JPG / GIF / TIFF File\0*.png;*.bmp;*.jpg;*.gif;*.tiff\0All File (*.*)\0*.*\0";
			constexpr LPCSTR def = nullptr;

			char file_name[MAX_PATH] = {};
			if (!magi.fp->exfunc->dlg_get_save_name(file_name, filter, def)) return {};
			return my::ts(file_name);
		}

		virtual BOOL export_image(const std::wstring& file_name, BOOL has_alpha, BOOL sel_item_only) override
		{
			return Executor().export_image(file_name, has_alpha, sel_item_only);
		}
	} app;
}
