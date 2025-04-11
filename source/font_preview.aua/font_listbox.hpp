#pragma once

namespace apn::font_preview
{
	//
	// このクラスは設定ダイアログのサブクラスです。
	//
	inline struct FontListBox : my::Window
	{
		HWND combobox = nullptr;

		//
		// リストボックスのフックを開始します。
		//
		BOOL start_hook(HWND combobox)
		{
			this->combobox = combobox;

			// リストボックス内のアイテムの高さを変更します。
			::SendMessage(combobox, CB_SETITEMHEIGHT, 0, hive.item_height);

			COMBOBOXINFO cbi = { sizeof(cbi) };
			::GetComboBoxInfo(combobox, &cbi);
			MY_TRACE_HWND(cbi.hwndList);

			unsubclass();
			subclass(cbi.hwndList);

			return TRUE;
		}

		//
		// リストボックスの表示予定位置を変更します。
		//
		BOOL set_window_pos(WINDOWPOS* wp)
		{
			MY_TRACE_FUNC("");

			auto listbox = (HWND)*this;

			auto rc_combobox = my::get_window_rect(combobox);
			auto rc_listbox = RECT { 0, 0, hive.window_size.cx, hive.window_size.cy };
			my::client_to_window(listbox, &rc_listbox);
			auto x = wp->x;
			auto y = wp->y;
			auto w = my::get_width(rc_listbox);
			auto h = my::get_height(rc_listbox);

			auto is_horz_available = FALSE;
			auto rc_monitor = my::get_monitor_rect(combobox);

			// リストボックスのボトムがモニタのボトムからはみ出す場合は
			if (rc_combobox.bottom + h > rc_monitor.bottom)
			{
				// リストボックスのトップがモニタのトップからはみ出す場合は
				if (rc_combobox.top - h < rc_monitor.top)
				{
					h = std::min<int>(h, my::get_height(rc_monitor));

					// 可能な限りコンボボックスの真横に配置します。
					y = my::get_center_y(rc_combobox) - h / 2;
					y = std::max<int>(y, rc_monitor.top);
					y = std::min<int>(y, rc_monitor.bottom - h);

					// リストボックスのレフトがモニタのレフトからはみ出す場合は
					if (rc_combobox.left - w < rc_monitor.left)
					{
						// リストボックスをコンボボックスの右側に配置します。
						x = rc_combobox.right;
						w = std::min<int>(w, rc_monitor.right - x);
					}
					else
					{
						// リストボックスをコンボボックスの左側に配置します。
						x = rc_combobox.left - w;
					}

					// xとwをセットしたのでフラグを立てます。
					is_horz_available = TRUE;
				}
				else
				{
					// リストボックスをコンボボックスの上側に配置します。
					y = rc_combobox.top - h;
				}
			}
			else
			{
				// リストボックスをコンボボックスの下側に配置します。
				y = rc_combobox.bottom;
			}

			// xとwがまだセットされていない場合は
			if (!is_horz_available)
			{
				// リストボックスの幅がモニタの幅を超えないように調整します。
				w = std::min<int>(w, my::get_width(rc_monitor));

				// リストボックスのレフトがモニタのレフトからはみ出す場合は
				if (rc_combobox.left < rc_monitor.left)
				{
					// リストボックスのレフトがモニタのレフトと一致するように配置します。
					x = rc_monitor.left;
				}
				// リストボックスのライトがモニタのライトからはみ出す場合は
				else if (rc_combobox.left + w > rc_monitor.right)
				{
					// リストボックスのライトがモニタのライトと一致するように配置します。
					x = rc_monitor.right - w;
				}
				else
				{
					// リストボックスのレフトがコンボボックスのレフトと一致するように配置します。
					x = rc_combobox.left;
				}
			}

			wp->x = x;
			wp->y = y;
			wp->cx = w;
			wp->cy = h;

			return TRUE;
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/hex}", hwnd, message, wParam, lParam);

			switch (message)
			{
			case WM_WINDOWPOSCHANGING:
				{
					auto wp = (WINDOWPOS*)lParam;

					MY_TRACE("{/hex}, {/hex}, {/}, {/}, {/}, {/}\n", wp->flags, wp->hwndInsertAfter, wp->x, wp->y, wp->cx, wp->cy);

					if (wp->flags & SWP_NOMOVE) break;
					if (wp->flags & SWP_NOSIZE) break;

					set_window_pos(wp);

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} font_listbox;
}
