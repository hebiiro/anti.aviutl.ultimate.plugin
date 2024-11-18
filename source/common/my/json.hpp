#pragma once

namespace my::json
{
	using n_json = nlohmann::json;
}

//
// get
//
namespace my::json
{
	inline n_json get_child(const n_json& node, const std::string& name)
	{
		try { return node.at(name); } catch (...) { return {}; }
	}

	inline void get_child_node(const n_json& node, const std::string& name, n_json& child)
	{
		try { child = node.at(name); } catch (...) {}
	}

	inline void get_child_nodes(const n_json& node, const std::string& name, auto func)
	{
		for (auto child_node : get_child(node, name))
			if (!func(child_node)) break;
	}

	inline void get_string(const n_json& node, std::wstring& value)
	{
		try { value = my::cp_to_wide(node.get<std::string>(), CP_UTF8); } catch (...) {}
	}

	inline void get_string(const n_json& node, const std::string& name, std::wstring& value)
	{
		get_string(get_child(node, name), value);
	}

	inline void get_file_name(const n_json& node, std::filesystem::path& path)
	{
		try { path = my::cp_to_wide(node.get<std::string>(), CP_UTF8); } catch (...) {}
	}

	inline void get_file_name(const n_json& node, const std::string& name, std::filesystem::path& path)
	{
		get_file_name(get_child(node, name), path);
	}

	template <typename T>
	inline void get_bool(const n_json& node, T& value)
	{
		try
		{
			if (node.is_string())
				value = node.get<std::string>() == "true";
			else
				value = node.get<bool>();
		}
		catch (...)
		{
		}
	}

	template <typename T>
	inline void get_bool(const n_json& node, const std::string& name, T& value)
	{
		get_bool(get_child(node, name), value);
	}

	template <typename T>
	inline void get_int(const n_json& node, T& value)
	{
		try
		{
			if (node.is_string())
				value = (T)strtol(node.get<std::string>().c_str(), nullptr, 0);
			else
				value = node.get<T>();
		}
		catch (...)
		{
		}
	}

	template <typename T>
	inline void get_int(const n_json& node, const std::string& name, T& value)
	{
		get_int(get_child(node, name), value);
	}

	template <typename T>
	inline void get_float(const n_json& node, T& value)
	{
		try
		{
			if (node.is_string())
				value = (T)strtod(node.get<std::string>().c_str(), nullptr);
			else
				value = node.get<T>();
		}
		catch (...)
		{
		}
	}

	template <typename T>
	inline void get_float(const n_json& node, const std::string& name, T& value)
	{
		get_float(get_child(node, name), value);
	}

	inline void get_point(const n_json& node, POINT& point)
	{
		get_int(node, "x", point.x);
		get_int(node, "y", point.y);
	}

	inline void get_point(const n_json& node, const std::string& name, POINT& point)
	{
		get_point(get_child(node, name), point);
	}

	inline void get_size(const n_json& node, SIZE& size)
	{
		get_int(node, "w", size.cx);
		get_int(node, "h", size.cy);
	}

	inline void get_size(const n_json& node, const std::string& name, SIZE& size)
	{
		get_size(get_child(node, name), size);
	}

	inline void get_rect(const n_json& node, RECT& rc)
	{
		get_int(node, "left", rc.left);
		get_int(node, "top", rc.top);
		get_int(node, "right", rc.right);
		get_int(node, "bottom", rc.bottom);
	}

	inline void get_rect(const n_json& node, const std::string& name, RECT& rc)
	{
		get_rect(get_child(node, name), rc);
	}

	inline void get_color(const n_json& node, COLORREF& value)
	{
		try
		{
			auto str = node.get<std::string>();
			auto c = str.length();
			if (c == 0) return;

			if (str[0] == '#')
			{
				// 先頭の'#'を取り除いた残りの部分を16進数に変換します。
				auto temp = strtoul(&str[1], nullptr, 16);

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
				auto sep1 = str.find(',');
				if (sep1 == str.npos) return;

				auto sep2 = str.find(',', sep1 + 1);
				if (sep2 == str.npos) return;

				str[sep1] = '\0';
				str[sep2] = '\0';

				auto r = strtoul(&str[0], nullptr, 0);
				auto g = strtoul(&str[sep1 + 1], nullptr, 0);
				auto b = strtoul(&str[sep2 + 1], nullptr, 0);

				value = RGB(r, g, b);
			}
		}
		catch (...)
		{
		}
	}

	inline void get_color(const n_json& node, const std::string& name, COLORREF& value)
	{
		get_color(get_child(node, name), value);
	}

