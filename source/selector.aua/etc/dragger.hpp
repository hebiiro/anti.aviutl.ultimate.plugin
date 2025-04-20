#pragma once

namespace apn::selector
{
	//
	// このクラスはドラッグを管理します。
	//
	struct Dragger
	{
		//
		// ドラッグ時に文字列を表示するためのイメージリストです。
		//
		my::image_list::unique_ptr image_list;

		//
		// ドラッグを開始します。
		//
		BOOL start(const std::wstring& text)
		{
			MY_TRACE_FUNC("{/}", text);

			auto padding = POINT { 4, 4 }; // 文字周りのパディングです。
			auto rc = RECT { 0, 0, 400, 40}; // イメージの最大サイズです。
			auto text_rc = rc; ::InflateRect(&text_rc, -padding.x, -padding.y);
			auto text_size = SIZE {};

			{
				auto si = AviUtl::SysInfo {};
				magi.fp->exfunc->get_sys_info(nullptr, &si);

				auto text_color = RGB(0, 0, 0);
				auto fill_color = RGB(255, 255, 255);

				my::ClientDC dc(nullptr);
				my::MemDC mdc(dc, &rc);
				my::gdi::selector font_selector(mdc, si.hfont);

				// 背景を塗りつぶします。
				{
					my::gdi::unique_ptr<HBRUSH> brush(::CreateSolidBrush(fill_color));

					::FillRect(mdc, &rc, brush.get());
				}

				// スクリプト名を描画します。
				{
					auto old_bk_mode = ::SetBkMode(mdc, TRANSPARENT);
					auto old_text_color = ::SetTextColor(mdc, text_color);
					::DrawText(mdc, text.c_str(), -1, &text_rc, DT_END_ELLIPSIS);
					::SetTextColor(mdc, old_text_color);
					::SetBkMode(mdc, old_bk_mode);
				}

				// テキストサイズを取得します。
				{
					::DrawText(mdc, text.c_str(), -1, &text_rc, DT_CALCRECT);
					text_size.cx = std::min<int32_t>(my::get_width(rc), my::get_width(text_rc) + padding.x * 2);
					text_size.cy = std::min<int32_t>(my::get_height(rc), my::get_height(text_rc) + padding.y * 2);
				}

				// テキストサイズのイメージリストを作成します。
				image_list.reset(::ImageList_Create(text_size.cx, text_size.cy, ILC_COLOR, 1, 1));

				// イメージリストの背景色を設定します。
				// ※こうしても背景色は透明になりませんでした。
				::ImageList_SetBkColor(image_list.get(), fill_color);

				// イメージを追加します。
				// ※背景色を与えていますが、透明になりませんでした。
				::ImageList_AddMasked(image_list.get(), mdc.bitmap, fill_color);
			}

			// ホットスポットを取得します。
			// ※デスクトップウィンドウに合わせてdpiを調整しています。
			auto dpi = ::GetDpiForWindow(::GetDesktopWindow());
			auto hot_spot = POINT {
				::MulDiv(text_size.cx / 2, USER_DEFAULT_SCREEN_DPI, dpi),
				::MulDiv(text_size.cy + padding.y, USER_DEFAULT_SCREEN_DPI, dpi),
			};

			// イメージリストのドラッグを開始します。
			// ※ドラッグを開始する位置は本来はウィンドウ左上からの相対座標ですが、
			// 今回はデスクトップウィンドウを基準にしているのでマウスカーソル位置をそのまま使用しています。
			auto point = my::get_cursor_pos();
			::ImageList_BeginDrag(image_list.get(), 0, hot_spot.x, hot_spot.y);
			::ImageList_DragEnter(nullptr, point.x, point.y);

			return TRUE;
		}

		//
		// ドラッグします。
		//
		BOOL move()
		{
			MY_TRACE_FUNC("");

			// マウスカーソルの位置を取得します。
			auto point = my::get_cursor_pos();

			// イメージリストをドラッグします。
			::ImageList_DragMove(point.x, point.y);

			return TRUE;
		}

		//
		// ドラッグを終了します。
		//
		BOOL stop()
		{
			MY_TRACE_FUNC("");

			// イメージリストのドラッグを終了します。
			::ImageList_EndDrag();
			::ImageList_DragLeave(nullptr);

			// イメージリストを削除します。
			image_list.reset();

			return TRUE;
		}
	};
}
