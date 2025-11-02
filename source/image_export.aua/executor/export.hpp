#pragma once

namespace apn::image_export::executor
{
	//
	// このクラスは画像ファイルをエクスポートします。
	//
	struct export_t : core_t
	{
		//
		// MSDNのコードです。
		//
		inline static int get_encoder_clsid(const WCHAR* format, CLSID* pClsid)
		{
			UINT  num = 0;          // number of image encoders
			UINT  size = 0;         // size of the image encoder array in bytes

			ImageCodecInfo* pImageCodecInfo = NULL;

			GetImageEncodersSize(&num, &size);
			if (size == 0)
				return -1;  // Failure

			pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
			if (pImageCodecInfo == NULL)
				return -1;  // Failure

			GetImageEncoders(num, size, pImageCodecInfo);

			for (UINT j = 0; j < num; ++j)
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
		// 拡張子に対応するエンコーダを返します。
		//
		inline static CLSID get_encoder(const std::wstring& extension)
		{
			auto encoder = GUID_NULL;
			if (extension == L".bmp") get_encoder_clsid(L"image/bmp", &encoder);
			else if (extension == L".jpg") get_encoder_clsid(L"image/jpeg", &encoder);
			else if (extension == L".gif") get_encoder_clsid(L"image/gif", &encoder);
			else if (extension == L".tif") get_encoder_clsid(L"image/tiff", &encoder);
			else if (extension == L".png") get_encoder_clsid(L"image/png", &encoder);
			return encoder;
		}

		//
		// 画像ファイルをエクスポートします。
		//
		BOOL export_image(const std::wstring& file_name, BOOL has_alpha, BOOL sel_item_only)
		{
			MY_TRACE_FUNC("{/}, {/}, {/}", file_name, has_alpha, sel_item_only);

			// 出力先パスを取得します。
			auto path = std::filesystem::path(file_name);

			// 出力先パスが無効の場合は何もしません。
			if (path.empty()) return FALSE;

			// 出力先パスのフォルダを作成します。
			std::filesystem::create_directories(path.parent_path());

			// 出力拡張子を取得します。
			auto extension = my::to_lower(path.extension());
			MY_TRACE_STR(extension);

			// 出力拡張子からエンコーダを取得します。
			auto encoder = get_encoder(extension);

			// エンコーダを取得できなかった場合は
			if (encoder == GUID_NULL)
			{
				// メッセージボックスを表示します。
				hive.message_box(L"拡張子が無効です");

				return FALSE;
			}

			try
			{
				// 画像データを作成します。
				create_image(has_alpha, sel_item_only);
			}
			// 例外が発生した場合は
			catch (LPCWSTR what)
			{
				// メッセージボックスを表示します。
				hive.message_box(what);

				return FALSE;
			}

			// エンコードパラメータを作成します。
			auto quality = (ULONG)hive.quality;
			auto encoder_parameters = EncoderParameters {};
			encoder_parameters.Count = 1;
			encoder_parameters.Parameter[0].Guid = EncoderQuality;
			encoder_parameters.Parameter[0].Type = EncoderParameterValueTypeLong;
			encoder_parameters.Parameter[0].NumberOfValues = 1;
			encoder_parameters.Parameter[0].Value = &quality;

			// ビットマップを出力先パスに保存します。
			auto status = bitmap->Save(file_name.c_str(), &encoder,
				(extension == L".jpg") ? &encoder_parameters : nullptr);
			MY_TRACE_HEX(status);

			// 保存に失敗した場合は
			if (status != S_OK)
			{
				// メッセージボックスを表示します。
				hive.message_box(L"ファイルの保存に失敗しました");

				return FALSE;
			}

			return TRUE;
		}
	};
}
