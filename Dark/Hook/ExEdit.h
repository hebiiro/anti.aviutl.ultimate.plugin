#pragma once
#include "Resource.h"
#include "Skin.h"

namespace fgo::dark::hook
{
	//
	// このクラスは拡張編集をフックします。
	//
	inline struct ExEdit
	{
		inline static HFONT* font = 0;
		inline static int drawingLayerIndex = 0;
		inline static int (*ShowColorDialog)(DWORD u1, COLORREF* color, DWORD u3) = 0;

		inline static COLORREF getColorFromLayerName(LPCSTR name)
		{
			if (!name) return CLR_NONE;

			LPCSTR sep = strchr(name, '#');
			if (!sep) return CLR_NONE;

			COLORREF color = strtoul(sep + 1, 0, 16);
			return RGB(GetBValue(color), GetGValue(color), GetRValue(color));
		}

		inline static COLORREF getColorFromLayerIndex(int index)
		{
			for (int i = index; i >= 0; i--)
			{
				::ExEdit::LayerSetting* layer = fgo::magi.auin.GetLayerSetting(i);
				LPCSTR name = layer->name;
				if (!name) continue; // レイヤー名が無効だった。
				LPCSTR sep = strchr(name, '#');
				if (!sep) continue; // セパレータがなかった。
				if (!sep[1]) return CLR_NONE; // セパレータ以降が空文字列ならデフォルトカラーを使用します。
				COLORREF color = strtoul(sep + 1, 0, 16);
				return RGB(GetBValue(color), GetGValue(color), GetRValue(color));
			}

			return CLR_NONE;
		}

		inline static COLORREF getLayerBackgroundColor()
		{
			::ExEdit::LayerSetting* layer = fgo::magi.auin.GetLayerSetting(drawingLayerIndex);

			// ステートIDを取得します。
			int stateId = 0;
			if (!(layer->flag & ::ExEdit::LayerSetting::Flag::UnDisp))
			{
				if (skin::config.getUseLayerColor())
				{
					// ユーザーカラーを取得します。
					COLORREF userColor = CLR_NONE;
					if (skin::config.getUseLayerColorEx())
						userColor = getColorFromLayerIndex(drawingLayerIndex);
					else
						userColor = getColorFromLayerName(layer->name);

					// ユーザーカラーが有効なら使用します。
					if (userColor != CLR_NONE)
						return userColor;
				}

				stateId = skin::theme::EXEDIT_LAYER_ACTIVE;
			}
			else
			{
				stateId = skin::theme::EXEDIT_LAYER_INACTIVE;
			}

			// テーマカラーがあればそれを使用します。
			HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_EXEDIT);
			auto state = skin::theme::manager.getState(theme, skin::theme::EXEDIT_LAYERBACKGROUND, stateId);
			if (state && state->fillColor != CLR_NONE)
				return state->fillColor;

			return CLR_NONE;
		}

		inline static int getBlendCount(HDC dc)
		{
			COLORREF color = ::GetDCBrushColor(dc);
//			MY_TRACE_HEX(color);
			switch (color)
			{
			case 0x00DEDEDE: return 1;
			case 0x00CCCCCC: return 2;
			case 0x00BABABA: return 3;

			case 0x00BEBEBE: return 1;
			case 0x00B1B1B1: return 2;
			case 0x00A3A3A3: return 3;
			}

			return 0;
		}

		inline static COLORREF blend(COLORREF color1, COLORREF color2, float alpha)
		{
			float omega = 1.0f - alpha;

			BYTE r = (BYTE)(GetRValue(color1) * omega + GetRValue(color2) * alpha);
			BYTE g = (BYTE)(GetGValue(color1) * omega + GetGValue(color2) * alpha);
			BYTE b = (BYTE)(GetBValue(color1) * omega + GetBValue(color2) * alpha);

			return RGB(r, g, b);
		}

