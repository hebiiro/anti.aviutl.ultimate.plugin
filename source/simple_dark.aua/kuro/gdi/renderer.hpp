#pragma once

namespace apn::dark::kuro::gdi
{
	//
	// このクラスはGDIレンダラーです。
	//
	struct renderer_t : std::enable_shared_from_this<renderer_t>
	{
		//
		// ウィンドウプロパティ名です。
		// TRUEの場合はダイアログとして描画します。
		//
		inline static constexpr LPCWSTR k_prop_is_dialog = L"apn::dark::kuro::gdi::is_dialog";

		//
		// このクラスは標準化されたウィンドウのクラス名です。
		//
		struct normalized_class_name_t : my::tstring {
			normalized_class_name_t(const my::tstring& class_name) : my::tstring(class_name) {}
			bool operator==(LPCTSTR b) const { return ::lstrcmpi(c_str(), b) == 0; }
		};

		//
		// このクラスはメッセージの状態です。
		//
		struct message_state_t {
			HWND hwnd;
			UINT message;
			WPARAM w_param;
			LPARAM l_param;
		};

		//
		// 現在のメッセージの状態です。
		//
		thread_local inline static message_state_t current_message_state;

		//
		// レンダラーのコレクションです。
		//
		thread_local inline static std::unordered_map<HWND, std::shared_ptr<renderer_t>> collection;

		//
		// 仮想デストラクタです。
		//
		virtual ~renderer_t()
		{
//			MY_TRACE_FUNC("");
		}

		//
		// レンダラーとウィンドウを関連付けます。
		//
		void attach(HWND hwnd)
		{
			MY_TRACE_FUNC("{/}", my::hwnd_to_string(hwnd));

			// レンダラーをコレクションに追加します。
			collection[hwnd] = shared_from_this();

			// ウィンドウをサブクラス化します。
			::SetWindowSubclass(hwnd, renderer_t::subclass_proc, (UINT_PTR)this, 0);

			on_attach(hwnd);
		}

		//
		// レンダラーとウィンドウの関連付けを解除します。
		//
		void detach(HWND hwnd)
		{
			MY_TRACE_FUNC("{/}", my::hwnd_to_string(hwnd));

			on_detach(hwnd);

			// ウィンドウのサブクラス化を解除します。
			::RemoveWindowSubclass(hwnd, renderer_t::subclass_proc, (UINT_PTR)this);

			// レンダラーをコレクションから削除します。
			collection.erase(hwnd);
		}

		//
		// すべてのレンダラーとウィンドウの関連付けを解除します。
		//
		inline static void clear()
		{
			// コレクションを走査します。
			for (const auto& pair : collection)
			{
				// ウィンドウを取得します。
				auto hwnd = pair.first;
				MY_TRACE_HWND(hwnd);

				// レンダラーを取得します。
				const auto& renderer = pair.second;

				// レンダラーにデタッチを通知します。
				renderer->on_detach(hwnd);

				// ウィンドウのサブクラス化を解除します。
				::RemoveWindowSubclass(hwnd, renderer_t::subclass_proc, (UINT_PTR)renderer.get());
			}

			// コレクションを消去します。
			collection.clear();
		}

		//
		// 指定されたウィンドウに関連付けられているレンダラーを返します。
		//
		inline static std::shared_ptr<renderer_t> from_handle(HWND hwnd)
		{
			auto it = collection.find(hwnd);
			if (it == collection.end()) return nullptr;
			return it->second;
		}

		//
		// オリジナルのウィンドウプロシージャを呼び出します。
		//
		inline static LRESULT CALLBACK orig_wnd_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			return ::DefSubclassProc(hwnd, message, w_param, l_param);
		}

