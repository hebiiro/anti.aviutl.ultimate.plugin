#pragma once

namespace apn::dark
{
	using namespace share;

	//
	// ダークモード化アドインのエクスポートシンボルです。
	//
	Exports exports;

	//
	// ダークモード化アドインのコンテキストです。
	//
	inline struct Context {} context;

	//
	// 初期化処理を実行します。
	//
	BOOL init(void* addin)
	{
		MY_TRACE_FUNC("{/hex}", addin);

		// ダークモード化アドインのエクスポートシンボルを取得します。
		return exports.init((apn::Addin*)addin);
	}

	//
	// 各オブジェクトを代入します。
	//
	inline void POINT_assign(POINT& a, const POINT& b) { a = b; }
	inline void Point_assign(Point& a, const Point& b) { a = b; }
	inline void RECT_assign(RECT& a, const RECT& b) { a = b; }
	inline void Rect_assign(Rect& a, const Rect& b) { a = b; }
	inline void Dark_Stuff_assign(share::Stuff& a, const share::Stuff& b) { a = b; }
	inline void Stuff_assign(Stuff& a, const Stuff& b) { a = b; }
	inline void VisualStyle_assign(VisualStyle& a, const VisualStyle& b) { a = b; }

	//
	// 各オブジェクトのコピーを返します。
	//
	inline POINT POINT_copy(const POINT& a) { return a; }
	inline Point Point_copy(const Point& a) { return a; }
	inline RECT RECT_copy(const RECT& a) { return a; }
	inline Rect Rect_copy(const Rect& a) { return a; }
	inline share::Stuff Dark_Stuff_copy(const share::Stuff& a) { return a; }
	inline Stuff Stuff_copy(const Stuff& a) { return a; }
	inline VisualStyle VisualStyle_copy(const VisualStyle& a) { return a; }

	//
	// 各オブジェクトを文字列化して返します。
	//
	inline auto POINT_str(const POINT& pt) { return my::format(L"x = {/}, y = {/}", pt.x, pt.y); }
	inline auto Point_str(const Point& pt) { return POINT_str(pt); }
	inline auto RECT_str(const RECT& rc) { return my::format(L"x = {/}, y = {/}, w = {/}, h = {/}", rc.left, rc.top, my::get_width(rc), my::get_height(rc)); }
	inline auto Rect_str(const Rect& rc) { return RECT_str(rc); }
	inline auto Dark_Stuff_str(const share::Stuff& stuff) { return static_cast<const Stuff&>(stuff).__str__(); }
	inline auto Stuff_str(const Stuff& stuff) { return stuff.__str__(); }
	inline auto VisualStyle_str(const VisualStyle& vs) { return vs.__str__(); }

	inline std::wstring safe_string(LPCRECT rc)
	{
		if (rc)
			return RECT_str(*rc);
		else
			return L"null";
	}

	inline std::wstring safe_string(LPCWSTR str, int c)
	{
		if (c < 0)
			return str;
		else
			return std::wstring(str, c);
	}

	inline std::wstring safe_string(LPCWSTR str, int c, UINT options)
	{
		if (options & ETO_GLYPH_INDEX)
			return L"ETO_GLYPH_INDEX";
		else
			return safe_string(str, c);
	}

	inline auto DrawFigureArgs_str(const share::DrawFigureArgs& args)
	{
		return my::format(L"{/}", safe_string(args.rc));
	}

	inline auto DrawTextArgs_str(const share::DrawTextArgs& args)
	{
		return my::format(L"{/}, {/}", safe_string(args.rc), safe_string(args.text, args.c));
	}

	inline auto TextOutArgs_str(const share::TextOutArgs& args)
	{
		return my::format(L"rc = {{{/}}}, options = {/hex}, text = {/}", safe_string(args.rc), args.options, safe_string(args.text, args.c, args.options));
	}

	//
	// 矩形を移動させます。
	//
	inline void RECT_offset(RECT* rc, LONG x, LONG y) { ::OffsetRect(rc, x, y); }

	//
	// 矩形を膨張させます。
	//
	inline void RECT_inflate(RECT* rc, LONG x, LONG y) { ::InflateRect(rc, x, y); }

	//
	// 矩形を縮小させます。
	//
	inline void RECT_deflate(RECT* rc, LONG x, LONG y) { ::InflateRect(rc, -x, -y); }

	//
	// 矩形の幅を返します。
	//
	inline LONG RECT_get_width(LPCRECT rc) { return rc->right - rc->left; }

