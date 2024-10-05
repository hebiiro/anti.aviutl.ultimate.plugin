#pragma once

namespace apn::dark::share
{
	using HANDLE = my::addr_t;
	using HWND = HANDLE;
	using HDC = HANDLE;
	using HPEN = HANDLE;
	using HBRUSH = HANDLE;
	using HFONT = HANDLE;
	using HBITMAP = HANDLE;
	using HRGN = HANDLE;
	using HTHEME = HANDLE;

	//
	// このクラスはPOINTの再定義です。
	//
	struct Point : POINT
	{
		//
		// コンストラクタです。
		//
		Point()
		{
		}

		//
		// コンストラクタです。
		//
		Point(const POINT& other)
			: POINT(other)
		{
		}

		//
		// コンストラクタです。
		//
		Point(LONG x, LONG y)
		{
			__init__(this, x, y);
		}

		//
		// 座標を移動させます。
		//
		void offset(LONG x, LONG y)
		{
			this->x += x;
			this->y += y;
		}

		//
		// このオブジェクトのコピーを作成して返します。
		//
		Point copy() const
		{
			return Point(*this);
		}

		//
		// Pythonの初期化関数です。
		//
		inline static void __init__(Point* self, LONG x, LONG y)
		{
			self->x = x;
			self->y = y;
		}

		//
		// Pythonの文字列化関数です。
		//
		auto __str__() const
		{
			return std::format(L"{}, {}", x, y);
		}

		//
		// Pythonの比較関数です。
		//
		auto __eq__(const Point& other) const
		{
			return
				x == other.x &&
				y == other.y;
		}

		//
		// Pythonのハッシュ関数です。
		//
		auto __hash__() const
		{
			return py::hash(py::make_tuple(x, y));
		}
	};

	//
	// このクラスはRECTの再定義です。
	//
	struct Rect : RECT
	{
		//
		// コンストラクタです。
		//
		Rect()
		{
		}

		//
		// コンストラクタです。
		//
		Rect(const RECT& other)
			: RECT(other)
		{
		}

		//
		// コンストラクタです。
		//
		Rect(LONG left, LONG top, LONG right, LONG bottom)
		{
			__init__(this, left, top, right, bottom);
		}

		//
		// 矩形を移動させます。
		//
		void offset(LONG x, LONG y)
		{
			::OffsetRect(this, x, y);
		}

		//
		// 矩形を膨張させます。
		//
		void inflate(LONG x, LONG y)
		{
			::InflateRect(this, x, y);
		}

		//
		// 矩形を縮小させます。
		//
		void deflate(LONG x, LONG y)
		{
			::InflateRect(this, -x, -y);
		}

		//
		// 矩形の幅を返します。
		//
		LONG get_width() const
		{
			return right - left;
		}

		//
		// 矩形の高さを返します。
		//
		LONG get_height() const
		{
			return bottom - top;
		}

		//
		// このオブジェクトのコピーを作成して返します。
		//
		Rect copy() const
		{
			return Rect(*this);
		}

		//
		// Pythonの初期化関数です。
		//
		inline static void __init__(Rect* self, LONG left, LONG top, LONG right, LONG bottom)
		{
			self->left = left;
			self->top = top;
			self->right = right;
			self->bottom = bottom;
		}

		//
		// Pythonの文字列化関数です。
		//
		auto __str__() const
		{
			return std::format(L"{}, {}, {}, {}", left, top, right, bottom);
		}

		//
		// Pythonの比較関数です。
		//
		auto __eq__(const Rect& other) const
		{
			return
				left == other.left &&
				top == other.top &&
				right == other.right &&
				bottom == other.bottom;
		}

		//
		// Pythonのハッシュ関数です。
		//
		auto __hash__() const
		{
			return py::hash(py::make_tuple(left, top, right, bottom));
		}
	};

	//
	// このクラスはDark::Stuffの再定義です。
	//
	struct Stuff : Dark::Stuff
	{
		//
		// コンストラクタです。
		//
		Stuff()
		{
		}

		//
		// コンストラクタです。
		//
		Stuff(const Dark::Stuff& other)
			: Dark::Stuff(other)
		{
		}