		//
		// サブクラスプロシージャです。
		//
		inline static LRESULT CALLBACK subclass_proc(
			HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param,
			UINT_PTR subclass_id, DWORD_PTR subclass_data)
		{
#ifdef _DEBUG
			if (0)
			{
				// デバッグ用のコードです。

				MY_TRACE_FUNC("{/}, {/}, {/hex}, {/hex}",
					my::hwnd_to_string(hwnd), my::message_to_string(message), w_param, l_param);
			}
#endif
			// レンダラーの使用が抑制されている場合は
			if (!hive.is_valid_thread())
			{
				// 最終メッセージの場合は
				if (message == WM_NCDESTROY)
				{
					// レンダラーを取得できた場合は
					if (auto renderer = from_handle(hwnd))
					{
						// デタッチします。
						renderer->detach(hwnd);
					}
				}

				// デフォルト処理を実行します。
				return orig_wnd_proc(hwnd, message, w_param, l_param);
			}

			// 独自のメッセージ処理を実行します。
			{
				BOOL skip_default = TRUE;
				auto result = custom_wnd_proc(hwnd, message, w_param, l_param, skip_default);
				if (skip_default) return result;
			}

			// 現在のメッセージの状態を保存します。
			struct MessageStateSaver
			{
				message_state_t old_message_state;

				MessageStateSaver(const message_state_t& message_state)
				{
					// 現在のメッセージの状態を更新します。
					old_message_state = current_message_state;
					current_message_state = message_state;
				}
				~MessageStateSaver()
				{
					// 現在のメッセージの状態を復元します。
					current_message_state = old_message_state;
				}
			} message_state_saver = {
				{ hwnd, message, w_param, l_param },
			};

			// レンダラーを取得できた場合は
			if (auto renderer = from_handle(hwnd))
			{
				// レンダラーに処理させます。
				auto result = renderer->on_subclass_proc(hwnd, message, w_param, l_param);

				// 最終メッセージの場合はデタッチします。
				if (message == WM_NCDESTROY) renderer->detach(hwnd);

				// 処理結果を返します。
				return result;
			}

			// デフォルト処理を実行します。
			return orig_wnd_proc(hwnd, message, w_param, l_param);
		}