	//
	// 矩形の高さを返します。
	//
	inline LONG RECT_get_height(LPCRECT rc) { return rc->bottom - rc->top; }

	//
	// 影付きの文字列を描画します。
	//
	inline auto draw_text_with_shadow(const DrawTextArgs& args, const share::Stuff& stuff)
	{
		return exports.painter->stuff_draw_text_with_shadow(args.dc, args.text, args.c, args.rc, args.text_flags, FALSE, &stuff);
	}

	//
	// 影付きの文字列を出力します。
	//
	inline auto text_out_with_shadow(const TextOutArgs& args, const share::Stuff& stuff)
	{
		return exports.painter->stuff_text_out_with_shadow(args.dc, args.x, args.y, args.options, args.rc, args.text, args.c, args.dx, &stuff);
	}

	//
	// 影付きの文字列を描画します。
	//
	inline auto RECT_draw_text_with_shadow(const DrawTextArgs& args, LPCRECT rc, const share::Stuff& stuff)
	{
		return exports.painter->stuff_draw_text_with_shadow(args.dc, args.text, args.c, rc, args.text_flags, FALSE, &stuff);
	}

	//
	// 影付きの文字列を出力します。
	//
	inline auto RECT_text_out_with_shadow(const TextOutArgs& args, LPCRECT rc, const share::Stuff& stuff)
	{
		return exports.painter->stuff_text_out_with_shadow(args.dc, args.x, args.y, args.options, rc, args.text, args.c, args.dx, &stuff);
	}

	//
	// 影付きの文字列を描画します。
	//
	inline auto draw_text_fill_with_shadow(const DrawTextArgs& args, const share::Stuff& stuff)
	{
		return exports.painter->stuff_draw_text_fill_with_shadow(args.dc, args.text, args.c, args.rc, args.text_flags, TRUE, &stuff);
	}

	//
	// 影付きの文字列を出力します。
	//
	inline auto text_out_fill_with_shadow(const TextOutArgs& args, const share::Stuff& stuff)
	{
		return exports.painter->stuff_text_out_fill_with_shadow(args.dc, args.x, args.y, args.options, args.rc, args.text, args.c, args.dx, &stuff);
	}

	//
	// 影付きの文字列を描画します。
	//
	inline auto RECT_draw_text_fill_with_shadow(const DrawTextArgs& args, LPCRECT rc, const share::Stuff& stuff)
	{
		return exports.painter->stuff_draw_text_fill_with_shadow(args.dc, args.text, args.c, rc, args.text_flags, TRUE, &stuff);
	}

	//
	// 影付きの文字列を出力します。
	//
	inline auto RECT_text_out_fill_with_shadow(const TextOutArgs& args, LPCRECT rc, const share::Stuff& stuff)
	{
		return exports.painter->stuff_text_out_fill_with_shadow(args.dc, args.x, args.y, args.options, rc, args.text, args.c, args.dx, &stuff);
	}

