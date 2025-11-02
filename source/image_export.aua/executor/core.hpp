#pragma once

namespace apn::image_export::executor
{
	//
	// このクラスは画像をエクスポートする処理のコアです。
	//
	struct core_t
	{
#pragma pack(push)
#pragma pack(1)
		//
		// このクラスはRGBAデータです。
		//
		struct pixel_argb_t { uint8_t b, g, r, a; };
#pragma pack(pop)

		//
		// このクラスは画像データです。
		//
		template <typename T>
		struct bits_t
		{
			int32_t width, height, stride;
			std::unique_ptr<uint8_t[]> bits;

			bits_t(int32_t width, int32_t height, int32_t stride)
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

		int32_t x = 0, y = 0;
		int32_t width = 0, height = 0;
		std::unique_ptr<bits_t<pixel_argb_t>> output;
		std::unique_ptr<Bitmap> bitmap;

		inline static decltype(AviUtl::FilterPlugin::func_proc) true_exedit_func_proc = nullptr;
		inline static BOOL hook_exedit_func_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip)
		{
			MY_TRACE("開始\n");

			my::scope_exit scope_exit([]() { MY_TRACE("終了\n"); });

			constexpr auto white = AviUtl::PixelYC { 4096, 0, 0 };
			auto ycp_edit = (AviUtl::PixelYC*)fpip->ycp_edit;

			for (decltype(fpip->h) y = 0; y < fpip->h; y++)
			{
				auto lineBegin = y * fpip->max_w;

				for (decltype(fpip->w) x = 0; x < fpip->w; x++)
				{
					ycp_edit[lineBegin + x] = white;
				}
			}

			return true_exedit_func_proc(fp, fpip);
		}

		//
		// 指定されたアイテムの映像位置を返します。
		//
		inline static my::SelectionVisual get_visual(int32_t object_index)
		{
			MY_TRACE_FUNC("{/}", object_index);

			auto visual = my::SelectionVisual {};
			auto c = magi.exin.get_selection_visual_count();
			for (decltype(c) i = 0; i < c; i++)
			{
				auto visual = magi.exin.get_selection_visual(i);

				MY_TRACE("{/} => {/hex}, {/}, {/}, {/}, {/}, {/}\n",
					i, visual->flags, visual->object_index,
					visual->x, visual->y, visual->w, visual->h);

				if (visual->flags & 0x04 && visual->object_index == object_index)
					return *visual;
			}

			throw L"アイテムに対応する映像位置が存在しませんでした";
		}

		//
		// 画像を作成します。
		//
		void create_image(BOOL has_alpha, BOOL sel_item_only)
		{
			MY_TRACE_FUNC("{/}, {/}", has_alpha, sel_item_only);

			return create_image_internal(has_alpha, sel_item_only ? magi.exin.get_current_object_index() : -1);
		}

		//
		// 画像を作成します。
		//
		void create_image_internal(BOOL has_alpha, int32_t object_index)
		{
			MY_TRACE_FUNC("{/}, {/}", has_alpha, object_index);

			auto fp = magi.fp;
			auto editp = magi.exin.get_editp();
			if (!fp || !editp) throw L"必要な変数を取得できませんでした";
			if (!fp->exfunc->is_editing(editp)) throw L"編集中で画像を読み取ることができませんでした";

			auto exedit = magi.auin.get_filter_plugin(fp, "拡張編集");
			if (!exedit) throw L"拡張編集を取得できませんでした";

			// フレーム番号を取得します。
			auto frame = fp->exfunc->get_frame(editp);

			// フレームの映像サイズを取得します。
			fp->exfunc->get_pixel_filtered(editp, frame, nullptr, &width, &height);

			// 出力バッファを確保します。
			output = std::make_unique<bits_t<pixel_argb_t>>(width, height, width * 4);

			// 選択オブジェクトを囲う点線を消すために func_save_start を呼ぶ
			if (exedit) exedit->func_save_start(exedit, frame, frame, editp);

			// 入力バッファのストライドを算出します。
			auto input_stride = width * 3 + width % 4;

			// 入力バッファを確保します。
			bits_t<AviUtl::PixelBGR> input(width, height, input_stride);
			fp->exfunc->get_pixel_filtered(editp, frame, input.get(), nullptr, nullptr);

			if (exedit && has_alpha)
			{
				// func_proc をすり替え
				true_exedit_func_proc = exedit->func_proc;
				exedit->func_proc = hook_exedit_func_proc;

				// アルファ算出用のサブ入力バッファを確保します。
				bits_t<AviUtl::PixelBGR> sub_input(width, height, input_stride);
				fp->exfunc->get_pixel_filtered(editp, frame, sub_input.get(), nullptr, nullptr);

				// func_proc を元に戻す
				exedit->func_proc = true_exedit_func_proc;

				for (int32_t y = 0; y < height; y++)
				{
					auto output_line = output->get_line(height - y - 1);
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
					auto output_line = output->get_line(height - y - 1);
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

			// アイテムが指定されている場合は
			if (object_index >= 0)
			{
				// アイテムの映像位置を取得します。
				auto visual = get_visual(object_index);

				// アイテムの映像位置を正規化します。
				if (visual.x < 0) visual.w += visual.x, visual.x = 0;
				if (visual.y < 0) visual.h += visual.y, visual.y = 0;
				visual.w = std::min(visual.w, width - visual.x);
				visual.h = std::min(visual.h, height - visual.y);

				if (visual.x < 0 || visual.w <= 0 || (visual.x + visual.w) > width)
					throw L"アイテムのX座標が無効です";

				if (visual.y < 0 || visual.h <= 0 || (visual.y + visual.h) > height)
					throw L"アイテムのY座標が無効です";

				auto dst_head = output->get();
				auto src_head = output->get() + visual.y * width + visual.x;

				for (int32_t y = 0; y < visual.h; y++)
				{
					auto dst_line = dst_head + y * visual.w;
					auto src_line = src_head + y * width;

					for (int32_t x = 0; x < visual.w; x++)
					{
						dst_line[x] = src_line[x];
					}
				}

				x = visual.x;
				y = visual.y;
				width = visual.w;
				height = visual.h;
			}

			// ビットマップを作成します。
			bitmap = std::make_unique<Bitmap>(
				width, height, width * 4,
				PixelFormat32bppARGB, (uint8_t*)output->get());
		}
	};
}
