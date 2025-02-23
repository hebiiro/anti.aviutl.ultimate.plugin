#pragma once

namespace apn::workspace
{
	//
	// 指定されたウィンドウを持つフィルタを返します。
	//
	inline AviUtl::FilterPlugin* get_filter(HWND hwnd)
	{
		// magi.fpがまだ設定されていない場合は失敗します。
		if (!magi.fp) return nullptr;

		// aviutlのシステム情報を取得します。
		AviUtl::SysInfo si = {};
		magi.fp->exfunc->get_sys_info(nullptr, &si);

		// 全てのフィルタを列挙します。
		for (auto i = 0; i < si.filter_n; i++)
		{
			// フィルタを取得します。
			auto fp = magi.fp->exfunc->get_filterp(i);
			if (!fp) continue; // フィルタが無効の場合は何もしません。

			// フィルタのウィンドウが指定されたウィンドウと同じ場合は
			if (fp->hwnd == hwnd) return fp; // このフィルタを返します。
		}

		return nullptr;
	}

	//
	// 強制的にWM_SIZEを送信する::SetWindowPos()です。
	//
	inline BOOL set_window_pos_force(HWND hwnd, HWND insert_after, int x, int y, int w, int h, UINT flags)
	{
		auto old_rc = my::get_window_rect(hwnd);
		auto result = ::SetWindowPos(hwnd, insert_after, x, y, w, h, flags);

		if (!(flags & SWP_NOSIZE))
		{
			auto new_rc = my::get_window_rect(hwnd);

			// ウィンドウサイズが変更されなかった場合は
			if (my::get_width(old_rc) == my::get_width(new_rc) &&
				my::get_height(old_rc) == my::get_height(new_rc))
			{
				// 手動でWM_SIZEを送信します。
				::SendMessage(hwnd, WM_SIZE, 0, 0);
			}
		}

		return result;
	}

	inline BOOL set_window_pos_force(HWND hwnd, const WINDOWPOS* wp)
	{
		return set_window_pos_force(hwnd, wp->hwndInsertAfter, wp->x, wp->y, wp->cx, wp->cy, wp->flags);
	}

	inline BOOL set_window_pos_force(HWND hwnd, LPCRECT rc, UINT flags = SWP_NOZORDER | SWP_NOACTIVATE)
	{
		return set_window_pos_force(hwnd, nullptr, rc->left, rc->top, my::get_width(*rc), my::get_height(*rc), flags);
	}

	inline BOOL set_window_pos(HWND hwnd, const WINDOWPOS* wp)
	{
		return ::SetWindowPos(hwnd, wp->hwndInsertAfter, wp->x, wp->y, wp->cx, wp->cy, wp->flags);
	}

	inline BOOL set_window_pos(HWND hwnd, LPCRECT rc, UINT flags = SWP_NOZORDER | SWP_NOACTIVATE)
	{
		return ::SetWindowPos(hwnd, nullptr, rc->left, rc->top, my::get_width(*rc), my::get_height(*rc), flags);
	}

	inline WINDOWPOS rc_to_wp(LPCRECT rc, UINT flags = SWP_NOZORDER | SWP_NOACTIVATE)
	{
		return { nullptr, nullptr, rc->left, rc->top, my::get_width(*rc), my::get_height(*rc), flags };
	}

	inline RECT wp_to_rc(const WINDOWPOS* wp)
	{
		return { wp->x, wp->y, wp->x + wp->cx, wp->y + wp->cy };
	}
}
