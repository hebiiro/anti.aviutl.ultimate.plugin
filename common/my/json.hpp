﻿#pragma once

namespace my
{
	template <typename T>
	constexpr std::vector<T> split(const T& str, const T& separator)
	{
		std::vector<T> vec;
		size_t start = 0, end;
		while ((end = str.find(separator, start)) != str.npos) {
			vec.emplace_back(str.substr(start, end));
			start = end + separator.length();
		}
		vec.emplace_back(str.substr(start, end));
		return vec;
	}

	template <typename T>
	constexpr T join(const std::vector<T>& vec, const T& separator)
	{
		T str;
		for (const auto& value : vec) {
			str += value;
			if (value != vec.back()) str += separator;
		}
		return str;
	}

	//
	// str内のsrcをdstに置換します。
	//
	template <typename T>
	constexpr T replace(const T& str, const T& src, const T& dst)
	{
		return join(split(str, src), dst);
	}
/*
	//
	// str内のsrcをdstに置換します。
	//
	template <typename T>
	constexpr bool replace(T& str, const T& src, const T& dst)
	{
		size_t pos = str.find(src), len = src.length();
		if (pos != str.npos) return false;
		else return str.replace(pos, len, dst), true;
	}
*/
	namespace json
	{
		using namespace boost::property_tree;

		inline constexpr std::string with_eol(size_t indent, const std::string& str)
		{
			return std::string(indent, '\t') + str + "\n"s;
		}

		inline constexpr std::string escape(const std::string& str)
		{
			std::string result;

			size_t start = 0, stride = 0;
			for (size_t pos = 0; pos < str.size(); pos += stride)
			{
				auto lead = (char8_t)str[pos];

				if (lead < 0x80)
				{
					stride = 1;

					switch (lead)
					{
					case u8'/':
					case u8'\"':
					case u8'\b':
					case u8'\f':
					case u8'\n':
					case u8'\r':
					case u8'\t':
						{
							result += str.substr(start, pos) + '\\' + (char)lead;
							start = pos + stride;
							break;
						}
					}
				}
				else if (lead < 0xE0)
				{
					stride = 2;
				}
				else if (lead < 0xF0)
				{
					stride = 3;
				}
				else
				{
					stride = 4;
				}
			}

			result += str.substr(start);

			return result;
		}

		inline ptree get_child(const ptree& node, const std::string& name)
		{
			return node.get_child(name, ptree());
		}

		inline void get_string(const ptree& node, std::wstring& value)
		{
			value = my::cp_to_wide(node.get_value<std::string>(my::wide_to_cp(value, CP_UTF8)), CP_UTF8);
		}

		inline void get_string(const ptree& node, const std::string& name, std::wstring& value)
		{
			if (auto optional = node.get_child_optional(name)) get_string(optional.value(), value);
		}

		template <typename T>
		inline void get_bool(const ptree& node, T& value)
		{
			value = node.get_value<bool>(!!value);
		}

		template <typename T>
		inline void get_bool(const ptree& node, const std::string& name, T& value)
		{
			if (auto optional = node.get_child_optional(name)) get_bool(optional.value(), value);
		}

		template <typename T>
		inline void get_int(const ptree& node, T& value)
		{
			value = node.get_value<T>(value);
		}

		template <typename T>
		inline void get_int(const ptree& node, const std::string& name, T& value)
		{
			if (auto optional = node.get_child_optional(name)) get_int(optional.value(), value);
		}

		template <typename T>
		inline void get_float(const ptree& node, T& value)
		{
			value = node.get_value<T>(value);
		}

		template <typename T>
		inline void get_float(const ptree& node, const std::string& name, T& value)
		{
			if (auto optional = node.get_child_optional(name)) get_float(optional.value(), value);
		}

		inline void get_point(const ptree& node, POINT& value)
		{
			get_int(node, "x", value.x);
			get_int(node, "y", value.y);
		}

		inline void get_point(const ptree& node, const std::string& name, POINT& value)
		{
			if (auto optional = node.get_child_optional(name)) get_point(optional.value(), value);
		}

		inline void get_size(const ptree& node, SIZE& value)
		{
			get_int(node, "w", value.cx);
			get_int(node, "h", value.cy);
		}

