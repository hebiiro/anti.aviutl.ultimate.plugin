#pragma once

namespace apn::image_export
{
	//
	// このクラスは画像をエクスポートする処理を実行します。
	//
	inline struct Executor
	{
#pragma pack(push)
#pragma pack(1)
		struct PixelARGB { uint8_t b, g, r, a; };
#pragma pack(pop)

		template <typename T>
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

		inline static decltype(AviUtl::FilterPlugin::func_proc) true_exedit_func_proc = nullptr;
		static BOOL hook_exedit_func_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip)
		{
			MY_TRACE_FUNC("begin");

			constexpr AviUtl::PixelYC white = { 4096, 0, 0 };
			auto ycp_edit = (AviUtl::PixelYC*)fpip->ycp_edit;

			for (int32_t y = 0; y < fpip->h; y++)
			{
				auto lineBegin = y * fpip->max_w;

				for (int32_t x = 0; x < fpip->w; x++)
				{
					ycp_edit[lineBegin + x] = white;
				}
			}

			auto result = true_exedit_func_proc(fp, fpip);

			MY_TRACE_FUNC("end");

			return result;
		}

		inline static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
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
				if (::lstrcmpW(pImageCodecInfo[j].MimeType, format) == 0)
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
		BOOL export_image(const std::wstring& file_name, BOOL has_alpha, BOOL sel_item_only)
		{
			MY_TRACE_FUNC("{}, {}, {}", file_name, has_alpha, sel_item_only);

			auto fp = magi.fp;
			auto editp = magi.exin.get_editp();
			if (!fp || !editp) return FALSE;
			if (!fp->exfunc->is_editing(editp)) return FALSE;

			auto exedit = magi.auin.get_filter_plugin(fp, "拡張編集");
			if (!exedit) return FALSE;

			std::filesystem::path path(file_name);
			std::filesystem::create_directories(path.parent_path());
			auto extension = my::to_lower(path.extension());
			MY_TRACE_STR(extension);

			ULONG quality = hive.quality;
			EncoderParameters encoder_parameters = {};
			encoder_parameters.Count = 1;
			encoder_parameters.Parameter[0].Guid = EncoderQuality;
			encoder_parameters.Parameter[0].Type = EncoderParameterValueTypeLong;
			encoder_parameters.Parameter[0].NumberOfValues = 1;
			encoder_parameters.Parameter[0].Value = &quality;

			CLSID encoder = {};
			int result = -1;
			if (extension == L".bmp") result = GetEncoderClsid(L"image/bmp", &encoder);
			else if (extension == L".jpg") result = GetEncoderClsid(L"image/jpeg", &encoder);
			else if (extension == L".gif") result = GetEncoderClsid(L"image/gif", &encoder);
			else if (extension == L".tif") result = GetEncoderClsid(L"image/tiff", &encoder);
			else if (extension == L".png") result = GetEncoderClsid(L"image/png", &encoder);

			if (result == -1)
			{
				hive.message_box(L"拡張子が無効です");

				return FALSE;
			}

			// フレームの映像サイズを取得します。
			int32_t frame = fp->exfunc->get_frame(editp);
			int32_t width = 0, height = 0;
			fp->exfunc->get_pixel_filtered(editp, frame, nullptr, &width, &height);
			int32_t stride = width * 3 + width % 4;

			// 出力バッファを確保します。
			Bits<PixelARGB> output(width, height, width * 4);

			// 選択オブジェクトを囲う点線を消すために func_save_start を呼ぶ
			if (exedit) exedit->func_save_start(exedit, frame, frame, editp);

			// 入力バッファを確保します。
			Bits<AviUtl::PixelBGR> input(width, height, stride);
			fp->exfunc->get_pixel_filtered(editp, frame, input.get(), nullptr, nullptr);

			if (exedit && has_alpha)
			{
				// func_proc をすり替え
				true_exedit_func_proc = exedit->func_proc;
				exedit->func_proc = hook_exedit_func_proc;

				// アルファ算出用のサブ入力バッファを確保します。
				Bits<AviUtl::PixelBGR> sub_input(width, height, stride);
				fp->exfunc->get_pixel_filtered(editp, frame, sub_input.get(), nullptr, nullptr);

				// func_proc を元に戻す
				exedit->func_proc = true_exedit_func_proc;

				for (int32_t y = 0; y < height; y++)
				{
					auto output_line = output.get_line(height - y - 1);
					auto input_line = input.get_line(y);
					auto sub_input_line = sub_input.get_line(y);

					for (int32_t x = 0; x < width; x++)
					{
						auto ra = (uint8_t)(255 - sub_input_line[x].r + input_line[x].r);
						auto ga = (uint8_t)(255 - sub_input_line[x].g + input_line[x].g);
						auto ba = (uint8_t)(255 - sub_input_line[x].b + input_line[x].b);
						output_line[x].a = (uint8_t)((ra+ga+ba)/3);
						output_line[x].r = ra ? (uint8_t)(input_line[x].r * 255 / ra) : input_line[x].r;
						output_line[x].g = ga ? (uint8_t)(input_line[x].g * 255 / ga) : input_line[x].g;
						output_line[x].b = ba ? (uint8_t)(input_line[x].b * 255 / ba) : input_line[x].b;
					}
				}
			}
			else
			{
				for (int32_t y = 0; y < height; y++)
				{
					auto output_line = output.get_line(height - y - 1);
					auto input_line = input.get_line(y);

					for (int32_t x = 0; x < width; x++)
					{
						output_line[x].a = 255;
						output_line[x].r = input_line[x].r;
						output_line[x].g = input_line[x].g;
						output_line[x].b = input_line[x].b;
					}
				}
			}

			if (exedit) exedit->func_save_end(exedit, editp);

			if (sel_item_only)
			{
				// アイテムの位置を取得します。
				my::SelectionVisual visual = {};
				auto object_index = magi.exin.get_current_object_index();
				auto c = magi.exin.get_selection_visual_count();
				MY_TRACE_INT(c);
				for (decltype(c) i = 0; i < c; i++)
				{
					auto visual_temp = magi.exin.get_selection_visual(i);

					MY_TRACE("{} => {:#010x}, {}, {}, {}, {}, {}\n",
						i, visual_temp->flags, visual_temp->object_index,
						visual_temp->x, visual_temp->y, visual_temp->w, visual_temp->h);

					if (visual_temp->flags & 0x04 && visual_temp->object_index == object_index)
					{
						visual = *visual_temp;

						break;
					}
				}

				// アイテムの位置を正規化します。
				if (visual.x < 0) visual.w += visual.x, visual.x = 0;
				if (visual.y < 0) visual.h += visual.y, visual.y = 0;
				visual.w = std::min(visual.w, width - visual.x);
				visual.h = std::min(visual.h, height - visual.y);

				if (visual.x < 0 || visual.w <= 0 || (visual.x + visual.w) > width)
				{
					hive.message_box(L"アイテムのX座標が無効です");

					return FALSE;
				}

				if (visual.y < 0 || visual.h <= 0 || (visual.y + visual.h) > height)
				{
					hive.message_box(L"アイテムのY座標が無効です");

					return FALSE;
				}

				auto dst_head = output.get();
				auto src_head = output.get() + visual.y * width + visual.x;

				for (int32_t y = 0; y < visual.h; y++)
				{
					auto dst_line = dst_head + y * visual.w;
					auto src_line = src_head + y * width;

					for (int32_t x = 0; x < visual.w; x++)
					{
						dst_line[x] = src_line[x];
					}
				}

				width = visual.w;
				height = visual.h;
			}

			Bitmap bitmap(width, height, width * 4,
				PixelFormat32bppARGB, (uint8_t*)output.get());
			Status status = bitmap.Save(file_name.c_str(), &encoder,
				(extension == L".jpg") ? &encoder_parameters : 0);
			MY_TRACE_HEX(status);

			if (status != S_OK)
			{
				hive.message_box(L"ファイルの保存に失敗しました");

				return FALSE;
			}

			return TRUE;
		}
	} action;
}
