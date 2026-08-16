#pragma once

namespace apn::dark::kuro::paint
{
	//
	// 指定されたD2Dポイントを加算して返します。
	//
	inline D2D1_POINT_2F operator+(const D2D1_POINT_2F& lhs, const D2D1_POINT_2F& rhs)
	{
		return { lhs.x + rhs.x, lhs.y + rhs.y };
	}

	//
	// 指定されたD2Dポイントを減算して返します。
	//
	inline D2D1_POINT_2F operator-(const D2D1_POINT_2F& lhs, const D2D1_POINT_2F& rhs)
	{
		return { lhs.x - rhs.x, lhs.y - rhs.y };
	}

	//
	// 指定されたD2Dポイントを乗算して返します。
	//
	inline D2D1_POINT_2F operator*(const D2D1_POINT_2F& lhs, float rhs)
	{
		return { lhs.x * rhs, lhs.y * rhs };
	}

	//
	// 指定されたD2Dポイントを除算して返します。
	//
	inline D2D1_POINT_2F operator/(const D2D1_POINT_2F& lhs, float rhs)
	{
		return { lhs.x / rhs, lhs.y / rhs };
	}

	//
	// 指定されたD2Dポイントの内積を返します。
	//
	inline float dot_product(const D2D1_POINT_2F& lhs, const D2D1_POINT_2F& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y;
	}

	//
	// 指定されたD2Dポイントの外積を返します。
	//
	inline float cross_product(const D2D1_POINT_2F& lhs, const D2D1_POINT_2F& rhs)
	{
		return lhs.x * rhs.y - lhs.y * rhs.x;
	}

	//
	// 指定されたD2Dポイントの長さを返します。
	//
	inline float get_length(const D2D1_POINT_2F& lhs)
	{
		return std::sqrtf(lhs.x * lhs.x + lhs.y * lhs.y);
	}

	//
	// 指定されたD2Dポイントを正規化して返します。
	//
	inline D2D1_POINT_2F normalize(const D2D1_POINT_2F& lhs)
	{
		return lhs / get_length(lhs);
	}

	//
	// 矩形を収縮して返します。
	//
	inline D2D1_RECT_F deflate(const D2D1_RECT_F& rc, float x, float y)
	{
		D2D1_RECT_F out_rc = rc;
		out_rc.left += x;
		out_rc.top += y;
		out_rc.right -= x;
		out_rc.bottom -= y;
		return out_rc;
	}

	//
	// 矩形を収縮して返します。
	//
	inline D2D1_ROUNDED_RECT deflate(const D2D1_ROUNDED_RECT& rc, float x, float y)
	{
		D2D1_ROUNDED_RECT out_rc = rc;
		out_rc.rect.left += x;
		out_rc.rect.top += y;
		out_rc.rect.right -= x;
		out_rc.rect.bottom -= y;
		return out_rc;
	}

	//
	// 配色をD2D形式に変換して返します。
	//
	inline auto to_d2d_color(const color_t& color)
	{
		return D2D1::ColorF(color.rgba.value >> 8, color.alpha());
	}

	//
	// 配色をD2D形式に変換して返します。
	//
	inline auto to_d2d_color(const color_t& color, float alpha)
	{
		return D2D1::ColorF(color.rgba.value >> 8, alpha);
	}

	//
	// 背景の終了色のアルファを実数で返します。
	//
	inline float get_background_end_alpha(float base_alpha)
	{
		if (hive.gradient.flag_end_alpha)
			return base_alpha * get_gradient_end_alpha();
		else
			return base_alpha;
	}

	//
	// 背景の終了色をD2D形式で返します。
	//
	inline auto get_background_end_color(const color_entry_t& color_entry)
	{
		// 終了色が有効の場合は
		if (color_entry.parts[1].is_valid())
		{
			// 終了色を返します。
			return to_d2d_color(color_entry.parts[1]);
		}

		// 開始色を取得します。
		const auto& start_color = color_entry.parts[0];

		// 終了色アルファを取得します。
		auto end_alpha = get_background_end_alpha(start_color.alpha());

		// 終了色アルファを持つ開始色を返します。
		return to_d2d_color(start_color, end_alpha);
	}

	//
	// 疑似3Dエッジのカラーエントリを返します。
	//
	inline const color_entry_t* get_3d_edge_color_entry()
	{
		if (hive.border.flag_3d_edge)
			return custom_style.get_color_entry(L"3DEdge", L"Raised");
		else
			return {};
	}

	//
	// 基本色に補助色をブレンドしてD2D形式で返します。
	//
	inline auto blend(const color_t& base_color, const color_t& sub_color)
	{
		if (!sub_color.is_valid()) return to_d2d_color(base_color);

		auto t = sub_color.alpha();
		auto s = 1.0f - t;

		auto r = (base_color.red() * s) + (sub_color.red() * t);
		auto g = (base_color.green() * s) + (sub_color.green() * t);
		auto b = (base_color.blue() * s) + (sub_color.blue() * t);
		auto a = base_color.alpha();

		return D2D1::ColorF(r, g, b, a);
	}

	//
	// 基本色に補助色をブレンドしてD2D形式で返します。
	//
	inline auto blend(const color_t& base_color, const color_entry_t* color_entry, size_t color_index)
	{
		if (!color_entry) return to_d2d_color(base_color);

		return blend(base_color, color_entry->parts[color_index]);
	}
}
