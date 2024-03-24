#pragma once

namespace apn
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr LPCWSTR c_name = L"exedit_tweaker";
		inline static constexpr LPCWSTR c_display_name = L"拡張編集微調整";

		HINSTANCE instance = 0;

		struct Fill {
			COLORREF color;
		};

		struct Stroke {
			COLORREF color;

			void draw(HDC dc, int x, int y, int w, int h, HPEN pen) const
			{
				if (pen) ::SelectObject(dc, pen);
				if (color == CLR_NONE) return;
				HBRUSH brush = ::CreateSolidBrush(color);
				HBRUSH oldBrush = (HBRUSH)::SelectObject(dc, brush);
				::PatBlt(dc, x, y, w, h, PATCOPY);
				::SelectObject(dc, oldBrush);
				::DeleteObject(brush);
			}
		};

		struct FrameRect {
			COLORREF color;
			SIZE size;

			//
			// 矩形の枠を描画します。
			//
			void draw(HDC dc, LPCRECT rc) const
			{
				if (color == CLR_NONE) return;

				int x = rc->left;
				int y = rc->top;
				int w = rc->right - rc->left;
				int h = rc->bottom - rc->top;

				HBRUSH brush = ::CreateSolidBrush(color);
				HBRUSH oldBrush = (HBRUSH)::SelectObject(dc, brush);
				if (size.cy > 0) {
					::PatBlt(dc, x, y, w, size.cy, PATCOPY);
					::PatBlt(dc, x, y + h, w, -size.cy, PATCOPY);
				}
				if (size.cx > 0) {
					::PatBlt(dc, x, y, size.cx, h, PATCOPY);
					::PatBlt(dc, x + w, y, -size.cx, h, PATCOPY);
				}
				::SelectObject(dc, oldBrush);
				::DeleteObject(brush);
			}

			//
			// 矩形を収縮させます。
			//
			void deflate_rect(LPRECT rc) const
			{
				::InflateRect(rc, -size.cx, -size.cy);
			}
		};

		struct GradientFill {
			BOOL enable = FALSE; // APIによるグラデーション描画を有効にします。
			struct {
				FrameRect inner = { RGB(0xff, 0xff, 0xff), { 1, 1 } }; // アイテム矩形の内側の枠です。
				FrameRect outer = { RGB(0x00, 0x00, 0x00), { 1, 1 } }; // アイテム矩形の外側の枠です。
			} edge;
		} gradient_fill;

		struct Selection {
			Fill fill = { CLR_NONE }; // 選択部分の塗りつぶしです。
			Stroke stroke = { CLR_NONE }; // 選択部分の端のストロークです。
			Fill background = { CLR_NONE }; // 選択以外の部分の塗りつぶしです。
		} selection;

		struct Layer {
			struct Bound {
				Stroke left = { CLR_NONE };
				Stroke top = { CLR_NONE };
				Stroke right = { CLR_NONE };
				Stroke bottom = { CLR_NONE };
			} bound;
			Stroke separator = { CLR_NONE };
		} layer;

		int message_box(const std::wstring& text) {
			return ::MessageBoxW(0, text.c_str(), c_name, MB_OK | MB_ICONWARNING);
		}
	} hive;
}
