﻿#pragma once

namespace apn::font_tree
{
	//
	// このクラスはフォントのプレビューを表示します。
	//
	inline struct PreviewManager : my::Window
	{
		//
		// このクラスはプレビューウィンドウです。
		//
		struct PreviewWindow : my::Window
		{
			//
			// ウィンドウプロシージャです。
			//
			virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
			{
				switch (message)
				{
				case WM_PAINT:
					{
						MY_TRACE_FUNC("WM_PAINT, {/hex}, {/hex}", wParam, lParam);

						auto rc = my::get_client_rect(hwnd);
						my::PaintDC pdc(hwnd);
						my::DoubleBufferDC dc(pdc, &rc);

						preview_manager.draw_preview(dc, rc);

						return 0;
					}
				}

				return __super::on_wnd_proc(hwnd, message, wParam, lParam);
			}
		} preview_window;

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// フォントコンボボックスを取得します。
			auto font_combobox = magi.exin.get_font_combobox();
			if (!font_combobox) return FALSE;

			// フォントコンボボックスのリストボックスをサブクラス化します。
			COMBOBOXINFO cbi = { sizeof(cbi) };
			::GetComboBoxInfo(font_combobox, &cbi);
			if (!subclass(cbi.hwndList)) return FALSE;

			constexpr auto class_name = _T("apn::font_tree::preview_window");

			WNDCLASSEX wc = { sizeof(wc) };
			wc.lpszClassName = class_name;
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = hive.instance;
			wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
			::RegisterClassEx(&wc);

			// プレビューウィンドウを作成します。
			return preview_window.create(
				0,
				class_name,
				class_name,
				WS_POPUP | WS_BORDER | 	WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
				0, 0, 100, 100,
				*this, nullptr, hive.instance, nullptr);
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			// フォントコンボボックスのリストボックスのサブクラス化を解除します。
			unsubclass();

			// プレビューウィンドウを削除します。
			return preview_window.destroy();
		}

		//
		// プレビューウィンドウを表示します。
		//
		void show_preview()
		{
			MY_TRACE_FUNC("");

			if (hive.preview.mode != hive.preview.c_mode.c_none)
				::ShowWindow(preview_window, SW_SHOWNA);
		}

		//
		// プレビューウィンドウを非表示にします。
		//
		void hide_preview()
		{
			MY_TRACE_FUNC("");

			::ShowWindow(preview_window, SW_HIDE);
		}

		//
		// プレビューウィンドウを再描画します。
		//
		void redraw_preview()
		{
			MY_TRACE_FUNC("");

			my::invalidate(preview_window);
		}

		//
		// プレビューウィンドウのレイアウトを更新します。
		//
		void update_preview()
		{
			MY_TRACE_FUNC("");

			// リストボックスのウィンドウ位置を取得します。
			auto base_rc = my::get_window_rect(*this);
			auto x = base_rc.left;
			auto y = base_rc.top;
			auto w = my::get_width(base_rc);
			auto h = my::get_height(base_rc);

			// 非クライアント領域のサイズを取得します。
			auto style = GetWindowStyle(preview_window);
			auto menu = ::GetMenu(preview_window);
			auto ex_style = GetWindowExStyle(preview_window);
			auto nc_rc = RECT {};
			::AdjustWindowRectEx(&nc_rc, style, !!menu, ex_style);
			auto nc_w = my::get_width(nc_rc);
			auto nc_h = my::get_height(nc_rc);

			// リストボックスのスクロール情報を取得します。
			SCROLLINFO si = { sizeof(si) };
			si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE | SIF_TRACKPOS;
			::GetScrollInfo(*this, SB_VERT, &si);

			// ユーザー指定のサイズからウィンドウサイズを算出します。
			auto w2 = hive.preview.item_size.cx + nc_w;
			auto h2 = hive.preview.item_size.cy * (int32_t)si.nPage + nc_h;

			if (hive.preview.item_size.cx)
			{
				if (hive.preview.mode == hive.preview.c_mode.c_left)
					x = base_rc.left - w2;
				else
					x = base_rc.right;
				w = w2;
			}

			if (hive.preview.item_size.cy)
			{
				y += (h - h2) / 2;
				h = h2;
			}

			// プレビューウィンドウを移動します。
			::SetWindowPos(preview_window, HWND_TOPMOST, x, y, w, h, SWP_NOACTIVATE);
		}

