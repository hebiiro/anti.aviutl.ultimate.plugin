#pragma once

namespace fgo::single_out
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static const LPCWSTR Name = L"SingleOut";
		inline static const LPCWSTR DisplayName = L"画像のエクスポート";

		struct Mode {
			inline static const int Manual = 0;
			inline static const int Auto = 1;
			inline static const int Overwrite = 2;
			inline static const Tools::Label label[] = {
				{ Manual, L"Manual" },
				{ Auto, L"Auto" },
				{ Overwrite, L"Overwrite" },
			};
		};

		HINSTANCE instance = 0;
		HWND mainWindow = 0;

		int quality = 90;
		int mode = Mode::Manual;
		_bstr_t fileName = L"";
		int index = 0;
		int zero = 0;

		BOOL browse(LPSTR fileName)
		{
			const LPCSTR filter = "PNG / BMP / JPG / GIF / TIFF File\0*.png;*.bmp;*.jpg;*.gif;*.tiff\0All File (*.*)\0*.*\0";
			const LPCSTR def = 0;
			return magi.fp->exfunc->dlg_get_save_name(fileName, filter, def);
		}

	} hive;
}
