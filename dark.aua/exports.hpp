#pragma once

namespace apn::dark
{
	//
	// このクラスはエクスポートされるシンボルを保持します。
	//
	inline struct Exports {
		struct Skin : share::Exports::Skin {
			virtual BOOL init() { return app->init_skin(); }
		} *skin = nullptr;

		struct ThemeManager : share::Exports::ThemeManager {
			virtual share::HTHEME open_theme_data(LPCWSTR vsclass_name) override { return (share::HTHEME)skin::theme::manager.open_theme_data(vsclass_name); }
			virtual share::HTHEME open_theme_data_for_dpi(LPCWSTR vsclass_name) override { return (share::HTHEME)skin::theme::manager.open_theme_data_for_dpi(vsclass_name); }
			virtual HRESULT close_theme_data(share::HTHEME theme) override { return skin::theme::manager.close_theme_data((HTHEME)theme); }
			virtual share::HTHEME get_theme_data(LPCWSTR vsclass_name, BOOL for_dpi) override { return (share::HTHEME)skin::theme::manager.get_theme_data(vsclass_name, for_dpi); }
		} theme_manager;

		struct StuffManager : share::Exports::StuffManager {
			virtual share::Stuff* get_stuff(LPCWSTR name) override { return skin::stuff_manager.get(name); }
			virtual void set_stuff(LPCWSTR name, const share::Stuff* stuff) override { skin::stuff_manager.set(name, stuff); }
		} stuff_manager;

		struct VisualStyleManager : share::Exports::VisualStyleManager {
			virtual share::Stuff* get_stuff(const share::VisualStyle* vs) override
			{
				auto state = skin::theme::manager.get_state((HTHEME)vs->theme, vs->part_id, vs->state_id);
				if (state) return &state->stuff;
				return nullptr;
			}

			virtual void set_stuff(const share::VisualStyle* vs, const share::Stuff* stuff) override
			{
				auto state = skin::theme::manager.get_state((HTHEME)vs->theme, vs->part_id, vs->state_id);
				if (state) state->stuff = *stuff;
			}
		} visual_style_manager;

		struct DwmManager : share::Exports::DwmManager {
			virtual int get_dark_mode() override { return skin::dwm.dark_mode; }
			virtual void set_dark_mode(int dark_mode) override { skin::dwm.dark_mode = dark_mode; }

			virtual int get_corner_mode() override { return skin::dwm.corner_mode; }
			virtual void set_corner_mode(int corner_mode) override { skin::dwm.corner_mode = corner_mode; }

			virtual COLORREF get_active_border_color() override { return skin::dwm.active.border_color; }
			virtual void set_active_border_color(COLORREF border_color) override { skin::dwm.active.border_color = border_color; }

			virtual COLORREF get_active_caption_color() override { return skin::dwm.active.caption_color; }
			virtual void set_active_caption_color(COLORREF caption_color) override { skin::dwm.active.caption_color = caption_color; }

			virtual COLORREF get_active_text_color() override { return skin::dwm.active.text_color; }
			virtual void set_active_text_color(COLORREF text_color) override { skin::dwm.active.text_color = text_color; }

			virtual COLORREF get_inactive_border_color() override { return skin::dwm.inactive.border_color; }
			virtual void set_inactive_border_color(COLORREF border_color) override { skin::dwm.inactive.border_color = border_color; }

			virtual COLORREF get_inactive_caption_color() override { return skin::dwm.inactive.caption_color; }
			virtual void set_inactive_caption_color(COLORREF caption_color) override { skin::dwm.inactive.caption_color = caption_color; }

			virtual COLORREF get_inactive_text_color() override { return skin::dwm.inactive.text_color; }
			virtual void set_inactive_text_color(COLORREF text_color) override { skin::dwm.inactive.text_color = text_color; }

			virtual int get_popup_menu_corner_mode() override { return skin::dwm.popup_menu.corner_mode; }
			virtual void set_popup_menu_corner_mode(int popup_menu_corner_mode) override { skin::dwm.popup_menu.corner_mode = popup_menu_corner_mode; }

			virtual COLORREF get_popup_menu_border_color() override { return skin::dwm.popup_menu.border_color; }
			virtual void set_popup_menu_border_color(COLORREF popup_menu_border_color) override { skin::dwm.popup_menu.border_color = popup_menu_border_color; }
		} dwm_manager;

		struct IconManager : share::Exports::IconManager {
			virtual void change_color(LPCWSTR icon_name, COLORREF src, COLORREF dst) override { skin::icon_editor.change_color(icon_name, src, dst); }
		} icon_manager;

		struct ConfigManager : share::Exports::ConfigManager {
			ConfigManager() {
				__super::edges = &edges;
				__super::edges->light_raised = &edges.light_raised;
				__super::edges->light_sunken = &edges.light_sunken;
				__super::edges->raised = &edges.raised;
				__super::edges->raised->inner = &edges.raised.inner;
				__super::edges->raised->outer = &edges.raised.outer;
				__super::edges->sunken = &edges.sunken;
				__super::edges->sunken->inner = &edges.sunken.inner;
				__super::edges->sunken->outer = &edges.sunken.outer;
				__super::edges->bump = &edges.bump;
				__super::edges->bump->inner = &edges.bump.inner;
				__super::edges->bump->outer = &edges.bump.outer;
				__super::edges->etched = &edges.etched;
				__super::edges->etched->inner = &edges.etched.inner;
				__super::edges->etched->outer = &edges.etched.outer;
				__super::exedit = &exedit;
				__super::exedit->group = &exedit.group;
				__super::exedit->xor_pen = &exedit.xor_pen;
			}

			struct Edges : share::Exports::ConfigManager::Edges {
				struct Edge : share::Exports::ConfigManager::Edges::Edge {
					virtual COLORREF get_near_color() const override { return skin::config.edges.light_raised.near_color; }
					virtual void set_near_color(COLORREF near_color) override { skin::config.edges.light_raised.near_color = near_color; }

					virtual COLORREF get_far_color() const override { return skin::config.edges.light_raised.far_color; }
					virtual void set_far_color(COLORREF far_color) override { skin::config.edges.light_raised.far_color = far_color; }
				};
				struct : share::Exports::ConfigManager::Edges::Edge {
					virtual COLORREF get_near_color() const override { return skin::config.edges.light_raised.near_color; }
					virtual void set_near_color(COLORREF near_color) override { skin::config.edges.light_raised.near_color = near_color; }

					virtual COLORREF get_far_color() const override { return skin::config.edges.light_raised.far_color; }
					virtual void set_far_color(COLORREF far_color) override { skin::config.edges.light_raised.far_color = far_color; }
				} light_raised;

				struct : share::Exports::ConfigManager::Edges::Edge {
					virtual COLORREF get_near_color() const override { return skin::config.edges.light_sunken.near_color; }
					virtual void set_near_color(COLORREF near_color) override { skin::config.edges.light_sunken.near_color = near_color; }

					virtual COLORREF get_far_color() const override { return skin::config.edges.light_sunken.far_color; }
					virtual void set_far_color(COLORREF far_color) override { skin::config.edges.light_sunken.far_color = far_color; }
				} light_sunken;

				struct Raised : share::Exports::ConfigManager::Edges::DoubleEdge {
					struct Inner : share::Exports::ConfigManager::Edges::Edge {
						virtual COLORREF get_near_color() const override { return skin::config.edges.raised.inner.near_color; }
						virtual void set_near_color(COLORREF near_color) override { skin::config.edges.raised.inner.near_color = near_color; }

						virtual COLORREF get_far_color() const override { return skin::config.edges.raised.inner.far_color; }
						virtual void set_far_color(COLORREF far_color) override { skin::config.edges.raised.inner.far_color = far_color; }
					} inner;

					struct Outer : share::Exports::ConfigManager::Edges::Edge {
						virtual COLORREF get_near_color() const override { return skin::config.edges.raised.outer.near_color; }
						virtual void set_near_color(COLORREF near_color) override { skin::config.edges.raised.outer.near_color = near_color; }

						virtual COLORREF get_far_color() const override { return skin::config.edges.raised.outer.far_color; }
						virtual void set_far_color(COLORREF far_color) override { skin::config.edges.raised.outer.far_color = far_color; }
					} outer;
				} raised;

				struct Sunken : share::Exports::ConfigManager::Edges::DoubleEdge {
					struct Inner : share::Exports::ConfigManager::Edges::Edge {
						virtual COLORREF get_near_color() const override { return skin::config.edges.sunken.inner.near_color; }
						virtual void set_near_color(COLORREF near_color) override { skin::config.edges.sunken.inner.near_color = near_color; }

						virtual COLORREF get_far_color() const override { return skin::config.edges.sunken.inner.far_color; }
						virtual void set_far_color(COLORREF far_color) override { skin::config.edges.sunken.inner.far_color = far_color; }
					} inner;

					struct Outer : share::Exports::ConfigManager::Edges::Edge {
						virtual COLORREF get_near_color() const override { return skin::config.edges.sunken.outer.near_color; }
						virtual void set_near_color(COLORREF near_color) override { skin::config.edges.sunken.outer.near_color = near_color; }

						virtual COLORREF get_far_color() const override { return skin::config.edges.sunken.outer.far_color; }
						virtual void set_far_color(COLORREF far_color) override { skin::config.edges.sunken.outer.far_color = far_color; }
					} outer;
				} sunken;

				struct Bump : share::Exports::ConfigManager::Edges::DoubleEdge {
					struct Inner : share::Exports::ConfigManager::Edges::Edge {
						virtual COLORREF get_near_color() const override { return skin::config.edges.bump.inner.near_color; }
						virtual void set_near_color(COLORREF near_color) override { skin::config.edges.bump.inner.near_color = near_color; }

						virtual COLORREF get_far_color() const override { return skin::config.edges.bump.inner.far_color; }
						virtual void set_far_color(COLORREF far_color) override { skin::config.edges.bump.inner.far_color = far_color; }
					} inner;

					struct Outer : share::Exports::ConfigManager::Edges::Edge {
						virtual COLORREF get_near_color() const override { return skin::config.edges.bump.outer.near_color; }
						virtual void set_near_color(COLORREF near_color) override { skin::config.edges.bump.outer.near_color = near_color; }

						virtual COLORREF get_far_color() const override { return skin::config.edges.bump.outer.far_color; }
						virtual void set_far_color(COLORREF far_color) override { skin::config.edges.bump.outer.far_color = far_color; }
					} outer;
				} bump;

				struct Etched : share::Exports::ConfigManager::Edges::DoubleEdge {
					struct Inner : share::Exports::ConfigManager::Edges::Edge {
						virtual COLORREF get_near_color() const override { return skin::config.edges.etched.inner.near_color; }
						virtual void set_near_color(COLORREF near_color) override { skin::config.edges.etched.inner.near_color = near_color; }

						virtual COLORREF get_far_color() const override { return skin::config.edges.etched.inner.far_color; }
						virtual void set_far_color(COLORREF far_color) override { skin::config.edges.etched.inner.far_color = far_color; }
					} inner;

					struct Outer : share::Exports::ConfigManager::Edges::Edge {
						virtual COLORREF get_near_color() const override { return skin::config.edges.etched.outer.near_color; }
						virtual void set_near_color(COLORREF near_color) override { skin::config.edges.etched.outer.near_color = near_color; }

						virtual COLORREF get_far_color() const override { return skin::config.edges.etched.outer.far_color; }
						virtual void set_far_color(COLORREF far_color) override { skin::config.edges.etched.outer.far_color = far_color; }
					} outer;
				} etched;
			} edges;

			struct ExEdit : share::Exports::ConfigManager::ExEdit {
				struct Group : share::Exports::ConfigManager::ExEdit::Group {
					virtual COLORREF get_color() override { return skin::config.exedit.group.color; }
					virtual void set_color(COLORREF color) override { skin::config.exedit.group.color = color; }

					virtual float get_alpha() override { return skin::config.exedit.group.alpha; }
					virtual void set_alpha(float alpha) override { skin::config.exedit.group.alpha = alpha; }
				} group;

				struct XorPen : share::Exports::ConfigManager::ExEdit::XorPen {
					virtual int get_style() override { return skin::config.exedit.xor_pen.style; }
					virtual void set_style(int style) override { skin::config.exedit.xor_pen.style = style; }

					virtual int get_width() override { return skin::config.exedit.xor_pen.width; }
					virtual void set_width(int width) override { skin::config.exedit.xor_pen.width = width; }

					virtual COLORREF get_color() override { return skin::config.exedit.xor_pen.color; }
					virtual void set_color(COLORREF color) override { skin::config.exedit.xor_pen.color = color; }
				} xor_pen;
			} exedit;
		} config_manager;

		struct Painter : share::Exports::Painter {
			virtual COLORREF get_pen_color(share::HPEN pen) override { return painter::get_pen_color((HPEN)pen); }
			virtual COLORREF get_brush_color(share::HBRUSH brush) override { return painter::get_brush_color((HBRUSH)brush); }
			virtual void clip_rect(share::HDC dc, LPCRECT rc, int mode) override { return painter::clip_rect((HDC)dc, rc, mode); }
			virtual void fill_rect(share::HDC dc, LPCRECT rc, COLORREF fill_color) override { return painter::fill_rect((HDC)dc, rc, fill_color); }
			virtual void frame_rect(share::HDC dc, LPCRECT rc, COLORREF edge_color, int edge_width) override { return painter::frame_rect((HDC)dc, rc, edge_color, edge_width); }
			virtual void draw_rect(share::HDC dc, LPCRECT rc, COLORREF fill_color, COLORREF edge_color, int edge_width) override { return painter::draw_rect((HDC)dc, rc, fill_color, edge_color, edge_width); }
			virtual void draw_rect_with_alpha(share::HDC dc, LPCRECT rc, COLORREF fill_color, COLORREF edge_color, int edge_width, int alpha) override { return painter::draw_rect_with_alpha((HDC)dc, rc, fill_color, edge_color, edge_width, alpha); }
			virtual void draw_round_rect(share::HDC dc, LPCRECT rc, COLORREF fill_color, COLORREF edge_color, int edge_width, int round_width, int round_height) override { return painter::draw_round_rect((HDC)dc, rc, fill_color, edge_color, edge_width, round_width, round_height); }
			virtual void draw_round_rect_with_alpha(share::HDC dc, LPCRECT rc, COLORREF fill_color, COLORREF edge_color, int edge_width, int round_width, int round_height, int alpha) override { return painter::draw_round_rect_with_alpha((HDC)dc, rc, fill_color, edge_color, edge_width, round_width, round_height, alpha); }
			virtual void draw_light_edge(share::HDC dc, LPCRECT rc, COLORREF near_color, COLORREF far_color) override { return painter::draw_light_edge((HDC)dc, rc, near_color, far_color); }
			virtual void draw_edge(share::HDC dc, LPCRECT rc, COLORREF inner_near_color, COLORREF inner_far_color, COLORREF outer_near_color, COLORREF outer_far_color) override { return painter::draw_edge((HDC)dc, rc, inner_near_color, inner_far_color, outer_near_color, outer_far_color); }
			virtual void draw_focus_rect(share::HDC dc, LPCRECT rc, COLORREF line_color, UINT line_style) override { return painter::draw_focus_rect((HDC)dc, rc, line_color, line_style); }
			virtual void draw_text(share::HDC dc, LPCWSTR text, int length, LPCRECT rc, UINT format, BOOL opaque, COLORREF fill_color, COLORREF text_color) override { return painter::draw_text((HDC)dc, text, length, rc, format, opaque, fill_color, text_color); }
			virtual void text_out(share::HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, my::addr_t dx, COLORREF fill_color, COLORREF text_color) override { return painter::text_out((HDC)dc, x, y, options, rc, text, c, (const int*)dx, fill_color, text_color); }
			virtual void draw_text_with_shadow(share::HDC dc, LPCWSTR text, int length, LPCRECT rc, UINT format, BOOL opaque, COLORREF fill_color, COLORREF text_color, COLORREF text_shadow_color) override { return painter::draw_text_with_shadow((HDC)dc, text, length, rc, format, opaque, fill_color, text_color, text_shadow_color); }
			virtual void text_out_with_shadow(share::HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, my::addr_t dx, COLORREF fill_color, COLORREF text_color, COLORREF text_shadow_color) override { return painter::text_out_with_shadow((HDC)dc, x, y, options, rc, text, c, (const int*)dx, fill_color, text_color, text_shadow_color); }
			virtual void stuff_fill_rect(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) override { return painter::fill_rect((HDC)dc, rc, stuff->fill.color); }
			virtual void stuff_frame_rect(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) override { return painter::frame_rect((HDC)dc, rc, stuff->border.color, (int)stuff->border.width); }
			virtual void stuff_draw_rect(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) override { return painter::draw_rect((HDC)dc, rc, stuff->fill.color, stuff->border.color, (int)stuff->border.width); }
			virtual void stuff_draw_rect_with_alpha(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) override { return painter::draw_rect_with_alpha((HDC)dc, rc, stuff->fill.color, stuff->border.color, (int)stuff->border.width, stuff->etc.alpha); }
			virtual void stuff_draw_round_rect(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) override { return painter::draw_round_rect((HDC)dc, rc, stuff->fill.color, stuff->border.color, (int)stuff->border.width, (int)stuff->etc.ellipse, (int)stuff->etc.ellipse); }
			virtual void stuff_draw_round_rect_with_alpha(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) override { return painter::draw_round_rect_with_alpha((HDC)dc, rc, stuff->fill.color, stuff->border.color, (int)stuff->border.width, (int)stuff->etc.ellipse, (int)stuff->etc.ellipse, stuff->etc.alpha); }
			virtual void stuff_draw_light_edge(share::HDC dc, LPCRECT rc, const share::Exports::ConfigManager::Edges::Edge* edge) override { return painter::draw_light_edge((HDC)dc, rc, edge->get_near_color(), edge->get_far_color()); }
			virtual void stuff_draw_edge(share::HDC dc, LPCRECT rc, const share::Exports::ConfigManager::Edges::DoubleEdge* edge) override { return painter::draw_edge((HDC)dc, rc, edge->inner->get_near_color(), edge->inner->get_far_color(), edge->outer->get_near_color(), edge->outer->get_far_color()); }
			virtual void stuff_draw_text(share::HDC dc, LPCWSTR text, int length, LPCRECT rc, UINT format, BOOL opaque, const share::Stuff* stuff) override { return painter::draw_text((HDC)dc, text, length, rc, format, opaque, stuff->text.background_color, stuff->text.color); }
			virtual void stuff_text_out(share::HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, my::addr_t dx, const share::Stuff* stuff) override { return painter::text_out((HDC)dc, x, y, options, rc, text, c, (const int*)dx, stuff->text.background_color, stuff->text.color); }
			virtual void stuff_draw_text_with_shadow(share::HDC dc, LPCWSTR text, int length, LPCRECT rc, UINT format, BOOL opaque, const share::Stuff* stuff) override { return painter::draw_text_with_shadow((HDC)dc, text, length, rc, format, opaque, stuff->text.background_color, stuff->text.color, stuff->text.shadow_color); }
			virtual void stuff_text_out_with_shadow(share::HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, my::addr_t dx, const share::Stuff* stuff) override { return painter::text_out_with_shadow((HDC)dc, x, y, options, rc, text, c, (const int*)dx, stuff->text.background_color, stuff->text.color, stuff->text.shadow_color); }
			virtual void stuff_draw_text_fill(share::HDC dc, LPCWSTR text, int length, LPCRECT rc, UINT format, BOOL opaque, const share::Stuff* stuff) override { return painter::draw_text((HDC)dc, text, length, rc, format, opaque, stuff->fill.color, stuff->text.color); }
			virtual void stuff_text_out_fill(share::HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, my::addr_t dx, const share::Stuff* stuff) override { return painter::text_out((HDC)dc, x, y, options, rc, text, c, (const int*)dx, stuff->fill.color, stuff->text.color); }
			virtual void stuff_draw_text_fill_with_shadow(share::HDC dc, LPCWSTR text, int length, LPCRECT rc, UINT format, BOOL opaque, const share::Stuff* stuff) override { return painter::draw_text_with_shadow((HDC)dc, text, length, rc, format, opaque, stuff->fill.color, stuff->text.color, stuff->text.shadow_color); }
			virtual void stuff_text_out_fill_with_shadow(share::HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, my::addr_t dx, const share::Stuff* stuff) override { return painter::text_out_with_shadow((HDC)dc, x, y, options, rc, text, c, (const int*)dx, stuff->fill.color, stuff->text.color, stuff->text.shadow_color); }
		} painter;

		struct Gdiplus : share::Exports::Gdiplus {
			struct Stuff : share::Stuff {
				Stuff(
					COLORREF fill_color,
					COLORREF border_color,
					REAL border_width,
					REAL ellipse, int alpha)
				{
					this->fill.color = fill_color;
					this->border.color = border_color;
					this->border.width = border_width;
					this->etc.ellipse = ellipse;
					this->etc.alpha = alpha;
				}
			};

			virtual void fill_rect(share::HDC dc, LPCRECT rc, COLORREF fill_color, int alpha) override { return painter::gdiplus::fill_rect((HDC)dc, rc, Stuff(fill_color, CLR_NONE, 0, 0.0f, alpha)); }
			virtual void frame_rect(share::HDC dc, LPCRECT rc, COLORREF border_color, REAL border_width, int alpha) override { return painter::gdiplus::frame_rect((HDC)dc, rc, Stuff(CLR_NONE, border_color, border_width, 0.0f, alpha)); }
			virtual void draw_rect(share::HDC dc, LPCRECT rc, COLORREF fill_color, COLORREF border_color, REAL border_width, int alpha) override { return painter::gdiplus::draw_rect((HDC)dc, rc, Stuff(fill_color, border_color, border_width, 0.0f, alpha)); }
			virtual void draw_round_left(share::HDC dc, LPCRECT rc, COLORREF fill_color, COLORREF border_color, REAL border_width, int alpha) override { return painter::gdiplus::draw_round_left((HDC)dc, rc, Stuff(fill_color, border_color, border_width, 0.0f, alpha)); }
			virtual void draw_round_right(share::HDC dc, LPCRECT rc, COLORREF fill_color, COLORREF border_color, REAL border_width, int alpha) override { return painter::gdiplus::draw_round_right((HDC)dc, rc, Stuff(fill_color, border_color, border_width, 0.0f, alpha)); }
			virtual void draw_round_top(share::HDC dc, LPCRECT rc, COLORREF fill_color, COLORREF border_color, REAL border_width, int alpha) override { return painter::gdiplus::draw_round_top((HDC)dc, rc, Stuff(fill_color, border_color, border_width, 0.0f, alpha)); }
			virtual void draw_round_bottom(share::HDC dc, LPCRECT rc, COLORREF fill_color, COLORREF border_color, REAL border_width, int alpha) override { return painter::gdiplus::draw_round_bottom((HDC)dc, rc, Stuff(fill_color, border_color, border_width, 0.0f, alpha)); }
			virtual void draw_round_horz(share::HDC dc, LPCRECT rc, COLORREF fill_color, COLORREF border_color, REAL border_width, int alpha) override { return painter::gdiplus::draw_round_horz((HDC)dc, rc, Stuff(fill_color, border_color, border_width, 0.0f, alpha)); }
			virtual void draw_round_vert(share::HDC dc, LPCRECT rc, COLORREF fill_color, COLORREF border_color, REAL border_width, int alpha) override { return painter::gdiplus::draw_round_vert((HDC)dc, rc, Stuff(fill_color, border_color, border_width, 0.0f, alpha)); }
			virtual void draw_round_all(share::HDC dc, LPCRECT rc, COLORREF fill_color, COLORREF border_color, REAL border_width, REAL ellipse, int alpha) override { return painter::gdiplus::draw_round_all((HDC)dc, rc, Stuff(fill_color, border_color, border_width, ellipse, alpha)); }
			virtual void draw_ellipse(share::HDC dc, LPCRECT rc, COLORREF fill_color, COLORREF border_color, REAL border_width, int alpha) override { return painter::gdiplus::draw_ellipse((HDC)dc, rc, Stuff(fill_color, border_color, border_width, 0.0f, alpha)); }
			virtual void stuff_fill_rect(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) override { if (stuff) return painter::gdiplus::fill_rect((HDC)dc, rc, *stuff); }
			virtual void stuff_frame_rect(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) override { if (stuff) return painter::gdiplus::frame_rect((HDC)dc, rc, *stuff); }
			virtual void stuff_draw_rect(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) override { if (stuff) return painter::gdiplus::draw_rect((HDC)dc, rc, *stuff); }
			virtual void stuff_draw_round_left(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) override { if (stuff) return painter::gdiplus::draw_round_left((HDC)dc, rc, *stuff); }
			virtual void stuff_draw_round_right(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) override { if (stuff) return painter::gdiplus::draw_round_right((HDC)dc, rc, *stuff); }
			virtual void stuff_draw_round_top(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) override { if (stuff) return painter::gdiplus::draw_round_top((HDC)dc, rc, *stuff); }
			virtual void stuff_draw_round_bottom(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) override { if (stuff) return painter::gdiplus::draw_round_bottom((HDC)dc, rc, *stuff); }
			virtual void stuff_draw_round_horz(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) override { if (stuff) return painter::gdiplus::draw_round_horz((HDC)dc, rc, *stuff); }
			virtual void stuff_draw_round_vert(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) override { if (stuff) return painter::gdiplus::draw_round_vert((HDC)dc, rc, *stuff); }
			virtual void stuff_draw_round_all(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) override { if (stuff) return painter::gdiplus::draw_round_all((HDC)dc, rc, *stuff); }
			virtual void stuff_draw_ellipse(share::HDC dc, LPCRECT rc, const share::Stuff* stuff) override { if (stuff) return painter::gdiplus::draw_ellipse((HDC)dc, rc, *stuff); }
		} gdiplus;
	} exports;
}