		//
		// プレビューウィンドウを描画します。
		//
		void draw_preview(HDC dc, const RECT& rc)
		{
			MY_TRACE_FUNC("");

			// プレビューウィンドウのサイズを取得します。
			auto rc_w = my::get_width(rc);
			auto rc_h = my::get_height(rc);

			// リストボックスのフォント情報を取得します。
			auto font = GetWindowFont(*this);
			auto lf = LOGFONT {};
			::GetObject(font, sizeof(lf), &lf);
			if (hive.preview.item_size.cy) lf.lfHeight = hive.preview.item_size.cy;

			// リストボックスのスクロール情報を取得します。
			SCROLLINFO si = { sizeof(si) };
			si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE | SIF_TRACKPOS;
			::GetScrollInfo(*this, SB_VERT, &si);

			// アイテムのサイズを取得します。
			auto item_rc = RECT {};
			ListBox_GetItemRect(*this, 0, &item_rc);
			auto item_w = my::get_width(item_rc);
			auto item_h = my::get_height(item_rc);

			// 項目の描画位置を算出します。
			auto x = rc.left;
			auto y = rc.top;
			auto w = hive.preview.item_size.cx ? hive.preview.item_size.cx : item_w;
			auto h = hive.preview.item_size.cy ? hive.preview.item_size.cy : item_h;

			// 背景を描画します。
			{
				my::gdi::unique_ptr<HBRUSH> brush(::CreateSolidBrush(hive.preview.fill_color));

				::FillRect(dc, &rc, brush.get());
			}

			// デバイスコンテキストの設定を変更します。
			auto old_bk_mode = ::SetBkMode(dc, TRANSPARENT);
			auto old_text_color = ::SetTextColor(dc, hive.preview.text_color);

			for (int32_t i = 0; i < (int32_t)si.nPage; i++)
			{
				// 項目のテキストを取得します。
				auto text = my::listbox::get_text(*this, i + si.nPos);

				// 項目のテキストが有効の場合は
				if (text.length())
				{
					// 表示用テキストを取得します。
					auto display_text = my::replace(hive.preview.sample_format, L"%font_name%", text);

					// フォントをセットします。
					_tcscpy_s(lf.lfFaceName, text.c_str());
					my::gdi::unique_ptr<HFONT> font(::CreateFontIndirect(&lf));
					my::gdi::selector font_selector(dc, font.get());

					// 表示用テキストを描画します。
					::TextOut(dc, x, y, display_text.c_str(), (int)display_text.length());
				}

				// 次の描画位置に移動します。
				y += h;
			}

			// デバイスコンテキストの設定を元に戻します。
			::SetBkMode(dc, old_bk_mode);
			::SetTextColor(dc, old_text_color);
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			if (::GetKeyState(VK_SHIFT) < 0)
				MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}", message, wParam, lParam);

			switch (message)
			{
			case WM_WINDOWPOSCHANGED:
				{
					MY_TRACE_FUNC("WM_WINDOWPOSCHANGED, {/hex}, {/hex}", wParam, lParam);

					// WM_SHOWWINDOWではリストボックスの表示タイミングを
					// ハンドルすることができないので、代わりにここで処理します。

					auto wp = (WINDOWPOS*)lParam;
					MY_TRACE_HEX(wp->flags);

					if (wp->flags & SWP_ASYNCWINDOWPOS)
					{
						MY_TRACE("リストボックスが表示されました\n");

						update_preview();
						show_preview();
					}

					break;
				}
			case WM_SHOWWINDOW:
				{
					MY_TRACE_FUNC("WM_SHOWWINDOW, {/hex}, {/hex}", wParam, lParam);

					if (wParam)
					{
						// ここには処理がきません。

						MY_TRACE("リストボックスが表示されました\n");

						update_preview();
						show_preview();
					}
					else
					{
						MY_TRACE("リストボックスが非表示になりました\n");

						hide_preview();
					}

					break;
				}
			case WM_PAINT:
			case WM_PRINT:
//			case WM_VSCROLL:
//			case WM_MOUSEWHEEL:
				{
					MY_TRACE("リストボックスが再描画されました\n");

					// プレビューを再描画します。
					redraw_preview();

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		//
		// プレビューの設定を読み込みます。
		//
		void read(n_json& node)
		{
			MY_TRACE_FUNC("");

			n_json preview_node;
			read_child_node(node, "preview", preview_node);
			read_label(node, "mode", hive.preview.mode, hive.preview.c_mode.labels);
			read_size(preview_node, "item_size", hive.preview.item_size);
			read_string(preview_node, "sample_format_v2", hive.preview.sample_format);
			read_color(preview_node, "fill_color", hive.preview.fill_color);
			read_color(preview_node, "text_color", hive.preview.text_color);
		}

		//
		// プレビューの設定を書き込みます。
		//
		void write(n_json& node)
		{
			MY_TRACE_FUNC("");

			n_json preview_node;
			write_label(node, "mode", hive.preview.mode, hive.preview.c_mode.labels);
			write_size(preview_node, "item_size", hive.preview.item_size);
			write_string(preview_node, "sample_format_v2", hive.preview.sample_format);
			write_color(preview_node, "fill_color", hive.preview.fill_color);
			write_color(preview_node, "text_color", hive.preview.text_color);
			write_child_node(node, "preview", preview_node);
		}
	} preview_manager;
}
