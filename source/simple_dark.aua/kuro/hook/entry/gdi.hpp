#pragma once

namespace apn::dark::kuro::hook
{
	//
	// このクラスはGDIのフックを担当します。
	//
	inline struct gdi_t : entry_t::node_t
	{
		//
		// 初期化処理を実行します。
		//
		virtual BOOL on_init() override
		{
			MY_TRACE_FUNC("");

			auto gdi32 = ::GetModuleHandleW(L"gdi32.dll");
			MY_TRACE_HEX(gdi32);

			auto user32 = ::GetModuleHandleW(L"user32.dll");
			MY_TRACE_HEX(user32);

			// APIフックを開始します。
			{
				my::hook::detours detours;
				my::hook::attach(Rectangle);
				my::hook::attach(FillRect);
				my::hook::attach(DrawEdge);
				my::hook::attach(DrawFocusRect);
				my::hook::attach(ExtTextOutW);
				my::hook::attach(GetSysColor);
				my::hook::attach(GetSysColorBrush);
				my::hook::attach(DrawStateW);
				my::hook::attach(GrayStringW);
				my::hook::attach(DrawTextW);
				my::hook::attach(DrawTextExW);
#if 0 // テスト用コードです。
				my::hook::attach(DrawFrame, ::GetProcAddress(user32, "DrawFrame"));
				my::hook::attach(DrawFrameControl);
				my::hook::attach(FrameRect);
				my::hook::attach(DrawMenuBar);
				my::hook::attach(ExtTextOutA);
				my::hook::attach(PatBlt);
				my::hook::attach(PolyPatBlt, ::GetProcAddress(gdi32, "PolyPatBlt"));
				my::hook::attach(BitBlt);
				my::hook::attach(GetWindowLongW);
				my::hook::attach(GetWindowLongPtrW);
#endif
			}

			// フック開始後にオリジナルの関数を取得しておきます。
			hive.orig.Rectangle = Rectangle.orig_proc;
			hive.orig.FillRect = FillRect.orig_proc;
			hive.orig.DrawFrame = DrawFrame.orig_proc;
			hive.orig.DrawFrameControl = DrawFrameControl.orig_proc;
			hive.orig.FrameRect = FrameRect.orig_proc;
			hive.orig.DrawEdge = DrawEdge.orig_proc;
			hive.orig.DrawFocusRect = DrawFocusRect.orig_proc;
			hive.orig.DrawStateW = DrawStateW.orig_proc;
			hive.orig.GrayStringW = GrayStringW.orig_proc;
			hive.orig.DrawTextW = DrawTextW.orig_proc;
			hive.orig.DrawTextExW = DrawTextExW.orig_proc;
			hive.orig.DrawMenuBar = DrawMenuBar.orig_proc;
			hive.orig.ExtTextOutW = ExtTextOutW.orig_proc;
			hive.orig.PatBlt = PatBlt.orig_proc;
			hive.orig.PolyPatBlt = PolyPatBlt.orig_proc;
			hive.orig.GetSysColor = GetSysColor.orig_proc;
			hive.orig.GetSysColorBrush = GetSysColorBrush.orig_proc;

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL on_exit() override
		{
			MY_TRACE_FUNC("");

			// APIフックを終了します。
			{
				my::hook::detours detours;
				my::hook::detach(Rectangle);
				my::hook::detach(FillRect);
				my::hook::detach(DrawEdge);
				my::hook::detach(DrawFocusRect);
				my::hook::detach(ExtTextOutW);
				my::hook::detach(GetSysColor);
				my::hook::detach(GetSysColorBrush);
				my::hook::detach(DrawTextW);
				my::hook::detach(DrawTextExW);
			}

			return TRUE;
		}

		//
		// このクラスは::CallWindowProcInternal()をフックします。
		//
		struct {
			inline static LRESULT WINAPI hook_proc(void* u1, WNDPROC wnd_proc, HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
//			inline static LRESULT WINAPI hook_proc(void* u1, WNDPROC wnd_proc, HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param, void* u2, void* u3)
			{
				// この関数はテスト用コードです。

				switch (message)
				{
				case WM_CREATE:
					{
						auto class_name = my::get_class_name(hwnd);

						MY_TRACE_FUNC("{/hex}, {/}, {/}, {/}, {/}, {/}",
							hwnd, class_name, message, w_param, l_param, ::GetCurrentThreadId());

						break;
					}
				}

				return orig_proc(u1, wnd_proc, hwnd, message, w_param, l_param);
//				return orig_proc(u1, wnd_proc, hwnd, message, w_param, l_param, u2, u3);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} CallWindowProcInternal;

		//
		// このクラスは::Rectangle()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, int left, int top, int right, int bottom)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/}",
					ret_addr(&dc), dc, left, top, right, bottom);

				if (hive.is_valid_thread())
					return gdi::renderer_t::fire_rectangle(dc, left, top, right, bottom);

				return orig_proc(dc, left, top, right, bottom);
			}
			inline static decltype(&hook_proc) orig_proc = ::Rectangle;
		} Rectangle;

		//
		// このクラスは::FillRect()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, LPCRECT rc, HBRUSH brush)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, ({/}), {/hex} : {/}",
					ret_addr(&dc), dc, safe_string(rc), brush, ::GetCurrentThreadId());

				if (theme::renderer_t::is_gdi_hook_locked())
				{
					MY_TRACE("GDIフックがロックされています\n");

					return orig_proc(dc, rc, brush);
				}

				if (hive.is_valid_thread())
					return gdi::renderer_t::fire_fill_rect(dc, rc, brush);

				return orig_proc(dc, rc, brush);
			}
			inline static decltype(&hook_proc) orig_proc = ::FillRect;
		} FillRect;

		//
		// このクラスは::DrawFrame()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, LPRECT rc, UINT width, UINT type)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, ({/}), {/}, {/hex}",
					ret_addr(&dc), dc, safe_string(rc), width, type);

				if (hive.is_valid_thread())
					return gdi::renderer_t::fire_draw_frame(dc, rc, width, type);

				return orig_proc(dc, rc, width, type);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} DrawFrame;

		//
		// このクラスは::DrawFrameControl()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, LPRECT rc, UINT type, UINT state)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, ({/}), {/hex}, {/hex}",
					ret_addr(&dc), dc, safe_string(rc), type, state);

				if (hive.is_valid_thread())
					return gdi::renderer_t::fire_draw_frame_control(dc, rc, type, state);

				return orig_proc(dc, rc, type, state);
			}
			inline static decltype(&hook_proc) orig_proc = ::DrawFrameControl;
		} DrawFrameControl;

		//
		// このクラスは::FrameRect()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, LPCRECT rc, HBRUSH brush)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, ({/}), {/hex}",
					ret_addr(&dc), dc, safe_string(rc), brush);

				if (hive.is_valid_thread())
					return gdi::renderer_t::fire_frame_rect(dc, rc, brush);

				return orig_proc(dc, rc, brush);
			}
			inline static decltype(&hook_proc) orig_proc = ::FrameRect;
		} FrameRect;

		//
		// このクラスは::DrawEdge()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, LPRECT rc, UINT edge, UINT flags)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, ({/}), {/hex}, {/hex}",
					ret_addr(&dc), dc, safe_string(rc), edge, flags);

				if (hive.is_valid_thread())
					return gdi::renderer_t::fire_draw_edge(dc, rc, edge, flags);

				return orig_proc(dc, rc, edge, flags);
			}
			inline static decltype(&hook_proc) orig_proc = ::DrawEdge;
		} DrawEdge;

		//
		// このクラスは::DrawFocusRect()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, LPCRECT rc)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, ({/})",
					ret_addr(&dc), dc, safe_string(rc));

				if (hive.is_valid_thread())
					return gdi::renderer_t::fire_draw_focus_rect(dc, rc);

				return orig_proc(dc, rc);
			}
			inline static decltype(&hook_proc) orig_proc = ::DrawFocusRect;
		} DrawFocusRect;

		//
		// このクラスは::DrawStateW()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, HBRUSH fore, DRAWSTATEPROC cb, LPARAM lData, WPARAM wData, int x, int y, int cx, int cy, UINT flags)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/}, {/}, {/}, {/}, {/hex}",
					ret_addr(&dc), dc, fore, x, y, cx, cy, flags);

				if (hive.is_valid_thread())
					return gdi::renderer_t::fire_draw_state_w(dc, fore, cb, lData, wData, x, y, cx, cy, flags);

				return orig_proc(dc, fore, cb, lData, wData, x, y, cx, cy, flags);
			}
			inline static decltype(&hook_proc) orig_proc = ::DrawStateW;
		} DrawStateW;

		//
		// このクラスは::GrayStringW()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, HBRUSH brush,
				GRAYSTRINGPROC output_func, LPARAM data, int c, int x, int y, int w, int h)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/}, {/}, {/}, {/}, {/}",
					ret_addr(&dc), dc, brush, c, x, y, w, h);

				return orig_proc(dc, brush, output_func, data, c, x, y, w, h);
			}
			inline static decltype(&hook_proc) orig_proc = ::GrayStringW;
		} GrayStringW;

		//
		// このクラスは::DrawTextW()をフックします。
		//
		struct {
			inline static int WINAPI hook_proc(HDC dc, LPCWSTR text, int c, LPRECT rc, UINT flags)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, {/}, ({/}), {/hex}",
					ret_addr(&dc), dc, safe_string(text, c), safe_string(rc), flags);

				if (hive.is_valid_thread())
					return gdi::renderer_t::fire_draw_text_ex_w(dc, (LPWSTR)text, c, rc, flags, nullptr);

				return orig_proc(dc, text, c, rc, flags);
			}
			inline static decltype(&hook_proc) orig_proc = ::DrawTextW;
		} DrawTextW;

		//
		// このクラスは::DrawTextExW()をフックします。
		//
		struct {
			inline static int WINAPI hook_proc(HDC dc, LPWSTR text, int c, LPRECT rc, UINT flags, LPDRAWTEXTPARAMS dtp)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, {/}, ({/}), {/hex}, {/hex}",
					ret_addr(&dc), dc, safe_string(text, c), safe_string(rc), flags, dtp);

				if (hive.is_valid_thread())
					return gdi::renderer_t::fire_draw_text_ex_w(dc, text, c, rc, flags, dtp);

				return orig_proc(dc, text, c, rc, flags, dtp);
			}
			inline static decltype(&hook_proc) orig_proc = ::DrawTextExW;
		} DrawTextExW;

		//
		// このクラスは::DrawMenuBar()をフックします。
		//
		struct {
			inline static int WINAPI hook_proc(HWND hwnd)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}", ret_addr(&hwnd), hwnd);

				return orig_proc(hwnd);
			}
			inline static decltype(&hook_proc) orig_proc = ::DrawMenuBar;
		} DrawMenuBar;

		//
		// このクラスは::ExtTextOutA()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCSTR text, UINT c, CONST INT* dx)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/hex}, ({/}), {/} : {/}",
					ret_addr(&dc), dc, x, y, options, safe_string(rc), c, ::GetCurrentThreadId());

				return orig_proc(dc, x, y, options, rc, text, c, dx);
			}
			inline static decltype(&hook_proc) orig_proc = ::ExtTextOutA;
		} ExtTextOutA;

		//
		// このクラスは::ExtTextOutW()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx)
			{
				// ::ExtTextOut()のフックがロックされている場合は
				if (ext_text_out_lock.is_locked())
				{
//					MY_TRACE_FUNC("再帰呼び出し時はフックしません\n");

					return orig_proc(dc, x, y, options, rc, text, c, dx);
				}

				// ::ExtTextOut()のフックをロックします。
				my::locker_t locker(&ext_text_out_lock);

				MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/hex}, ({/}), {/}, {/} : {/}",
					ret_addr(&dc), dc, x, y, options, safe_string(rc),
					safe_string(text, c, options), c, ::GetCurrentThreadId());

				if (hive.is_valid_thread())
				{
					{
						// コマンドモジュールDCがセットされている場合は
						if (dc == paint::command_module_material.dc)
						{
							MY_TRACE("コマンドモジュールを描画します\n");

							// コマンドモジュールのテキストを描画します。
							if (gdi::renderer_t::draw_dialog_text(dc, x, y, options, rc, text, c, dx))
								return TRUE;
						}
						else
						{
							// コマンドモジュールDCをリセットします。
							paint::command_module_material.dc = {};
						}
					}

					if (theme::renderer_t::is_gdi_hook_locked())
					{
						MY_TRACE("GDIフックがロックされています\n");

						return orig_proc(dc, x, y, options, rc, text, c, dx);
					}

					return gdi::renderer_t::fire_ext_text_out_w(dc, x, y, options, rc, text, c, dx);
				}

				return orig_proc(dc, x, y, options, rc, text, c, dx);
			}
			inline static decltype(&hook_proc) orig_proc = ::ExtTextOutW;
		} ExtTextOutW;

		//
		// このクラスは::PatBlt()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, int x, int y, int w, int h, DWORD rop)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/}, {/hex}",
					ret_addr(&dc), dc, x, y, w, h, rop);

				if (hive.is_valid_thread())
					return gdi::renderer_t::fire_pat_blt(dc, x, y, w, h, rop);

				return orig_proc(dc, x, y, w, h, rop);
			}
			inline static decltype(&hook_proc) orig_proc = ::PatBlt;
		} PatBlt;

		//
		// このクラスは::PolyPatBlt()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, DWORD rop, const PATRECT* rects, int nb_rects, ULONG reserved)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/}",
					ret_addr(&dc), dc, rop, nb_rects);

				return orig_proc(dc, rop, rects, nb_rects, reserved);
			}
			inline static decltype(&hook_proc) orig_proc = {};
		} PolyPatBlt;

		//
		// このクラスは::BitBlt()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, int x, int y, int w, int h, HDC src_dc, int src_x, int src_y, DWORD rop)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/}, {/hex}, {/}, {/}, {/hex}",
					ret_addr(&dc), dc, x, y, w, h, src_dc, src_x, src_y, rop);

				return orig_proc(dc, x, y, w, h, src_dc, src_x, src_y, rop);
			}
			inline static decltype(&hook_proc) orig_proc = ::BitBlt;
		} BitBlt;

		//
		// このクラスは::GetSysColor()をフックします。
		//
		struct {
			inline static COLORREF WINAPI hook_proc(int color_id)
			{
				MY_TRACE_FUNC("{/hex}, {/}", ret_addr(&color_id), color_id);

				if (hive.is_valid_thread())
					return gdi::renderer_t::fire_get_sys_color(color_id);

				return orig_proc(color_id);
			}
			inline static decltype(&hook_proc) orig_proc = ::GetSysColor;
		} GetSysColor;

		//
		// このクラスは::GetSysColorBrush()をフックします。
		//
		struct {
			inline static HBRUSH WINAPI hook_proc(int color_id)
			{
				MY_TRACE_FUNC("{/hex}, {/}", ret_addr(&color_id), color_id);

				if (hive.is_valid_thread())
					return gdi::renderer_t::fire_get_sys_color_brush(color_id);

				return orig_proc(color_id);
			}
			inline static decltype(&hook_proc) orig_proc = ::GetSysColorBrush;
		} GetSysColorBrush;

		//
		// このクラスは::GetWindowLongW()をフックします。
		//
		struct {
			inline static LONG WINAPI hook_proc(HWND hwnd, int index)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, {/}", ret_addr(&hwnd), hwnd, index);

				return orig_proc(hwnd, index);
			}
			inline static decltype(&hook_proc) orig_proc = ::GetWindowLongW;
		} GetWindowLongW;
#if 0
		//
		// このクラスは::GetWindowLongPtrW()をフックします。
		//
		struct {
			inline static LONG_PTR WINAPI hook_proc(HWND hwnd, int index)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, {/}", ret_addr(&hwnd), hwnd, index);

				return orig_proc(hwnd, index);
			}
			inline static decltype(&hook_proc) orig_proc = ::GetWindowLongPtrW;
		} GetWindowLongPtrW;
#endif
	} gdi;
}
