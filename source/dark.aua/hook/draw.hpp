#pragma once

namespace apn::dark::hook
{
	//
	// リターン先のアドレスを返します。
	//
	__declspec(naked) inline void get_ret_addr()
	{
		__asm {
			MOV EAX,[ESP]
			RETN
		}
	}

	//
	// このクラスは描画関係のフックを担当します。
	//
	inline struct Draw
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			auto user32 = ::GetModuleHandle(_T("user32.dll"));
			MY_TRACE_HEX(user32);

			// 2B = 7671339B - 76713370
			my::addr_t address1 = ::CallWindowProcW((WNDPROC)get_ret_addr, nullptr, 0, 0, 0) - 0x2B;
			{
				BYTE code[1] = {};
				::ReadProcessMemory(::GetCurrentProcess(), (LPCVOID)address1, code, sizeof(code), nullptr);
				if (code[0] == 0xCC) address1 += 0x01;
			}
			my::hook::attach(CallWindowProcInternal, address1);

			my::hook::attach(Rectangle);
			my::hook::attach(FillRect);
//			my::hook::attach(DrawFrame, ::GetProcAddress(user32, "DrawFrame"));
//			my::hook::attach(DrawFrameControl);
//			my::hook::attach(FrameRect);
			my::hook::attach(DrawEdge);
			my::hook::attach(DrawFocusRect);
//			my::hook::attach(DrawStateW);
			my::hook::attach(ExtTextOutW);
//			my::hook::attach(PatBlt);

			if (DetourTransactionCommit() != NO_ERROR)
			{
				MY_TRACE("APIフックに失敗しました\n");

				return FALSE;
			}

			hive.orig.CallWindowProcInternal = CallWindowProcInternal.orig_proc;
			hive.orig.Rectangle = Rectangle.orig_proc;
			hive.orig.FillRect = FillRect.orig_proc;
			hive.orig.DrawFrame = DrawFrame.orig_proc;
			hive.orig.DrawFrameControl = DrawFrameControl.orig_proc;
			hive.orig.FrameRect = FrameRect.orig_proc;
			hive.orig.DrawEdge = DrawEdge.orig_proc;
			hive.orig.DrawFocusRect = DrawFocusRect.orig_proc;
			hive.orig.DrawStateW = DrawStateW.orig_proc;
			hive.orig.ExtTextOutW = ExtTextOutW.orig_proc;
			hive.orig.PatBlt = PatBlt.orig_proc;

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// このクラスは::CallWindowProcInternal()をフックします。
		//
		struct {
			inline static LRESULT WINAPI hook_proc(WNDPROC wnd_proc, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
			{
				return gdi::client.on_wnd_proc(wnd_proc, hwnd, message, wParam, lParam);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} CallWindowProcInternal;

		//
		// このクラスは::Rectangle()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, int left, int top, int right, int bottom)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/}", ret_addr(&dc), dc, left, top, right, bottom);
				return gdi::manager.on_rectangle(dc, left, top, right, bottom);
			}
			inline static decltype(&hook_proc) orig_proc = ::Rectangle;
		} Rectangle;

		//
		// このクラスは::FillRect()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, LPCRECT rc, HBRUSH brush)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, ({/}), {/hex}", ret_addr(&dc), dc, safe_string(rc), brush);
				return gdi::manager.on_fill_rect(dc, rc, brush);
			}
			inline static decltype(&hook_proc) orig_proc = ::FillRect;
		} FillRect;

		//
		// このクラスは::DrawFrame()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, LPRECT rc, UINT width, UINT type)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, ({/}), {/}, {/hex}", ret_addr(&dc), dc, safe_string(rc), width, type);
				return gdi::manager.on_draw_frame(dc, rc, width, type);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} DrawFrame;

		//
		// このクラスは::DrawFrameControl()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, LPRECT rc, UINT type, UINT state)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, ({/}), {/hex}, {/hex}", ret_addr(&dc), dc, safe_string(rc), type, state);
				return gdi::manager.on_draw_frame_control(dc, rc, type, state);
			}
			inline static decltype(&hook_proc) orig_proc = ::DrawFrameControl;
		} DrawFrameControl;

		//
		// このクラスは::FrameRect()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, LPCRECT rc, HBRUSH brush)
			{
				auto from = *((my::addr_t*)&dc - 1);
				MY_TRACE_FUNC("{/hex}, {/hex}, ({/}), {/hex}", from, dc, safe_string(rc), brush);
				return gdi::manager.on_frame_rect(dc, rc, brush);
			}
			inline static decltype(&hook_proc) orig_proc = ::FrameRect;
		} FrameRect;

		//
		// このクラスは::DrawEdge()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, LPRECT rc, UINT edge, UINT flags)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, ({/}), {/hex}, {/hex}", ret_addr(&dc), dc, safe_string(rc), edge, flags);
				return gdi::manager.on_draw_edge(dc, rc, edge, flags);
			}
			inline static decltype(&hook_proc) orig_proc = ::DrawEdge;
		} DrawEdge;

		//
		// このクラスは::DrawFocusRect()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, LPCRECT rc)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, ({/})", ret_addr(&dc), dc, safe_string(rc));
				return gdi::manager.on_draw_focus_rect(dc, rc);
			}
			inline static decltype(&hook_proc) orig_proc = ::DrawFocusRect;
		} DrawFocusRect;

		//
		// このクラスは::DrawStateW()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, HBRUSH fore, DRAWSTATEPROC cb, LPARAM lData, WPARAM wData, int x, int y, int cx, int cy, UINT flags)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/}, {/}, {/}, {/}, {/hex}", ret_addr(&dc), dc, fore, x, y, cx, cy, flags);
				return gdi::manager.on_draw_state_w(dc, fore, cb, lData, wData, x, y, cx, cy, flags);
			}
			inline static decltype(&hook_proc) orig_proc = ::DrawStateW;
		} DrawStateW;

		//
		// このクラスは::ExtTextOutW()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx)
			{
				if (ExtTextOutLocker::locked)
				{
//					MY_TRACE_FUNC("再帰呼び出し時はフックしません\n");

					return orig_proc(dc, x, y, options, rc, text, c, dx);
				}
				ExtTextOutLocker locker;

				MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/hex}, ({/}), {/}",
					ret_addr(&dc), dc, x, y, options, safe_string(rc), safe_string(text, c, options));

				return gdi::manager.on_ext_text_out_w(dc, x, y, options, rc, text, c, dx);
			}
			inline static decltype(&hook_proc) orig_proc = ::ExtTextOutW;
		} ExtTextOutW;

		//
		// このクラスは::PatBlt()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, int x, int y, int w, int h, DWORD rop)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/}, {/hex}", ret_addr(&dc), dc, x, y, w, h, rop);
				return gdi::manager.on_pat_blt(dc, x, y, w, h, rop);
			}
			inline static decltype(&hook_proc) orig_proc = ::PatBlt;
		} PatBlt;
	} draw;
}