		void init()
		{
			MY_TRACE_FUNC("");

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			auto exedit = fgo::magi.auin.GetExEdit();

			font = (HFONT*)(exedit + 0x00167D84);

			Tools::attach_abs_call(FillTimelineHeader, exedit + 0x0003826C); // タイムライン左上部分の塗りつぶし。
			Tools::attach_abs_call(FillTimelineHeader, exedit + 0x00037F4F); // タイムラインヘッダー部分の塗りつぶし。

			Tools::attach_abs_call(DrawRootText, exedit + 0x0003833E);
			Tools::attach_abs_call(DrawRootEdge, exedit + 0x0003836A);
			Tools::attach_abs_call(DrawLayerText, exedit + 0x00037CFF);
			Tools::attach_abs_call(DrawLayerEdge, exedit + 0x00037D46);

			Tools::attach_call(DrawTimelineLongScale, exedit + 0x000380DF);
			Tools::attach_call(DrawTimelineShortScale, exedit + 0x000381D7);
			Tools::attach_call(DrawTimelineTime, exedit + 0x000381A2);

			Tools::attach_abs_call(FillLayerBackground, exedit + 0x00038538);
			Tools::attach_abs_call(FillLayerBackground, exedit + 0x0003860E);
			Tools::attach_abs_call(FillGroupBackground, exedit + 0x000386E4);

			Tools::attach_call(DrawLayerLeft, exedit + 0x00038845);
			Tools::attach_call(DrawLayerRight, exedit + 0x000388AA);
			Tools::attach_call(DrawLayerTop, exedit + 0x00038871);
			Tools::attach_call(DrawLayerBottom, exedit + 0x000388DA);
			Tools::attach_call(DrawLayerSeparator, exedit + 0x00037A1F);

			Tools::get_pointer(ShowColorDialog, exedit + 0x0004D2A0);
			Tools::attach_call(ShowLayerNameDialog, exedit + 0x426D2);
			Tools::attach(DrawLayerBackground, exedit + 0x00038410);

			Tools::attach_import(CreatePen, (HMODULE)exedit, "CreatePen");

			if (DetourTransactionCommit() == NO_ERROR)
			{
				MY_TRACE(_T("APIフックに成功しました\n"));
			}
			else
			{
				MY_TRACE(_T("APIフックに失敗しました\n"));
			}
		}

