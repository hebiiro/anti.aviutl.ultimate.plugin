#pragma once
#include "MainDialog.h"

namespace fgo::font_select
{
	//
	// このクラスはメインウィンドウです。
	// このウィンドウはプラグインウィンドウのように振る舞います。
	//
	struct MainWindow : Tools::AviUtl::PluginWindow
	{
		MainDialog mainDialog;

		//
		// 初期化を行います。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// ウィンドウを作成します。
			return createAsPlugin(
				hive.instance,
				magi.auin.GetAviUtlWindow(),
				Hive::DisplayName,
				WS_EX_NOPARENTNOTIFY,
				WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				100, 100, 300, 300);
		}

		//
		// 後始末を行います。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return destroy();
		}

		//
		// オーナードロー用の処理です。アイテムの寸法を返します。
		//
		BOOL onMeasureItem(MEASUREITEMSTRUCT* mis)
		{
//			MY_TRACE_FUNC("%d", mis->itemID);

			mis->itemWidth = hive.itemWidth;
			mis->itemHeight = hive.itemHeight;

			return TRUE;
		}

		//
		// オーナードロー用の処理です。アイテムを描画します。
		//
		BOOL onDrawItem(DRAWITEMSTRUCT* dis)
		{
//			MY_TRACE_FUNC("%d", dis->itemID);

			auto dc = dis->hDC;
			auto rc = dis->rcItem;
			auto menu = (HMENU)dis->hwndItem;
			auto id = dis->itemID;
			auto fontName = (LPCWSTR)dis->itemData;

			MY_TRACE(_T("%ws, %d, 0x%08X, 0x%08X\n"), fontName, dis->itemID, dis->itemAction, dis->itemState);

			{
				// ドロップダウンリストのアイテムを描画します。

				int stateId = Paint::StateID::Normal;

				if (dis->itemState & ODS_FOCUS) return FALSE;
				else if (dis->itemState & ODS_SELECTED) stateId = Paint::StateID::Select;

				{
					// 背景を描画します。
					GdiObj<HBRUSH> brush = ::CreateSolidBrush(paint.getFillColor(stateId));
					::FillRect(dc, &rc, brush);
				}

				{
					// テキストを描画します。

					int bkMode = ::SetBkMode(dc, TRANSPARENT);

					{
						// プレビュー用のテキストを描画します。

						GdiObj<HFONT> font = ::CreateFont(
							hive.fontHeight, 0, 0, 0, 0, FALSE, FALSE, FALSE,
							DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
							DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, fontName);
						GdiObjSelector fontSelector(dc, font);

						COLORREF textColor = ::SetTextColor(dc, paint.getFontColor(stateId));

						::DrawTextW(dc, hive.previewText, -1, &rc, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE | DT_NOPREFIX);

						::SetTextColor(dc, textColor);
					}

					{
						// フォント名を描画します。

						COLORREF textColor = ::SetTextColor(dc, paint.getTextColor(stateId));

						::DrawTextW(dc, fontName, -1, &rc, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);

						::SetTextColor(dc, textColor);
					}

					::SetBkMode(dc, bkMode);
				}
			}

			return TRUE;
		}

		//
		// ウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_CREATE:
				{
					MY_TRACE_FUNC("WM_CREATE, 0x%08X, 0x%08X", wParam, lParam);

					if (!mainDialog.create(hwnd))
					{
						MY_TRACE(_T("メインダイアログの作成に失敗しました\n"));

						return -1;
					}

					setTarget(mainDialog);
					resize();

					break;
				}
			case WM_MEASUREITEM:
				{
					auto mis = (MEASUREITEMSTRUCT*)lParam;
					if (mis->CtlType == ODT_MENU)
						onMeasureItem(mis);

					break;
				}
			case WM_DRAWITEM:
				{
					auto dis = (DRAWITEMSTRUCT*)lParam;
					if (dis->CtlType == ODT_MENU)
						onDrawItem(dis);

					break;
				}
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}
	};
}
