#pragma once

namespace apn::item_wave
{
	//
	// このクラスはフックを管理します。
	//
	inline struct hook_manager_t
	{
		//
		// 描画中のアイテムのインデックスです。
		//
		int32_t current_object_index = -1;

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

		//
		// 音量をボトムアップY座標に変換して返します。
		//
		inline static int32_t bottom_up_y_from_volume(const share::volume_t& volume, int32_t origin, int32_t scale)
		{
			return origin - (int32_t)(volume.to_real() * scale);
		}

		//
		// 音量をトップダウンY座標に変換して返します。
		//
		inline static int32_t top_down_y_from_volume(const share::volume_t& volume, int32_t origin, int32_t scale)
		{
			return origin + (int32_t)(volume.to_real() * scale);
		}

		//
		// 指定された矩形内に波形を描画します。
		//
		void draw_wave(HDC dc, LPCRECT rc_clip, LPCRECT rc_item)
		{
			MY_TRACE_FUNC("");

			// 数値の型です。
			using num_t = decltype(rc_item->left);

			// 描画中のアイテムが無効の場合は何もしません。
			if (current_object_index == -1) return;

			// 描画中のアイテムを取得します。
			auto object = magi.exin.get_object(current_object_index);
			if (!object) return;

			// 描画中のアイテムに関連付けられているキャッシュを取得します。
			auto cache = item_cache::manager.get(current_object_index);
			if (!cache) return;

			// 描画スケールを取得します。
			auto scale = (num_t)hive.scale;

			// 波形を中央に描画する場合は描画スケールを半分にします。
			if (hive.wave_type == hive.c_wave_type.c_center) scale /= 2;

			// 描画スケールを正規化します。
			scale = std::max<num_t>(1, scale);

			// 頻繁に使用する値を取得しておきます。
			auto w = rc_item->right - rc_item->left;
			auto h = rc_item->bottom - rc_item->top;
			auto cy = (rc_item->top + rc_item->bottom) / 2;

			// 音量の数を取得します。
			auto c = (num_t)cache->volumes.size();
			MY_TRACE_INT(c);

			// 描画する頂点のコレクションです。
			std::vector<POINT> points;

			//
			// この関数は音量のインデックスをX座標に変換して返します。
			//
			auto x_from_index = [object](num_t index) {
				return (num_t)magi.exin.frame_to_x(index + object->frame_begin);
			};

			//
			// この関数はボトムアップ頂点を追加します。
			//
			auto add_bottom_up_point = [&points](num_t x, num_t y)
			{
				// 末尾の頂点を取得します。
				auto& back = points.back();

				// X座標が変動していない場合は
				if (back.x == x)
				{
					// 末尾の頂点のY座標を更新します。
					back.y = std::min<num_t>(back.y, y);
				}
				// X座標が変動している場合は
				else
				{
					// 新しい頂点を追加します。
					points.emplace_back(x, y);
				}
			};

			//
			// この関数はトップダウン頂点を追加します。
			//
			auto add_top_down_point = [&points](num_t x, num_t y)
			{
				// 末尾の頂点を取得します。
				auto& back = points.back();

				// X座標が変動していない場合は
				if (back.x == x)
				{
					// 末尾の頂点のY座標を更新します。
					back.y = std::max<num_t>(back.y, y);
				}
				// X座標が変動している場合は
				else
				{
					// 新しい頂点を追加します。
					points.emplace_back(x, y);
				}
			};

			//
			// この関数はポリゴン頂点群を構築します。
			//
			auto build_points = [&](num_t oy, const auto& y_from_volume, const auto& add_point, BOOL mirror)
			{
				// 原点を取得します。
				auto origin = POINT {
					.x = std::max<num_t>(rc_clip->left, rc_item->left),
					.y = oy,
				};

				// 音量のインデックスです。
				auto i = (num_t)0;

				// 音量のインデックスを描画範囲まで進めます。
				for (; i < c; i++) if (x_from_index(i) >= rc_clip->left) break;

				// 音量のインデックスを一つ戻して描画範囲外にします。
				i = std::max<num_t>(0, i - 1);

				// X座標の初期値です。
				auto x = x_from_index(i);

				// Y座標の初期値です。
				auto y = origin.y;

				// 最初の頂点を追加します。
				points.emplace_back(x, y);

				// 音量を走査してボトムアップ頂点群を構築します。
				for (; i < c; i++)
				{
					// 音量を取得します。
					const auto& volume = cache->volumes[i];

					// フレーム番号をX座標に変換します。
					x = x_from_index(i);

					// 音量をY座標に変換します。
					y = y_from_volume(volume, origin.y, scale);

					// 頂点を追加します。
					add_point(x, y);

					// X座標がクリップ矩形外になった場合は音量の走査を終了します。
					if (x >= rc_clip->right) break;
				}

				// フレーム番号をX座標に変換します。
				x = x_from_index(i);

				// X座標を正規化します。
				x = std::min<num_t>(x, rc_clip->right);

				// 最終頂点を追加します。
				points.emplace_back(x, y);

				// 右端頂点を追加します。
				points.emplace_back(x, origin.y);

				// ミラーリングが指定されている場合は
				if (mirror)
				{
					// 頂点を逆順に走査してトップダウン頂点群を構築します。
					for (num_t i = (num_t)points.size() - 1; i >= 0; i--)
						points.emplace_back(points[i].x, origin.y * 2 - points[i].y);
				}

				// 原点頂点を追加します。
				points.emplace_back(origin);
			};

			// 波形タイプ毎に分岐します。
			switch (hive.wave_type)
			{
			// 中央から上下へ描画する場合は
			case hive.c_wave_type.c_center:
				{
					build_points(cy, bottom_up_y_from_volume, add_bottom_up_point, TRUE);

					break;
				}
			// 下から上へ描画する場合は
			case hive.c_wave_type.c_bottom_up:
				{
					build_points(rc_item->bottom, bottom_up_y_from_volume, add_bottom_up_point, FALSE);

					break;
				}
			// 上から下へ描画する場合は
			case hive.c_wave_type.c_top_down:
				{
					build_points(rc_item->top, top_down_y_from_volume, add_top_down_point, FALSE);

					break;
				}
			}

			// ペンとブラシを作成します。
			my::gdi::unique_ptr<HPEN> pen(hive.pen_color != CLR_NONE ?
				::CreatePen(PS_SOLID, 1, hive.pen_color) : (HPEN)::GetStockObject(NULL_PEN));
			my::gdi::unique_ptr<HBRUSH> brush(hive.brush_color != CLR_NONE ?
				::CreateSolidBrush(hive.brush_color) : (HBRUSH)::GetStockObject(NULL_BRUSH));

			{
				// 頂点群をポリゴンとして描画します。
				my::gdi::selector pen_selector(dc, pen.get());
				my::gdi::selector brush_selector(dc, brush.get());
				auto rop2 = ::GetROP2(dc);
				switch (hive.xor_mode) {
				case hive.c_xor_mode.c_xor: ::SetROP2(dc, R2_XORPEN); break;
				case hive.c_xor_mode.c_not_xor: ::SetROP2(dc, R2_NOTXORPEN); break;
				case hive.c_xor_mode.c_not: ::SetROP2(dc, R2_NOT); break;
				}
				auto old_poly_fill_mode = ::SetPolyFillMode(dc, WINDING);
				::Polygon(dc, points.data(), points.size());
				::SetPolyFillMode(dc, old_poly_fill_mode);
				::SetROP2(dc, rop2);
			}

			// ::GetStockObject()でGDIオブジェクトを取得した場合は
			// 削除してはいけないので、ここでリリースしておきます。
			if (hive.pen_color == CLR_NONE) pen.release();
			if (hive.brush_color == CLR_NONE) brush.release();
		}

		//
		// このクラスは拡張編集内のアイテムを描画する関数をフックします。
		//
		inline static struct {
			inline static void CDECL hook_proc(HDC dc, int32_t object_index)
			{
				MY_TRACE_FUNC("{/}", object_index);

				// 描画中のアイテムを取得しておきます。
				hook_manager.current_object_index = object_index;

				// デフォルト処理を実行します。
				orig_proc(dc, object_index);

				// 描画中のアイテムを消去します。
				hook_manager.current_object_index = -1;
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} draw_item;

		//
		// このクラスは拡張編集内の::ExtTextOutA()の呼び出しをフックします。
		//
		inline static struct {
			inline static BOOL WINAPI hook_proc(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCSTR text, UINT c, CONST INT* dx)
			{
				MY_TRACE_FUNC("{/hex}, {/}, {/}, {/hex}", dc, x, y, options);
//				MY_TRACE_RECT2(rc[0]); // クリッピング矩形
//				MY_TRACE_RECT2(rc[1]); // アイテム全体の矩形

				// フラグが立っている場合はテキストが見切れないように調整します。
				if (hive.namecage)
				{
					// 70は拡張編集内の定数です。
					// それにユーザーが指定できるオフセットを加えます。
					x = std::max<int>(70 + hive.namecage_offset, x);
				}

				// 描画中アイテムが無効の場合は何もしません。
				if (hook_manager.current_object_index == -1)
					return orig_proc(dc, x, y, options, rc, text, c, dx);

				// 描画中アイテムを取得できなかった場合は何もしません。
				auto object = magi.exin.get_object(hook_manager.current_object_index);
				if (!object) return orig_proc(dc, x, y, options, rc, text, c, dx);

				// 音声アイテム以外には何もしません。
				if (!(object->flag & ExEdit::Object::Flag::Sound))
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
