#pragma once

namespace apn::exedit_tweaker
{
	//
	// このクラスはフック処理を担当します。
	//
	inline struct HookManager
	{
		//
		// 初期化処理です。
		// 拡張編集の関数をフックします。
		// 拡張編集の定数を書き換えます。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			// 拡張編集のモジュールハンドルを取得します。
			auto exedit = magi.exin.get_exedit();

			// 拡張編集内のタイムラインアイテム矩形を描画する関数をフックします。
			if (hive.gradient_fill.enabled) my::hook::attach(gradient_fill, exedit + 0x00036a70);

			// 拡張編集内の選択領域に関するコンスト値を書き換えます。
			if (hive.selection.fill.color != CLR_NONE) my::tools::set_abs_addr(exedit + 0x0003807E, &hive.selection.fill.color);
			if (hive.selection.stroke.color != CLR_NONE) my::tools::set_abs_addr(exedit + 0x00038076, &hive.selection.stroke.color);
			if (hive.selection.background.color != CLR_NONE) my::tools::set_abs_addr(exedit + 0x00038087, &hive.selection.background.color);

			// 拡張編集内のレイヤー境界を描画する関数をフックします。
			if (hive.layer.bound.left.color != CLR_NONE) my::hook::attach_call(layer.bound.left, exedit + 0x00038845);
			if (hive.layer.bound.top.color != CLR_NONE) my::hook::attach_call(layer.bound.top, exedit + 0x00038871);
			if (hive.layer.bound.right.color != CLR_NONE) my::hook::attach_call(layer.bound.right, exedit + 0x000388AA);
			if (hive.layer.bound.bottom.color != CLR_NONE) my::hook::attach_call(layer.bound.bottom, exedit + 0x000388DA);
			if (hive.layer.separator.color != CLR_NONE) my::hook::attach_call(layer.separator, exedit + 0x00037A1F);

			return DetourTransactionCommit() == NO_ERROR;
		}

		//
		// 後始末処理です。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}
		struct {
			//
			// 2色のグラデーションを描画する関数です
			// http://iooiau.net/tips/web20back.html
			//
			static BOOL TwoColorsGradient(
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
			inline static void CDECL hook_proc(HDC dc, const RECT *rc, BYTE r, BYTE g, BYTE b, BYTE gr, BYTE gg, BYTE gb, int gs, int ge)
			{
				MY_TRACE_FUNC("{}, {}", gs, ge);

				// グラデーションで塗りつぶします。
				auto color1 = RGB(r, g, b);
				auto color2 = RGB(gr, gg, gb);
				TwoColorsGradient(dc, rc, color1, color2, TRUE);

				// 矩形の枠を描画します。
				auto rc_frame = *rc;
				hive.gradient_fill.edge.outer.draw(dc, &rc_frame);
				hive.gradient_fill.edge.outer.deflate_rect(&rc_frame);
				hive.gradient_fill.edge.inner.draw(dc, &rc_frame);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} gradient_fill;

		struct {
			struct {
				struct {
					inline static void CDECL hook_proc(HDC dc, int mx, int my, int lx, int ly, HPEN pen)
					{
						MY_TRACE_FUNC("{:#010x}, {}, {}, {}, {}, {:#010x}", dc, mx, my, lx, ly, pen);

						hive.layer.bound.left.draw(dc, mx, my, 1, ly - my, pen);
					}
					inline static decltype(&hook_proc) orig_proc = nullptr;
				} left;

				struct {
					inline static void CDECL hook_proc(HDC dc, int mx, int my, int lx, int ly, HPEN pen)
					{
						MY_TRACE_FUNC("{:#010x}, {}, {}, {}, {}, {:#010x}", dc, mx, my, lx, ly, pen);

						hive.layer.bound.top.draw(dc, mx, my, lx - mx, 1, pen);
					}
					inline static decltype(&hook_proc) orig_proc = nullptr;
				} top;

				struct {
					inline static void CDECL hook_proc(HDC dc, int mx, int my, int lx, int ly, HPEN pen)
					{
						MY_TRACE_FUNC("{:#010x}, {}, {}, {}, {}, {:#010x}", dc, mx, my, lx, ly, pen);

						hive.layer.bound.right.draw(dc, mx, my, 1, ly - my, pen);
					}
					inline static decltype(&hook_proc) orig_proc = nullptr;
				} right;

				struct {
					inline static void CDECL hook_proc(HDC dc, int mx, int my, int lx, int ly, HPEN pen)
					{
						MY_TRACE_FUNC("{:#010x}, {}, {}, {}, {}, {:#010x}", dc, mx, my, lx, ly, pen);

						hive.layer.bound.bottom.draw(dc, mx, my, lx - mx, 1, pen);
					}
					inline static decltype(&hook_proc) orig_proc = nullptr;
				} bottom;
			} bound;

			struct {
				inline static void CDECL hook_proc(HDC dc, int mx, int my, int lx, int ly, HPEN pen)
				{
					MY_TRACE_FUNC("{:#010x}, {}, {}, {}, {}, {:#010x}", dc, mx, my, lx, ly, pen);

					hive.layer.separator.draw(dc, mx, my, 1, ly - my, pen);
				}
				inline static decltype(&hook_proc) orig_proc = nullptr;
			} separator;
		} layer;
	} hook_manager;
}
