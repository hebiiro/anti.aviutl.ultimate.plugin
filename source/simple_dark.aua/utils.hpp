#pragma once

namespace apn::dark
{
	// リターン先のアドレスを返します。
	//
	inline LRESULT CALLBACK get_ret_addr(HWND hwnd, UINT message, WPARAM wp, LPARAM lp)
	{
		return (LRESULT)(&hwnd)[-1];
	}

	//
	// リターンアドレスを返します。
	//
	inline constexpr auto ret_addr(auto* arg1)
	{
		return reinterpret_cast<my::addr_t*>(arg1)[-1];
	}

	inline std::string safe_string(LPCSTR name)
	{
		if ((ULONG_PTR)name > 0x0000FFFF)
			return name;
		else
			return my::format("{/hex}", (my::addr_t)name);
	}

	inline std::wstring safe_string(LPCWSTR name)
	{
		if ((ULONG_PTR)name > 0x0000FFFF)
			return name;
		else
			return my::format(L"{/hex}", (my::addr_t)name);
	}

	inline std::wstring safe_string(LPCRECT rc)
	{
		if (rc)
			return my::format(L"{/}, {/}, {/}, {/}", rc->left, rc->top, rc->right, rc->bottom);
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

	//
	// 指定されたウィンドウとその子ウィンドウを再描画します。
	//
	BOOL redraw_window(HWND hwnd)
	{
//		MY_TRACE_FUNC("{/hex}", hwnd);

		// ウィンドウキャプションを再描画します。
		if (::GetWindowLong(hwnd, GWL_STYLE) & WS_CAPTION)
			::SendMessage(hwnd, WM_ACTIVATE, hwnd == ::GetActiveWindow(), 0);

		// ウィンドウを再描画します。
		::RedrawWindow(hwnd, 0, 0,
			RDW_ERASE | RDW_FRAME | RDW_INTERNALPAINT |
			RDW_INVALIDATE | RDW_ALLCHILDREN);

		// 子孫ウィンドウを列挙します。
		return ::EnumChildWindows(hwnd,
			[](HWND hwnd, LPARAM l_param)
		{
			// 子孫ウィンドウのクラス名を取得します。
			auto class_name = my::get_class_name(hwnd);

			// トラックバーの場合は
			if (class_name == TRACKBAR_CLASS)
			{
				// トラックバー専用の再描画処理を実行します。
				::SendMessage(hwnd, WM_SETFOCUS, 0, 0);
			}
			// ボタンの場合は
			else if (class_name == WC_BUTTON)
			{
				// ボタン専用の再描画処理を実行します。
				if (auto icon = (HICON)::SendMessage(hwnd, BM_GETIMAGE, IMAGE_ICON, 0))
					::SendMessage(hwnd, BM_SETIMAGE, IMAGE_ICON, (LPARAM)icon);
			}
#if 0
			else
			{
				redraw_window(hwnd);
			}
#endif
			return TRUE;
		}, 0);
	}

	//
	// プロセス内のすべてのウィンドウを再描画します。
	//
	BOOL redraw_windows()
	{
		MY_TRACE_FUNC("");

		return ::EnumWindows(
			[](HWND hwnd, LPARAM l_param)
		{
			auto pid = DWORD {};
			auto tid = ::GetWindowThreadProcessId(hwnd, &pid);

			if (pid == ::GetCurrentProcessId())
				redraw_window(hwnd);

			return TRUE;
		}, 0);
	}

	//
	// このクラスは与えられた矩形をクリッピングします。
	//
	struct clipper_t
	{
		HDC dc = {};
		HRGN rgn = {};

		clipper_t(HDC dc, LPCRECT rc, LPCRECT rc_clip)
			: dc(dc)
		{
			if (rc && rc_clip && !::IsRectEmpty(rc_clip) && !::EqualRect(rc, rc_clip))
			{
				::CreateRectRgn(0, 0, 0, 0);
				::GetClipRgn(dc, rgn);

				auto rc = *rc_clip;
				::LPtoDP(dc, (LPPOINT)&rc, 2);

				my::gdi::unique_ptr<HRGN> rgn(
					::CreateRectRgnIndirect(&rc));

				::ExtSelectClipRgn(dc, rgn.get(), RGN_COPY);
			}
		}

		~clipper_t()
		{
			if (rgn)
			{
				::SelectClipRgn(dc, rgn);
				::DeleteObject(rgn);
			}
		}
	};

	//
	// ::ExtTextOut()のフックをロックします。
	//
	thread_local inline my::lockable_t ext_text_out_lock;
}