	inline void get_window(const n_json& node, HWND hwnd, UINT flags = 0, UINT show_cmd = -1)
	{
		if (show_cmd == -1)
			get_int(node, "show_cmd", show_cmd);

		auto rc = RECT {};
		get_int(node, "left", rc.left);
		get_int(node, "top", rc.top);
		get_int(node, "right", rc.right);
		get_int(node, "bottom", rc.bottom);

		auto x = rc.left;
		auto y = rc.top;
		auto w = my::get_width(rc);
		auto h = my::get_height(rc);
		::SetWindowPos(hwnd, nullptr, x, y, w, h,
			flags | SWP_NOZORDER | SWP_NOACTIVATE);

		if (show_cmd != -1)
			::ShowWindow(hwnd, show_cmd);
	}

	inline void get_window(const n_json& node, const std::string& name, HWND hwnd, UINT flags = 0, UINT show_cmd = -1)
	{
		get_window(get_child(node, name), hwnd, flags, show_cmd);
	}

	template <typename T, typename Labels>
	inline void get_label(const n_json& node, T& value, const Labels& labels)
	{
		std::wstring str; get_string(node, str);

		for (const auto& label : labels)
		{
			if (str == label.text)
			{
				value = label.value;
				return;
			}
		}
	}

	template <typename T, typename Labels>
	inline void get_label(const n_json& node, const std::string& name, T& value, const Labels& labels)
	{
		get_label(get_child(node, name), value, labels);
	}

	template <typename ShortcutKey>
	inline void get_shortcut_key(const n_json& node, ShortcutKey& shortcut_key)
	{
		std::wstring key; get_string(node, "key", key);
		if (!key.empty()) shortcut_key.key = shortcut_key.str_to_key(key);

		std::wstring mod; get_string(node, "mod", mod);
		if (!mod.empty()) shortcut_key.mod = shortcut_key.str_to_mod(mod);
	}

	template <typename ShortcutKey>
	inline void get_shortcut_key(const n_json& node, const std::string& name, ShortcutKey& shortcut_key)
	{
		get_shortcut_key(get_child(node, name), shortcut_key);
	}
}

//
// set
//
namespace my::json
{
	inline void set_child(n_json& node, const std::string& name, const n_json& child)
	{
		node[name] = child;
	}

	inline void set_child_node(n_json& node, const std::string& name, const n_json& child)
	{
		node[name] = child;
	}

	inline void set_child_nodes(n_json& node, const std::string& name, const auto& collection, auto func)
	{
		for (const auto& value : collection)
		{
			n_json child_node;
			if (!func(child_node, value)) break;
			node[name].emplace_back(child_node);
		}
	}

	inline void set_string(n_json& node, const std::wstring& value)
	{
		node = my::wide_to_cp(value, CP_UTF8);
	}

	inline void set_string(n_json& node, const std::string& name, const std::wstring& value)
	{
		return set_string(node[name], value);
	}

	inline void set_file_name(n_json& node, const std::filesystem::path& path)
	{
		return set_string(node, path);
	}

	inline void set_file_name(n_json& node, const std::string& name, const std::filesystem::path& path)
	{
		return set_string(node, name, path);
	}

	template <typename T>
	inline void set_bool(n_json& node, T value)
	{
		node = !!value;
	}

	template <typename T>
	inline void set_bool(n_json& node, const std::string& name, T value)
	{
		return set_bool(node[name], value);
	}

	template <typename T>
	inline void set_int(n_json& node, T value)
	{
		node = value;
	}

	template <typename T>
	inline void set_int(n_json& node, const std::string& name, T value)
	{
		return set_int(node[name], value);
	}

	template <typename T>
	inline void set_float(n_json& node, T value)
	{
		node = value;
	}

	template <typename T>
	inline void set_float(n_json& node, const std::string& name, T value)
	{
		return set_float(node[name], value);
	}

	inline void set_point(n_json& node, const POINT& point)
	{
		node["x"] = point.x;
		node["y"] = point.y;
	}

	inline void set_point(n_json& node, const std::string& name, const POINT& point)
	{
		return set_point(node[name], point);
	}

	inline void set_size(n_json& node, const SIZE& size)
	{
		node["w"] = size.cx;
		node["h"] = size.cy;
	}

	inline void set_size(n_json& node, const std::string& name, const SIZE& size)
	{
		return set_size(node[name], size);
	}

	inline void set_rect(n_json& node, const RECT& rc)
	{
		node["left"] = rc.left;
		node["top"] = rc.top;
		node["right"] = rc.right;
		node["bottom"] = rc.bottom;
	}

	inline void set_rect(n_json& node, const std::string& name, const RECT& rc)
	{
		return set_rect(node[name], rc);
	}

	inline void set_color(n_json& node, COLORREF color)
	{
		if (color == CLR_NONE)
			node = "none";
		else
			node = std::format("{}, {}, {}", GetRValue(color), GetGValue(color), GetBValue(color));
	}

	inline void set_color(n_json& node, const std::string& name, COLORREF color)
	{
		return set_color(node[name], color);
	}

