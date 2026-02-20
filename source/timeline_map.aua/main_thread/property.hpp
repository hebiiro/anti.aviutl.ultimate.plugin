#pragma once

namespace apn::timeline_map::main_thread
{
#pragma pack(push)
#pragma pack(1)
	//
	// このクラスはRGBAを保持します。
	//
	union rgba_t {
		uint32_t value; // 0xrrggbbaa;
		uint8_t vec[4];
		struct { uint8_t a, b, g, r; };
		rgba_t() {}
		rgba_t(const rgba_t& rhs) : value(rhs.value) {}
		rgba_t(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
			set(r, g, b, a);
		}
		void set(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
			this->r = r, this->g = g, this->b = b, this->a = a;
		}
		operator D2D1::ColorF() const {
			return { r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
		}
	};
#pragma pack(pop)

	//
	// このクラスはメインスレッドのプロパティです。
	// ファイルに入出力されるような生のデータを管理します。
	//
	inline struct property_t
	{
		//
		// 角丸めモードです。
		//
		inline static constexpr struct round_mode_e {
			inline static constexpr int32_t c_none = 0;
			inline static constexpr int32_t c_absolute = 1;
			inline static constexpr int32_t c_relative = 2;
			inline static constexpr int32_t c_discrete = 3;
			inline static constexpr my::Label labels[] = {
				{ c_none, L"none" },
				{ c_absolute, L"absolute" },
				{ c_relative, L"relative" },
				{ c_discrete, L"discrete" },
			};
		} c_round_mode;

		struct text_t {
			//
			// テキストの水平方向アラインです。
			//
			inline static constexpr struct horz_align_e {
				inline static constexpr int32_t c_left = 0;
				inline static constexpr int32_t c_right = 1;
				inline static constexpr int32_t c_center = 2;
				inline static constexpr my::Label labels[] = {
					{ c_left, L"left" },
					{ c_right, L"right" },
					{ c_center, L"center" },
				};
			} c_horz_align;

			//
			// テキストの垂直方向アラインです。
			//
			inline static constexpr struct vert_align_e {
				inline static constexpr int32_t c_top = 0;
				inline static constexpr int32_t c_bottom = 1;
				inline static constexpr int32_t c_center = 2;
				inline static constexpr my::Label labels[] = {
					{ c_top, L"top" },
					{ c_bottom, L"bottom" },
					{ c_center, L"center" },
				};
			} c_vert_align;

			std::wstring font_name = L"Consolas";
			int32_t font_size = 0;
			rgba_t color = { 255, 255, 255, 255 }; // base07 (白)
			rgba_t shadow_color = { 40, 44, 52, 128 }; // base00 (黒)
			POINT padding = { 200, 0 };
			POINT shadow_offset = { 100, 100 };
			int32_t horz_align = c_horz_align.c_left;
			int32_t vert_align = c_vert_align.c_center;
			BOOL flag_draw_text = TRUE;
			BOOL flag_draw_shadow = TRUE;
			int32_t min_draw_width = 200;
		} text;

		struct layer_t {
			int32_t top_space = 10;
//			rgba_t odd_color = { 79, 86, 102, 255 }; // base02 (黒灰色)
			rgba_t odd_color = { 84, 88, 98, 255 }; // base03 (黒灰色)
			rgba_t even_color = { 63, 68, 81, 255 }; // base01 (黒灰色)
			rgba_t undisp_color = { 40, 44, 52, 150 }; // base00 (黒)
			rgba_t undisp_stroke_color = { 40, 44, 52, 200 }; // base00 (黒)
			int32_t undisp_stroke_width = 0;
			rgba_t locked_stroke_color = { 224, 108, 117, 255 }; // base08 (赤)
			int32_t locked_stroke_width = 200;
			rgba_t coordlink_stroke_color = { 152, 195, 121, 255 }; // base0B (緑)
			int32_t coordlink_stroke_width = 200;
			rgba_t clip_stroke_color = { 97, 175, 239, 255 }; // base0D (青)
			int32_t clip_stroke_width = 200;
		} layer;

		struct item_t {
			int32_t round_size = 20;
			int32_t round_mode = c_round_mode.c_relative;;
//			rgba_t stroke_color = { 230, 230, 230, 255 }; // base06 (白)
			rgba_t stroke_color = { 40, 44, 52, 255 }; // base00 (黒)
			int32_t stroke_width = 100;

			//
			// 各アイテムノードのインデックスです。
			//
			inline static constexpr struct item_e {
				inline static constexpr size_t c_image_media = 0;
				inline static constexpr size_t c_image_filter_effect = 1;
				inline static constexpr size_t c_image_filter = 2;
				inline static constexpr size_t c_sound_media = 3;
				inline static constexpr size_t c_sound_filter = 4;
				inline static constexpr size_t c_control = 5;
				inline static constexpr size_t c_max_size = 6;
				inline static constexpr LPCSTR labels[] = {
					"image_media",
					"image_filter_effect",
					"image_filter",
					"sound_media",
					"sound_filter",
					"control",
				};
			} c_item;

			struct node_t {
				rgba_t start_color, end_color;
			} nodes[c_item.c_max_size] = {
				make_pair_color(97, 175, 239), // base0D (青)
				make_pair_color(86, 182, 194), // base0C (青緑)
				make_pair_color(152, 195, 121), // base0B (緑)
				make_pair_color(224, 108, 117), // base08 (赤)
				make_pair_color(229, 192, 123), // base0A (黄色)
				make_pair_color(198, 120, 221), // base0E (紫)
			};

			//
			// アイテムの配色を二色同時にセットします。
			//
			inline static item_t::node_t make_pair_color(uint8_t r, uint8_t g, uint8_t b)
			{
				return {
					rgba_t(r / 2, g / 2, b / 2, 255),
					rgba_t(r, g, b, 255),
				};
			}
		} item;

		struct midpt_t {
			rgba_t color = { 190, 80, 70, 255 }; // base0F (濃い赤)
			rgba_t line_color = { 40, 44, 52, 240 }; // base00 (黒)
			int32_t line_width = 50;
		} midpt;

		struct current_frame_t {
			rgba_t line_color = { 190, 80, 70, 200 }; // base0F (濃い赤)
			int32_t line_width = 100;
		} current_frame;

		struct visible_area_t {
			int32_t round_size = 500;
			int32_t round_mode = c_round_mode.c_absolute;
			rgba_t color = { 255, 255, 255, 100 }; // base07 (純白)
			rgba_t stroke_color = { 255, 255, 255, 150 }; // base07 (純白)
			int32_t stroke_width = 100;
		} visible_area;

		struct control_range_t {
			rgba_t color = { 198, 120, 221, 50 }; // base0E (紫)
			rgba_t stroke_color = { 40, 44, 52, 150 }; // base00 (黒)
			int32_t stroke_width = 100;
		} control_range;
	} property;
}
