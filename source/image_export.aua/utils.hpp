#pragma once

namespace apn::image_export::utils
{
	inline std::wstring browse()
	{
		constexpr auto filter = "PNG / BMP / JPG / GIF / TIFF File\0*.png;*.bmp;*.jpg;*.gif;*.tiff\0All File (*.*)\0*.*\0";
		constexpr auto def = LPCSTR {};

		char file_name[MAX_PATH] = {};
		if (!magi.fp->exfunc->dlg_get_save_name(file_name, filter, def)) return {};
		return my::ts(file_name);
	}
}