		inline void get_size(const ptree& node, const std::string& name, SIZE& value)
		{
			if (auto optional = node.get_child_optional(name)) get_size(optional.value(), value);
		}

		inline void get_color(const ptree& node, COLORREF& value)
		{
			std::string str = node.get_value<std::string>("");
			size_t c = str.length();
			if (c == 0) return;

			if (str[0] == '#')
			{
				// 先頭の'#'を取り除いた残りの部分を16進数に変換します。
				auto temp = std::stoul(&str[1], 0, 16);

				if (c == 4)
				{
					// `#fc8`の形式です。

					auto r = (temp & 0x0F00) >> 8;
					auto g = (temp & 0x00F0) >> 4;
					auto b = (temp & 0x000F) >> 0;

					r |= r << 4;
					g |= g << 4;
					b |= b << 4;

					value = RGB(r, g, b);
				}
				else
				{
					// `#ffcc88`の形式です。

					auto r = (temp & 0x00FF0000) >> 16;
					auto g = (temp & 0x0000FF00) >> 8;
					auto b = (temp & 0x000000FF) >> 0;

					value = RGB(r, g, b);
				}
			}
			else
			{
				size_t sep1 = str.find(',');
				if (sep1 == str.npos) return;

				size_t sep2 = str.find(',', sep1 + 1);
				if (sep2 == str.npos) return;

				str[sep1] = '\0';
				str[sep2] = '\0';

				auto r = std::stoul(&str[0], 0, 0);
				auto g = std::stoul(&str[sep1 + 1], 0, 0);
				auto b = std::stoul(&str[sep2 + 1], 0, 0);

				value = RGB(r, g, b);
			}
		}

		inline void get_color(const ptree& node, const std::string& name, COLORREF& value)
		{
			if (auto optional = node.get_child_optional(name)) get_color(optional.value(), value);
		}

		inline void get_window(const ptree& node, HWND hwnd, DWORD show_cmd = -1)
		{
			WINDOWPLACEMENT wp = { sizeof(wp) };
			if (!::GetWindowPlacement(hwnd, &wp)) return;
			if (!::IsWindowVisible(hwnd)) wp.showCmd = SW_HIDE;
			wp.flags = WPF_ASYNCWINDOWPLACEMENT | WPF_SETMINPOSITION;

			if (show_cmd == -1)
				get_int(node, "show_cmd", wp.showCmd);
			else
				wp.showCmd = show_cmd;

			get_int(node, "left", wp.rcNormalPosition.left);
			get_int(node, "top", wp.rcNormalPosition.top);
			get_int(node, "right", wp.rcNormalPosition.right);
			get_int(node, "bottom", wp.rcNormalPosition.bottom);

			get_int(node, "min_x", wp.ptMinPosition.x);
			get_int(node, "min_y", wp.ptMinPosition.y);
			get_int(node, "max_x", wp.ptMaxPosition.x);
			get_int(node, "max_y", wp.ptMaxPosition.y);

			::SetWindowPlacement(hwnd, &wp);
		}

		inline void get_window(const ptree& node, const std::string& name, HWND hwnd, DWORD show_cmd = -1)
		{
			if (auto optional = node.get_child_optional(name)) get_window(optional.value(), hwnd, show_cmd);
		}

		inline constexpr std::string from_string(const std::string& str)
		{
			return R"(")"s + escape(str) + R"(")"s;
		}

		inline constexpr std::string from_string(const std::wstring& str)
		{
			return from_string(my::wide_to_cp(str, CP_UTF8));
		}

		template <typename T>
		inline constexpr std::string from_bool(T value)
		{
			return value ? "true"s : "false"s;
		}

		template <typename T>
		inline constexpr std::string from_int(T value)
		{
			return std::format(R"({})", value);
		}

		template <typename T>
		inline constexpr std::string from_float(T value)
		{
			return std::format(R"({})", value);
		}

		inline std::string from_point(const POINT& point)
		{
			return std::format(R"({{ "w": {}, "h": {} }})", point.x, point.y);
		}

		inline std::string from_size(const SIZE& size)
		{
			return std::format(R"({{ "w": {}, "h": {} }})", size.cx, size.cy);
		}

