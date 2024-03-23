#pragma once

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

		template <typename T>
		inline void get_int(const ptree& node, T& value)
		{
			value = node.get_value<T>(value);
		}

		template <typename T>
		inline void get_bool(const ptree& node, T& value)
		{
			value = node.get_value<bool>(!!value);
		}

		inline void get_string(const ptree& node, std::wstring& value)
		{
			value = my::cp_to_wide(node.get_value<std::string>(my::wide_to_cp(value, CP_UTF8)), CP_UTF8);
		}

		inline void get_window(const ptree& node, HWND hwnd, DWORD cmdShow = -1)
		{
			WINDOWPLACEMENT wp = { sizeof(wp) };
			if (!::GetWindowPlacement(hwnd, &wp)) return;
			if (!::IsWindowVisible(hwnd)) wp.showCmd = SW_HIDE;
			wp.flags = WPF_ASYNCWINDOWPLACEMENT | WPF_SETMINPOSITION;

			if (cmdShow == -1)
				get_int(node.get_child("flags"), wp.showCmd);
			else
				wp.showCmd = cmdShow;

			get_int(node.get_child("left"), wp.rcNormalPosition.left);
			get_int(node.get_child("top"), wp.rcNormalPosition.top);
			get_int(node.get_child("right"), wp.rcNormalPosition.right);
			get_int(node.get_child("bottom"), wp.rcNormalPosition.bottom);

			get_int(node.get_child("min_x"), wp.ptMinPosition.x);
			get_int(node.get_child("min_y"), wp.ptMinPosition.y);
			get_int(node.get_child("max_x"), wp.ptMaxPosition.x);
			get_int(node.get_child("max_y"), wp.ptMaxPosition.y);

			::SetWindowPlacement(hwnd, &wp);
		}

		inline constexpr std::string from_bool(bool value)
		{
			return value ? "true"s : "false"s;
		}

		inline constexpr std::string from_string(const std::string& str)
		{
			return R"(")"s + escape(str) + R"(")"s;
		}

		inline constexpr std::string from_string(const std::wstring& str)
		{
			return from_string(my::wide_to_cp(str, CP_UTF8));
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
				R"( "flags": {},)"
				R"( "left": {}, "top": {}, "right": {}, "bottom": {},)"
				R"( "min_x": {}, "min_y": {}, "max_x": {}, "max_y": {} )"
				R"(}})",
				wp.showCmd,
				wp.rcNormalPosition.left, wp.rcNormalPosition.top, wp.rcNormalPosition.right, wp.rcNormalPosition.bottom, 
				wp.ptMinPosition.x, wp.ptMinPosition.y, wp.ptMaxPosition.x, wp.ptMaxPosition.y);
		}
	}
}
