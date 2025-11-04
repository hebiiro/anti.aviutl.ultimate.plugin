#pragma once

namespace my::json
{
	using n_json = nlohmann::basic_json<nlohmann::ordered_map>;
}

//
// get
//
namespace my::json
{
	inline n_json read_child(const n_json& node, const std::string& name)
	{
		try { return node.at(name); } catch (...) { return {}; }
	}

	inline void read_child_node(const n_json& node, const std::string& name, n_json& child)
	{
		try { child = node.at(name); } catch (...) {}
	}

	inline void read_child_nodes(const n_json& node, const std::string& name, auto func)
	{
		size_t index = 0;
		for (auto child_node : read_child(node, name))
			if (!func(child_node, index++)) break;
	}

	inline void read_string(const n_json& node, std::wstring& value)
	{
		try { value = my::cp_to_wide(node.get<std::string>(), CP_UTF8); } catch (...) {}
	}

	inline void read_string(const n_json& node, const std::string& name, std::wstring& value)
	{
		return read_string(read_child(node, name), value);
	}

	inline void read_file_name(const n_json& node, std::filesystem::path& path)
	{
		try { path = my::cp_to_wide(node.get<std::string>(), CP_UTF8); } catch (...) {}
	}

	inline void read_file_name(const n_json& node, const std::string& name, std::filesystem::path& path)
	{
		return read_file_name(read_child(node, name), path);
	}

	template <typename T>
	inline void read_bool(const n_json& node, T& value)
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
	inline void read_bool(const n_json& node, const std::string& name, T& value)
	{
		return read_bool(read_child(node, name), value);
	}

	template <typename T>
	inline void read_int(const n_json& node, T& value)
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
	inline void read_int(const n_json& node, const std::string& name, T& value)
	{
		return read_int(read_child(node, name), value);
	}

	template <typename T>
	inline void read_real(const n_json& node, T& value)
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
	inline void read_real(const n_json& node, const std::string& name, T& value)
	{
		return read_real(read_child(node, name), value);
	}

	inline void read_point(const n_json& node, POINT& point)
	{
		read_int(node, "x", point.x);
		read_int(node, "y", point.y);
	}

	inline void read_point(const n_json& node, const std::string& name, POINT& point)
	{
		return read_point(read_child(node, name), point);
	}

	inline void read_size(const n_json& node, SIZE& size)
	{
		read_int(node, "w", size.cx);
		read_int(node, "h", size.cy);
	}

	inline void read_size(const n_json& node, const std::string& name, SIZE& size)
	{
		return read_size(read_child(node, name), size);
	}

	inline void read_rect(const n_json& node, RECT& rc)
	{
		read_int(node, "left", rc.left);
		read_int(node, "top", rc.top);
		read_int(node, "right", rc.right);
		read_int(node, "bottom", rc.bottom);
	}

	inline void read_rect(const n_json& node, const std::string& name, RECT& rc)
	{
		return read_rect(read_child(node, name), rc);
	}

	inline void read_color(const n_json& node, COLORREF& value)
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

	inline void read_color(const n_json& node, const std::string& name, COLORREF& value)
	{
		return read_color(read_child(node, name), value);
	}

	//
	// ウィンドウ位置をノードから読み込みます。
	//
	inline void read_window_pos(const n_json& node, HWND hwnd, UINT flags = 0, UINT show_cmd = -1)
	{
		// ウィンドウプレースメントを取得します。
		WINDOWPLACEMENT wp = { sizeof(wp) };
		::GetWindowPlacement(hwnd, &wp);

		wp.flags = WPF_SETMINPOSITION;
		if (!::IsWindowVisible(hwnd)) wp.showCmd = SW_HIDE;

		if (show_cmd == -1)
			read_int(node, "show_cmd", wp.showCmd);
		else
			wp.showCmd = show_cmd;

		read_rect(node, "normal", wp.rcNormalPosition);
		read_point(node, "min", wp.ptMinPosition);
		read_point(node, "max", wp.ptMaxPosition);

		::SetWindowPlacement(hwnd, &wp);
	}