		//
		// このオブジェクトのコピーを作成して返します。
		//
		Stuff copy() const
		{
			return Stuff(*this);
		}

		//
		// Pythonの文字列化関数です。
		//
		auto __str__() const
		{
			return std::format(
				L"fill({:#010x}, {:#010x}, {}), "
				L"border({:#010x}, {}), "
				L"text({:#010x}, {:#010x}, {:#010x}), "
				L"accent({:#010x}, {:#010x}), "
				L"etc({}, {})",
				fill.color,
				fill.sub_color,
				fill.mode,
				border.color,
				border.width,
				text.color,
				text.shadow_color,
				text.background_color,
				accent.color,
				accent.sub_color,
				etc.ellipse,
				etc.alpha);
		}

		//
		// Pythonの比較関数です。
		//
		auto __eq__(const Stuff& other) const
		{
			return
				fill.color == other.fill.color &&
				fill.sub_color == other.fill.sub_color &&
				fill.mode == other.fill.mode &&
				border.color == other.border.color &&
				border.width == other.border.width &&
				text.color == other.text.color &&
				text.shadow_color == other.text.shadow_color &&
				text.background_color == other.text.background_color &&
				accent.color == other.accent.color &&
				accent.sub_color == other.accent.sub_color &&
				etc.ellipse == other.etc.ellipse &&
				etc.alpha == other.etc.alpha;
		}

		//
		// Pythonのハッシュ関数です。
		//
		auto __hash__() const
		{
			return py::hash(py::make_tuple(
				fill.color,
				fill.sub_color,
				fill.mode,
				border.color,
				border.width,
				text.color,
				text.shadow_color,
				text.background_color,
				accent.color,
				accent.sub_color,
				etc.ellipse,
				etc.alpha));
		}
	};

	//
	// このクラスはビジュアルスタイルを識別するのに使用されます。
	// このクラスはテーマ・パーツID・ステートIDで構成されています。
	//
	struct VisualStyle
	{
		//
		// テーマハンドルです。
		//
		HTHEME theme;

		//
		// パーツIDです。
		//
		int part_id;

		//
		// ステートIDです。
		//
		int state_id;

		//
		// コンストラクタです。
		//
		VisualStyle(HTHEME theme, int part_id, int state_id)
		{
			__init__(this, theme, part_id, state_id);
		}

		//
		// コンストラクタです。
		//
		VisualStyle(const VisualStyle& other)
		{
			operator=(other);
		}

		//
		// このオブジェクトのコピーを作成して返します。
		//
		VisualStyle copy() const
		{
			return VisualStyle(*this);
		}

		//
		// Pythonの初期化関数です。
		//
		inline static void __init__(VisualStyle* self, HTHEME theme, int part_id, int state_id)
		{
			self->theme = theme;
			self->part_id = part_id;
			self->state_id = state_id;
		}

		//
		// Pythonの文字列化関数です。
		//
		auto __str__() const
		{
			return std::format(L"theme = {:#010x}, part_id = {}, state_id = {}", theme, part_id, state_id);
		}

		//
		// Pythonの比較関数です。
		//
		auto __eq__(const VisualStyle& other) const
		{
			return
				theme == other.theme &&
				part_id == other.part_id &&
				state_id == other.state_id;
		}

		//
		// Pythonのハッシュ関数です。
		//
		auto __hash__() const
		{
			return py::hash(py::make_tuple(theme, part_id, state_id));
		}

		//
		// 指定されたオブジェクトと等しい場合はTRUEを返します。
		//
		auto operator==(const VisualStyle& other) const
		{
			return __eq__(other);
		}

		//
		// 指定されたオブジェクトと等しくない場合はTRUEを返します。
		//
		auto operator!=(const VisualStyle& other) const
		{
			return !__eq__(other);
		}
	};

	//
	// このクラスはカラーセットです。
	//
	struct ColorSet
	{
		COLORREF primary_fill;
		COLORREF primary_border;
		COLORREF primary_text;
		COLORREF secondary_fill;
		COLORREF secondary_border;
		COLORREF secondary_text;
	};

	//
	// このクラスは初期化関数の引数です。
	//
	struct InitArgs
	{
		ColorSet color_set;
	};

	//
	// このクラスは更新関数の引数です。
	//
	struct UpdateArgs
	{
		ColorSet color_set;
	};

