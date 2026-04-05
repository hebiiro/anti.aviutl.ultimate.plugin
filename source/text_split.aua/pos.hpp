#pragma once

namespace apn::text_split
{
	//
	// このクラスはアイテムの位置パラメータを管理します。
	//
	struct pos_t
	{
		using value_t = double[2];

		inline static constexpr struct mode_t {
			inline static constexpr int32_t c_solo = 0;
			inline static constexpr int32_t c_pair = 1;
			inline static constexpr int32_t c_animation = 2;
		} c_mode;

		inline static constexpr int32_t c_begin = 0;
		inline static constexpr int32_t c_end = 1;

		int32_t mode = {};
		value_t params = {};
		std::string animation;

		//
		// パラメータの数を返します。
		//
		size_t get_nb_params() const { return (mode == c_mode.c_solo) ? 1 : std::size(params); }

		//
		// 文字列に変換して返します。
		//
		std::string to_string(const value_t value) const
		{
			switch (mode) {
			case c_mode.c_solo: return my::format("{/.1f}", value[0]);
			case c_mode.c_pair: return my::format("{/.1f},{/.1f}", value[0], value[1]);
			case c_mode.c_animation: return my::format("{/.1f},{/.1f},{/}", value[0], value[1], animation);
			}

			return {};
		}

		//
		// 文字列から値を取得します。
		//
		BOOL from_string(const std::string& value)
		{
			constexpr auto separator = ',';

			auto sep1 = value.find(separator);

			// valueが数字のみの場合は
			if (sep1 == value.npos)
			{
				mode = c_mode.c_solo;
				params[c_begin] = stof(value);
				return TRUE;
			}

			auto sep2 = value.find(separator, sep1 + 1);

			// valueが数字2個のみの場合は
			if (sep2 == value.npos)
			{
				mode = c_mode.c_pair;
				params[c_begin] = stof(value);
				params[c_end] = stof(value.substr(sep1 + 1));
				return TRUE;
			}

			// valueが数字2個とアニメーションテキストの場合は
			{
				mode = c_mode.c_animation;
				params[c_begin] = stof(value);
				params[c_end] = stof(value.substr(sep1 + 1, sep2 - (sep1 + 1)));
				animation = value.substr(sep2 + 1);
				return TRUE;
			}
		}
	};
}
