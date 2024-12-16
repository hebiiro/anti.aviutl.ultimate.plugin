#pragma once

namespace apn::settings_browser
{
	inline std::string u8(LPCSTR s)
	{
		return my::wide_to_cp(my::cp_to_wide(s, CP_ACP), CP_UTF8);
	}

	inline std::string u8(LPCWSTR s)
	{
		return my::wide_to_cp(s, CP_UTF8);
	}

	inline std::string u8(int32_t value, int32_t scale)
	{
		auto get_digit = [](int32_t scale) {
			int32_t digit = 0;
			while (scale) scale /= 10, digit++;
			return digit;
		};

		if (scale)
			return std::format("{:0.{}f}", (double)value / scale, get_digit(scale) - 1);
		else
			return std::format("{}", value);
	}

	inline std::string u8(const ExEdit::Exdata::ExdataColor& color)
	{
		return std::format("#{:02x}{:02x}{:02x}", color.r, color.g, color.b);
	}

	inline std::string u8(const ExEdit::Exdata::ExdataColorOpt& color)
	{
		return std::format("#{:02x}{:02x}{:02x}", color.r, color.g, color.b);
	}

	inline n_json to_json(const ExEdit::Exdata::ExdataColorYC& color)
	{
		return {
			{ "y", color.y },
			{ "cb", color.cb },
			{ "cr", color.cr },
			{ "padding", color.padding },
		};
	}

	inline n_json to_json(const ExEdit::Exdata::ExdataColorYCOpt& color)
	{
		return {
			{ "y", color.y },
			{ "cb", color.cb },
			{ "cr", color.cr },
			{ "status", color.status },
		};
	}
}