	//
	// このクラスは図形描画関数の引数です。
	//
	struct DrawFigureArgs
	{
		VisualStyle vs;
		HWND hwnd;
		HDC dc;
		LPCRECT rc;
	};

	//
	// このクラスはテキスト描画関数の引数です。
	//
	struct DrawTextArgs
	{
		VisualStyle vs;
		HWND hwnd;
		HDC dc;
		LPCRECT rc;
		LPCWSTR text;
		int c;
		UINT text_flags;
	};

	//
	// このクラスはテキスト出力関数の引数です。
	//
	struct TextOutArgs
	{
		VisualStyle vs;
		HWND hwnd;
		HDC dc;
		LPCRECT rc;
		LPCWSTR text;
		UINT c;
		UINT options;
		int x;
		int y;
		my::addr_t dx;
	};

	//
	// ダークモード化アドインが読み込まれている場合はTRUEを返します。
	//
	inline BOOL exists()
	{
		return !!apn::addin_manager.get_addin<>(L"dark");
	}

	//
	// このクラスはダークモード化アドインのエクスポート関数群です。
	//
	struct Exports
	{
		struct Skin {
			virtual BOOL init() = 0;
		} *skin = nullptr;

		struct ThemeManager {
			virtual HTHEME open_theme_data(LPCWSTR vsclass_name) = 0;
			virtual HTHEME open_theme_data_for_dpi(LPCWSTR vsclass_name) = 0;
			virtual HRESULT close_theme_data(HTHEME theme) = 0;
			virtual HTHEME get_theme_data(LPCWSTR vsclass_name, BOOL for_dpi) = 0;
		} *theme_manager = nullptr;

		struct StuffManager {
			virtual share::Stuff* get_stuff(LPCWSTR name) = 0;
			virtual void set_stuff(LPCWSTR name, const share::Stuff* stuff) = 0;
		} *stuff_manager = nullptr;

		struct VisualStyleManager {
			virtual share::Stuff* get_stuff(const VisualStyle* vs) = 0;
			virtual void set_stuff(const VisualStyle* vs, const share::Stuff* stuff) = 0;
		} *visual_style_manager = nullptr;

		struct DwmManager {
			virtual int get_dark_mode() = 0;
			virtual void set_dark_mode(int dark_mode) = 0;

			virtual int get_corner_mode() = 0;
			virtual void set_corner_mode(int corner_mode) = 0;

			virtual COLORREF get_active_border_color() = 0;
			virtual void set_active_border_color(COLORREF border_color) = 0;

			virtual COLORREF get_active_caption_color() = 0;
			virtual void set_active_caption_color(COLORREF caption_color) = 0;

			virtual COLORREF get_active_text_color() = 0;
			virtual void set_active_text_color(COLORREF text_color) = 0;

			virtual COLORREF get_inactive_border_color() = 0;
			virtual void set_inactive_border_color(COLORREF border_color) = 0;

			virtual COLORREF get_inactive_caption_color() = 0;
			virtual void set_inactive_caption_color(COLORREF caption_color) = 0;

			virtual COLORREF get_inactive_text_color() = 0;
			virtual void set_inactive_text_color(COLORREF text_color) = 0;

			virtual int get_popup_menu_corner_mode() = 0;
			virtual void set_popup_menu_corner_mode(int popup_menu_corner_mode) = 0;

			virtual COLORREF get_popup_menu_border_color() = 0;
			virtual void set_popup_menu_border_color(COLORREF popup_menu_border_color) = 0;
		} *dwm_manager = nullptr;

		struct IconManager {
			virtual void change_color(LPCWSTR icon_name, COLORREF src, COLORREF dst) = 0;
		} *icon_manager = nullptr;

		struct ConfigManager {
			struct Edges {
				struct Edge {
					virtual COLORREF get_near_color() const = 0;
					virtual void set_near_color(COLORREF near_color) = 0;
					virtual COLORREF get_far_color() const = 0;
					virtual void set_far_color(COLORREF far_color) = 0;
				};

				struct DoubleEdge {
					Edge* inner;
					Edge* outer;
				};

