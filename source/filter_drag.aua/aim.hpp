#pragma once

namespace apn::filter_drag
{
	inline struct Aim : my::Window
	{
		BOOL available = FALSE;
		int alpha = 0;
		COLORREF color = CLR_NONE;

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			constexpr auto class_name = _T("apn::filter_drag::aim");

			WNDCLASS wc = {};
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_NOCLOSE;
			wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = hive.instance;
			wc.lpszClassName = class_name;
			::RegisterClass(&wc);

			return __super::create(
				WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
				class_name,
				class_name,
				WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				0, 0, 0, 0,
				nullptr, nullptr, hive.instance, nullptr);
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return destroy();
		}

		//
		// エイムが使用可能な場合はTRUEを返します。
		//
		BOOL is_available() const
		{
			return available && ::IsWindow(*this);
		}

		//
		// エイムを表示します。
		//
		void show(const Layout& layout, const FilterHolder& filter)
		{
			auto name = filter.get_name();
			::SetWindowTextA(*this, name);
			::InvalidateRect(*this, nullptr, FALSE);

			auto rc = layout.get_filter_rect(filter);
			auto pos = POINT { rc.left, rc.top };
			auto size = SIZE { rc.right - rc.left, rc.bottom - rc.top };
			::ClientToScreen(magi.exin.get_setting_dialog(), &pos);
			::SetLayeredWindowAttributes(*this, 0, alpha, LWA_ALPHA);
			::SetWindowPos(*this, HWND_TOPMOST, pos.x, pos.y, size.cx, size.cy, SWP_NOACTIVATE | SWP_SHOWWINDOW);
		}

		//
		// エイムを非表示にします。
		//
		void hide()
		{
			::ShowWindow(*this, SW_HIDE);
		}

		//
		// テキストを描画します。
		//
		static void draw_text(HDC dc, LPCWSTR text, int c, LPRECT rc, UINT format)
		{
			::SetBkMode(dc, TRANSPARENT);
			::SetTextColor(dc, RGB(0xff, 0xff, 0xff));
			::SetBkColor(dc, RGB(0x00, 0x00, 0x00));
			::DrawTextW(dc, text, -1, rc, format);
		}

		//
		// エイムのウィンドウプロシージャです。
		//
		LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_PAINT:
				{
					auto rc = my::get_client_rect(hwnd);
					my::PaintDC pdc(hwnd);
					my::UxDC dc(pdc, &rc, BPBF_COMPATIBLEBITMAP);

					if (dc.is_valid())
					{
						{
							// 背景をcolorで塗りつぶします。

							my::gdi::unique_ptr<HBRUSH> brush(::CreateSolidBrush(color));

							::FillRect(dc, &rc, brush.get());
						}

						{
							// ウィンドウテキストを描画します。

							auto text = my::get_window_text(hwnd);

							draw_text(dc, text.c_str(), -1, &rc, DT_CENTER | DT_TOP | DT_NOCLIP);
						}
					}

					return 0;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} aim_src, aim_dst;
}
