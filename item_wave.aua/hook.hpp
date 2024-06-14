#pragma once

namespace apn::item_wave
{
	//
	// このクラスはフックを管理します。
	//
	inline struct HookManager
	{
		//
		// 現在描画中のアイテムです。
		//
		ExEdit::Object* current_item = nullptr;

		//
		// 初期化処理です。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// 拡張編集のモジュールハンドルを取得します。
			auto exedit = magi.exin.get_exedit();
			if (!exedit) return FALSE;

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			draw_item.orig_proc = magi.exin.address.function.draw_item;
			my::hook::attach(draw_item);
			my::hook::attach_abs_call(ExtTextOutA, exedit + 0x0003794B);

			if (DetourTransactionCommit() == NO_ERROR)
			{
				MY_TRACE("拡張編集のフックに成功しました\n");
			}
			else
			{
				MY_TRACE("拡張編集のフックに失敗しました\n");
			}

			return TRUE;
		}

		//
		// 後始末処理です。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		inline BOOL check_min(std::vector<POINT>& points, int x, int y)
		{
			auto& back = points.back();
			if (back.x != x) return TRUE;
			back.y = std::min<int>(back.y, y);
			return FALSE;
		}

		inline BOOL check_max(std::vector<POINT>& points, int x, int y)
		{
			auto& back = points.back();
			if (back.x != x) return TRUE;
			back.y = std::max<int>(back.y, y);
			return FALSE;
		}

		void draw_wave(HDC dc, LPCRECT rc_clip, LPCRECT rc_item)
		{
			MY_TRACE_FUNC("");

			if (!current_item) return;

			auto cache = item_cache_manager.get_cache(current_item);
			if (!cache) return;

			auto scale = std::max<int>(1, hive.scale);

			auto w = rc_item->right - rc_item->left;
			auto h = rc_item->bottom - rc_item->top;
			auto cy = (rc_item->top + rc_item->bottom) / 2;

			auto c = (int)cache->volumes.size();
			MY_TRACE_INT(c);

			std::vector<POINT> points;

			switch (hive.wave_type)
			{
			case hive.c_wave_type.c_center:
				{
					// 中央から上下へ描画します。

					points.emplace_back(std::max<int>(rc_clip->left, rc_item->left), cy);

					decltype(c) i = 0;

					for (; i < c; i++)
					{
						auto x = (int)magi.exin.frame_to_x(i + current_item->frame_begin);

						if (x > rc_clip->left) break;
					}

					auto check_index = i = std::max<int>(0, i - 1);

					for (; i < c; i++)
					{
						const auto& volume = cache->volumes[i];
						auto x = (int)magi.exin.frame_to_x(i + current_item->frame_begin);
						auto y = cy - (int)(volume.level * scale);

						if (i != check_index && check_min(points, x, y))
							points.emplace_back(x, y);

						if (x > rc_clip->right) break;
					}

					auto lx = (int)magi.exin.frame_to_x(i + current_item->frame_begin);
					lx = std::min<int>(lx, rc_clip->right);

					points.emplace_back(lx, points.back().y);
					points.emplace_back(lx, cy);
					points.emplace_back(lx, points.back().y);

					check_index = i = std::min<int>(i, c - 1);

					for (; i >= 0; i--)
					{
						const auto& volume = cache->volumes[i];
						auto x = (int)magi.exin.frame_to_x(i + current_item->frame_begin);
						auto y = cy + (int)(volume.level * scale);

						if (i != check_index && check_max(points, x, y))
							points.emplace_back(x, y);

						if (x < rc_clip->left) break;
					}

					break;
				}
			case hive.c_wave_type.c_bottom_up:
				{
					// 下から上へ描画します。

					points.emplace_back(std::max<int>(rc_clip->left, rc_item->left), rc_item->bottom);

					decltype(c) i = 0;

					for (; i < c; i++)
					{
						auto x = (int)magi.exin.frame_to_x(i + current_item->frame_begin);

						if (x > rc_clip->left) break;
					}

					auto check_index = i = std::max<int>(0, i - 1);

					for (; i < c; i++)
					{
						const auto& volume = cache->volumes[i];
						auto x = (int)magi.exin.frame_to_x(i + current_item->frame_begin);
						auto y = rc_item->bottom - (int)(volume.level * scale);

						if (i != check_index && check_min(points, x, y))
							points.emplace_back(x, y);

						if (x > rc_clip->right) break;
					}

					auto lx = (int)magi.exin.frame_to_x(i + current_item->frame_begin);
					lx = std::min<int>(lx, rc_clip->right);

					points.emplace_back(lx, points.back().y);
					points.emplace_back(lx, rc_item->bottom);

					break;
				}
			case hive.c_wave_type.c_top_down:
				{
					// 上から下へ描画します。

					points.emplace_back(std::max<int>(rc_clip->left, rc_item->left), rc_item->top);

					decltype(c) i = 0;

					for (; i < c; i++)
					{
						auto x = (int)magi.exin.frame_to_x(i + current_item->frame_begin);

						if (x > rc_clip->left) break;
					}

					auto check_index = i = std::max<int>(0, i - 1);

					for (; i < c; i++)
					{
						const auto& volume = cache->volumes[i];
						auto x = (int)magi.exin.frame_to_x(i + current_item->frame_begin);
						auto y = rc_item->top + (int)(volume.level * scale);

						if (i != check_index && check_max(points, x, y))
							points.emplace_back(x, y);

						if (x > rc_clip->right) break;
					}

					auto lx = (int)magi.exin.frame_to_x(i + current_item->frame_begin);
					lx = std::min<int>(lx, rc_clip->right);

					points.emplace_back(lx, points.back().y);
					points.emplace_back(lx, rc_item->top);

					break;
				}
			}

			my::gdi::unique_ptr<HPEN> pen(hive.pen_color != CLR_NONE ?
				::CreatePen(PS_SOLID, 1, hive.pen_color) : (HPEN)::GetStockObject(NULL_PEN));
			my::gdi::unique_ptr<HBRUSH> brush(hive.brush_color != CLR_NONE ?
				::CreateSolidBrush(hive.brush_color) : (HBRUSH)::GetStockObject(NULL_BRUSH));
			{
				my::gdi::selector pen_selector(dc, pen.get());
				my::gdi::selector brush_selector(dc, brush.get());
				auto rop2 = ::GetROP2(dc);
				switch (hive.xor_mode) {
				case hive.c_xor_mode.c_xor: ::SetROP2(dc, R2_XORPEN); break;
				case hive.c_xor_mode.c_not_xor: ::SetROP2(dc, R2_NOTXORPEN); break;
				case hive.c_xor_mode.c_not: ::SetROP2(dc, R2_NOT); break;
				}
				::Polygon(dc, points.data(), points.size());
				::SetROP2(dc, rop2);
			}
			if (hive.pen_color == CLR_NONE) pen.release();
			if (hive.brush_color == CLR_NONE) brush.release();
		}