		inline std::string from_color(COLORREF color)
		{
			if (color == CLR_NONE)
				return std::format(R"("none")");
			else
				return std::format(R"("{}, {}, {}")", GetRValue(color), GetGValue(color), GetBValue(color));
		}

		inline std::string from_window(HWND hwnd)
		{
			WINDOWPLACEMENT wp = { sizeof(wp) };
			if (!::GetWindowPlacement(hwnd, &wp)) return "";

			if (::IsIconic(hwnd)) wp.showCmd = SW_SHOW;
			if (wp.flags == WPF_RESTORETOMAXIMIZED) wp.showCmd = SW_SHOWMAXIMIZED;
			if (!::IsWindowVisible(hwnd)) wp.showCmd = SW_HIDE;

			return std::format(
				R"({{)"
				R"( "show_cmd": {},)"
				R"( "left": {}, "top": {}, "right": {}, "bottom": {},)"
				R"( "min_x": {}, "min_y": {}, "max_x": {}, "max_y": {} )"
				R"(}})",
				wp.showCmd,
				wp.rcNormalPosition.left, wp.rcNormalPosition.top, wp.rcNormalPosition.right, wp.rcNormalPosition.bottom, 
				wp.ptMinPosition.x, wp.ptMinPosition.y, wp.ptMaxPosition.x, wp.ptMaxPosition.y);
		}

		inline void set_string(ptree& node, const std::string& name, const std::wstring& value)
		{
			node.put(name, my::wide_to_cp(value, CP_UTF8));
		}

		template <typename T>
		inline void set_bool(ptree& node, const std::string& name, T value)
		{
			node.put(name, !!value);
		}

		template <typename T>
		inline void set_int(ptree& node, const std::string& name, T value)
		{
			node.put(name, value);
		}

		template <typename T>
		inline void set_float(ptree& node, const std::string& name, T value)
		{
			node.put(name, value);
		}

		inline void set_point(ptree& node, const std::string& name, const POINT& point)
		{
			node.put(name + ".x", point.x);
			node.put(name + ".y", point.y);
		}

		inline void set_size(ptree& node, const std::string& name, const SIZE& size)
		{
			node.put(name + ".w", size.cx);
			node.put(name + ".h", size.cy);
		}

		inline void set_color(ptree& node, const std::string& name, COLORREF color)
		{
			if (color == CLR_NONE)
				node.put(name, "none");
			else
				node.put(name, std::format(R"({}, {}, {})", GetRValue(color), GetGValue(color), GetBValue(color)));
		}

		inline void set_window(ptree& node, const std::string& name, HWND hwnd)
		{
			WINDOWPLACEMENT wp = { sizeof(wp) };
			if (!::GetWindowPlacement(hwnd, &wp)) return;

			if (::IsIconic(hwnd)) wp.showCmd = SW_SHOW;
			if (wp.flags == WPF_RESTORETOMAXIMIZED) wp.showCmd = SW_SHOWMAXIMIZED;
			if (!::IsWindowVisible(hwnd)) wp.showCmd = SW_HIDE;

			ptree window_node;
			window_node.put("show_cmd", wp.showCmd);
			window_node.put("left", wp.rcNormalPosition.left);
			window_node.put("top", wp.rcNormalPosition.top);
			window_node.put("right", wp.rcNormalPosition.right);
			window_node.put("bottom", wp.rcNormalPosition.bottom);
			window_node.put("min_x", wp.ptMinPosition.x);
			window_node.put("min_y", wp.ptMinPosition.y);
			window_node.put("max_x", wp.ptMaxPosition.x);
			window_node.put("max_y", wp.ptMaxPosition.y);
			node.add_child(name, window_node);
		}
#ifdef _GDIPLUS_H
		inline void get_point(const ptree& node, Point& point)
		{
			get_int(node, "x", point.X);
			get_int(node, "y", point.Y);
		}

		inline void get_point(const ptree& node, const std::string& name, Point& point)
		{
			if (auto optional = node.get_child_optional(name)) get_point(optional.value(), point);
		}

		inline void get_size(const ptree& node, Size& size)
		{
			get_int(node, "w", size.Width);
			get_int(node, "h", size.Height);
		}

