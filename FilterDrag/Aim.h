#pragma once

namespace fgo::filter_drag
{
	struct Aim : Tools::Window
	{
		int alpha;
		COLORREF color;

		//
		// デストラクタです。
		//
		~Aim()
		{
			destroy();
		}

		//
		// エイムを作成します。
		//
		BOOL create(HINSTANCE instance)
		{
			static const LPCTSTR className = _T("FilterDrag.Aim");

			WNDCLASS wc = {};
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_NOCLOSE;
			wc.hCursor = ::LoadCursor(0, IDC_ARROW);
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = instance;
			wc.lpszClassName = className;
			::RegisterClass(&wc);

			return __super::create(
				WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
				className,
				className,
				WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				0, 0, 0, 0,
				0, 0, instance, 0);
		}

		//
		// エイムを表示します。
		//
		void show(const Layout& layout, FilterHolder filter)
		{
			LPCSTR name = filter.getName();
//			MY_TRACE_STR(name);
			::SetWindowTextA(*this, name);
			::InvalidateRect(*this, 0, FALSE);

			RECT rc; layout.getFilterRect(filter, &rc);
			POINT pos = { rc.left, rc.top };
			SIZE size = { rc.right - rc.left, rc.bottom - rc.top };
			::ClientToScreen(fate.auin.GetSettingDialog(), &pos);
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

		static void drawText(HDC dc, LPCWSTR text, int c, LPRECT rc, UINT format)
		{
			::SetBkMode(dc, TRANSPARENT);
			::SetTextColor(dc, RGB(0xff, 0xff, 0xff));
			::SetBkColor(dc, RGB(0x00, 0x00, 0x00));
			::DrawTextW(dc, text, -1, rc, format);
		}

		//
		// エイムのウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_PAINT:
				{
					PAINTSTRUCT ps;
					HDC dc = ::BeginPaint(hwnd, &ps);
					RECT rc = ps.rcPaint;

					BP_PAINTPARAMS pp = { sizeof(pp) };
					HDC mdc = 0;
					HPAINTBUFFER pb = ::BeginBufferedPaint(dc, &rc, BPBF_COMPATIBLEBITMAP, &pp, &mdc);

					if (pb)
					{
						HDC dc = mdc;

						// 背景を color で塗りつぶす。
						HBRUSH brush = ::CreateSolidBrush(color);
						::FillRect(dc, &ps.rcPaint, brush);
						::DeleteObject(brush);

						// ウィンドウテキストを描画する。
						WCHAR text[MAX_PATH];
						::GetWindowTextW(hwnd, text, MAX_PATH);
						drawText(dc, text, -1, &ps.rcPaint, DT_CENTER | DT_TOP | DT_NOCLIP);

						::EndBufferedPaint(pb, TRUE);
					}

					::EndPaint(hwnd, &ps);

					return 0;
				}
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}
	};
}
