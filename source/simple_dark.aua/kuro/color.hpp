#pragma once

namespace apn::dark::kuro
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
	};
#pragma pack(pop)

	//
	// このクラスは配色データを保持します。
	//
	struct color_t
	{
		rgba_t rgba;
		COLORREF win32;

		//
		// デフォルトコンストラクタです。
		//
		color_t()
			: rgba(0)
			, win32(CLR_NONE)
		{
		}

		//
		// コンストラクタです。
		//
		color_t(const rgba_t& rgba)
			: rgba(rgba)
			, win32(RGB(rgba.r, rgba.g, rgba.b))
		{
		}

		//
		// コンストラクタです。
		//
		color_t(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
			: rgba { .a = a, .b = b, .g = g, .r = r }
			, win32(RGB(r, g, b))
		{
		}

		//
		// コンストラクタです。
		//
		color_t(COLORREF win32_color)
			: rgba(0)
			, win32(win32_color)
		{
			if (win32 != CLR_NONE)
			{
				rgba.r = GetRValue(win32);
				rgba.g = GetGValue(win32);
				rgba.b = GetBValue(win32);
				rgba.a = 0xff;
			}
		}

		//
		// 配色データが有効の場合はTRUEを返します。
		//
		BOOL is_valid() const
		{
//			return !!rgba.a;
			return win32 != CLR_NONE;
		}

		//
		// 各配色成分を実数で返します。
		//
		float red() const { return rgba.r / 255.0f; }
		float green() const { return rgba.g / 255.0f; }
		float blue() const { return rgba.b / 255.0f; }
		float alpha() const { return rgba.a / 255.0f; }
	};

	//
	// このクラスはカラーエントリです。
	//
	struct color_entry_t
	{
		//
		// カラーパーツの最大数です。
		//
		inline static constexpr size_t c_max_size = 2;

		//
		// カラーパーツです。
		//
		color_t parts[c_max_size] = {};
	};
}