		inline void get_size(const ptree& node, const std::string& name, Size& size)
		{
			if (auto optional = node.get_child_optional(name)) get_size(optional.value(), size);
		}

		inline void get_color(const ptree& node, Color& value)
		{
			auto str = node.get_value<std::string>("");
			auto c = str.length();
			if (c == 0) return;

			if (str[0] == '#')
			{
				// 先頭の'#'を取り除いた残りの部分を16進数に変換します。
				auto temp = std::stoul(&str[1], 0, 16);

				if (c == 4)
				{
					// `#fc8`の形式です。

					auto r = (BYTE)(temp >> 8 & 0xF);
					auto g = (BYTE)(temp >> 4 & 0xF);
					auto b = (BYTE)(temp >> 0 & 0xF);

					r |= r << 4;
					g |= g << 4;
					b |= b << 4;

					value = RGB(r, g, b);
				}
				else
				{
					// `#ffcc88`の形式です。

					auto r = (BYTE)(temp >> 16);
					auto g = (BYTE)(temp >> 8);
					auto b = (BYTE)(temp >> 0);

					value = RGB(r, g, b);
				}
			}
			else
			{
				auto sep1 = str.find(',');
				if (sep1 == str.npos)
				{
					// `rrggbbaa'の16進数の形式だと仮定します。

					size_t index = 0;
					auto rgba = std::stoul(&str[0], &index, 16);
					if (index > 0)
					{
						auto r = (BYTE)(rgba >> 24);
						auto g = (BYTE)(rgba >> 16);
						auto b = (BYTE)(rgba >> 8);
						auto a = (BYTE)(rgba >> 0);

						value.SetValue(value.MakeARGB(a, r, g, b));
					}

					return;
				}

				auto sep2 = str.find(',', sep1 + 1);
				if (sep2 == str.npos) return;

				auto sep3 = str.find(',', sep2 + 1);
				if (sep3 == str.npos)
				{
					// `r, g, b`の形式です。

					str[sep1] = '\0';
					str[sep2] = '\0';

					auto r = (BYTE)std::stoul(&str[0], 0, 0);
					auto g = (BYTE)std::stoul(&str[sep1 + 1], 0, 0);
					auto b = (BYTE)std::stoul(&str[sep2 + 1], 0, 0);

					value.SetFromCOLORREF(RGB(r, g, b));

					return;
				}

				// `r, g, b, a`の形式です。

				str[sep1] = '\0';
				str[sep2] = '\0';
				str[sep3] = '\0';

				auto r = (BYTE)std::stoul(&str[0], 0, 0);
				auto g = (BYTE)std::stoul(&str[sep1 + 1], 0, 0);
				auto b = (BYTE)std::stoul(&str[sep2 + 1], 0, 0);
				auto a = (BYTE)std::stoul(&str[sep3 + 1], 0, 0);

				value.SetValue(value.MakeARGB(a, r, g, b));
			}
		}

		inline void get_color(const ptree& node, const std::string& name, Color& value)
		{
			if (auto optional = node.get_child_optional(name)) get_color(optional.value(), value);
		}

		inline std::string from_point(const Point& point)
		{
			return std::format(R"({{ "x": {}, "y": {} }})", point.X, point.Y);
		}

		inline std::string from_size(const Size& size)
		{
			return std::format(R"({{ "w": {}, "h": {} }})", size.Width, size.Height);
		}

		inline std::string from_color(const Color& color)
		{
			return std::format(R"("{}, {}, {}, {}")", color.GetR(), color.GetG(), color.GetB(), color.GetA());
		}

		inline void set_point(ptree& node, const std::string& name, const Point& point)
		{
			node.put(name + ".x", point.X);
			node.put(name + ".y", point.Y);
		}

		inline void set_size(ptree& node, const std::string& name, const Size& size)
		{
			node.put(name + ".w", size.Width);
			node.put(name + ".h", size.Height);
		}

		inline void set_color(ptree& node, const std::string& name, const Color& color)
		{
			node.put(name, std::format(R"({}, {}, {}, {})", color.GetR(), color.GetG(), color.GetB(), color.GetA()));
		}
#endif
	}
}
