#pragma once
#include "Tools/Hook.h"
#include "Hive.h"
#include "Servant.h"

namespace aut
{
	//
	// このクラスはエディットボックスを微調整するサーヴァントです。
	//
	inline struct ExEditTweaker : Servant
	{
		//
		// このサーヴァントを識別するための名前を返します。
		//
		inline static LPCWSTR getServantNameStatic() { return L"ExEditTweaker"; }

		struct Fill {
			COLORREF color;
		};

		struct Stroke {
			COLORREF color;
		};

		struct GradientFill {
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

			BOOL enable;
			FrameRect inner;
			FrameRect outer;

			//
			// 2色のグラデーションを描画する関数です
			// http://iooiau.net/tips/web20back.html
			//
			BOOL TwoColorsGradient(
				HDC hdc,            // 描画先のデバイスコンテキスト・ハンドルです
				const RECT *pRect,  // 描画する範囲の矩形です
				COLORREF Color1,    // 描画する一つ目の色です
				COLORREF Color2,    // 描画する二つ目の色です
				BOOL fHorizontal    // 水平のグラデーションを描画する場合は TRUE にします
			)
			{
				TRIVERTEX vert[2];
				GRADIENT_RECT rect = {0, 1};

				// 描画範囲と色を設定します
				vert[0].x = pRect->left;
				vert[0].y = pRect->top;
				vert[0].Red   = GetRValue(Color1) << 8;
				vert[0].Green = GetGValue(Color1) << 8;
				vert[0].Blue  = GetBValue(Color1) << 8;
				vert[0].Alpha = 0;
				vert[1].x = pRect->right;
				vert[1].y = pRect->bottom;
				vert[1].Red   = GetRValue(Color2) << 8;
				vert[1].Green = GetGValue(Color2) << 8;
				vert[1].Blue  = GetBValue(Color2) << 8;
				vert[1].Alpha = 0;
				return ::GdiGradientFill(hdc, vert, 2, &rect, 1,
					fHorizontal ? GRADIENT_FILL_RECT_H : GRADIENT_FILL_RECT_V);
			}

			//
			// 拡張編集内のタイムラインアイテム矩形を描画する関数と置き換えられます。
			// グラデーション描画処理を簡素化します。
			// ついでに枠も描画します。
			//
			static void CDECL hook(HDC dc, const RECT *rc, BYTE r, BYTE g, BYTE b, BYTE gr, BYTE gg, BYTE gb, int gs, int ge)
			{
			//	MY_TRACE(_T("GradientFill(%d, %d)\n"), gs, ge);

				// グラデーションで塗りつぶします。
				COLORREF color1 = RGB(r, g, b);
				COLORREF color2 = RGB(gr, gg, gb);
//				TwoColorsGradient(dc, rc, color1, color2, TRUE);

				// 矩形の枠を描画します。
				RECT rcFrame = *rc;
				exedit_tweaker.gradientFill.outer.draw(dc, &rcFrame);
				exedit_tweaker.gradientFill.outer.deflate_rect(&rcFrame);
				exedit_tweaker.gradientFill.inner.draw(dc, &rcFrame);
			}
		}; Tools::Hook<GradientFill> gradientFill;

		struct Selection {
			Fill fill; // 選択部分の塗りつぶし。
			Stroke stroke; // 選択部分の端のストローク。
			Fill background; // 選択以外の部分の塗りつぶし。
		} selection;

		struct Layer {
			struct Line : Stroke {
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

			struct Bound {
				struct Left : Line {
					static void CDECL hook(HDC dc, int mx, int my, int lx, int ly, HPEN pen)
					{
//						MY_TRACE(_T("ExEditTweaker::Layer::Bound::Left::hook(0x%08X, %d, %d, %d, %d, 0x%08X)\n"), dc, mx, my, lx, ly, pen);

						exedit_tweaker.layer.bound.left.draw(dc, mx, my, 1, ly - my, pen);
					}
				}; Tools::Hook<Left> left;

				struct Right : Line {
					static void CDECL hook(HDC dc, int mx, int my, int lx, int ly, HPEN pen)
					{
//						MY_TRACE(_T("ExEditTweaker::Layer::Bound::Right::hook(0x%08X, %d, %d, %d, %d, 0x%08X)\n"), dc, mx, my, lx, ly, pen);

						exedit_tweaker.layer.bound.right.draw(dc, mx, my, 1, ly - my, pen);
					}
				}; Tools::Hook<Right> right;

				struct Top : Line {
					static void CDECL hook(HDC dc, int mx, int my, int lx, int ly, HPEN pen)
					{
//						MY_TRACE(_T("ExEditTweaker::Layer::Bound::Top::hook(0x%08X, %d, %d, %d, %d, 0x%08X)\n"), dc, mx, my, lx, ly, pen);

						exedit_tweaker.layer.bound.left.draw(dc, mx, my, lx - mx, 1, pen);
					}
				}; Tools::Hook<Top> top;

				struct Bottom : Line {
					static void CDECL hook(HDC dc, int mx, int my, int lx, int ly, HPEN pen)
					{
//						MY_TRACE(_T("ExEditTweaker::Layer::Bound::Bottom::hook(0x%08X, %d, %d, %d, %d, 0x%08X)\n"), dc, mx, my, lx, ly, pen);

						exedit_tweaker.layer.bound.left.draw(dc, mx, my, lx - mx, 1, pen);
					}
				}; Tools::Hook<Bottom> bottom;
			} bound;

			struct Separator : Line {
				static void CDECL hook(HDC dc, int mx, int my, int lx, int ly, HPEN pen)
				{
//					MY_TRACE(_T("ExEditTweaker::Layer::Separator::hook(0x%08X, %d, %d, %d, %d, 0x%08X)\n"), dc, mx, my, lx, ly, pen);

					exedit_tweaker.layer.separator.draw(dc, mx, my, 1, ly - my, pen);
				}
			}; Tools::Hook<Separator> separator;
		} layer;

		struct EditBox {
			int delta;
			struct Font {
				_bstr_t name;
				int height;
				int pitch;
				HFONT handle;
			} font;

			static LRESULT CALLBACK subclassproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR id, DWORD_PTR)
			{
				switch (msg) {
				case WM_SETFONT:
					MY_TRACE(_T("WM_SETFONT, 0x%08X, 0x%08X\n"), wparam, lparam);
					wparam = reinterpret_cast<WPARAM>(exedit_tweaker.editbox.font.handle); // handle is not null here.
				//[[fallthrough]];
				case WM_DESTROY:
					::RemoveWindowSubclass(hwnd, subclassproc, id);
					break;
				}

				return ::DefSubclassProc(hwnd, msg, wparam, lparam);
			}

			//
			// 拡張編集内の複数行エディットボックスを作成する関数(::CreateWindowExW())と置き換えられます。
			//
			static HWND WINAPI hook(DWORD exStyle, LPCWSTR className, LPCWSTR windowName, DWORD style,
				int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param)
			{
				MY_TRACE(_T("CreateWindowExW(%ws, %d, %d)\n"), className, w, h);

				HWND hwnd = exedit_tweaker.editbox.orig(exStyle, className, windowName,
					style, x, y, w, h + exedit_tweaker.editbox.delta, parent, menu, instance, param);

				if (exedit_tweaker.editbox.font.handle != nullptr)
					::SetWindowSubclass(hwnd, subclassproc, 0, 0);

				return hwnd;
			}
		}; Tools::Hook<EditBox> editbox;

		//
		// コンストラクタです。
		// コンフィグの初期値を設定します。
		//
		ExEditTweaker()
		{
			gradientFill.enable = FALSE;
			gradientFill.inner.color = RGB(0xff, 0xff, 0xff);
			gradientFill.inner.size.cx = 1;
			gradientFill.inner.size.cy = 1;
			gradientFill.outer.color = RGB(0x00, 0x00, 0x00);
			gradientFill.outer.size.cx = 1;
			gradientFill.outer.size.cy = 1;

			selection.fill.color = CLR_NONE;
			selection.stroke.color = CLR_NONE;
			selection.background.color = CLR_NONE;

			layer.bound.left.color = CLR_NONE;
			layer.bound.right.color = CLR_NONE;
			layer.bound.top.color = CLR_NONE;
			layer.bound.bottom.color = CLR_NONE;
			layer.separator.color = CLR_NONE;

			editbox.delta = 0;
			editbox.font.name = L"";
			editbox.font.height = 0;
			editbox.font.pitch = 0;
		}

		//
		// マスターから呼ばれます。
		//
		BOOL on_init() override
		{
			if (!load()) return FALSE;
			if (!init()) return FALSE;
			return TRUE;
		}

		//
		// マスターから呼ばれます。
		//
		BOOL on_exit() override
		{
			if (!save()) return FALSE;
			if (!exit()) return FALSE;
			return TRUE;
		}

		//
		// 設定をコンフィグファイルから読み込みます。
		//
		BOOL load()
		{
			std::wstring path = hive.getConfigFileName(L"ExEditTweaker.ini");

			getPrivateProfileInt(path.c_str(), L"Config", L"gradientFill.enable", gradientFill.enable);
			getPrivateProfileColor(path.c_str(), L"Config", L"gradientFill.inner.color", gradientFill.inner.color);
			getPrivateProfileInt(path.c_str(), L"Config", L"gradientFill.inner.size.cx", gradientFill.inner.size.cx);
			getPrivateProfileInt(path.c_str(), L"Config", L"gradientFill.inner.size.cy", gradientFill.inner.size.cy);
			getPrivateProfileColor(path.c_str(), L"Config", L"gradientFill.outer.color", gradientFill.outer.color);
			getPrivateProfileInt(path.c_str(), L"Config", L"gradientFill.outer.size.cx", gradientFill.outer.size.cx);
			getPrivateProfileInt(path.c_str(), L"Config", L"gradientFill.outer.size.cy", gradientFill.outer.size.cy);

			getPrivateProfileColor(path.c_str(), L"Config", L"selection.fill.color", selection.fill.color);
			getPrivateProfileColor(path.c_str(), L"Config", L"selection.stroke.color", selection.stroke.color);
			getPrivateProfileColor(path.c_str(), L"Config", L"selection.background.color", selection.background.color);

			getPrivateProfileColor(path.c_str(), L"Config", L"layer.bound.left.color", layer.bound.left.color);
			getPrivateProfileColor(path.c_str(), L"Config", L"layer.bound.right.color", layer.bound.right.color);
			getPrivateProfileColor(path.c_str(), L"Config", L"layer.bound.top.color", layer.bound.top.color);
			getPrivateProfileColor(path.c_str(), L"Config", L"layer.bound.bottom.color", layer.bound.bottom.color);
			getPrivateProfileColor(path.c_str(), L"Config", L"layer.separator.color", layer.separator.color);

			getPrivateProfileInt(path.c_str(), L"Config", L"editbox.delta", editbox.delta);
			getPrivateProfileBSTR(path.c_str(), L"Config", L"editbox.font.name", editbox.font.name);
			getPrivateProfileInt(path.c_str(), L"Config", L"editbox.font.height", editbox.font.height);
			getPrivateProfileInt(path.c_str(), L"Config", L"editbox.font.pitch", editbox.font.pitch);

			if (wcslen(editbox.font.name) != 0)
			{
				// DPI に合わせてフォントのサイズを調整します。
				int dpi = ::GetSystemDpiForProcess(::GetCurrentProcess());
				int height = ::MulDiv(editbox.font.height, dpi, 96);

				// HFONT を作成します。
				// 複数行エディットボックスの WM_SETFONT でこのハンドルが渡されます。
				editbox.font.handle = ::CreateFontW(height, 0,
					0, 0, 0, 0, 0, 0, DEFAULT_CHARSET,
					0, 0, 0, editbox.font.pitch, editbox.font.name);
			}

			return TRUE;
		}

		//
		// 設定をコンフィグファイルに保存します。
		//
		BOOL save()
		{
			return TRUE;
		}

		//
		// 初期化処理です。
		// 拡張編集の関数をフックします。
		// 拡張編集のコンスト値 (コードメモリ) を書き換えます。
		//
		BOOL init()
		{
			using namespace Tools;

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			// 拡張編集のモジュールハンドルを取得します。
			addr_t exedit_auf = (addr_t)::GetModuleHandle(_T("exedit.auf"));

			// 拡張編集内のタイムラインアイテム矩形を描画する関数をフックします。
			if (gradientFill.enable) gradientFill.attach(exedit_auf + 0x00036a70);

			// 拡張編集内の選択領域に関するコンスト値を書き換えます。
			if (selection.fill.color != CLR_NONE) writeAbsoluteAddress(exedit_auf + 0x0003807E, &selection.fill.color);
			if (selection.stroke.color != CLR_NONE) writeAbsoluteAddress(exedit_auf + 0x00038076, &selection.stroke.color);
			if (selection.background.color != CLR_NONE) writeAbsoluteAddress(exedit_auf + 0x00038087, &selection.background.color);

			// 拡張編集内のレイヤー境界を描画する関数をフックします。
			if (layer.bound.left.color != CLR_NONE) layer.bound.left.attach_to_call(exedit_auf + 0x00038845);
			if (layer.bound.right.color != CLR_NONE) layer.bound.right.attach_to_call(exedit_auf + 0x000388AA);
			if (layer.bound.top.color != CLR_NONE) layer.bound.top.attach_to_call(exedit_auf + 0x00038871);
			if (layer.bound.bottom.color != CLR_NONE) layer.bound.bottom.attach_to_call(exedit_auf + 0x000388DA);
			if (layer.separator.color != CLR_NONE) layer.separator.attach_to_call(exedit_auf + 0x00037A1F);

			// 拡張編集内の ::CreateWindowExW() の呼び出しをフックします。
			// コンスト値も書き換えます。
			if (editbox.delta != 0 || editbox.font.handle != nullptr) {
				editbox.attach_to_abs_call(exedit_auf + 0x0008C46E);
				editbox.attach_to_abs_call(exedit_auf + 0x00087658);
				add_int32(exedit_auf + 0x0008CC56 + 1, editbox.delta);
				add_int32(exedit_auf + 0x000876DE + 1, editbox.delta);
			}

			return DetourTransactionCommit() == NO_ERROR;
		}

		//
		// 後始末処理です。
		//
		BOOL exit()
		{
			if (editbox.font.handle)
				::DeleteObject(editbox.font.handle), editbox.font.handle = 0;

			return TRUE;
		}
	} exedit_tweaker;
}
