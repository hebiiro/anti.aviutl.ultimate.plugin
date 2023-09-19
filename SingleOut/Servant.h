#pragma once
#include "MainWindow.h"

namespace fgo::single_out
{
	//
	// このクラスはフレームまたはアイテムの画像をエクスポートします。
	//
	inline struct SingleOut : Servant, Magi::CommandID::Plugin, Hive::Commander
	{
		MainWindow mainWindow;

		//
		// この仮想関数は、このサーヴァントの識別名が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_name() override
		{
			return Hive::Name;
		}

		//
		// この仮想関数は、このサーヴァントの表示名が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_display_name() override
		{
			return Hive::DisplayName;
		}

		//
		// この仮想関数は、DLLの初期化のタイミングで呼ばれます。
		//
		BOOL on_dll_init(HINSTANCE instance) override
		{
			MY_TRACE_FUNC("0x%08X", instance);

			hive.instance = instance;
			hive.commander = this;

			return TRUE;
		}

		//
		// この仮想関数は、DLLの後始末のタイミングで呼ばれます。
		//
		BOOL on_dll_exit(HINSTANCE instance) override
		{
			MY_TRACE_FUNC("0x%08X", instance);

			return TRUE;
		}

		//
		// この仮想関数は、ウィンドウの初期化のタイミングで呼ばれます。
		//
		BOOL on_window_init(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			if (::GetModuleHandleW(L"SaveImage.auf"))
			{
				::MessageBoxW(hwnd,
					L"SingleOutとSaveImage.aufが競合しています"
					L"\nどちらかを除外してからAviUtlを再起動してください",
					Hive::DisplayName, MB_OK | MB_ICONWARNING);
			}

			if (!load()) return FALSE;
			if (!mainWindow.create()) return FALSE;

			hive.mainWindow = mainWindow;

			fp->exfunc->add_menu_item(fp, "現在フレーム画像を保存", mainWindow.mainDialog, IDC_EXPORT_FRAME_RGB, 0, AviUtl::ExFunc::AddMenuItemFlag::None);
			fp->exfunc->add_menu_item(fp, "現在フレーム画像をアルファ付きで保存", mainWindow.mainDialog, IDC_EXPORT_FRAME_RGBA, 0, AviUtl::ExFunc::AddMenuItemFlag::None);
			fp->exfunc->add_menu_item(fp, "選択アイテム画像を保存", mainWindow.mainDialog, IDC_EXPORT_ITEM_RGB, 0, AviUtl::ExFunc::AddMenuItemFlag::None);
			fp->exfunc->add_menu_item(fp, "選択アイテム画像をアルファ付きで保存", mainWindow.mainDialog, IDC_EXPORT_ITEM_RGBA, 0, AviUtl::ExFunc::AddMenuItemFlag::None);

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウの後始末のタイミングで呼ばれます。
		//
		BOOL on_window_exit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			save();
			mainWindow.destroy();

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウのコマンドのタイミングで呼ばれます。
		//
		BOOL on_window_command(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			switch (wParam)
			{
			case ID_ADDIN:
				{
					if (mainWindow)
						Tools::AviUtl::PluginWindowExtension::show(mainWindow);

					break;
				}
			}

			return FALSE;
		}

		//
		// コンフィグファイルのフルパスを返します。
		//
		inline static std::wstring getConfigFileName()
		{
			return magi.getConfigFileName(L"SingleOut.ini");
		}

		//
		// コンフィグファイル名を取得し、設定を読み込みます。
		//
		BOOL load()
		{
			return load(getConfigFileName().c_str());
		}

		//
		// 指定されたファイルから設定を読み込みます。
		//
		BOOL load(LPCWSTR path)
		{
			getPrivateProfileInt(path, L"Config", L"quality", hive.quality);
			getPrivateProfileWindow(path, L"MainWindow", mainWindow);

			return TRUE;
		}

		//
		// コンフィグファイル名を取得し、設定を保存します。
		//
		BOOL save()
		{
			return save(getConfigFileName().c_str());
		}

		//
		// 指定されたファイルに設定を保存します。
		//
		BOOL save(LPCWSTR path)
		{
			setPrivateProfileInt(path, L"Config", L"quality", hive.quality);
			setPrivateProfileWindow(path, L"MainWindow", mainWindow);

			return TRUE;
		}

#pragma pack(push)
#pragma pack(1)
		struct PixelARGB { uint8_t b, g, r, a; };
#pragma pack(pop)

		template<class T>
		struct Bits
		{
			int32_t width, height, stride;
			std::unique_ptr<uint8_t[]> bits;

			Bits(int32_t width, int32_t height, int32_t stride)
				: width(width), height(height), stride(stride)
				, bits(std::make_unique<uint8_t[]>(height * stride))
			{
			}

			T* get()
			{
				return reinterpret_cast<T*>(bits.get());
			}

			T* get_line(int32_t y)
			{
				return reinterpret_cast<T*>(bits.get() + y * stride);
			}
		};

		inline static decltype(AviUtl::FilterPlugin::func_proc) true_exedit_func_proc = 0;
		static BOOL hook_exedit_func_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip)
		{
			MY_TRACE(_T("hook_exedit_func_proc() begin\n"));

			const AviUtl::PixelYC white = { 4096, 0, 0 };
			AviUtl::PixelYC* ycp_edit = (AviUtl::PixelYC*)fpip->ycp_edit;

			for (int y = 0; y < fpip->h; y++)
			{
				int lineBegin = y * fpip->max_w;

				for (int x = 0; x < fpip->w; x++)
				{
					ycp_edit[lineBegin + x] = white;
				}
			}

			BOOL result = true_exedit_func_proc(fp, fpip);

			MY_TRACE(_T("hook_exedit_func_proc() end\n"));

			return result;
		}

		static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
		{
			UINT  num = 0;          // number of image encoders
			UINT  size = 0;         // size of the image encoder array in bytes

			ImageCodecInfo* pImageCodecInfo = NULL;

			GetImageEncodersSize(&num, &size);
			if(size == 0)
				return -1;  // Failure

			pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
			if(pImageCodecInfo == NULL)
				return -1;  // Failure

			GetImageEncoders(num, size, pImageCodecInfo);

			for(UINT j = 0; j < num; ++j)
			{
				if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
				{
					*pClsid = pImageCodecInfo[j].Clsid;
					free(pImageCodecInfo);
					return j;  // Success
				}    
			}

			free(pImageCodecInfo);
			return -1;  // Failure
		}

		//
		// 画像をエクスポートします。
		//
		BOOL onExportImage(BOOL hasAlpha, BOOL itemOnly) override
		{
			MY_TRACE_FUNC("%d, %d", hasAlpha, itemOnly);

			auto fp = magi.fp;
			auto editp = magi.auin.GetEditp();
			if (!fp || !editp) return FALSE;
			if (!fp->exfunc->is_editing(editp)) return FALSE;

			auto exedit = magi.auin.GetFilter(fp, "拡張編集");
			if (!exedit) return FALSE;

			char fileName[MAX_PATH] = {};
			if (!fp->exfunc->dlg_get_save_name(fileName, "PNG / BMP / JPG / GIF / TIFF File\0*.png;*.bmp;*.jpg;*.gif;*.tiff\0All File (*.*)\0*.*\0", 0))
				return FALSE;

			LPCSTR extension = ::PathFindExtensionA(fileName);
			MY_TRACE_STR(extension);

			ULONG quality = hive.quality;
			EncoderParameters encoderParameters = {};
			encoderParameters.Count = 1;
			encoderParameters.Parameter[0].Guid = EncoderQuality;
			encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
			encoderParameters.Parameter[0].NumberOfValues = 1;
			encoderParameters.Parameter[0].Value = &quality;

			CLSID encoder = {};
			int result = -1;
			if (::lstrcmpiA(extension, ".bmp") == 0) result = GetEncoderClsid(L"image/bmp", &encoder);
			else if (::lstrcmpiA(extension, ".jpg") == 0) result = GetEncoderClsid(L"image/jpeg", &encoder);
			else if (::lstrcmpiA(extension, ".gif") == 0) result = GetEncoderClsid(L"image/gif", &encoder);
			else if (::lstrcmpiA(extension, ".tif") == 0) result = GetEncoderClsid(L"image/tiff", &encoder);
			else if (::lstrcmpiA(extension, ".png") == 0) result = GetEncoderClsid(L"image/png", &encoder);

			if (result == -1)
			{
				::MessageBoxW(hive.mainWindow, L"拡張子が無効です", Hive::DisplayName, MB_OK);

				return FALSE;
			}

			// フレームの映像サイズを取得します。
			int32_t frame = fp->exfunc->get_frame(editp);
			int32_t width = 0, height = 0;
			fp->exfunc->get_pixel_filtered(editp, frame, 0, &width, &height);
			int32_t stride = width * 3 + width % 4;

			// 出力バッファを確保します。
			Bits<PixelARGB> output(width, height, width * 4);

			// 選択オブジェクトを囲う点線を消すために func_save_start を呼ぶ
			if (exedit) exedit->func_save_start(exedit, frame, frame, editp);

			// 入力バッファを確保します。
			Bits<AviUtl::PixelBGR> input(width, height, stride);
			fp->exfunc->get_pixel_filtered(editp, frame, input.get(), 0, 0);

			if (exedit && hasAlpha)
			{
				// func_proc をすり替え
				true_exedit_func_proc = exedit->func_proc;
				exedit->func_proc = hook_exedit_func_proc;

				// アルファ算出用のサブ入力バッファを確保します。
				Bits<AviUtl::PixelBGR> sub_input(width, height, stride);
				fp->exfunc->get_pixel_filtered(editp, frame, sub_input.get(), 0, 0);

				// func_proc を元に戻す
				exedit->func_proc = true_exedit_func_proc;

				for (int y = 0; y < height; y++)
				{
					auto output_line = output.get_line(height - y - 1);
					auto input_line = input.get_line(y);
					auto sub_input_line = sub_input.get_line(y);

					for (int x = 0; x < width; x++)
					{
						BYTE ra = (BYTE)(255 - sub_input_line[x].r + input_line[x].r);
						BYTE ga = (BYTE)(255 - sub_input_line[x].g + input_line[x].g);
						BYTE ba = (BYTE)(255 - sub_input_line[x].b + input_line[x].b);
						output_line[x].a = (BYTE)((ra+ga+ba)/3);
						output_line[x].r = ra ? (BYTE)(input_line[x].r * 255 / ra) : input_line[x].r;
						output_line[x].g = ga ? (BYTE)(input_line[x].g * 255 / ga) : input_line[x].g;
						output_line[x].b = ba ? (BYTE)(input_line[x].b * 255 / ba) : input_line[x].b;
					}
				}
			}
			else
			{
				for (int y = 0; y < height; y++)
				{
					auto output_line = output.get_line(height - y - 1);
					auto input_line = input.get_line(y);

					for (int x = 0; x < width; x++)
					{
						output_line[x].a = 255;
						output_line[x].r = input_line[x].r;
						output_line[x].g = input_line[x].g;
						output_line[x].b = input_line[x].b;
					}
				}
			}

			if (exedit) exedit->func_save_end(exedit, editp);

			if (itemOnly)
			{
				BYTE* positionData = magi.auin.GetPositionDataArray();
				int pd_x = *(int*)(positionData + 0x1C);
				int pd_y = *(int*)(positionData + 0x20);
				int pd_w = *(int*)(positionData + 0x24);
				int pd_h = *(int*)(positionData + 0x28);

				if (pd_x < 0 || pd_w <= 0 || (pd_x + pd_w) > width)
				{
					::MessageBoxW(hive.mainWindow, L"アイテムの位置情報が無効です", Hive::DisplayName, MB_OK);

					return FALSE;
				}

				if (pd_y < 0 || pd_h <= 0 || (pd_y + pd_h) > height)
				{
					::MessageBoxW(hive.mainWindow, L"アイテムの位置情報が無効です", Hive::DisplayName, MB_OK);

					return FALSE;
				}

				PixelARGB* dstHead = output.get();
				PixelARGB* srcHead = output.get() + pd_y * width + pd_x;

				for (int y = 0; y < pd_h; y++)
				{
					PixelARGB* dstLine = dstHead + y * pd_w;
					PixelARGB* srcLine = srcHead + y * width;

					for (int x = 0; x < pd_w; x++)
					{
						dstLine[x] = srcLine[x];
					}
				}

				width = pd_w;
				height = pd_h;
			}

			Bitmap bitmap(width, height, width * 4, PixelFormat32bppARGB, (BYTE*)output.get());

			Status status;
			if (::lstrcmpiA(extension, ".jpg") == 0)
				status = bitmap.Save((_bstr_t)fileName, &encoder, &encoderParameters);
			else
				status = bitmap.Save((_bstr_t)fileName, &encoder);
			MY_TRACE_COM_ERROR(status);

			if (status != S_OK)
			{
				::MessageBoxW(hive.mainWindow, L"ファイルの保存に失敗しました", Hive::DisplayName, MB_OK);

				return FALSE;
			}

			return TRUE;
		}
	} servant;
}
