#pragma once

namespace apn::volume_meter
{
	//
	// このクラスは音量メーターの描画設定です。
	//
	struct paint_option_t
	{
		//
		// 背景の色です。
		//
		COLORREF background_color;

		//
		// 警告用の色です。
		//
		COLORREF warning_color;

		//
		// レベルの色です。
		//
		COLORREF level_color;

		//
		// レベルの幅です。
		//
		int32_t level_width;

		//
		// 下限dBです。
		//
		int32_t min_db;

		//
		// 上限dBです。
		//
		int32_t max_db;

		//
		// 縦軸を補正するガンマです。
		// 実際には100で除算されてから使用されます。
		//
		int32_t gamma;

		//
		// セパレータの数です。
		//
		int32_t nb_separators;

		//
		// セパレータの幅です。
		//
		int32_t separator_width;

		//
		// 文字の描画エリアの高さです。
		//
		int32_t text_area_height;

		//
		// 各ゾーンのインデックスです。
		//
		inline static constexpr struct zone_e {
			inline static constexpr size_t c_red = 0;
			inline static constexpr size_t c_yellow = 1;
			inline static constexpr size_t c_green = 2;
			inline static constexpr size_t c_max_size = 3;
			inline static constexpr LPCSTR c_labels[c_max_size] = {
				"red_zone", "yellow_zone", "green_zone",
			};
		} c_zone;

		//
		// 各ゾーンの設定です。
		//
		struct zone_t {
			int32_t db;
			COLORREF color;
		} zones[c_zone.c_max_size];

		//
		// 各フォントのインデックスです。
		//
		inline static constexpr struct font_e {
			inline static constexpr size_t c_total = 0;
			inline static constexpr size_t c_zone = 1;
			inline static constexpr size_t c_peak = 2;
			inline static constexpr size_t c_level = 3;
			inline static constexpr size_t c_max_size = 4;
			inline static constexpr LPCSTR c_labels[c_max_size] = {
				"total", "zone", "peak", "level",
			};
		} c_font;

		//
		// 各フォントの設定です。
		//
		struct font_t {
			std::wstring name;
			int32_t width;
			int32_t height;
			COLORREF text_color;
			COLORREF shadow_color;
			POINT text_offset;
			POINT shadow_offset;
			BOOL vertical;
		} fonts[c_font.c_max_size];
	};
}
