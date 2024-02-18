#pragma once
#include "Hive.h"

namespace fgo::single_out
{
	//
	// このクラスはアクションを担当します。
	//
	inline struct Action
	{
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
		BOOL onExportImage(LPCWSTR fileName, BOOL hasAlpha, BOOL itemOnly)
		{
			MY_TRACE_FUNC("%ws, %d, %d", fileName, hasAlpha, itemOnly);

			auto fp = magi.fp;
			auto editp = magi.auin.GetEditp();
			if (!fp || !editp) return FALSE;
			if (!fp->exfunc->is_editing(editp)) return FALSE;

			auto exedit = magi.auin.GetFilter(fp, "拡張編集");
			if (!exedit) return FALSE;

			LPCWSTR extension = ::PathFindExtension(fileName);
			MY_TRACE_WSTR(extension);

			ULONG quality = hive.quality;
			EncoderParameters encoderParameters = {};
			encoderParameters.Count = 1;
			encoderParameters.Parameter[0].Guid = EncoderQuality;
			encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
			encoderParameters.Parameter[0].NumberOfValues = 1;
			encoderParameters.Parameter[0].Value = &quality;

			CLSID encoder = {};
			int result = -1;
			if (::lstrcmpiW(extension, L".bmp") == 0) result = GetEncoderClsid(L"image/bmp", &encoder);
			else if (::lstrcmpiW(extension, L".jpg") == 0) result = GetEncoderClsid(L"image/jpeg", &encoder);
			else if (::lstrcmpiW(extension, L".gif") == 0) result = GetEncoderClsid(L"image/gif", &encoder);
			else if (::lstrcmpiW(extension, L".tif") == 0) result = GetEncoderClsid(L"image/tiff", &encoder);
			else if (::lstrcmpiW(extension, L".png") == 0) result = GetEncoderClsid(L"image/png", &encoder);

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
				// アイテムの位置を取得します。
				AviUtlInternal::SelectItem si = {};
				int32_t objectIndex = magi.auin.GetCurrentObjectIndex();
				size_t c = magi.auin.GetSelectItemCount();
				MY_TRACE_INT(c);
				for (size_t i = 0; i < c; i++)
				{
					AviUtlInternal::SelectItem* si2 = magi.auin.GetSelectItem(i);

					MY_TRACE(_T("%d => 0x%08X, %d, %d, %d, %d, %d\n"),
						i, si2->flags, si2->objectIndex, si2->x, si2->y, si2->w, si2->h);

					if (si2->flags & 0x04 && si2->objectIndex == objectIndex)
					{
						si = *si2;

						break;
					}
				}

				// アイテムの位置を正規化します。
				if (si.x < 0) si.w += si.x, si.x = 0;
				if (si.y < 0) si.h += si.y, si.y = 0;
				si.w = std::min(si.w, width - si.x);
				si.h = std::min(si.h, height - si.y);

				if (si.x < 0 || si.w <= 0 || (si.x + si.w) > width)
				{
					::MessageBoxW(hive.mainWindow, L"アイテムの位置情報が無効です", Hive::DisplayName, MB_OK);

					return FALSE;
				}

				if (si.y < 0 || si.h <= 0 || (si.y + si.h) > height)
				{
					::MessageBoxW(hive.mainWindow, L"アイテムの位置情報が無効です", Hive::DisplayName, MB_OK);

					return FALSE;
				}

				PixelARGB* dstHead = output.get();
				PixelARGB* srcHead = output.get() + si.y * width + si.x;

				for (int y = 0; y < si.h; y++)
				{
					PixelARGB* dstLine = dstHead + y * si.w;
					PixelARGB* srcLine = srcHead + y * width;

					for (int x = 0; x < si.w; x++)
					{
						dstLine[x] = srcLine[x];
					}
				}

				width = si.w;
				height = si.h;
			}

			Bitmap bitmap(width, height, width * 4, PixelFormat32bppARGB, (BYTE*)output.get());

			Status status;
			if (::lstrcmpiW(extension, L".jpg") == 0)
				status = bitmap.Save(fileName, &encoder, &encoderParameters);
			else
				status = bitmap.Save(fileName, &encoder);
			MY_TRACE_COM_ERROR(status);

			if (status != S_OK)
			{
				::MessageBoxW(hive.mainWindow, L"ファイルの保存に失敗しました", Hive::DisplayName, MB_OK);

				return FALSE;
			}

			return TRUE;
		}
	} action;
}