		inline static struct {
			inline static BOOL WINAPI hook(HDC dc, LPCRECT rc, HBRUSH brush)
			{
				MY_TRACE_FUNC("0x%08X, 0x%08X", dc, brush);

				HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_WINDOW);
				if (skin::theme::manager.onDrawThemeBackground(theme, dc, skin::theme::WINDOW_DIALOGFACE, 0, rc))
					return TRUE;

				return TRUE;
			}
			inline static decltype(&hook) orig = ::FillRect;
		} FillTimelineHeader;

		inline static struct {
			inline static BOOL WINAPI hook(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCSTR text, UINT c, CONST INT* dx)
			{
				MY_TRACE_FUNC("0x%08X, %d, %d, 0x%08X", dc, x, y, options);

				WCHAR text2[MAX_PATH] = {};
				UINT c2 = ::MultiByteToWideChar(CP_ACP, 0, text, c, text2, std::size(text2));

				HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_EXEDIT);
				if (skin::theme::manager.onExtTextOut(theme, dc, skin::theme::EXEDIT_ROOT, 0, x, y, options, rc, text2, c2, dx))
					return TRUE;

				return TRUE;
			}
			inline static decltype(&hook) orig = ::ExtTextOutA;
		} DrawRootText;

		inline static struct {
			inline static BOOL WINAPI hook(HDC dc, LPRECT rc, UINT edge, UINT flags)
			{
				MY_TRACE_FUNC("0x%08X, 0x%08X, 0x%08X", dc, edge, flags);

				HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_EXEDIT);
				if (skin::theme::manager.onDrawThemeBackground(theme, dc, skin::theme::EXEDIT_ROOT, 0, rc))
					return TRUE;

				return TRUE;
			}
			inline static decltype(&hook) orig = ::DrawEdge;
		} DrawRootEdge;

		inline static struct {
			inline static BOOL WINAPI hook(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCSTR text, UINT c, CONST INT* dx)
			{
				// レイヤーボタンのテキストを描画します。
				// 背景色で塗りつぶしも行います。

//				MY_TRACE_FUNC("0x%08X, %d, %d, 0x%08X", dc, x, y, options);

				WCHAR text2[MAX_PATH] = {};
				UINT c2 = ::MultiByteToWideChar(CP_ACP, 0, text, c, text2, std::size(text2));

				int stateId = skin::theme::EXEDIT_LAYER_ACTIVE;

				COLORREF bkColor = ::GetBkColor(dc);
//				MY_TRACE_HEX(bkColor);
				switch (bkColor)
				{
				case 0x00F0F0F0: stateId = skin::theme::EXEDIT_LAYER_ACTIVE; break;
				case 0x00CCCCCC: stateId = skin::theme::EXEDIT_LAYER_INACTIVE; break;
				}

				HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_EXEDIT);

				skin::theme::manager.onDrawThemeBackground(theme, dc, skin::theme::EXEDIT_LAYER, stateId, rc);

				{
					LPCWSTR sep = wcschr(text2, L'#');
					if (sep)
						c2 = (UINT)(sep - text2);
				}

				if (skin::theme::manager.drawText(dc, theme, skin::theme::EXEDIT_LAYER, stateId, (LPRECT)rc, text2, c2, DT_CENTER | DT_VCENTER | DT_SINGLELINE))
					return TRUE;
//				if (skin::theme::manager.onExtTextOut(theme, dc, skin::theme::EXEDIT_LAYER, stateId, x, y, options, rc, text2, c2, dx))
//					return TRUE;

				return ::ExtTextOutA(dc, x, y, options, rc, text, c, dx);
			}
			inline static decltype(&hook) orig = ::ExtTextOutA;
		} DrawLayerText;

		inline static struct {
			inline static BOOL WINAPI hook(HDC dc, LPRECT rc, UINT edge, UINT flags)
			{
				MY_TRACE_FUNC("0x%08X, 0x%08X, 0x%08X", dc, edge, flags);

				HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_EXEDIT);
				if (skin::theme::manager.onDrawThemeBackground(theme, dc, skin::theme::EXEDIT_LAYEREDGE, 0, rc))
					return TRUE;

				return ::DrawEdge(dc, rc, edge, flags);
			}
			inline static decltype(&hook) orig = ::DrawEdge;
		} DrawLayerEdge;

		inline static struct {
			inline static void CDECL hook(HDC dc, int mx, int my, int lx, int ly, HPEN pen)
			{
//				MY_TRACE_FUNC("0x%08X, %d, %d, %d, %d, 0x%08X", dc, mx, my, lx, ly, pen);

				HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_EXEDIT);
				auto state = skin::theme::manager.getState(theme, skin::theme::EXEDIT_LONGSCALE, 0);
				if (state && state->fillColor != CLR_NONE)
				{
					RECT rc = { mx, my, mx + 1, ly };
					painter::fillRect(dc, &rc, state->fillColor);
				}
			}
			inline static decltype(&hook) orig = 0;
		} DrawTimelineLongScale;

		inline static struct {
			inline static void CDECL hook(HDC dc, int mx, int my, int lx, int ly, HPEN pen)
			{
//				MY_TRACE_FUNC("0x%08X, %d, %d, %d, %d, 0x%08X", dc, mx, my, lx, ly, pen);

				HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_EXEDIT);
				auto state = skin::theme::manager.getState(theme, skin::theme::EXEDIT_SHORTSCALE, 0);
				if (state && state->fillColor != CLR_NONE)
				{
					RECT rc = { mx, my, mx + 1, ly };
					painter::fillRect(dc, &rc, state->fillColor);
				}
			}
			inline static decltype(&hook) orig = 0;
		} DrawTimelineShortScale;

		inline static struct {
			inline static void hook(HDC dc, LPCSTR text, int x, int y, int w, int h, int scroll_x)
			{
//				MY_TRACE_FUNC("0x%08X, %hs, %d, %d, %d, %d, %d", dc, text, x, y, w, h, scroll_x);

				::SelectObject(dc, *font);

				TEXTMETRIC tm = {};
				::GetTextMetrics(dc, &tm);

				RECT rc = { x, y, x + w, y + h };
				::OffsetRect(&rc, tm.tmHeight / 4 + scroll_x, 0);

				HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_EXEDIT);

				skin::theme::manager.onDrawThemeText(theme, dc, skin::theme::EXEDIT_LONGSCALE, 0,
					(_bstr_t)text, -1, DT_LEFT | DT_VCENTER | DT_SINGLELINE, &rc);
			}
			inline static decltype(&hook) orig = 0;
		} DrawTimelineTime;

		inline static struct {
			inline static int WINAPI hook(HDC dc, LPCRECT rc, HBRUSH brush)
			{
//				MY_TRACE_FUNC("0x%08X, 0x%08X", dc, brush);

				// レイヤーの背景色を取得します。
				COLORREF color = getLayerBackgroundColor();
				if (color != CLR_NONE)
				{
					// カラーを適用します。
					::SetDCBrushColor(dc, color);
				}

				return hive.orig.FillRect(dc, rc, (HBRUSH)::GetStockObject(DC_BRUSH));
			}
			inline static decltype(&hook) orig = ::FillRect;
		} FillLayerBackground;

		//
		// このクラスはグループ制御などの描画をフックします。
		//
		inline static struct {
			inline static int WINAPI hook(HDC dc, LPCRECT rc, HBRUSH brush)
			{
				MY_TRACE_FUNC("0x%08X, 0x%08X", dc, brush);

				// レイヤーの背景色を取得します。
				COLORREF color = getLayerBackgroundColor();
				if (color != CLR_NONE)
				{
					// ブレンド回数を取得します。
					int blendCount = getBlendCount(dc);
					for (int i = 0; i < blendCount; i++)
					{
						// ブレンド回数分だけグループカラーとブレンドします。
						color = blend(color, skin::config.exedit.group.color, skin::config.exedit.group.alpha);
					}
					// ブレンドしたカラーを適用します。
					::SetDCBrushColor(dc, color);
				}
				return hive.orig.FillRect(dc, rc, brush);
			}
			inline static decltype(&hook) orig = ::FillRect;
		} FillGroupBackground;

		inline static struct {
			inline static void hook(HDC dc, int mx, int my, int lx, int ly, HPEN pen)
			{
				MY_TRACE_FUNC("0x%08X, %d, %d, %d, %d, 0x%08X", dc, mx, my, lx, ly, pen);

				if (pen) ::SelectObject(dc, pen);

				HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_EXEDIT);
				auto state = skin::theme::manager.getState(theme, skin::theme::EXEDIT_LAYERLEFT, 0);
				if (state && state->fillColor != CLR_NONE)
				{
					HBRUSH oldBrush = (HBRUSH)::SelectObject(dc, state->getFillBrush());
					::PatBlt(dc, mx, my, 1, ly - my, PATCOPY);
					::SelectObject(dc, oldBrush);
				}
			}
			inline static decltype(&hook) orig = 0;
		} DrawLayerLeft;

		inline static struct {
			inline static void hook(HDC dc, int mx, int my, int lx, int ly, HPEN pen)
			{
				MY_TRACE_FUNC("0x%08X, %d, %d, %d, %d, 0x%08X", dc, mx, my, lx, ly, pen);

				if (pen) ::SelectObject(dc, pen);

				HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_EXEDIT);
				auto state = skin::theme::manager.getState(theme, skin::theme::EXEDIT_LAYERRIGHT, 0);
				if (state && state->fillColor != CLR_NONE)
				{
					HBRUSH oldBrush = (HBRUSH)::SelectObject(dc, state->getFillBrush());
					::PatBlt(dc, mx, my, 1, ly - my, PATCOPY);
					::SelectObject(dc, oldBrush);
				}
			}
			inline static decltype(&hook) orig = 0;
		} DrawLayerRight;

		inline static struct {
			inline static void hook(HDC dc, int mx, int my, int lx, int ly, HPEN pen)
			{
				MY_TRACE_FUNC("0x%08X, %d, %d, %d, %d, 0x%08X", dc, mx, my, lx, ly, pen);

				if (pen) ::SelectObject(dc, pen);

				HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_EXEDIT);
				auto state = skin::theme::manager.getState(theme, skin::theme::EXEDIT_LAYERTOP, 0);
				if (state && state->fillColor != CLR_NONE)
				{
					HBRUSH oldBrush = (HBRUSH)::SelectObject(dc, state->getFillBrush());
					::PatBlt(dc, mx, my, lx - mx, 1, PATCOPY);
					::SelectObject(dc, oldBrush);
				}
			}
			inline static decltype(&hook) orig = 0;
		} DrawLayerTop;

		inline static struct {
			inline static void hook(HDC dc, int mx, int my, int lx, int ly, HPEN pen)
			{
				MY_TRACE_FUNC("0x%08X, %d, %d, %d, %d, 0x%08X", dc, mx, my, lx, ly, pen);

				if (pen) ::SelectObject(dc, pen);

				HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_EXEDIT);
				auto state = skin::theme::manager.getState(theme, skin::theme::EXEDIT_LAYERBOTTOM, 0);
				if (state && state->fillColor != CLR_NONE)
				{
					HBRUSH oldBrush = (HBRUSH)::SelectObject(dc, state->getFillBrush());
					::PatBlt(dc, mx, my, lx - mx, 1, PATCOPY);
					::SelectObject(dc, oldBrush);
				}
			}
			inline static decltype(&hook) orig = 0;
		} DrawLayerBottom;

		inline static struct {
			inline static void hook(HDC dc, int mx, int my, int lx, int ly, HPEN pen)
			{
//				MY_TRACE_FUNC("0x%08X, %d, %d, %d, %d, 0x%08X", dc, mx, my, lx, ly, pen);

				if (pen) ::SelectObject(dc, pen);

				HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_EXEDIT);
				auto state = skin::theme::manager.getState(theme, skin::theme::EXEDIT_LAYERSEPARATOR, 0);
				if (state && state->fillColor != CLR_NONE)
				{
					HBRUSH oldBrush = (HBRUSH)::SelectObject(dc, state->getFillBrush());
					::PatBlt(dc, mx, my, 1, ly - my, PATCOPY);
					::SelectObject(dc, oldBrush);
				}
			}
			inline static decltype(&hook) orig = 0;
		} DrawLayerSeparator;

		inline static struct {
			inline static void CDECL hook(HDC dc, int layerIndex, DWORD a3, DWORD a4, DWORD a5, DWORD a6, DWORD a7)
			{
//				MY_TRACE_FUNC("0x%08X, %d, %d, %d, %d, %d, %d", dc, layerIndex, a3, a4, a5, a6, a7);

				// ここで現在描画しているレイヤーのインデックスを取得します。
				drawingLayerIndex = layerIndex;

				orig(dc, layerIndex, a3, a4, a5, a6, a7);
			}
			inline static decltype(&hook) orig = 0;
		} DrawLayerBackground;

		inline static struct {
			inline static INT_PTR CALLBACK hook(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
			{
				switch (message)
				{
				case WM_INITDIALOG:
					{
						MY_TRACE_FUNC("WM_INITDIALOG");

						break;
					}
				case WM_COMMAND:
					{
						UINT code = HIWORD(wParam);
						UINT id = LOWORD(wParam);
						HWND sender = (HWND)lParam;

						MY_TRACE_FUNC("WM_COMMAND, 0x%04X, 0x%04X, 0x%08X", code, id, sender);

						if (id == IDC_CHOOSE_COLOR)
						{
							char text[MAX_PATH] = {};
							::GetDlgItemTextA(hdlg, 171, text, std::size(text));
							MY_TRACE_STR(text);

							COLORREF color = getColorFromLayerName(text);
							MY_TRACE_HEX(color);

							if (IDOK != ShowColorDialog(0, &color, 2))
								break;

							LPSTR sep = strchr(text, '#');
							if (sep) *sep = '\0';

							char newText[MAX_PATH] = {};
							::StringCchPrintfA(newText, std::size(newText), "%s#%02x%02x%02x",
								text, GetRValue(color), GetGValue(color), GetBValue(color));
							::SetDlgItemTextA(hdlg, 171, newText);
						}

						break;
					}
				}

				return orig(hdlg, message, wParam, lParam);
			}
			inline static decltype(&hook) orig = 0;
		} LayerNameDialogProc;

		inline static struct {
			inline static INT_PTR CDECL hook(HINSTANCE instance, LPCSTR templateName, HWND parent, DLGPROC dialogProc)
			{
				MY_TRACE_FUNC("0x%08X, %hs, 0x%08X, 0x%08X", instance, templateName, parent, dialogProc);

				if (::lstrcmpiA(templateName, "GET_LAYER_NAME") == 0)
				{
					MY_TRACE(_T("「レイヤー名」ダイアログをフックします\n"));

					LayerNameDialogProc.orig = dialogProc;
					return orig(hive.instance, templateName, parent, LayerNameDialogProc.hook);
				}

				return orig(instance, templateName, parent, dialogProc);
			}
			inline static decltype(&hook) orig = 0;
		} ShowLayerNameDialog;

		struct {
			inline static HPEN WINAPI hook(int style, int width, COLORREF color)
			{
				if (style == PS_SOLID && width == 0 && color == RGB(0x00, 0xff, 0xff))
				{
					if (skin::config.exedit.xorPen.style != -1) style = skin::config.exedit.xorPen.style;
					if (skin::config.exedit.xorPen.width != -1) width = skin::config.exedit.xorPen.width;
					if (skin::config.exedit.xorPen.color != CLR_NONE) color = skin::config.exedit.xorPen.color;
				}

				return orig(style, width, color);
			}
			inline static decltype(&hook) orig = ::CreatePen;
		} CreatePen;
	} exedit;
}