				Edge* light_raised = nullptr;
				Edge* light_sunken = nullptr;
				DoubleEdge* raised = nullptr;
				DoubleEdge* sunken = nullptr;
				DoubleEdge* bump = nullptr;
				DoubleEdge* etched = nullptr;
			} *edges = nullptr;

			struct ExEdit {
				struct Group {
					virtual COLORREF get_color() = 0;
					virtual void set_color(COLORREF color) = 0;
					virtual float get_alpha() = 0;
					virtual void set_alpha(float alpha) = 0;
				} *group = nullptr;

				struct XorPen {
					virtual int get_style() = 0;
					virtual void set_style(int style) = 0;
					virtual int get_width() = 0;
					virtual void set_width(int width) = 0;
					virtual COLORREF get_color() = 0;
					virtual void set_color(COLORREF color) = 0;
				} *xor_pen = nullptr;
			} *exedit = nullptr;
		} *config_manager = nullptr;

		struct Painter {
			virtual COLORREF get_pen_color(share::HPEN pen) = 0;
			virtual COLORREF get_brush_color(share::HBRUSH brush) = 0;
			virtual void clip_rect(share::HDC dc, LPCRECT rc, int mode) = 0;
			virtual void fill_rect(share::HDC dc, LPCRECT rc, COLORREF fill_color) = 0;
			virtual void frame_rect(share::HDC dc, LPCRECT rc, COLORREF edge_color, int edge_width) = 0;
			virtual void draw_rect(share::HDC dc, LPCRECT rc, COLORREF fill_color, COLORREF edge_color, int edge_width) = 0;
			virtual void draw_rect_with_alpha(share::HDC dc, LPCRECT rc, COLORREF fill_color, COLORREF edge_color, int edge_width, int alpha) = 0;
			virtual void draw_round_rect(share::HDC dc, LPCRECT rc, COLORREF fill_color, COLORREF edge_color, int edge_width, int round_width, int round_height) = 0;
			virtual void draw_round_rect_with_alpha(share::HDC dc, LPCRECT rc, COLORREF fill_color, COLORREF edge_color, int edge_width, int round_width, int round_height, int alpha) = 0;
			virtual void draw_light_edge(share::HDC dc, LPCRECT rc, COLORREF top_left_color, COLORREF bottom_right_color) = 0;
			virtual void draw_edge(share::HDC dc, LPCRECT rc, COLORREF inner_top_left_color, COLORREF inner_bottom_right_color, COLORREF outer_top_left_color, COLORREF outer_bottom_right_color) = 0;
			virtual void draw_focus_rect(share::HDC dc, LPCRECT rc, COLORREF line_color, UINT line_style) = 0;
			virtual void draw_text(share::HDC dc, LPCWSTR text, int length, LPCRECT rc, UINT format, BOOL opaque, COLORREF fill_color, COLORREF text_color) = 0;
			virtual void text_out(share::HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, my::addr_t dx, COLORREF fill_color, COLORREF text_color) = 0;
			virtual void draw_text_with_shadow(share::HDC dc, LPCWSTR text, int length, LPCRECT rc, UINT format, BOOL opaque, COLORREF fill_color, COLORREF text_color, COLORREF text_shadow_color) = 0;
			virtual void text_out_with_shadow(share::HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, my::addr_t dx, COLORREF fill_color, COLORREF text_color, COLORREF text_shadow_color) = 0;
			virtual void stuff_fill_rect(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) = 0;
			virtual void stuff_frame_rect(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) = 0;
			virtual void stuff_draw_round_rect(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) = 0;
			virtual void stuff_draw_rect(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) = 0;
			virtual void stuff_draw_rect_with_alpha(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) = 0;
			virtual void stuff_draw_round_rect_with_alpha(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) = 0;
			virtual void stuff_draw_light_edge(share::HDC dc, LPCRECT rc, const ConfigManager::Edges::Edge* edge) = 0;
			virtual void stuff_draw_edge(share::HDC dc, LPCRECT rc, const ConfigManager::Edges::DoubleEdge* edge) = 0;
			virtual void stuff_draw_text(share::HDC dc, LPCWSTR text, int length, LPCRECT rc, UINT format, BOOL opaque, const share::Stuff* stuff) = 0;
			virtual void stuff_text_out(share::HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, my::addr_t dx, const share::Stuff* stuff) = 0;
			virtual void stuff_draw_text_with_shadow(share::HDC dc, LPCWSTR text, int length, LPCRECT rc, UINT format, BOOL opaque, const share::Stuff* stuff) = 0;
			virtual void stuff_text_out_with_shadow(share::HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, my::addr_t dx, const share::Stuff* stuff) = 0;
			virtual void stuff_draw_text_fill(share::HDC dc, LPCWSTR text, int length, LPCRECT rc, UINT format, BOOL opaque, const share::Stuff* stuff) = 0;
			virtual void stuff_text_out_fill(share::HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, my::addr_t dx, const share::Stuff* stuff) = 0;
			virtual void stuff_draw_text_fill_with_shadow(share::HDC dc, LPCWSTR text, int length, LPCRECT rc, UINT format, BOOL opaque, const share::Stuff* stuff) = 0;
			virtual void stuff_text_out_fill_with_shadow(share::HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, my::addr_t dx, const share::Stuff* stuff) = 0;
		} *painter = nullptr;