		//
		// このクラスは拡張編集内の::ExtTextOutA()の呼び出しをフックします。
		//
		inline static struct {
			inline static void CDECL hook_proc(HDC dc, int object_index)
			{
				MY_TRACE_FUNC("{}", object_index);

				// 描画中のアイテムを取得しておきます。
				hook_manager.current_item = magi.exin.get_object(object_index);

				// デフォルト処理を実行します。
				orig_proc(dc, object_index);

				// 描画中のアイテムを消去します。
				hook_manager.current_item = nullptr;
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} draw_item;

		//
		// このクラスは拡張編集内の::ExtTextOutA()の呼び出しをフックします。
		//
		inline static struct {
			inline static BOOL WINAPI hook_proc(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCSTR text, UINT c, CONST INT* dx)
			{
				MY_TRACE_FUNC("{:#010x}, {}, {}, {:#010x}", dc, x, y, options);
//				MY_TRACE_RECT2(rc[0]); // クリッピング矩形
//				MY_TRACE_RECT2(rc[1]); // アイテム全体の矩形

				// フラグが立っている場合はテキストが見切れないように調整します。
				if (hive.namecage)
					x = std::max(70, x);

				// 音声アイテム以外には何もしません。
				if (!(hook_manager.current_item->flag & ExEdit::Object::Flag::Sound))
					return orig_proc(dc, x, y, options, rc, text, c, dx);

				// フラグが立っている場合はここでテキストを描画します。
				if (!hive.behind && hive.show_text)
					orig_proc(dc, x, y, options, rc, text, c, dx);

				// フラグが立っている場合は波形を描画します。
				if (hive.show_wave)
				{
					// 描画がはみ出てもいいようにクリッピング領域を設定しておきます。
					my::gdi::unique_ptr<HRGN> rgn(::CreateRectRgnIndirect(rc));
					::SelectClipRgn(dc, rgn.get());

					// 音声波形を描画します。
					hook_manager.draw_wave(dc, &rc[0], &rc[1]);

					// クリッピングを解除します。
					::SelectClipRgn(dc, nullptr);
				}

				// フラグが立っている場合はここでテキストを描画します。
				if (hive.behind && hive.show_text)
					orig_proc(dc, x, y, options, rc, text, c, dx);

				return TRUE;
			}
			inline static decltype(&hook_proc) orig_proc = ::ExtTextOutA;
		} ExtTextOutA;
	} hook_manager;
}
