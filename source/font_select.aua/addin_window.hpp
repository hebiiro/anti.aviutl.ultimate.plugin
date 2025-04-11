#pragma once

namespace apn::font_select
{
	//
	// このクラスはメインウィンドウです。
	// このウィンドウはプラグインウィンドウのように振る舞います。
	//
	inline struct AddinWindow : StdAddinWindow
	{
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
		// オーナードロー用の処理です。アイテムの寸法を返します。
		//
		BOOL on_measure_item(MEASUREITEMSTRUCT* mis)
		{
//			MY_TRACE_FUNC("{/}", mis->itemID);

			mis->itemWidth = hive.item_size.cx;
			mis->itemHeight = hive.item_size.cy;

			return TRUE;
		}

		//
		// オーナードロー用の処理です。アイテムを描画します。
		//
		BOOL on_draw_item(DRAWITEMSTRUCT* dis)
		{
//			MY_TRACE_FUNC("{/}", dis->itemID);

			auto dc = dis->hDC;
			auto rc = dis->rcItem;
			auto menu = (HMENU)dis->hwndItem;
			auto id = dis->itemID;
			auto font_name = std::wstring((LPCWSTR)dis->itemData);

			MY_TRACE("{/}, {/}, {/hex}, {/hex}\n", font_name, dis->itemID, dis->itemAction, dis->itemState);

			{
				// ドロップダウンリストのアイテムを描画します。

				auto style = paint.c_style.c_normal;

				if (dis->itemState & ODS_FOCUS) return FALSE;
				if (dis->itemState & ODS_SELECTED) style = paint.c_style.c_select;

				// 背景を描画します。
				paint.draw_background(dc, &rc, style);

				if (hive.single_line)
				{
					// フォント名とサンプルをまとめて描画します。

					auto text = font_name + hive.sample;

					my::gdi::unique_ptr<HFONT> font(::CreateFont(
						my::get_height(rc), 0, 0, 0, 0, FALSE, FALSE, FALSE,
						DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
						DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name.c_str()));
					my::gdi::selector font_selector(dc, font.get());

					paint.draw_sample(dc, text.c_str(), -1, &rc, DT_LEFT | DT_TOP | DT_NOPREFIX, style);
				}
				else
				{
					{
						// サンプルを描画します。

						auto text = hive.sample;

						my::gdi::unique_ptr<HFONT> font(::CreateFont(
							my::get_height(rc), 0, 0, 0, 0, FALSE, FALSE, FALSE,
							DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
							DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name.c_str()));
						my::gdi::selector font_selector(dc, font.get());

						paint.draw_sample(dc, text.c_str(), -1, &rc, DT_RIGHT | DT_BOTTOM | DT_NOPREFIX, style);
					}

					{
						// フォント名を描画します。

						auto text = font_name;

						paint.draw_font_name(dc, text.c_str(), -1, &rc, DT_LEFT | DT_TOP | DT_NOPREFIX, style);
					}
				}
			}

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
					MY_TRACE_FUNC("WM_CREATE, {/hex}, {/hex}", wParam, lParam);

					if (!addin_dialog.init(hwnd))
					{
						MY_TRACE("メインダイアログの作成に失敗しました\n");

						return -1;
					}

					set_target(addin_dialog);
					resize();

					// ウィンドウの初期化が完了したので、
					// このウィンドウをメインウィンドウに設定します。
					hive.main_window = hwnd;

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {/hex}, {/hex}", wParam, lParam);

					addin_dialog.exit();

					break;
				}
			case WM_MEASUREITEM:
				{
					auto mis = (MEASUREITEMSTRUCT*)lParam;
					if (mis->CtlType == ODT_MENU)
						on_measure_item(mis);

					break;
				}
			case WM_DRAWITEM:
				{
					auto dis = (DRAWITEMSTRUCT*)lParam;
					if (dis->CtlType == ODT_MENU)
						on_draw_item(dis);

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} addin_window;
}