		struct Gdiplus {
			virtual void draw_round_left(share::HDC dc, LPCRECT rc, COLORREF fill_color, COLORREF border_color, REAL border_width, int alpha) = 0;
			virtual void draw_round_right(share::HDC dc, LPCRECT rc, COLORREF fill_color, COLORREF border_color, REAL border_width, int alpha) = 0;
			virtual void draw_round_top(share::HDC dc, LPCRECT rc, COLORREF fill_color, COLORREF border_color, REAL border_width, int alpha) = 0;
			virtual void draw_round_bottom(share::HDC dc, LPCRECT rc, COLORREF fill_color, COLORREF border_color, REAL border_width, int alpha) = 0;
			virtual void draw_round_horz(share::HDC dc, LPCRECT rc, COLORREF fill_color, COLORREF border_color, REAL border_width, int alpha) = 0;
			virtual void draw_round_vert(share::HDC dc, LPCRECT rc, COLORREF fill_color, COLORREF border_color, REAL border_width, int alpha) = 0;
			virtual void draw_round_all(share::HDC dc, LPCRECT rc, COLORREF fill_color, COLORREF border_color, REAL border_width, REAL ellipse, int alpha) = 0;
			virtual void draw_ellipse(share::HDC dc, LPCRECT rc, COLORREF fill_color, COLORREF border_color, REAL border_width, int alpha) = 0;
			virtual void stuff_draw_round_left(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) = 0;
			virtual void stuff_draw_round_right(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) = 0;
			virtual void stuff_draw_round_top(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) = 0;
			virtual void stuff_draw_round_bottom(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) = 0;
			virtual void stuff_draw_round_horz(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) = 0;
			virtual void stuff_draw_round_vert(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) = 0;
			virtual void stuff_draw_round_all(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) = 0;
			virtual void stuff_draw_ellipse(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) = 0;
		} *gdiplus = nullptr;

		//
		// ダークモード化アドインのエクスポート関数群を初期化します。
		//
		BOOL init()
		{
			return init(addin_manager.get_addin<>(L"dark"));
		}

		//
		// ダークモード化アドインのエクスポート関数群を初期化します。
		//
		BOOL init(apn::Addin* addin)
		{
			addin->on_get_address(L"skin", skin);
			if (!skin) return FALSE;

			addin->on_get_address(L"theme_manager", theme_manager);
			if (!theme_manager) return FALSE;

			addin->on_get_address(L"stuff_manager", stuff_manager);
			if (!stuff_manager) return FALSE;

			addin->on_get_address(L"visual_style_manager", visual_style_manager);
			if (!visual_style_manager) return FALSE;

			addin->on_get_address(L"dwm_manager", dwm_manager);
			if (!dwm_manager) return FALSE;

			addin->on_get_address(L"icon_manager", icon_manager);
			if (!icon_manager) return FALSE;

			addin->on_get_address(L"config_manager", config_manager);
			if (!config_manager) return FALSE;

			addin->on_get_address(L"painter", painter);
			if (!painter) return FALSE;

			addin->on_get_address(L"gdiplus", gdiplus);
			if (!gdiplus) return FALSE;

			return TRUE;
		}
	};
}