	//
	// ウィンドウ位置をノードから読み込みます。
	//
	inline void read_window_pos(const n_json& node, const std::string& name, HWND hwnd, UINT flags = 0, UINT show_cmd = -1)
	{
		return read_window_pos(read_child(node, name), hwnd, flags, show_cmd);
	}

	template <typename T, typename Labels>
	inline void read_label(const n_json& node, T& value, const Labels& labels)
	{
		std::wstring str; read_string(node, str);

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
	inline void read_label(const n_json& node, const std::string& name, T& value, const Labels& labels)
	{
		return read_label(read_child(node, name), value, labels);
	}

	template <typename ShortcutKey>
	inline void read_shortcut_key(const n_json& node, ShortcutKey& shortcut_key)
	{
		std::wstring key; read_string(node, "key", key);
		if (!key.empty()) shortcut_key.key = shortcut_key.str_to_key(key);

		std::wstring mod; read_string(node, "mod", mod);
		if (!mod.empty()) shortcut_key.mod = shortcut_key.str_to_mod(mod);
	}

	template <typename ShortcutKey>
	inline void read_shortcut_key(const n_json& node, const std::string& name, ShortcutKey& shortcut_key)
	{
		return read_shortcut_key(read_child(node, name), shortcut_key);
	}
}

//
// set
//
namespace my::json
{
	inline void write_child(n_json& node, const std::string& name, const n_json& child)
	{
		node[name] = child;
	}

	inline void write_child_node(n_json& node, const std::string& name, const n_json& child)
	{
		node[name] = child;
	}

	inline void write_child_nodes(n_json& node, const std::string& name, const auto& collection, auto func)
	{
		size_t index = 0;
		for (const auto& value : collection)
		{
			n_json child_node;
			if (!func(child_node, value, index++)) break;
			node[name].emplace_back(child_node);
		}
	}

	inline void write_string(n_json& node, const std::wstring& value)
	{
		node = my::wide_to_cp(value, CP_UTF8);
	}

	inline void write_string(n_json& node, const std::string& name, const std::wstring& value)
	{
		return write_string(node[name], value);
	}

	inline void write_file_name(n_json& node, const std::filesystem::path& path)
	{
		return write_string(node, path);
	}

	inline void write_file_name(n_json& node, const std::string& name, const std::filesystem::path& path)
	{
		return write_string(node, name, path);
	}

	template <typename T>
	inline void write_bool(n_json& node, T value)
	{
		node = !!value;
	}

	template <typename T>
	inline void write_bool(n_json& node, const std::string& name, T value)
	{
		return write_bool(node[name], value);
	}

	template <typename T>
	inline void write_int(n_json& node, T value)
	{
		node = value;
	}

	template <typename T>
	inline void write_int(n_json& node, const std::string& name, T value)
	{
		return write_int(node[name], value);
	}

	template <typename T>
	inline void write_real(n_json& node, T value)
	{
		node = value;
	}

	template <typename T>
	inline void write_real(n_json& node, const std::string& name, T value)
	{
		return write_real(node[name], value);
	}

	inline void write_point(n_json& node, const POINT& point)
	{
		node["x"] = point.x;
		node["y"] = point.y;
	}

	inline void write_point(n_json& node, const std::string& name, const POINT& point)
	{
		return write_point(node[name], point);
	}

	inline void write_size(n_json& node, const SIZE& size)
	{
		node["w"] = size.cx;
		node["h"] = size.cy;
	}

	inline void write_size(n_json& node, const std::string& name, const SIZE& size)
	{
		return write_size(node[name], size);
	}

	inline void write_rect(n_json& node, const RECT& rc)
	{
		node["left"] = rc.left;
		node["top"] = rc.top;
		node["right"] = rc.right;
		node["bottom"] = rc.bottom;
	}

	inline void write_rect(n_json& node, const std::string& name, const RECT& rc)
	{
		return write_rect(node[name], rc);
	}

	inline void write_color(n_json& node, COLORREF color)
	{
		if (color == CLR_NONE)
			node = "none";
		else
			node = my::format("{/}, {/}, {/}", GetRValue(color), GetGValue(color), GetBValue(color));
	}

