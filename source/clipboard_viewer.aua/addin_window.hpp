#pragma once

namespace apn::clipboard_viewer
{
	//
	// このクラスはアドインウィンドウです。
	// このウィンドウはプラグインウィンドウのように振る舞います。
	//
	inline struct AddinWindow : StdAddinWindow
	{
		inline static constexpr struct CommandID {
			inline static constexpr auto c_erase_current_bitmap = 1000U;
			inline static constexpr auto c_clear_all_bitmaps = 1001U;
			inline static constexpr auto c_zoom_in = 1002U;
			inline static constexpr auto c_zoom_out = 1003U;
			inline static constexpr auto c_zoom_reset = 1004U;
			inline static constexpr auto c_backward = 1005U;
			inline static constexpr auto c_forward = 1006U;
		} c_command_id;

		//
		// クリップボードから取得した画像のコレクションです。
		//
		std::vector<my::gdi::shared_ptr<HBITMAP>> bitmaps;

		//
		// コンテンツウィンドウで現在表示している画像のインデックスです。
		//
		int32_t current_bitmap_index = 0;

		//
		// コンテンツウィンドウです。
		//
		HWND window = nullptr;

		//
		// 描画に使用するテーマハンドルです。
		//
		my::theme::unique_ptr<> theme;

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			return create_as_plugin(
				hive.instance,
				magi.exin.get_aviutl_window(),
				hive.c_display_name,
				WS_EX_NOPARENTNOTIFY,
				WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				100, 100, 800, 600);
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
		// 現在の画像のインデックスを設定して正規化します。
		//
		void set_current_bitmap_index(int32_t new_current_bitmap_index)
		{
			current_bitmap_index = new_current_bitmap_index;
			current_bitmap_index = std::max(current_bitmap_index, 0);
			current_bitmap_index = std::min(current_bitmap_index, (int32_t)bitmaps.size() - 1);
		}

		//
		// 拡大率を設定して正規化します。
		//
		void set_zoom(int32_t new_zoom)
		{
			hive.zoom = new_zoom;
			hive.zoom = std::max(hive.zoom, hive.c_zoom.c_min);
			hive.zoom = std::min(hive.zoom, hive.c_zoom.c_max);
		}

		//
		// スクロールバーを更新します。
		//
		BOOL update_scrollbar()
		{
			if (bitmaps.empty()) return FALSE;

			// クライアント領域のサイズを取得します。
			auto rc = my::get_client_rect(*this);
			auto w = my::get_width(rc);
			auto h = my::get_height(rc);

			// 画像を取得します。
			auto bitmap = bitmaps[current_bitmap_index];

			// 画像のサイズを取得します。
			auto bm = BITMAP {}; ::GetObject(bitmap.get(), sizeof(bm), &bm);
			auto bm_w = ::MulDiv(bm.bmWidth, hive.zoom, 100);
			auto bm_h = ::MulDiv(bm.bmHeight, hive.zoom, 100);

			// 現在のスクロール位置を取得します。
			auto horz_pos = ::GetScrollPos(*this, SB_HORZ);
			auto vert_pos = ::GetScrollPos(*this, SB_VERT);

			// スクロールバーを更新します。
			SCROLLINFO si = { sizeof(si) };
			si.fMask = SIF_DISABLENOSCROLL | SIF_ALL;
			si.nMin = 0;
			si.nMax = bm_w;
			si.nPage = w + 1;
			si.nPos = horz_pos;
			::SetScrollInfo(*this, SB_HORZ, &si, TRUE);
			si.nMin = 0;
			si.nMax = bm_h;
			si.nPage = h + 1;
			si.nPos = vert_pos;
			::SetScrollInfo(*this, SB_VERT, &si, TRUE);

			return TRUE;
		}

		//
		// コンテンツを更新します。
		//
		BOOL update()
		{
			// ウィンドウを再描画します。
			my::invalidate(*this);

			if (bitmaps.empty())
			{
				// ウィンドウ名を更新します。
				::SetWindowText(*this, my::format(_T("{} - {}% - (0/0)"), hive.c_display_name, hive.zoom).c_str());

				return FALSE;
			}
			else
			{
				// ウィンドウ名を更新します。
				::SetWindowText(*this, my::format(_T("{} - {}% - ({}/{})"),
					hive.c_display_name, hive.zoom, current_bitmap_index + 1, bitmaps.size()).c_str());

				// スクロールバーを更新します。
				update_scrollbar();

				return TRUE;
			}
		}

		//
		// WM_PAINTを処理します。
		//
		BOOL on_paint()
		{
			// クライアント領域のサイズを取得します。
			auto rc = my::get_client_rect(*this);
			auto w = my::get_width(rc);
			auto h = my::get_height(rc);

			// 描画の準備をします。
			my::PaintDC pdc(*this);
			my::UxDC dc(pdc, &rc);

			// 背景を塗りつぶします。
			::DrawThemeBackground(theme.get(), dc, WP_DIALOG, 0, &rc, nullptr);

			// 画像が存在しない場合はここで処理を終了します。
			if (bitmaps.empty()) return FALSE;

			// 画像を取得します。
			auto bitmap = bitmaps[current_bitmap_index];

			// 画像のサイズを取得します。
			auto bm = BITMAP {}; ::GetObject(bitmap.get(), sizeof(bm), &bm);
			auto bm_w = ::MulDiv(bm.bmWidth, hive.zoom, 100);
			auto bm_h = ::MulDiv(bm.bmHeight, hive.zoom, 100);

			// 現在のスクロール位置を取得します。
			auto horz_pos = ::GetScrollPos(*this, SB_HORZ);
			auto vert_pos = ::GetScrollPos(*this, SB_VERT);

			// 画像を描画します。
			{
				my::dc::unique_ptr<> mdc(::CreateCompatibleDC(dc));
				my::gdi::selector bitmap_selector(mdc.get(), bitmap.get());

				::SetStretchBltMode(dc, HALFTONE);
				::StretchBlt(dc, -horz_pos, -vert_pos, bm_w, bm_h,
					mdc.get(), 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
			}

			return TRUE;
		}

		//
		// WM_CONTEXTMENUを処理します。
		//
		BOOL on_context_menu()
		{
			my::menu::unique_ptr<> menu(::CreatePopupMenu());

			::AppendMenuW(menu.get(), MF_STRING, c_command_id.c_backward, L"戻る");
			::AppendMenuW(menu.get(), MF_STRING, c_command_id.c_forward, L"進む");
			::AppendMenuW(menu.get(), MF_SEPARATOR, 0, nullptr);
			::AppendMenuW(menu.get(), MF_STRING, c_command_id.c_zoom_in, L"拡大");
			::AppendMenuW(menu.get(), MF_STRING, c_command_id.c_zoom_out, L"縮小");
			::AppendMenuW(menu.get(), MF_STRING, c_command_id.c_zoom_reset, L"拡大率をリセット");
			::AppendMenuW(menu.get(), MF_SEPARATOR, 0, nullptr);
			::AppendMenuW(menu.get(), MF_STRING, c_command_id.c_erase_current_bitmap, L"現在の画像を削除");
			::AppendMenuW(menu.get(), MF_STRING, c_command_id.c_clear_all_bitmaps, L"すべての画像を削除");

			if (current_bitmap_index <= 0)
				::EnableMenuItem(menu.get(), c_command_id.c_backward, MF_DISABLED);

			if (current_bitmap_index >= (int32_t)bitmaps.size() - 1)
				::EnableMenuItem(menu.get(), c_command_id.c_forward, MF_DISABLED);

			if (hive.zoom <= hive.c_zoom.c_min)
				::EnableMenuItem(menu.get(), c_command_id.c_zoom_out, MF_DISABLED);

			if (hive.zoom >= hive.c_zoom.c_max)
				::EnableMenuItem(menu.get(), c_command_id.c_zoom_in, MF_DISABLED);

			if (hive.zoom >= hive.c_zoom.c_default)
				::EnableMenuItem(menu.get(), c_command_id.c_zoom_reset, MF_DISABLED);

			if (bitmaps.empty())
			{
				::EnableMenuItem(menu.get(), c_command_id.c_erase_current_bitmap, MF_DISABLED);
				::EnableMenuItem(menu.get(), c_command_id.c_clear_all_bitmaps, MF_DISABLED);
			}

			auto point = my::get_cursor_pos();
			auto id = ::TrackPopupMenuEx(menu.get(),
				TPM_NONOTIFY | TPM_RETURNCMD, point.x, point.y, *this, nullptr);
			if (!id) return FALSE;

			switch (id)
			{
			case c_command_id.c_erase_current_bitmap:
				{
					// 現在の画像をコレクションから削除します。
					if (current_bitmap_index < (int32_t)bitmaps.size())
						bitmaps.erase(bitmaps.begin() + current_bitmap_index);
					set_current_bitmap_index(current_bitmap_index);

					break;
				}
			case c_command_id.c_clear_all_bitmaps:
				{
					// コレクション内のすべての画像を消去します。
					bitmaps.clear();
					set_current_bitmap_index(0);

					break;
				}
			case c_command_id.c_zoom_in:
				{
					// 拡大率を倍にします。
					set_zoom(hive.zoom * 2);

					break;
				}
			case c_command_id.c_zoom_out:
				{
					// 拡大率を半分にします。
					set_zoom(hive.zoom / 2);

					break;
				}
			case c_command_id.c_zoom_reset:
				{
					// 拡大率をリセットします。
					set_zoom(hive.c_zoom.c_default);

					break;
				}
			case c_command_id.c_backward:
				{
					set_current_bitmap_index(current_bitmap_index - 1);

					break;
				}
			case c_command_id.c_forward:
				{
					set_current_bitmap_index(current_bitmap_index + 1);

					break;
				}
			}

			update();

			return TRUE;
		}

		//
		// WM_HSCROLL、WM_VSCROLLを処理します。
		//
		BOOL on_scroll(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			auto sb = (message == WM_HSCROLL) ? SB_HORZ : SB_VERT;

			SCROLLINFO si = { sizeof(si) };
			si.fMask = SIF_DISABLENOSCROLL | SIF_ALL;
			::GetScrollInfo(hwnd, sb, &si);
					
			switch (LOWORD(wParam))
			{
			case SB_LEFT:
				{
					si.nPos = si.nMin;

					break;
				}
			case SB_RIGHT:
				{
					si.nPos = si.nMax;

					break;
				}
			case SB_LINELEFT:
				{
					si.nPos--;

					break;
				}
			case SB_LINERIGHT:
				{
					si.nPos++;

					break;
				}
			case SB_PAGELEFT:
				{
					si.nPos -= 100;

					break;
				}
			case SB_PAGERIGHT:
				{
					si.nPos += 100;

					break;
				}
			case SB_THUMBPOSITION:
			case SB_THUMBTRACK:
				{
					si.nPos = HIWORD(wParam);

					break;
				}
			}

			::SetScrollPos(hwnd, sb, si.nPos, TRUE);

			my::invalidate(hwnd);

			return TRUE;
		}

		//
		// WM_CLIPBOARDUPDATEを処理します。
		//
		BOOL on_clipboard_update()
		{
			// クリップボードを開きます。
			if (!::OpenClipboard(*this)) return FALSE;

			// クリップボードの画像を取得し、複製します。
			auto bitmap = (HBITMAP)::OleDuplicateData(
				::GetClipboardData(CF_BITMAP), CF_BITMAP, 0);

			// クリップボードを閉じます。
			::CloseClipboard(); 

			// 画像が取得できなかった場合は何もしません。
			if (!bitmap) return FALSE;

			// 画像をコレクションに追加します。
			bitmaps.emplace_back(bitmap);

			// 追加した画像をカレントに設定します。
			set_current_bitmap_index((int32_t)bitmaps.size() - 1);

			// コンテンツを更新します。
			update();

			return TRUE;
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_CREATE:
				{
					MY_TRACE_FUNC("WM_CREATE, {:#010x}, {:#010x}", wParam, lParam);

					// テーマを開きます。
					theme.reset(::OpenThemeData(hwnd, VSCLASS_WINDOW));

					// クリップボードの監視を開始します。
					::AddClipboardFormatListener(hwnd);

					// 初期化が完了したので、このウィンドウをハイブに格納します。
					hive.main_window = hwnd;

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {:#010x}, {:#010x}", wParam, lParam);

					// クリップボードの監視を終了します。
					::RemoveClipboardFormatListener(hwnd);

					// テーマを閉じます。
					theme.reset();

					break;
				}
			case WM_SIZE:
				{
					update_scrollbar();

					break;
				}
			case WM_PAINT:
				{
					on_paint();

					break;
				}
			case WM_CONTEXTMENU:
				{
					on_context_menu();

					break;
				}
			case WM_HSCROLL:
			case WM_VSCROLL:
				{
					on_scroll(hwnd, message, wParam, lParam);

					break;
				}
			case WM_APPCOMMAND:
				{
					MY_TRACE_FUNC("WM_APPCOMMAND, {:#010x}, {:#010x}", wParam, lParam);

					switch (GET_APPCOMMAND_LPARAM(lParam))
					{
					case APPCOMMAND_BROWSER_BACKWARD:
						{
							set_current_bitmap_index(current_bitmap_index - 1);

							update();

							break;
						}
					case APPCOMMAND_BROWSER_FORWARD:
						{
							set_current_bitmap_index(current_bitmap_index + 1);

							update();

							break;
						}
					}

					break;
				}
			case WM_CLIPBOARDUPDATE:
				{
					on_clipboard_update();

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} addin_window;
}