		//
		// 独自のメッセージ処理を実行します。
		//
		inline static LRESULT CALLBACK custom_wnd_proc(
			HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param, BOOL& skip_default)
		{
			switch (message)
			{
			case WM_ACTIVATE:
				{
					auto active = LOWORD(w_param);

					MY_TRACE_FUNC("WM_ACTIVATE, {/}", active);

					auto pigment = active ? &paint::window_material.active : &paint::window_material.inactive;

					auto caption_color = pigment->background.get_win32_color();
					auto border_color = pigment->border.get_win32_color();
					auto text_color = pigment->text.get_win32_color();

					::DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR , &caption_color, sizeof(caption_color));
					::DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR , &border_color, sizeof(border_color));
					::DwmSetWindowAttribute(hwnd, DWMWA_TEXT_COLOR , &text_color, sizeof(text_color));

					break;
				}
			case WM_CTLCOLORDLG:
			case WM_CTLCOLORMSGBOX:
			case WM_CTLCOLORBTN:
			case WM_CTLCOLOREDIT:
			case WM_CTLCOLORLISTBOX:
			case WM_CTLCOLORSCROLLBAR:
			case WM_CTLCOLORSTATIC:
				{
					MY_TRACE_FUNC("WM_CTLCOLOR, {/hex}, {/hex}, {/hex}, {/hex}", hwnd, message, w_param, l_param);

					// デフォルト処理を実行してデフォルトのブラシを取得します。
					auto brush = (HBRUSH)orig_wnd_proc(hwnd, message, w_param, l_param);

					auto dc = (HDC)w_param;
					auto control = (HWND)l_param;
					MY_TRACE_HWND(control);

					// レンダラーを取得できた場合は処理を任せます。
					if (auto renderer = from_handle(control))
						return (LRESULT)renderer->on_ctl_color(hwnd, message, dc, control, brush);
					else
						return (LRESULT)brush;
				}
			case WM_NOTIFY:
				{
					auto nm = (NMHDR*)l_param;

					switch (nm->code)
					{
					case NM_CUSTOMDRAW:
						{
							// レンダラーを取得できた場合は処理を任せます。
							if (auto renderer = from_handle(nm->hwndFrom))
								return renderer->on_custom_draw(hwnd, message, w_param, l_param);

							break;
						}
					}

					break;
				}
#ifdef _DEBUG // テスト用コードです。
			case WM_GETTEXT:
			case WM_GETTEXTLENGTH:
				return orig_wnd_proc(hwnd, message, w_param, l_param);
#endif
			}

			return skip_default = FALSE, 0;
		}

		inline static BOOL fire_rectangle(HDC dc, int left, int top, int right, int bottom)
		{
			auto renderer = from_handle(current_message_state.hwnd);
			if (renderer) return renderer->on_rectangle(&current_message_state, dc, left, top, right, bottom);
			return hive.orig.Rectangle(dc, left, top, right, bottom);
		}

		inline static BOOL fire_fill_rect(HDC dc, LPCRECT rc, HBRUSH brush)
		{
			auto hwnd = current_message_state.hwnd;
			MY_TRACE_HWND(hwnd);

			// ダイアログとマークされているウィンドウはダイアログとして描画します。
			if (::GetPropW(hwnd, k_prop_is_dialog))
				return draw_dialog_background(dc, rc);

			auto renderer = from_handle(current_message_state.hwnd);
			if (renderer) return renderer->on_fill_rect(&current_message_state, dc, rc, brush);
			return hive.orig.FillRect(dc, rc, brush);
		}

		inline static BOOL fire_draw_frame(HDC dc, LPRECT rc, UINT width, UINT type)
		{
			auto renderer = from_handle(current_message_state.hwnd);
			if (renderer) return renderer->on_draw_frame(&current_message_state, dc, rc, width, type);
			return hive.orig.DrawFrame(dc, rc, width, type);
		}

		inline static BOOL fire_draw_frame_control(HDC dc, LPRECT rc, UINT type, UINT state)
		{
			auto renderer = from_handle(current_message_state.hwnd);
			if (renderer) return renderer->on_draw_frame_control(&current_message_state, dc, rc, type, state);
			return hive.orig.DrawFrameControl(dc, rc, type, state);
		}

		inline static BOOL fire_frame_rect(HDC dc, LPCRECT rc, HBRUSH brush)
		{
			auto renderer = from_handle(current_message_state.hwnd);
			if (renderer) return renderer->on_frame_rect(&current_message_state, dc, rc, brush);
			return hive.orig.FrameRect(dc, rc, brush);
		}

		inline static BOOL fire_draw_edge(HDC dc, LPRECT rc, UINT edge, UINT flags)
		{
			auto renderer = from_handle(current_message_state.hwnd);
			if (renderer) return renderer->on_draw_edge(&current_message_state, dc, rc, edge, flags);
			return hive.orig.DrawEdge(dc, rc, edge, flags);
		}

		inline static BOOL fire_draw_focus_rect(HDC dc, LPCRECT rc)
		{
			auto renderer = from_handle(current_message_state.hwnd);
			if (renderer) return renderer->on_draw_focus_rect(&current_message_state, dc, rc);
			return hive.orig.DrawFocusRect(dc, rc);
		}

		inline static BOOL fire_draw_state_w(HDC dc, HBRUSH fore, DRAWSTATEPROC cb, LPARAM lData, WPARAM wData, int x, int y, int cx, int cy, UINT flags)
		{
			auto renderer = from_handle(current_message_state.hwnd);
			if (renderer) return renderer->on_draw_state_w(&current_message_state, dc, fore, cb, lData, wData, x, y, cx, cy, flags);
			return hive.orig.DrawStateW(dc, fore, cb, lData, wData, x, y, cx, cy, flags);
		}

		inline static BOOL fire_ext_text_out_w(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx)
		{
			auto hwnd = current_message_state.hwnd;
			MY_TRACE_HWND(hwnd);

			auto renderer = from_handle(current_message_state.hwnd);
			if (renderer) return renderer->on_ext_text_out_w(&current_message_state, dc, x, y, options, rc, text, c, dx);
			return hive.orig.ExtTextOutW(dc, x, y, options, rc, text, c, dx);
		}

		inline static BOOL fire_draw_text_ex_w(HDC dc, LPWSTR text, int c, LPRECT rc, UINT flags, LPDRAWTEXTPARAMS dtp)
		{
			auto hwnd = current_message_state.hwnd;
			MY_TRACE_HWND(hwnd);

			// ダイアログとマークされているウィンドウはダイアログとして描画します。
			if (::GetPropW(hwnd, k_prop_is_dialog))
			{
				const auto& palette = paint::dialog_material.palette;

				auto part_id = WP_DIALOG;
				auto state_id = ETS_NORMAL;

				if (auto pigment = palette.get(part_id, state_id))
					return paint::stylus.d2d_draw_text(dc, rc, text, c, flags, pigment);
			}

			auto renderer = from_handle(current_message_state.hwnd);
			if (renderer) return renderer->on_draw_text_ex_w(&current_message_state, dc, text, c, rc, flags, dtp);
			return hive.orig.DrawTextExW(dc, text, c, rc, flags, dtp);
		}

		inline static BOOL fire_pat_blt(HDC dc, int x, int y, int w, int h, DWORD rop)
		{
			auto renderer = from_handle(current_message_state.hwnd);
			if (renderer) return renderer->on_pat_blt(&current_message_state, dc, x, y, w, h, rop);
			return hive.orig.PatBlt(dc, x, y, w, h, rop);
		}

		inline static COLORREF fire_get_sys_color(int color_id)
		{
			auto hwnd = current_message_state.hwnd;
			MY_TRACE_HWND(hwnd);

			auto renderer = from_handle(current_message_state.hwnd);
			if (renderer) return renderer->on_get_sys_color(color_id);
			return hive.orig.GetSysColor(color_id);
		}

		inline static HBRUSH fire_get_sys_color_brush(int color_id)
		{
			auto hwnd = current_message_state.hwnd;
			MY_TRACE_HWND(hwnd);

			auto renderer = from_handle(current_message_state.hwnd);
			if (renderer) return renderer->on_get_sys_color_brush(color_id);
			return hive.orig.GetSysColorBrush(color_id);
		}

		//
		// ダイアログ用のブラシを返します。
		//
		inline static HBRUSH get_dialog_brush(HWND hwnd, UINT message, HDC dc, HWND control, HBRUSH brush)
		{
			const auto& palette = paint::dialog_material.palette;

			auto part_id = WP_DIALOG;
			auto state_id = ::IsWindowEnabled(hwnd) ? ETS_NORMAL : ETS_DISABLED;

			if (auto pigment = palette.get(part_id, state_id))
				return pigment->background.get_brush();

			return brush;
		}

		//
		// ダイアログの背景を描画します。
		//
		inline static BOOL draw_dialog_background(HDC dc, LPCRECT rc)
		{
#if 0
			const auto& palette = paint::button_material.palette;

			auto part_id = BP_PUSHBUTTON;
			auto state_id = PBS_NORMAL;
#else
			const auto& palette = paint::dialog_material.palette;

			auto part_id = WP_DIALOG;
			auto state_id = ETS_NORMAL;
#endif
			if (auto pigment = palette.get(part_id, state_id))
				return paint::stylus.draw_rect(dc, rc, pigment);

			return FALSE;
		}

		//
		// ダイアログのテキストを描画します。
		//
		inline static BOOL draw_dialog_text(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx)
		{
			const auto& palette = paint::dialog_material.palette;

			auto part_id = WP_DIALOG;
			auto state_id = ETS_NORMAL;

			if (auto pigment = palette.get(part_id, state_id))
				return paint::stylus.ext_text_out(dc, x, y, options, rc, text, c, dx, pigment);

			return FALSE;
		}

		//
		// ボタンの背景を描画します。
		//
		inline static BOOL draw_button_background(HDC dc, LPCRECT rc)
		{
			const auto& palette = paint::button_material.palette;

			auto part_id = BP_PUSHBUTTON;
			auto state_id = PBS_NORMAL;

			if (auto pigment = palette.get(part_id, state_id))
				return paint::stylus.draw_rect(dc, rc, pigment);

			return FALSE;
		}

		//
		// 非クライアント領域を描画します。
		//
		virtual LRESULT draw_nc_paint(HWND hwnd, HDC dc, const POINT& origin, LPRECT rc)
		{
			auto style = my::get_style(hwnd);
			auto ex_style = my::get_ex_style(hwnd);

			if (ex_style & WS_EX_WINDOWEDGE)
				paint::draw_raised_edge(dc, rc), ::InflateRect(rc, -2, -2);
			else if (style & WS_BORDER)
				paint::draw_single_border(dc, rc), ::InflateRect(rc, -1, -1);

			if (ex_style & WS_EX_STATICEDGE)
				paint::draw_single_sunken_edge(dc, rc), ::InflateRect(rc, -1, -1);

			if (ex_style & WS_EX_CLIENTEDGE)
				paint::draw_sunken_edge(dc, rc), ::InflateRect(rc, -2, -2);

			if ((style & WS_HSCROLL) && (style & WS_VSCROLL))
			{
				auto corner_rc = *rc;
				corner_rc.top = corner_rc.bottom - ::GetSystemMetrics(SM_CYHSCROLL);
				corner_rc.left = corner_rc.right - ::GetSystemMetrics(SM_CXVSCROLL);

				const auto& palette = paint::dialog_material.palette;

				auto part_id = WP_DIALOG;
				auto state_id = ETS_NORMAL;

				if (auto pigment = palette.get(part_id, state_id))
					paint::stylus.draw_rect(dc, &corner_rc, pigment);
			}

			return 0;
		}

		virtual BOOL on_attach(HWND hwnd) { return TRUE; }
		virtual BOOL on_detach(HWND hwnd) { return TRUE; }

		virtual LRESULT on_subclass_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			return orig_wnd_proc(hwnd, message, w_param, l_param);
		}

		virtual LRESULT on_custom_draw(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			return orig_wnd_proc(hwnd, message, w_param, l_param);
		}

		virtual HBRUSH on_ctl_color(HWND hwnd, UINT message, HDC dc, HWND control, HBRUSH brush)
		{
			return brush;
		}

		virtual BOOL on_rectangle(message_state_t* current_state, HDC dc, int left, int top, int right, int bottom)
		{
			return hive.orig.Rectangle(dc, left, top, right, bottom);
		}

		virtual BOOL on_fill_rect(message_state_t* current_state, HDC dc, LPCRECT rc, HBRUSH brush)
		{
			return hive.orig.FillRect(dc, rc, brush);
		}

		virtual BOOL on_draw_frame(message_state_t* current_state, HDC dc, LPRECT rc, UINT width, UINT type)
		{
			return hive.orig.DrawFrame(dc, rc, width, type);
		}

		virtual BOOL on_draw_frame_control(message_state_t* current_state, HDC dc, LPRECT rc, UINT type, UINT state)
		{
			return hive.orig.DrawFrameControl(dc, rc, type, state);
		}

		virtual BOOL on_frame_rect(message_state_t* current_state, HDC dc, LPCRECT rc, HBRUSH brush)
		{
			return hive.orig.FrameRect(dc, rc, brush);
		}

		virtual BOOL on_draw_edge(message_state_t* current_state, HDC dc, LPRECT rc, UINT edge, UINT flags)
		{
			return hive.orig.DrawEdge(dc, rc, edge, flags);
		}

		virtual BOOL on_draw_focus_rect(message_state_t* current_state, HDC dc, LPCRECT rc)
		{
			return hive.orig.DrawFocusRect(dc, rc);
		}

		virtual BOOL on_draw_state_w(message_state_t* current_state, HDC dc, HBRUSH fore, DRAWSTATEPROC cb, LPARAM lData, WPARAM wData, int x, int y, int cx, int cy, UINT flags)
		{
			return hive.orig.DrawStateW(dc, fore, cb, lData, wData, x, y, cx, cy, flags);
		}

		virtual BOOL on_ext_text_out_w(message_state_t* current_state, HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx)
		{
			return hive.orig.ExtTextOutW(dc, x, y, options, rc, text, c, dx);
		}

		virtual BOOL on_draw_text_ex_w(message_state_t* current_state, HDC dc, LPWSTR text, int c, LPRECT rc, UINT flags, LPDRAWTEXTPARAMS dtp)
		{
			return hive.orig.DrawTextExW(dc, text, c, rc, flags, dtp);
		}

		virtual BOOL on_pat_blt(message_state_t* current_state, HDC dc, int x, int y, int w, int h, DWORD rop)
		{
			return hive.orig.PatBlt(dc, x, y, w, h, rop);
		}

		virtual COLORREF on_get_sys_color(int color_id)
		{
			return hive.orig.GetSysColor(color_id);
		}

		virtual HBRUSH on_get_sys_color_brush(int color_id)
		{
			return hive.orig.GetSysColorBrush(color_id);
		}
	};
}