	inline void write_color(n_json& node, const std::string& name, COLORREF color)
	{
		return write_color(node[name], color);
	}

	//
	// ウィンドウ位置をノードに書き込みます。
	//
	inline void write_window_pos(n_json& node, HWND hwnd, UINT show_cmd = -1)
	{
		// ウィンドウプレースメントを取得します。
		WINDOWPLACEMENT wp = { sizeof(wp) };
		::GetWindowPlacement(hwnd, &wp);

		auto is_visible = ::IsWindowVisible(hwnd);
		auto is_maximized = ::IsZoomed(hwnd);
		auto is_minimized = ::IsIconic(hwnd);
 
		if (is_minimized) wp.showCmd = SW_SHOW;
		if (wp.flags == WPF_RESTORETOMAXIMIZED) wp.showCmd = SW_SHOWMAXIMIZED;
		if (!is_visible) wp.showCmd = SW_HIDE;

		write_int(node, "show_cmd", (show_cmd == -1) ? wp.showCmd : show_cmd);
		write_rect(node, "normal", wp.rcNormalPosition);
		write_point(node, "min", wp.ptMinPosition);
		write_point(node, "max", wp.ptMaxPosition);
	}

	//
	// ウィンドウ位置をノードに書き込みます。
	//
	inline void write_window_pos(n_json& node, const std::string& name, HWND hwnd, UINT show_cmd = -1)
	{
		return write_window_pos(node[name], hwnd, show_cmd);
	}

	template <typename T, typename Labels>
	inline void write_label(n_json& node, const T& value, const Labels& labels)
	{
		for (const auto& label : labels)
		{
			if (label.value == value)
				write_string(node, label.text);
		}
	}

	template <typename T, typename Labels>
	inline void write_label(n_json& node, const std::string& name, const T& value, const Labels& labels)
	{
		return write_label(node[name], value, labels);
	}

	template <typename ShortcutKey>
	inline void write_shortcut_key(n_json& node, const ShortcutKey& shortcut_key)
	{
		write_string(node, "key", shortcut_key.key_to_str(shortcut_key.key));
		write_string(node, "mod", shortcut_key.mod_to_str(shortcut_key.mod));
	}

	template <typename ShortcutKey>
	inline void write_shortcut_key(n_json& node, const std::string& name, const ShortcutKey& shortcut_key)
	{
		return write_shortcut_key(node[name], shortcut_key);
	}
}

#ifdef _GDIPLUS_H
//
// get
//
namespace my::json
{
	inline void read_point(const n_json& node, Point& point)
	{
		read_int(node, "x", point.X);
		read_int(node, "y", point.Y);
	}

	inline void read_point(const n_json& node, const std::string& name, Point& point)
	{
		return read_point(read_child(node, name), point);
	}

	inline void read_size(const n_json& node, Size& size)
	{
		read_int(node, "w", size.Width);
		read_int(node, "h", size.Height);
	}

	inline void read_size(const n_json& node, const std::string& name, Size& size)
	{
		return read_size(read_child(node, name), size);
	}

	inline void read_color(const n_json& node, Color& value)
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

	inline void read_color(const n_json& node, const std::string& name, Color& value)
	{
		return read_color(read_child(node, name), value);
	}
}

//
// set
//
namespace my::json
{
	inline void write_point(n_json& node, const Point& point)
	{
		node["x"] = point.X;
		node["y"] = point.Y;
	}

	inline void write_point(n_json& node, const std::string& name, const Point& point)
	{
		return write_point(node[name], point);
	}

	inline void write_size(n_json& node, const Size& size)
	{
		node["w"] = size.Width;
		node["h"] = size.Height;
	}

	inline void write_size(n_json& node, const std::string& name, const Size& size)
	{
		return write_size(node[name], size);
	}

	inline void write_color(n_json& node, const Color& color)
	{
		node = my::format("{/}, {/}, {/}, {/}", color.GetR(), color.GetG(), color.GetB(), color.GetA());
	}

	inline void write_color(n_json& node, const std::string& name, const Color& color)
	{
		return write_color(node[name], color);
	}
}
#endif