	//
	// ここでPythonにバインドします。
	//
	PYBIND11_MODULE(dark, mod)
	{
		MY_TRACE_FUNC("");

		mod.def("init", &init);
		mod.def("assign", &POINT_assign);
		mod.def("assign", &Point_assign);
		mod.def("assign", &RECT_assign);
		mod.def("assign", &Rect_assign);
		mod.def("assign", &Dark_Stuff_assign);
		mod.def("assign", &Stuff_assign);
		mod.def("assign", &VisualStyle_assign);
		mod.def("copy", &POINT_copy, py::return_value_policy::copy);
		mod.def("copy", &Point_copy, py::return_value_policy::copy);
		mod.def("copy", &RECT_copy, py::return_value_policy::copy);
		mod.def("copy", &Rect_copy, py::return_value_policy::copy);
		mod.def("copy", &Dark_Stuff_copy, py::return_value_policy::copy);
		mod.def("copy", &Stuff_copy, py::return_value_policy::copy);
		mod.def("copy", &VisualStyle_copy, py::return_value_policy::copy);
		mod.def("str", &POINT_str);
		mod.def("str", &Point_str);
		mod.def("str", &RECT_str);
		mod.def("str", &Rect_str);
		mod.def("str", &Dark_Stuff_str);
		mod.def("str", &Stuff_str);
		mod.def("str", &VisualStyle_str);
		mod.def("str", &DrawFigureArgs_str);
		mod.def("str", &DrawTextArgs_str);
		mod.def("str", &TextOutArgs_str);
		mod.def("offset", &RECT_offset);
		mod.def("inflate", &RECT_inflate);
		mod.def("deflate", &RECT_deflate);
		mod.def("get_width", &RECT_get_width);
		mod.def("get_height", &RECT_get_height);
		mod.def("draw_str", &draw_text_with_shadow);
		mod.def("draw_str", &text_out_with_shadow);
		mod.def("draw_str", &RECT_draw_text_with_shadow);
		mod.def("draw_str", &RECT_text_out_with_shadow);
		mod.def("draw_str_fill", &draw_text_fill_with_shadow);
		mod.def("draw_str_fill", &text_out_fill_with_shadow);
		mod.def("draw_str_fill", &RECT_draw_text_fill_with_shadow);
		mod.def("draw_str_fill", &RECT_text_out_fill_with_shadow);

		// Win32のPOINTを登録します。
		py::class_<POINT>(mod, "tagPOINT")
	        .def(py::init<>())
	        .def(py::init<const POINT&>())
	        .def(py::init<LONG, LONG>())
			.def_readwrite("x", &POINT::x)
			.def_readwrite("y", &POINT::y)
		;

		// Pointを登録します。
		py::class_<Point, POINT>(mod, "POINT")
	        .def(py::init<>())
	        .def(py::init<const POINT&>())
	        .def(py::init<LONG, LONG>())
			.def("offset", &Point::offset)
			.def("__str__", &Point::__str__)
			.def("__eq__", &Point::__eq__)
			.def("__hash__", &Point::__hash__)
			.def_readwrite("x", &POINT::x)
			.def_readwrite("y", &POINT::y)
		;

		// Win32のRECTを登録します。
		py::class_<RECT>(mod, "tagRECT")
	        .def(py::init<>())
	        .def(py::init<const RECT&>())
	        .def(py::init<LONG, LONG, LONG, LONG>())
			.def_readwrite("left", &RECT::left)
			.def_readwrite("top", &RECT::top)
			.def_readwrite("right", &RECT::right)
			.def_readwrite("bottom", &RECT::bottom)
		;

		// Rectを登録します。
		py::class_<Rect, RECT>(mod, "RECT")
	        .def(py::init<>())
	        .def(py::init<const RECT&>())
	        .def(py::init<LONG, LONG, LONG, LONG>())
			.def("offset", &Rect::offset)
			.def("inflate", &Rect::inflate)
			.def("deflate", &Rect::deflate)
			.def("get_width", &Rect::get_width)
			.def("get_height", &Rect::get_height)
			.def_property_readonly("width", &Rect::get_width)
			.def_property_readonly("height", &Rect::get_height)
			.def("__str__", &Rect::__str__)
			.def("__eq__", &Rect::__eq__)
			.def("__hash__", &Rect::__hash__)
			.def_readwrite("left", &RECT::left)
			.def_readwrite("top", &RECT::top)
			.def_readwrite("right", &RECT::right)
			.def_readwrite("bottom", &RECT::bottom)
		;

		// RECTとRectを相互変換できるようにします。
		py::implicitly_convertible<Rect, RECT>();
		py::implicitly_convertible<RECT, Rect>();

		// Dark::Stuffを登録します。
		auto dark_stuff = py::class_<Dark::Stuff>(mod, "Dark::Stuff")
			.def(py::init<>())
			.def_readwrite("fill", &Dark::Stuff::fill)
			.def_readwrite("border", &Dark::Stuff::border)
			.def_readwrite("text", &Dark::Stuff::text)
			.def_readwrite("accent", &Dark::Stuff::accent)
			.def_readwrite("etc", &Dark::Stuff::etc)
		;
		{
			auto fill = py::class_<Dark::Stuff::Fill>(dark_stuff, "Fill")
				.def_readwrite("color", &Dark::Stuff::Fill::color)
				.def_readwrite("sub_color", &Dark::Stuff::Fill::sub_color)
				.def_readwrite("mode", &Dark::Stuff::Fill::mode)
				.def_readonly_static("c_mode", &Dark::Stuff::Fill::c_mode)
			;
			{
				auto mode = py::class_<Dark::Stuff::Fill::Mode>(fill, "Mode")
					.def_readonly_static("c_normal", &Dark::Stuff::Fill::Mode::c_normal)
					.def_readonly_static("c_left", &Dark::Stuff::Fill::Mode::c_left)
					.def_readonly_static("c_top", &Dark::Stuff::Fill::Mode::c_top)
					.def_readonly_static("c_right", &Dark::Stuff::Fill::Mode::c_right)
					.def_readonly_static("c_bottom", &Dark::Stuff::Fill::Mode::c_bottom)
					.def_readonly_static("c_top_left", &Dark::Stuff::Fill::Mode::c_top_left)
					.def_readonly_static("c_top_right", &Dark::Stuff::Fill::Mode::c_top_right)
					.def_readonly_static("c_bottom_left", &Dark::Stuff::Fill::Mode::c_bottom_left)
					.def_readonly_static("c_bottom_right", &Dark::Stuff::Fill::Mode::c_bottom_right)
					.def_readonly_static("c_center", &Dark::Stuff::Fill::Mode::c_center)
					;
			}
			auto border = py::class_<Dark::Stuff::Border>(dark_stuff, "Border")
				.def_readwrite("color", &Dark::Stuff::Border::color)
				.def_readwrite("width", &Dark::Stuff::Border::width)
			;
			auto text = py::class_<Dark::Stuff::Text>(dark_stuff, "Text")
				.def_readwrite("color", &Dark::Stuff::Text::color)
				.def_readwrite("shadow_color", &Dark::Stuff::Text::shadow_color)
				.def_readwrite("background_color", &Dark::Stuff::Text::background_color)
			;
			auto accent = py::class_<Dark::Stuff::Accent>(dark_stuff, "Accent")
				.def_readwrite("color", &Dark::Stuff::Accent::color)
				.def_readwrite("sub_color", &Dark::Stuff::Accent::sub_color)
			;
			auto etc = py::class_<Dark::Stuff::Etc>(dark_stuff, "Etc")
				.def_readwrite("ellipse", &Dark::Stuff::Etc::ellipse)
				.def_readwrite("alpha", &Dark::Stuff::Etc::alpha)
			;
		}

		// Stuffを登録します。
		py::class_<Stuff, Dark::Stuff>(mod, "Stuff")
			.def(py::init<>())
	        .def(py::init<const Dark::Stuff&>())
			.def("__str__", &Stuff::__str__)
			.def("__eq__", &Stuff::__eq__)
			.def("__hash__", &Stuff::__hash__)
			.def_readwrite("fill", &Stuff::fill)
			.def_readwrite("border", &Stuff::border)
			.def_readwrite("text", &Stuff::text)
			.def_readwrite("accent", &Stuff::accent)
			.def_readwrite("etc", &Stuff::etc)
		;

		// VisualStyleを登録します。
		py::class_<VisualStyle>(mod, "VisualStyle")
			.def(py::init<HTHEME, int, int>())
			.def("__str__", &VisualStyle::__str__)
			.def("__eq__", &VisualStyle::__eq__)
			.def("__hash__", &VisualStyle::__hash__)
			.def_readwrite("theme", &VisualStyle::theme)
			.def_readwrite("part_id", &VisualStyle::part_id)
			.def_readwrite("state_id", &VisualStyle::state_id)
		;

		// ColorSetを登録します。
		py::class_<ColorSet>(mod, "ColorSet")
			.def_readwrite("fill", &ColorSet::fill)
			.def_readwrite("border", &ColorSet::border)
			.def_readwrite("text", &ColorSet::text)
		;

		// InitArgsを登録します。
		py::class_<InitArgs>(mod, "InitArgs")
			.def_readwrite("dark_color", &InitArgs::dark_color)
			.def_readwrite("light_color", &InitArgs::light_color)
		;

		// UpdateArgsを登録します。
		py::class_<UpdateArgs>(mod, "UpdateArgs")
			.def_readwrite("dark_color", &UpdateArgs::dark_color)
			.def_readwrite("light_color", &UpdateArgs::light_color)
		;

		// DrawFigureArgsを登録します。
		py::class_<DrawFigureArgs>(mod, "DrawFigureArgs")
			.def_readwrite("vs", &DrawFigureArgs::vs)
			.def_readwrite("hwnd", &DrawFigureArgs::hwnd)
			.def_readwrite("dc", &DrawFigureArgs::dc)
			.def_readwrite("rc", &DrawFigureArgs::rc)
		;

		// DrawTextArgsを登録します。
		py::class_<DrawTextArgs>(mod, "DrawTextArgs")
			.def_readwrite("vs", &DrawTextArgs::vs)
			.def_readwrite("hwnd", &DrawTextArgs::hwnd)
			.def_readwrite("dc", &DrawTextArgs::dc)
			.def_readwrite("rc", &DrawTextArgs::rc)
			.def_readwrite("text", &DrawTextArgs::text)
			.def_readwrite("c", &DrawTextArgs::c)
			.def_readwrite("text_flags", &DrawTextArgs::text_flags)
		;

		// TextOutArgsを登録します。
		py::class_<TextOutArgs>(mod, "TextOutArgs")
			.def_readwrite("vs", &TextOutArgs::vs)
			.def_readwrite("hwnd", &TextOutArgs::hwnd)
			.def_readwrite("dc", &TextOutArgs::dc)
			.def_readwrite("rc", &TextOutArgs::rc)
			.def_readwrite("text", &TextOutArgs::text)
			.def_readwrite("c", &TextOutArgs::c)
			.def_readwrite("options", &TextOutArgs::options)
			.def_readwrite("x", &TextOutArgs::x)
			.def_readwrite("y", &TextOutArgs::y)
			.def_readwrite("dx", &TextOutArgs::dx)
		;

		// Exportsを登録します。
		auto exports = py::class_<Exports>(mod, "Exports")
			.def_readwrite("skin", &Exports::skin)
			.def_readwrite("theme_manager", &Exports::theme_manager)
			.def_readwrite("stuff_manager", &Exports::stuff_manager)
			.def_readwrite("visual_style_manager", &Exports::visual_style_manager)
			.def_readwrite("dwm_manager", &Exports::dwm_manager)
			.def_readwrite("icon_manager", &Exports::icon_manager)
			.def_readwrite("config_manager", &Exports::config_manager)
			.def_readwrite("painter", &Exports::painter)
			.def_readwrite("gdiplus", &Exports::gdiplus)
		;
		{
			// Skinを登録します。
			auto skin = py::class_<Exports::Skin>(exports, "Skin")
				.def("init", &Exports::Skin::init)
			;

			// ThemeManagerを登録します。
			auto theme_manager = py::class_<Exports::ThemeManager>(exports, "ThemeManager")
				.def("open_theme_data", &Exports::ThemeManager::open_theme_data)
				.def("open_theme_data_for_dpi", &Exports::ThemeManager::open_theme_data_for_dpi)
				.def("close_theme_data", &Exports::ThemeManager::close_theme_data)
				.def("get_theme_data", &Exports::ThemeManager::get_theme_data)
			;

			// StuffManagerを登録します。
			auto stuff_manager = py::class_<Exports::StuffManager>(exports, "StuffManager")
				.def("get_stuff", &Exports::StuffManager::get_stuff)
				.def("set_stuff", &Exports::StuffManager::set_stuff)
			;

			// VisualStyleManagerを登録します。
			auto visual_style_manager = py::class_<Exports::VisualStyleManager>(exports, "VisualStyleManager")
				.def("get_stuff", &Exports::VisualStyleManager::get_stuff)
				.def("set_stuff", &Exports::VisualStyleManager::set_stuff)
			;

			// DwmManagerを登録します。
			auto dwm_manager = py::class_<Exports::DwmManager>(exports, "DwmManager")
				.def("get_dark_mode", &Exports::DwmManager::get_dark_mode)
				.def("set_dark_mode", &Exports::DwmManager::set_dark_mode)
				.def_property("dark_mode",
					&Exports::DwmManager::get_dark_mode,
					&Exports::DwmManager::set_dark_mode)

				.def("get_corner_mode", &Exports::DwmManager::get_corner_mode)
				.def("set_corner_mode", &Exports::DwmManager::set_corner_mode)
				.def_property("corner_mode",
					&Exports::DwmManager::get_corner_mode,
					&Exports::DwmManager::set_corner_mode)

				.def("get_active_border_color", &Exports::DwmManager::get_active_border_color)
				.def("set_active_border_color", &Exports::DwmManager::set_active_border_color)
				.def_property("active_border_color",
					&Exports::DwmManager::get_active_border_color,
					&Exports::DwmManager::set_active_border_color)

				.def("get_active_caption_color", &Exports::DwmManager::get_active_caption_color)
				.def("set_active_caption_color", &Exports::DwmManager::set_active_caption_color)
				.def_property("active_caption_color",
					&Exports::DwmManager::get_active_caption_color,
					&Exports::DwmManager::set_active_caption_color)

				.def("get_active_text_color", &Exports::DwmManager::get_active_text_color)
				.def("set_active_text_color", &Exports::DwmManager::set_active_text_color)
				.def_property("active_text_color",
					&Exports::DwmManager::get_active_text_color,
					&Exports::DwmManager::set_active_text_color)

				.def("get_inactive_border_color", &Exports::DwmManager::get_inactive_border_color)
				.def("set_inactive_border_color", &Exports::DwmManager::set_inactive_border_color)
				.def_property("inactive_border_color",
					&Exports::DwmManager::get_inactive_border_color,
					&Exports::DwmManager::set_inactive_border_color)

				.def("get_inactive_caption_color", &Exports::DwmManager::get_inactive_caption_color)
				.def("set_inactive_caption_color", &Exports::DwmManager::set_inactive_caption_color)
				.def_property("inactive_caption_color",
					&Exports::DwmManager::get_inactive_caption_color,
					&Exports::DwmManager::set_inactive_caption_color)

				.def("get_inactive_text_color", &Exports::DwmManager::get_inactive_text_color)
				.def("set_inactive_text_color", &Exports::DwmManager::set_inactive_text_color)
				.def_property("inactive_text_color",
					&Exports::DwmManager::get_inactive_text_color,
					&Exports::DwmManager::set_inactive_text_color)

				.def("get_popup_menu_corner_mode", &Exports::DwmManager::get_popup_menu_corner_mode)
				.def("set_popup_menu_corner_mode", &Exports::DwmManager::set_popup_menu_corner_mode)
				.def_property("popup_menu_corner_mode",
					&Exports::DwmManager::get_popup_menu_corner_mode,
					&Exports::DwmManager::set_popup_menu_corner_mode)

				.def("get_popup_menu_border_color", &Exports::DwmManager::get_popup_menu_border_color)
				.def("set_popup_menu_border_color", &Exports::DwmManager::set_popup_menu_border_color)
				.def_property("popup_menu_border_color",
					&Exports::DwmManager::get_popup_menu_border_color,
					&Exports::DwmManager::set_popup_menu_border_color)
			;

			// IconManagerを登録します。
			auto icon_manager = py::class_<Exports::IconManager>(exports, "IconManager")
				.def("change_color", &Exports::IconManager::change_color)
			;

			// ConfigManagerを登録します。
			auto config_manager = py::class_<Exports::ConfigManager>(exports, "ConfigManager")
				.def_readwrite("edges", &Exports::ConfigManager::edges)
				.def_readwrite("exedit", &Exports::ConfigManager::exedit)
			;
			{
				// ConfigManager::Edgesを登録します。
				auto edges = py::class_<Exports::ConfigManager::Edges>(config_manager, "Edges")
					.def_readwrite("light_raised", &Exports::ConfigManager::Edges::light_raised)
					.def_readwrite("light_sunken", &Exports::ConfigManager::Edges::light_sunken)
					.def_readwrite("raised", &Exports::ConfigManager::Edges::raised)
					.def_readwrite("sunken", &Exports::ConfigManager::Edges::sunken)
					.def_readwrite("bump", &Exports::ConfigManager::Edges::bump)
					.def_readwrite("etched", &Exports::ConfigManager::Edges::etched)
				;
				{
					// ConfigManager::Edges::Edgeを登録します。
					auto edge = py::class_<Exports::ConfigManager::Edges::Edge>(edges, "Edge")
						.def("get_near_color", &Exports::ConfigManager::Edges::Edge::get_near_color)
						.def("set_near_color", &Exports::ConfigManager::Edges::Edge::set_near_color)
						.def_property("near_color",
							&Exports::ConfigManager::Edges::Edge::get_near_color,
							&Exports::ConfigManager::Edges::Edge::set_near_color)

						.def("get_far_color", &Exports::ConfigManager::Edges::Edge::get_far_color)
						.def("set_far_color", &Exports::ConfigManager::Edges::Edge::set_far_color)
						.def_property("far_color",
							&Exports::ConfigManager::Edges::Edge::get_far_color,
							&Exports::ConfigManager::Edges::Edge::set_far_color)
					;

					// ConfigManager::Edges::DoubleEdgeを登録します。
					auto double_edge = py::class_<Exports::ConfigManager::Edges::DoubleEdge>(edges, "DoubleEdge")
						.def_readwrite("inner", &Exports::ConfigManager::Edges::DoubleEdge::inner)
						.def_readwrite("outer", &Exports::ConfigManager::Edges::DoubleEdge::outer)
					;
				}

				// ConfigManager::ExEditを登録します。
				auto exedit = py::class_<Exports::ConfigManager::ExEdit>(config_manager, "ExEdit")
					.def_readwrite("group", &Exports::ConfigManager::ExEdit::group)
					.def_readwrite("xor_pen", &Exports::ConfigManager::ExEdit::xor_pen)
				;
				{
					// ConfigManager::ExEdit::Groupを登録します。
					auto group = py::class_<Exports::ConfigManager::ExEdit::Group>(exedit, "Group")
						.def("get_color", &Exports::ConfigManager::ExEdit::Group::get_color)
						.def("set_color", &Exports::ConfigManager::ExEdit::Group::set_color)
						.def_property("color",
							&Exports::ConfigManager::ExEdit::Group::get_color,
							&Exports::ConfigManager::ExEdit::Group::set_color)

						.def("get_alpha", &Exports::ConfigManager::ExEdit::Group::get_alpha)
						.def("set_alpha", &Exports::ConfigManager::ExEdit::Group::set_alpha)
						.def_property("alpha",
							&Exports::ConfigManager::ExEdit::Group::get_alpha,
							&Exports::ConfigManager::ExEdit::Group::set_alpha)
					;

					// ConfigManager::ExEdit::XorPenを登録します。
					auto xor_pen = py::class_<Exports::ConfigManager::ExEdit::XorPen>(exedit, "XorPen")
						.def("get_style", &Exports::ConfigManager::ExEdit::XorPen::get_style)
						.def("set_style", &Exports::ConfigManager::ExEdit::XorPen::set_style)
						.def_property("style",
							&Exports::ConfigManager::ExEdit::XorPen::get_style,
							&Exports::ConfigManager::ExEdit::XorPen::set_style)

						.def("get_width", &Exports::ConfigManager::ExEdit::XorPen::get_width)
						.def("set_width", &Exports::ConfigManager::ExEdit::XorPen::set_width)
						.def_property("width",
							&Exports::ConfigManager::ExEdit::XorPen::get_width,
							&Exports::ConfigManager::ExEdit::XorPen::set_width)

						.def("get_color", &Exports::ConfigManager::ExEdit::XorPen::get_color)
						.def("set_color", &Exports::ConfigManager::ExEdit::XorPen::set_color)
						.def_property("color",
							&Exports::ConfigManager::ExEdit::XorPen::get_color,
							&Exports::ConfigManager::ExEdit::XorPen::set_color)
					;
				}
			}

			// Painterを登録します。
			auto painter = py::class_<Exports::Painter>(exports, "Painter")
				.def("get_pen_color", &Exports::Painter::get_pen_color)
				.def("get_brush_color", &Exports::Painter::get_brush_color)
				.def("clip_rect", &Exports::Painter::clip_rect)
				.def("fill_rect", &Exports::Painter::fill_rect)
				.def("frame_rect", &Exports::Painter::frame_rect)
				.def("draw_rect", &Exports::Painter::draw_rect)
				.def("draw_rect_with_alpha", &Exports::Painter::draw_rect_with_alpha)
				.def("draw_round_rect", &Exports::Painter::draw_round_rect)
				.def("draw_round_rect_with_alpha", &Exports::Painter::draw_round_rect_with_alpha)
				.def("draw_edge", &Exports::Painter::draw_light_edge)
				.def("draw_edge", &Exports::Painter::draw_edge)
				.def("draw_focus_rect", &Exports::Painter::draw_focus_rect)
				.def("draw_text", &Exports::Painter::draw_text)
				.def("text_out", &Exports::Painter::text_out)
				.def("draw_text_with_shadow", &Exports::Painter::draw_text_with_shadow)
				.def("text_out_with_shadow", &Exports::Painter::text_out_with_shadow)
				.def("fill_rect", &Exports::Painter::stuff_fill_rect)
				.def("frame_rect", &Exports::Painter::stuff_frame_rect)
				.def("draw_rect", &Exports::Painter::stuff_draw_rect)
				.def("draw_rect_with_alpha", &Exports::Painter::stuff_draw_rect_with_alpha)
				.def("draw_round_rect", &Exports::Painter::stuff_draw_round_rect)
				.def("draw_round_rect_with_alpha", &Exports::Painter::stuff_draw_round_rect_with_alpha)
				.def("draw_edge", &Exports::Painter::stuff_draw_light_edge)
				.def("draw_edge", &Exports::Painter::stuff_draw_edge)
				.def("draw_text", &Exports::Painter::stuff_draw_text)
				.def("text_out", &Exports::Painter::stuff_text_out)
				.def("draw_text_with_shadow", &Exports::Painter::stuff_draw_text_with_shadow)
				.def("text_out_with_shadow", &Exports::Painter::stuff_text_out_with_shadow)
				.def("draw_text_fill", &Exports::Painter::stuff_draw_text_fill)
				.def("text_out_fill", &Exports::Painter::stuff_text_out_fill)
				.def("draw_text_fill_with_shadow", &Exports::Painter::stuff_draw_text_fill_with_shadow)
				.def("text_out_fill_with_shadow", &Exports::Painter::stuff_text_out_fill_with_shadow)
			;

			// Gdiplusを登録します。
			auto gdiplus = py::class_<Exports::Gdiplus>(exports, "Gdiplus")
				.def("fill_rect", &Exports::Gdiplus::fill_rect)
				.def("frame_rect", &Exports::Gdiplus::frame_rect)
				.def("draw_rect", &Exports::Gdiplus::draw_rect)
				.def("draw_round_left", &Exports::Gdiplus::draw_round_left, "dc"_a, "rc"_a, "fill_color"_a = CLR_NONE, "border_color"_a = CLR_NONE, "border_width"_a = 0, "alpha"_a = 255)
				.def("draw_round_right", &Exports::Gdiplus::draw_round_right, "dc"_a, "rc"_a, "fill_color"_a = CLR_NONE, "border_color"_a = CLR_NONE, "border_width"_a = 0, "alpha"_a = 255)
				.def("draw_round_top", &Exports::Gdiplus::draw_round_top, "dc"_a, "rc"_a, "fill_color"_a = CLR_NONE, "border_color"_a = CLR_NONE, "border_width"_a = 0, "alpha"_a = 255)
				.def("draw_round_bottom", &Exports::Gdiplus::draw_round_bottom, "dc"_a, "rc"_a, "fill_color"_a = CLR_NONE, "border_color"_a = CLR_NONE, "border_width"_a = 0, "alpha"_a = 255)
				.def("draw_round_horz", &Exports::Gdiplus::draw_round_horz, "dc"_a, "rc"_a, "fill_color"_a = CLR_NONE, "border_color"_a = CLR_NONE, "border_width"_a = 0, "alpha"_a = 255)
				.def("draw_round_vert", &Exports::Gdiplus::draw_round_vert, "dc"_a, "rc"_a, "fill_color"_a = CLR_NONE, "border_color"_a = CLR_NONE, "border_width"_a = 0, "alpha"_a = 255)
				.def("draw_round_all", &Exports::Gdiplus::draw_round_all, "dc"_a, "rc"_a, "fill_color"_a = CLR_NONE, "border_color"_a = CLR_NONE, "border_width"_a = 0, "ellipse"_a = 0, "alpha"_a = 255)
				.def("draw_ellipse", &Exports::Gdiplus::draw_ellipse, "dc"_a, "rc"_a, "fill_color"_a = CLR_NONE, "border_color"_a = CLR_NONE, "border_width"_a = 0, "alpha"_a = 255)
				.def("fill_rect", &Exports::Gdiplus::stuff_fill_rect)
				.def("frame_rect", &Exports::Gdiplus::stuff_frame_rect)
				.def("draw_rect", &Exports::Gdiplus::stuff_draw_rect)
				.def("draw_round_left", &Exports::Gdiplus::stuff_draw_round_left)
				.def("draw_round_right", &Exports::Gdiplus::stuff_draw_round_right)
				.def("draw_round_top", &Exports::Gdiplus::stuff_draw_round_top)
				.def("draw_round_bottom", &Exports::Gdiplus::stuff_draw_round_bottom)
				.def("draw_round_horz", &Exports::Gdiplus::stuff_draw_round_horz)
				.def("draw_round_vert", &Exports::Gdiplus::stuff_draw_round_vert)
				.def("draw_round_all", &Exports::Gdiplus::stuff_draw_round_all)
				.def("draw_ellipse", &Exports::Gdiplus::stuff_draw_ellipse)
			;
		}

		mod.attr("exports") = py::cast(&apn::dark::exports);

		// Contextを登録します。
		auto context = py::class_<Context>(mod, "Context", py::dynamic_attr())
			.def(py::init<>())
		;

//		auto global = py::dict(py::module::import("__main__").attr("__dict__"));
		auto global = py::dict(mod.attr("__dict__"));
		auto local = py::dict(mod.attr("__dict__"));

		py::eval<py::eval_single_statement>("context = Context()", global, global);
		py::eval<py::eval_single_statement>("context.exedit = Context()", global, global);
	}
}