	inline void set_window(n_json& node, HWND hwnd)
	{
		auto show_cmd = SW_RESTORE;
		if (::IsZoomed(hwnd)) show_cmd = SW_MAXIMIZE;
		if (::IsIconic(hwnd)) show_cmd = SW_MINIMIZE;
		if (!::IsWindowVisible(hwnd)) show_cmd = SW_HIDE;

		auto rc = my::get_window_rect(hwnd);
		if (my::get_style(hwnd) & WS_CHILD)
			my::map_window_points(nullptr, ::GetParent(hwnd), &rc);

		set_int(node, "show_cmd", show_cmd);
		set_int(node, "left", rc.left);
		set_int(node, "top", rc.top);
		set_int(node, "right", rc.right);
		set_int(node, "bottom", rc.bottom);
	}

	inline void set_window(n_json& node, const std::string& name, HWND hwnd)
	{
		return set_window(node[name], hwnd);
	}

	template <typename T, typename Labels>
	inline void set_label(n_json& node, const T& value, const Labels& labels)
	{
		for (const auto& label : labels)
		{
			if (label.value == value)
				set_string(node, label.text);
		}
	}

	template <typename T, typename Labels>
	inline void set_label(n_json& node, const std::string& name, const T& value, const Labels& labels)
	{
		return set_label(node[name], value, labels);
	}

	template <typename ShortcutKey>
	inline void set_shortcut_key(n_json& node, const ShortcutKey& shortcut_key)
	{
		set_string(node, "key", shortcut_key.key_to_str(shortcut_key.key));
		set_string(node, "mod", shortcut_key.mod_to_str(shortcut_key.mod));
	}

	template <typename ShortcutKey>
	inline void set_shortcut_key(n_json& node, const std::string& name, const ShortcutKey& shortcut_key)
	{
		return set_shortcut_key(node[name], shortcut_key);
	}
}

#ifdef _GDIPLUS_H
//
// get
//
namespace my::json
{
	inline void get_point(const n_json& node, Point& point)
	{
		get_int(node, "x", point.X);
		get_int(node, "y", point.Y);
	}

	inline void get_point(const n_json& node, const std::string& name, Point& point)
	{
		get_point(get_child(node, name), point);
	}

	inline void get_size(const n_json& node, Size& size)
	{
		get_int(node, "w", size.Width);
		get_int(node, "h", size.Height);
	}

	inline void get_size(const n_json& node, const std::string& name, Size& size)
	{
		get_size(get_child(node, name), size);
	}

	inline void get_color(const n_json& node, Color& value)
	{
		try
		{
			auto str = node.get<std::string>();
			auto c = str.length();
			if (c == 0) return;

			if (str[0] == '#')
			{
				// 先頭の'#'を取り除いた残りの部分を16進数に変換します。
				auto temp = strtoul(&str[1], nullptr, 16);

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

					auto src = &str[0];
					auto end_ptr = src;
					auto rgba = strtoul(src, &end_ptr, 16);
					if (end_ptr != src)
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

					auto r = (BYTE)strtoul(&str[0], nullptr, 0);
					auto g = (BYTE)strtoul(&str[sep1 + 1], nullptr, 0);
					auto b = (BYTE)strtoul(&str[sep2 + 1], nullptr, 0);

					value.SetFromCOLORREF(RGB(r, g, b));

					return;
				}

				// `r, g, b, a`の形式です。

				str[sep1] = '\0';
				str[sep2] = '\0';
				str[sep3] = '\0';

				auto r = (BYTE)strtoul(&str[0], nullptr, 0);
				auto g = (BYTE)strtoul(&str[sep1 + 1], nullptr, 0);
				auto b = (BYTE)strtoul(&str[sep2 + 1], nullptr, 0);
				auto a = (BYTE)strtoul(&str[sep3 + 1], nullptr, 0);

				value.SetValue(value.MakeARGB(a, r, g, b));
			}
		}
		catch (...)
		{
		}
	}

	inline void get_color(const n_json& node, const std::string& name, Color& value)
	{
		get_color(get_child(node, name), value);
	}
}

//
// set
//
namespace my::json
{
	inline void set_point(n_json& node, const Point& point)
	{
		node["x"] = point.X;
		node["y"] = point.Y;
	}

	inline void set_point(n_json& node, const std::string& name, const Point& point)
	{
		return set_point(node[name], point);
	}

	inline void set_size(n_json& node, const Size& size)
	{
		node["w"] = size.Width;
		node["h"] = size.Height;
	}

	inline void set_size(n_json& node, const std::string& name, const Size& size)
	{
		return set_size(node[name], size);
	}

	inline void set_color(n_json& node, const Color& color)
	{
		node = std::format("{}, {}, {}, {}", color.GetR(), color.GetG(), color.GetB(), color.GetA());
	}

	inline void set_color(n_json& node, const std::string& name, const Color& color)
	{
		return set_color(node[name], color);
	}
}
#endif
