#pragma once

namespace apn
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"exedit_tweaker";
		inline static constexpr auto c_display_name = L"拡張編集微調整";

		struct Fill {
			COLORREF color;
		};

		struct Stroke {
			COLORREF color;

			void draw(HDC dc, int x, int y, int w, int h, HPEN pen) const
			{
				if (pen) ::SelectObject(dc, pen);
				if (color == CLR_NONE) return;

				my::gdi::unique_ptr<HBRUSH> brush(::CreateSolidBrush(color));
				my::gdi::selector brush_selector(dc, brush.get());

				::PatBlt(dc, x, y, w, h, PATCOPY);
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

				auto x = rc->left;
				auto y = rc->top;
				auto w = rc->right - rc->left;
				auto h = rc->bottom - rc->top;

				my::gdi::unique_ptr<HBRUSH> brush(::CreateSolidBrush(color));
				my::gdi::selector brush_selector(dc, brush.get());

				if (size.cy > 0) {
					::PatBlt(dc, x, y, w, size.cy, PATCOPY);
					::PatBlt(dc, x, y + h, w, -size.cy, PATCOPY);
				}
				if (size.cx > 0) {
					::PatBlt(dc, x, y, size.cx, h, PATCOPY);
					::PatBlt(dc, x + w, y, -size.cx, h, PATCOPY);
				}
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
			BOOL enabled = FALSE; // APIによるグラデーション描画を有効にします。
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

		//
		// このアドインのインスタンスハンドルです。
		//
		HINSTANCE instance = nullptr;

		//
		// コンフィグのファイル名です。
		//
		std::wstring config_file_name;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
