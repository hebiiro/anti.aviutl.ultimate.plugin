#pragma once

namespace apn::scene_select
{
	//
	// このクラスはアドインウィンドウです。
	// このウィンドウはプラグインウィンドウのように振る舞います。
	//
	inline struct AddinWindow : StdAddinWindow
	{
		my::theme::unique_ptr<> window_theme;
		my::theme::unique_ptr<> button_theme;

		std::vector<RECT> buttons;
		int32_t hot_button = -1;
		int32_t drag_button = -1;

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			DWORD style = WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
			DWORD ex_style = WS_EX_NOPARENTNOTIFY;

			// 固定サイズモードのときははサイズ変更枠を取り除きます。
			if (hive.size_mode)
				style &= ~WS_THICKFRAME;

			// ウィンドウを作成します。
			// このウィンドウはAviUtlプラグインウィンドウのように振る舞います。
			return create_as_plugin(
				hive.instance,
				magi.exin.get_aviutl_window(),
				hive.c_display_name,
				WS_EX_NOPARENTNOTIFY,
				WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				100, 100, 400, 300);
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
		// ウィンドウを再描画します。
		//
		BOOL redraw()
		{
			return ::InvalidateRect(*this, nullptr, FALSE);
		}

		//
		// 指定されたボタンのインデックスが利用可能ならTRUEを返します。
		//
		inline BOOL is_available(int32_t button_index)
		{
			return button_index >= 0 && button_index < hive.button_count;
		}

		//
		// ボタンの配置位置を再計算します。
		//
		void calc_layout()
		{
			switch (hive.layout_mode)
			{
			case hive.c_layout_mode.c_vert: calc_layout_vert(); break;
			case hive.c_layout_mode.c_horz: calc_layout_horz(); break;
			}
		}

		//
		// レイアウトモードが垂直の場合のボタンの配置位置を再計算します。
		//
		void calc_layout_vert()
		{
			buttons.resize(hive.button_count);

			auto client_rc = my::get_client_rect(*this);
			auto client_pos = POINT { client_rc.left, client_rc.top };
			auto client_size = SIZE { my::get_width(client_rc), my::get_height(client_rc) };

			auto row_count = (hive.wrap > 0) ? hive.wrap : 5;
			auto col_count = (hive.button_count - 1) / row_count + 1;

			auto button_index = 0;

			for (auto col = 0; col < col_count; col++)
			{
				for (auto row = 0; row < row_count; row++, button_index++)
				{
					if (button_index >= hive.button_count) break;

					RECT rc = {};

					if (hive.size_mode)
					{
						rc.left = client_pos.x + hive.button_size.cx * col;
						rc.top = client_pos.y + hive.button_size.cy * row;
						rc.right = rc.left + hive.button_size.cx;
						rc.bottom = rc.top + hive.button_size.cy;
					}
					else
					{
						rc.left = client_pos.x + ::MulDiv(client_size.cx, col + 0, col_count);
						rc.right = client_pos.x + ::MulDiv(client_size.cx, col + 1, col_count);
						rc.top = client_pos.y + ::MulDiv(client_size.cy, row + 0, row_count);
						rc.bottom = client_pos.y + ::MulDiv(client_size.cy, row + 1, row_count);
					}

					buttons[button_index] = rc;
				}
			}

			if (hive.size_mode)
			{
				auto rc = client_rc;
				rc.right = rc.left + hive.button_size.cx * col_count;
				rc.bottom = rc.top + hive.button_size.cy * row_count;
				my::client_to_window(*this, &rc);

				::SetWindowPos(*this, nullptr,
					0, 0, my::get_width(rc), my::get_height(rc),
					SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
			}
		}

		//
		// レイアウトモードが水平の場合のボタンの配置位置を再計算します。
		//
		void calc_layout_horz()
		{
			buttons.resize(hive.button_count);

			auto client_rc = my::get_client_rect(*this);
			auto client_pos = POINT { client_rc.left, client_rc.top };
			auto client_size = SIZE { my::get_width(client_rc), my::get_height(client_rc) };

			auto col_count = (hive.wrap > 0) ? hive.wrap : 5;
			auto row_count = (hive.button_count - 1) / col_count + 1;

			auto button_index = 0;

			for (auto row = 0; row < row_count; row++)
			{
				for (auto col = 0; col < col_count; col++, button_index++)
				{
					if (button_index >= hive.button_count) break;

					RECT rc = {};

					if (hive.size_mode)
					{
						rc.left = client_pos.x + hive.button_size.cx * col;
						rc.top = client_pos.y + hive.button_size.cy * row;
						rc.right = rc.left + hive.button_size.cx;
						rc.bottom = rc.top + hive.button_size.cy;
					}
					else
					{
						rc.left = client_pos.x + ::MulDiv(client_size.cx, col + 0, col_count);
						rc.right = client_pos.x + ::MulDiv(client_size.cx, col + 1, col_count);
						rc.top = client_pos.y + ::MulDiv(client_size.cy, row + 0, row_count);
						rc.bottom = client_pos.y + ::MulDiv(client_size.cy, row + 1, row_count);
					}

					buttons[button_index] = rc;
				}
			}

			if (hive.size_mode)
			{
				auto rc = client_rc;
				rc.right = rc.left + hive.button_size.cx * col_count;
				rc.bottom = rc.top + hive.button_size.cy * row_count;
				my::map_window_points(*this, nullptr, &rc);
				my::client_to_window(*this, &rc);

				::SetWindowPos(*this, nullptr, 0, 0, my::get_width(rc), my::get_height(rc), SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
			}
		}

		//
		// 指定された座標にあるボタンのインデックスを返します。
		//
		int32_t hittest(HWND hwnd, const POINT& point)
		{
			for (size_t i = 0; i < buttons.size(); i++)
			{
				if (::PtInRect(&buttons[i], point))
					return i;
			}

			return -1;
		}

		//
		// ボタンを描画します。
		//
		void on_paint(HWND hwnd)
		{
			if (!magi.exin.get_exedit()) return;

			auto client_rc = my::get_client_rect(hwnd);

			my::PaintDC pdc(hwnd);
#if 1
			my::DoubleBufferDC dc(pdc, &client_rc);
#else
			my::UxDC dc(pdc, &client_rc);
			if (!dc.is_valid()) return;
#endif
			AviUtl::SysInfo si = {};
			magi.fp->exfunc->get_sys_info(nullptr, &si);

			// フォントをセットします。
			my::gdi::selector font_selector(dc, si.hfont);

			{
				// ブラシを取得します。
				auto brush = (HBRUSH)(COLOR_BTNFACE + 1);

				// 背景を塗りつぶします。
				::FillRect(dc, &client_rc, brush);
			}

			// ボタンを描画します。
			for (size_t i = 0; i < buttons.size(); i++)
			{
				auto button_index = i;
				const auto& rc = buttons[i];

				// シーン名を取得します。
				std::wstring text;
				{
					auto scene = magi.exin.get_scene_setting(button_index);

					if (scene->name)
					{
						text = my::ws(scene->name);
					}
					else
					{
						if (button_index == 0)
							text = L"root";
						else
							text = std::to_wstring(button_index);
					}
				}

				// ボタンのパートとステートを取得します。
				auto part_id = BP_PUSHBUTTON;
				auto state_id = PBS_NORMAL;
				{
					if (button_index == magi.exin.get_current_scene_index())
					{
						state_id = PBS_PRESSED;
					}
					else if (button_index == drag_button)
					{
						if (button_index == hot_button)
						{
							state_id = PBS_PRESSED;
						}
						else
						{
							state_id = PBS_HOT;
						}
					}
					else if (button_index == hot_button)
					{
						state_id = PBS_HOT;
					}
				}

				// テーマを使用してボタンを描画します。
				::DrawThemeBackground(button_theme.get(), dc, part_id, state_id, &rc, 0);
				::DrawThemeText(button_theme.get(), dc, part_id, state_id,
					text.c_str(), text.length(), DT_CENTER | DT_VCENTER | DT_SINGLELINE, 0, &rc);
			}
		}

		//
		// コンテキストメニューを表示します。
		//
		void on_context_menu(HWND hwnd)
		{
			MY_TRACE_FUNC("");

			my::menu::unique_ptr<> menu(::CreatePopupMenu());

			::AppendMenu(menu.get(), MF_STRING, hive.c_command_id.c_config, _T("設定"));

			auto pt = my::get_cursor_pos();
			auto id = ::TrackPopupMenu(menu.get(), TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, 0, hwnd, 0);

			switch (id)
			{
			case hive.c_command_id.c_config:
				{
					on_config(hwnd);

					break;
				}
			}
		}

		//
		// コンフィグダイアログを表示します。
		//
		void on_config(HWND hwnd)
		{
			MY_TRACE_FUNC("");

			ConfigDialog dialog;
			if (IDOK != dialog.do_modal(*this))
				return;

			calc_layout();
			redraw();
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

					window_theme.reset(::OpenThemeData(hwnd, VSCLASS_WINDOW));
					MY_TRACE_HEX(window_theme.get());

					button_theme.reset(::OpenThemeData(hwnd, VSCLASS_BUTTON));
					MY_TRACE_HEX(button_theme.get());

					// 初期化が完了したので、このウィンドウをメインウィンドウに設定します。
					hive.main_window = addin_window;

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {:#010x}, {:#010x}", wParam, lParam);

					window_theme.reset();
					button_theme.reset();

					break;
				}
			case WM_SIZE:
				{
					MY_TRACE_FUNC("WM_SIZE, {:#010x}, {:#010x}", wParam, lParam);

					// 固定サイズモードの場合はWM_SIZEでは何もしません。
					if (hive.size_mode == hive.c_size_mode.c_nonresizable) break;

					calc_layout();

					break;
				}
			case WM_PAINT:
				{
					MY_TRACE_FUNC("WM_PAINT, {:#010x}, {:#010x}", wParam, lParam);

					on_paint(hwnd);

					break;
				}
			case WM_LBUTTONDOWN:
				{
					MY_TRACE_FUNC("WM_LBUTTONDOWN, {:#010x}, {:#010x}", wParam, lParam);

					// マウスカーソルの座標を取得します。
					auto point = my::lp_to_pt(lParam);

					// ドラッグを開始するボタンを取得します。
					drag_button = hittest(hwnd, point);
					MY_TRACE_INT(drag_button);

					// ドラッグボタンが無効なら
					if (!is_available(drag_button))
						break; // 何もしません。

					// マウスキャプチャを開始します。
					::SetCapture(hwnd);

					// ウィンドウを再描画します。
					redraw();

					break;
				}
			case WM_LBUTTONUP:
				{
					MY_TRACE_FUNC("WM_LBUTTONUP, {:#010x}, {:#010x}", wParam, lParam);

					// マウスカーソルの座標を取得します。
					auto point = my::lp_to_pt(lParam);

					// マウスキャプチャ中なら
					if (::GetCapture() == hwnd)
					{
						// マウスキャプチャを終了します。
						::ReleaseCapture();

						// ホットボタンを取得します。
						hot_button = hittest(hwnd, point);
						MY_TRACE_INT(hot_button);

						// ドラッグボタンとホットボタンが同じなら
						if (drag_button == hot_button)
						{
							// ドラッグボタンが有効かつ現在のシーンと違うなら
							if (is_available(drag_button) && drag_button != magi.exin.get_current_scene_index())
							{
								// ボタンが押されたのでシーンを変更します。
								magi.exin.set_scene(drag_button, magi.auin.get_filter_plugin(magi.fp, "拡張編集"), magi.exin.get_editp());

								// AviUtlのプレビューウィンドウを再描画します。
								magi.redraw();
							}
						}

						// ドラッグボタンを初期値に戻します。
						drag_button = -1;

						// ウィンドウを再描画します。
						redraw();
					}

					break;
				}
			case WM_MOUSEMOVE:
				{
//					MY_TRACE_FUNC("WM_MOUSEMOVE, {:#010x}, {:#010x}", wParam, lParam);

					// マウスカーソルの座標を取得します。
					auto point = my::lp_to_pt(lParam);

					if (::GetCapture() == hwnd)
					{
						// マウス座標にあるボタンを取得します。
						auto button = hittest(hwnd, point);

						// ドラッグボタンとマウス座標にあるボタンが異なるなら
						if (drag_button != button)
							button = -1; // マウス座標にあるボタンを無効にします。

						// ホットボタンとマウス座標にあるボタンが異なるなら
						if (hot_button != button)
						{
							// ホットボタンを更新します。
							hot_button = button;

							// ウィンドウを再描画します。
							redraw();
						}
					}
					else
					{
						// マウス座標にあるシーンを取得します。
						auto button = hittest(hwnd, point);

						// ホットボタンとマウス座標にあるボタンが異なるなら
						if (hot_button != button)
						{
							// ホットボタンを更新します。
							hot_button = button;

							// ホットボタンが有効かつマウスキャプチャ中でないなら
							if (hot_button >= 0)
							{
								// WM_MOUSELEAVEが発行されるようにします。
								TRACKMOUSEEVENT tme = { sizeof(tme) };
								tme.dwFlags = TME_LEAVE;
								tme.hwndTrack = hwnd;
								::TrackMouseEvent(&tme);
							}

							// ウィンドウを再描画します。
							redraw();
						}
					}

					break;
				}
			case WM_MOUSELEAVE:
				{
					MY_TRACE_FUNC("WM_MOUSELEAVE, {:#010x}, {:#010x}", wParam, lParam);

					// ホットボタンが有効なら
					if (hot_button >= 0)
					{
						// ホットボタンを初期値に戻します。
						hot_button = -1;

						// ウィンドウを再描画します。
						redraw();
					}

					break;
				}
			case WM_RBUTTONUP:
				{
					MY_TRACE_FUNC("WM_RBUTTONUP, {:#010x}, {:#010x}", wParam, lParam);

					// コンテキストメニューを表示します。
					on_context_menu(hwnd);

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		//
		// このクラスは設定を変更するためのダイアログです。
		//
		struct ConfigDialog : my::Dialog2
		{
			int32_t do_modal(HWND parent)
			{
				create(hive.instance, MAKEINTRESOURCE(IDD_CONFIG), parent);

				::SendDlgItemMessage(*this, IDC_WRAP_SPIN, UDM_SETRANGE32, 1, 50);
				::SendDlgItemMessage(*this, IDC_BUTTON_COUNT_SPIN, UDM_SETRANGE32, 1, 50);

				init_combobox(IDC_SIZE_MODE, _T("可変サイズ"), _T("固定サイズ"));
				init_combobox(IDC_LAYOUT_MODE, _T("水平方向"), _T("垂直方向"));

				set_combobox_index(IDC_SIZE_MODE, hive.size_mode);
				set_combobox_index(IDC_LAYOUT_MODE, hive.layout_mode);
				set_int(IDC_WRAP, hive.wrap);
				set_int(IDC_BUTTON_COUNT, hive.button_count);
				set_int(IDC_BUTTON_SIZE_W, hive.button_size.cx);
				set_int(IDC_BUTTON_SIZE_H, hive.button_size.cy);

				return do_modal2(parent);
			}

			//
			// ダイアログプロシージャです。
			//
			virtual INT_PTR on_dlg_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
			{
				switch (message)
				{
				case WM_NOTIFY:
					{
						break;
					}
				}

				return __super::on_dlg_proc(hwnd, message, wParam, lParam);
			}
		};
	} addin_window;
}
