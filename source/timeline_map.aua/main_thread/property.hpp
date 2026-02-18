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

			std::wstring font_name;
			int32_t font_size;
			rgba_t color;
			rgba_t shadow_color;
			POINT padding;
			POINT shadow_offset;
			int32_t horz_align;
			int32_t vert_align;
		} text;

		struct layer_t {
			int32_t top_space;
			rgba_t odd_color;
			rgba_t even_color;
		} layer;

		struct item_t {
			int32_t round_size;
			int32_t round_mode;
			rgba_t stroke_color;
			int32_t stroke_width;

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
			} nodes[c_item.c_max_size];
		} item;

		struct midpt_t {
			rgba_t color;
			rgba_t line_color;
			int32_t line_width;
		} midpt;

		struct current_frame_t {
			rgba_t line_color;
			int32_t line_width;
		} current_frame;

		struct visible_area_t {
			int32_t round_size;
			int32_t round_mode;
			rgba_t color;
			rgba_t stroke_color;
			int32_t stroke_width;
		} visible_area;

		//
		// コンストラクタです。
		//
		property_t()
		{
			{
				text.font_name = L"Consolas";
				text.font_size = 0;
				text.color.set(255, 255, 255, 255); // base07 (白)
				text.shadow_color.set(40, 44, 52, 128); // base00 (黒)
				text.padding = { 200, 0 };
				text.shadow_offset = { 100, 100 };
				text.horz_align = text.c_horz_align.c_left;
				text.vert_align = text.c_vert_align.c_center;
			}

			{
				layer.top_space = 10;
//				layer.odd_color.set(79, 86, 102, 255); // base02
				layer.odd_color.set(84, 88, 98, 255); // base03
				layer.even_color.set(63, 68, 81, 255); // base01
			}

			{
				//
				// この関数はアイテムの配色を二色同時にセットします。
				//
				const auto make_pair_color = [](uint8_t r, uint8_t g, uint8_t b)
				{
					return property_t::item_t::node_t {
						.start_color = { .a = 255, .b = (uint8_t)(b / 2), .g = (uint8_t)(g / 2), .r = (uint8_t)(r / 2), },
						.end_color = { .a = 255, .b = b, .g = g, .r = r, },
					};
				};

				item.round_size = 20;
				item.round_mode = property.c_round_mode.c_relative;
//				item.stroke_color.set(230, 230, 230, 255); // base06 (白)
				item.stroke_color.set(40, 44, 52, 255); // base00 (黒)
				item.stroke_width = 100;

				item.nodes[item.c_item.c_image_media] = make_pair_color(97, 175, 239); // base0D (青)
				item.nodes[item.c_item.c_image_filter_effect] = make_pair_color(86, 182, 194); // base0C (青緑)
				item.nodes[item.c_item.c_image_filter] = make_pair_color(152, 195, 121); // base0B (緑)
				item.nodes[item.c_item.c_sound_media] = make_pair_color(224, 108, 117); // base08 (赤)
				item.nodes[item.c_item.c_sound_filter] = make_pair_color(229, 192, 123); // base0A (黄色)
				item.nodes[item.c_item.c_control] = make_pair_color(198, 120, 221); // base0E (紫)
			}

			{
				midpt.color.set(190, 80, 70, 255); // base0F (濃い赤)
				midpt.line_color.set(40, 44, 52, 240); // base00 (黒)
				midpt.line_width = 50;
			}

			{
				current_frame.line_color.set(190, 80, 70, 200); // base0F (濃い赤)
				current_frame.line_width = 100;
			}

			{
				visible_area.round_size = 500;
				visible_area.round_mode = property.c_round_mode.c_absolute;
				visible_area.color.set(255, 255, 255, 100); // base07 (純白)
				visible_area.stroke_color.set(255, 255, 255, 150); // base07 (純白)
				visible_area.stroke_width = 100;
			}
		}
	} property;
}
