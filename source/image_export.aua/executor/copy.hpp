#pragma once

namespace apn::image_export::executor
{
	//
	// このクラスは画像をクリップボードにコピーします。
	//
	inline struct copy_t : core_t
	{
		//
		// 画像をクリップボードにコピーします。
		//
		BOOL copy_image(BOOL has_alpha, BOOL sel_item_only)
		{
			MY_TRACE_FUNC("{/}, {/}", has_alpha, sel_item_only);

			try
			{
				create_image(has_alpha, sel_item_only);
			}
			catch (LPCWSTR what)
			{
				hive.message_box(what);

				return FALSE;
			}

			// クリップボードを開きます。
			struct clipboard_locker_t {
				clipboard_locker_t(HWND hwnd = nullptr) {
					::OpenClipboard(hwnd);
				}
				~clipboard_locker_t() {
					::CloseClipboard();
				}
			} clipboard_locker;

			// クリップボードを空にします。
			auto result1 = ::EmptyClipboard();
			MY_TRACE_HEX(result1);

			// クリップボードにCF_DIBをセットします。
			if (1)
			{
				auto pixel_format = PixelFormat32bppRGB;
				if (has_alpha)
				{
					if (hive.flag_use_pargb)
						pixel_format = PixelFormat32bppPARGB;
					else
						pixel_format = PixelFormat32bppARGB;
				}

				auto rect = Rect(0, 0, bitmap->GetWidth(), bitmap->GetHeight());
				auto bitmap_data = BitmapData {};
				bitmap->LockBits(&rect, ImageLockModeRead, pixel_format, &bitmap_data);
				auto width = (int32_t)bitmap_data.Width;
				auto height = (int32_t)bitmap_data.Height;
				auto stride = (int32_t)bitmap_data.Stride;

				// 画像のバイト数を取得します。
				auto bits_count = height * stride;

				// メモリバッファです。
				auto buffer = (BITMAPINFOHEADER*)nullptr;

				// メモリハンドルを作成します。
				auto handle = ::GlobalAlloc(GHND, sizeof(*buffer) + bits_count);

				// メモリハンドルをロックしてメモリバッファを取得します。
				buffer = (decltype(buffer))::GlobalLock(handle);

				// メモリバッファに画像データを書き込みます。
				buffer->biSize = sizeof(BITMAPINFOHEADER);
				buffer->biWidth = width;
				buffer->biHeight = height;
				buffer->biPlanes = 1;
				buffer->biBitCount = 32;
				buffer->biCompression = BI_RGB;

				// 画像のコピー元バッファを取得します。
				auto src = (uint8_t*)bitmap_data.Scan0;

				// 画像のコピー先バッファを取得します。
				auto dst = (uint8_t*)(buffer + 1);

				// 画像が上下反転するようにコピーします。
				for (auto dst_y = 0; dst_y < height; dst_y++)
				{
					// コピー元バッファのYを算出します。
					auto src_y = height - dst_y - 1;

					memcpy(dst + dst_y * stride, src + src_y * stride, stride);
				}
    
				bitmap->UnlockBits(&bitmap_data);

				// メモリハンドルのロックを解除します。
				::GlobalUnlock(handle);

				// メモリハンドルをクリップボードにセットします。
				// (メモリハンドルをセットしたあとは開放できません)
				auto result3 = ::SetClipboardData(CF_DIB, handle);
				MY_TRACE_HEX(result3);
			}

			return TRUE;
		}
	} action;
}
