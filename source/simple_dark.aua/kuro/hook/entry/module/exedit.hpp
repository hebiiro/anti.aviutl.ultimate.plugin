#pragma once

namespace apn::dark::kuro::hook
{
	//
	// このクラスは拡張編集をフックします。
	//
	inline struct exedit_t : entry_t::node_t
	{
		//
		// 拡張編集用のマテリアルパレットです。
		//
		inline static const paint::palette_t& palette = paint::exedit_material.palette;

		//
		// 拡張編集が使用しているフォントです。
		//
		inline static HFONT* font = nullptr;

		//
		// 拡張編集の選択レイヤーです。
		//
		inline static int32_t* selected_layer = nullptr;

		//
		// 現在描画しているレイヤーの番号です。
		//
		inline static int drawing_layer_index = 0;

		//
		// 色選択ダイアログを表示する拡張編集の関数です。
		//
		inline static int (*ShowColorDialog)(DWORD u1, COLORREF* color, DWORD u3) = nullptr;

		//
		// 初期化処理を実行します。
		//
		virtual BOOL on_init() override
		{
			MY_TRACE_FUNC("");

			auto exedit = magi.exin.get_exedit();
			if (!exedit) return TRUE;

			if (!exedit_window.subclass(magi.exin.get_exedit_window())) return FALSE;

			font = (HFONT*)(exedit + 0x00167D84);
			selected_layer = (int32_t*)(exedit + 0x00149800);

			// APIフックを開始します。
			{
				my::hook::detours detours;
				my::hook::attach_abs_call(fill_timerline_header, exedit + 0x0003826C); // タイムライン左上部分の塗りつぶし処理です。
				my::hook::attach_abs_call(fill_timerline_header, exedit + 0x00037F4F); // タイムラインヘッダー部分の塗りつぶし処理です。

				my::hook::attach_abs_call(draw_scene_button_text, exedit + 0x0003833E);
				my::hook::attach_abs_call(draw_scene_button_edge, exedit + 0x0003836A);
				my::hook::attach_abs_call(draw_layer_button_text, exedit + 0x00037CFF);
				my::hook::attach_abs_call(draw_layer_button_edge, exedit + 0x00037D46);

				my::hook::attach_call(draw_timeline_primary_scale, exedit + 0x000380DF);
				my::hook::attach_call(draw_timeline_secondary_scale, exedit + 0x000381D7);
				my::hook::attach_call(draw_timeline_time, exedit + 0x000381A2);

				my::hook::attach_abs_call(fill_layer_background, exedit + 0x00038538);
				my::hook::attach_abs_call(fill_layer_background, exedit + 0x0003860E);
				my::hook::attach_abs_call(fill_group_background, exedit + 0x000386E4);

				my::hook::attach_call(draw_layer_left, exedit + 0x00038845);
				my::hook::attach_call(draw_layer_right, exedit + 0x000388AA);
				my::hook::attach_call(draw_layer_top, exedit + 0x00038871);
				my::hook::attach_call(draw_layer_bottom, exedit + 0x000388DA);
				my::hook::attach_call(draw_layer_separator, exedit + 0x00037A1F);

				my::hook::attach_call(show_layer_name_dialog, exedit + 0x426D2);
				my::hook::attach(draw_layer_background, exedit + 0x00038410);

//				my::hook::attach_import(CreatePen, (HMODULE)exedit, "CreatePen");
			}

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL on_exit() override
		{
			MY_TRACE_FUNC("");

			exedit_window.unsubclass();

			return TRUE;
		}
/*
		//
		// 指定されたスタイルの色を返します。
		//
		inline static COLORREF get_style_color(style_t::color_e color_index)
		{
			// スタイルからカラーエントリを取得します。
			const auto& color_entry = style.get_color_entry(color_index);

			// カラーエントリの色を返します。
			return color_entry.parts[0].win32;
		}

		//
		// 指定された矩形を指定されたスタイルで塗りつぶします。
		//
		inline static BOOL fill_rect(HDC dc, LPCRECT rc, style_t::color_e color_index)
		{
			// スタイルからカラーエントリを取得します。
			const auto& color_entry = style.get_color_entry(color_index);

			// カラーエントリからブラシを作成します。
			my::gdi::unique_ptr<HBRUSH> brush(
				::CreateSolidBrush(color_entry.parts[0].win32));

			// ブラシで矩形を塗りつぶします。
			return hive.orig.FillRect(dc, rc, brush.get());
		}

		//
		// 奇数レイヤーの場合はTRUEを返します。
		//
		inline static BOOL is_odd()
		{
			return hive.exedit.as_zebra ? 
				!!(drawing_layer_index & 0x1) : FALSE;
		}
*/
		//
		// 指定されたステートを縞模様用のステートに変更して返します。
		//
		inline static int32_t as_zebra(int32_t state_id)
		{
			if (hive.exedit.flag_as_zebra)
				return state_id + (drawing_layer_index & 1);
			else
				return state_id;
		}

		//
		// 指定されたレイヤー名からレイヤーカラーを取り出して返します。
		//
		inline static COLORREF get_color_from_layer_name(const std::wstring& name)
		{
			auto sep = name.find(L'#');
			if (sep == name.npos) return CLR_NONE;
			auto color = wcstoul(name.substr(sep + 1).c_str(), nullptr, 16);
			return RGB(GetBValue(color), GetGValue(color), GetRValue(color));
		}

		//
		// 指定されたレイヤーからレイヤーカラーを取り出して返します。
		//
		inline static COLORREF get_color_from_layer_index(int32_t index)
		{
			for (int32_t i = index; i >= 0; i--)
			{
				auto layer = magi.exin.get_layer_setting(i);
				if (!layer) continue; // レイヤーが無効でした。

				auto name = layer->name;
				if (!name) continue; // レイヤー名が無効でした。

				auto sep = strchr(name, '#');
				if (!sep) continue; // レイヤー名にセパレータが存在しませんでした。

				if (!sep[1]) return CLR_NONE; // セパレータ以降が空文字列ならデフォルトカラーを使用します。

				auto color = strtoul(sep + 1, nullptr, 16);
				return RGB(GetBValue(color), GetGValue(color), GetRValue(color));
			}

			return CLR_NONE;
		}

		//
		// 描画中のレイヤーのレイヤーカラーを返します。
		//
		inline static COLORREF get_layer_background_color()
		{
			// 描画中のレイヤーの設定を取得します。
			auto layer = magi.exin.get_layer_setting(drawing_layer_index);
			if (!layer) return CLR_NONE;

			auto state_id = (int)EES_EVEN_LAYER_BACKGROUND_INACTIVE;

			// レイヤーが選択状態の場合は
			if (hive.exedit.flag_draw_selected_layer && drawing_layer_index == *selected_layer)
			{
				state_id = EES_EVEN_LAYER_BACKGROUND_SELECTED;
			}
			// レイヤーが表示状態の場合は
			else if (!has_flag(layer->flag, ::ExEdit::LayerSetting::Flag::UnDisp))
			{
				// ユーザーカラーを使用する場合は
				if (hive.exedit.flag_use_layer_color)
				{
					// ユーザーカラーを取得します。
					auto user_color = CLR_NONE;
					if (hive.exedit.flag_use_layer_color_multi)
						user_color = get_color_from_layer_index(drawing_layer_index);
					else if (layer->name)
						user_color = get_color_from_layer_name(my::ws(layer->name));

					// ユーザーカラーが有効の場合は
					if (user_color != CLR_NONE)
						return user_color; // ユーザーカラーを返します。
				}

				state_id = EES_EVEN_LAYER_BACKGROUND_ACTIVE;
			}

			// タイムラインを縞模様にするための処理です。
			state_id = as_zebra(state_id);

			// ピグメントを取得できた場合は
			if (auto pigment = palette.get(WP_EXEDIT, state_id))
				return pigment->background.get_win32_color();

			return CLR_NONE;
		}

		//
		// 描画中のレイヤーのブレンド回数を返します。
		//
		inline static int32_t get_blend_count(HDC dc)
		{
			switch (::GetDCBrushColor(dc))
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

		//
		// 指定されたカラーをブレンドして返します。
		//
		inline static COLORREF blend(COLORREF color1, COLORREF color2, float alpha)
		{
			auto omega = 1.0f - alpha;

			auto r = (BYTE)(GetRValue(color1) * omega + GetRValue(color2) * alpha);
			auto g = (BYTE)(GetGValue(color1) * omega + GetGValue(color2) * alpha);
			auto b = (BYTE)(GetBValue(color1) * omega + GetBValue(color2) * alpha);

			return RGB(r, g, b);
		}

		//
		// タイムライン内の矩形を塗りつぶします。
		//
		inline static BOOL fill_timeline_rect(HDC dc, LPCRECT rc, HBRUSH brush)
		{
			switch (hive.exedit.layer_border)
			{
			default:
			case hive.exedit.k_layer_border.k_both:
				{
					// 通常通り塗りつぶします。
					return hive.orig.FillRect(dc, rc, brush);
				}
			case hive.exedit.k_layer_border.k_top_only:
				{
					// 下辺の縁も塗りつぶします。
					auto rc_inflate = *rc;
					rc_inflate.bottom += 1;
					return hive.orig.FillRect(dc, &rc_inflate, brush);
				}
			case hive.exedit.k_layer_border.k_none:
				{
					// 上下の縁も塗りつぶします。
					auto rc_inflate = *rc;
					::InflateRect(&rc_inflate, 0, 1);
					return hive.orig.FillRect(dc, &rc_inflate, brush);
				}
			}
		}

		//
		// このクラスはタイムラインのヘッダーの描画をフックします。
		//
		inline static struct {
			inline static BOOL WINAPI hook_proc(HDC dc, LPCRECT rc, HBRUSH brush)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}", dc, brush);

				// ピグメントを取得できた場合は
				if (auto pigment = palette.get(WP_EXEDIT, 0))
				{
#if 0
					// スクロールバーのサイズ(太さ)です。
					constexpr auto k_scrollbar_size = 13;

					if (rc->top == k_scrollbar_size)
					{
						// 拡張編集ウィンドウのクライアント矩形を取得します。
						auto client_rc = my::get_client_rect(magi.exin.get_exedit_window());

						// スクロールバーの部分は除外します。
						client_rc.top += k_scrollbar_size;
						client_rc.right -= k_scrollbar_size;

						// タイムライン全体を塗りつぶします。
						return paint::stylus.draw_rect(dc, &client_rc, pigment);
					}
#endif
					// タイムラインのヘッダー領域を塗りつぶします。
					return paint::stylus.draw_rect(dc, rc, pigment);
				}

				return orig_proc(dc, rc, brush);
			}
			inline static decltype(&hook_proc) orig_proc = ::FillRect;
		} fill_timerline_header;

		//
		// このクラスはシーン選択ボタンの文字列の描画をフックします。
		//
		inline static struct {
			inline static BOOL WINAPI hook_proc(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCSTR text, UINT c, CONST INT* dx)
			{
				MY_TRACE_FUNC("{/hex}, {/}, {/}, {/hex}", dc, x, y, options);

				// ピグメントを取得できた場合は
				if (auto pigment = palette.get(WP_EXEDIT, EES_SCENE_BUTTON))
				{
					// 背景を描画します。※ここで縁も同時に描画します。
					paint::stylus.draw_round_rect(dc, rc, pigment);

					// シーン名をワイド文字列に変換します。
					auto str = my::ws(std::string(text, c));

					// 既に背景は描画済みなので、
					// 背景を塗りつぶさないようにします。
					options &= ~ETO_OPAQUE;

					// シーン名を描画します。
					return paint::stylus.ext_text_out(dc, x, y, options, rc, str.c_str(), (UINT)str.length(), dx, pigment);
				}

				return orig_proc(dc, x, y, options, rc, text, c, dx);
			}
			inline static decltype(&hook_proc) orig_proc = ::ExtTextOutA;
		} draw_scene_button_text;

		//
		// このクラスはシーン選択ボタンのエッジの描画をフックします。
		//
		inline static struct {
			inline static BOOL WINAPI hook_proc(HDC dc, LPRECT rc, UINT edge, UINT flags)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}", dc, edge, flags);

				// 縁もdraw_scene_button_textで描画しているので、ここでは何もしません。
				return TRUE;
//				return orig_proc(dc, rc, edge,flags);
			}
			inline static decltype(&hook_proc) orig_proc = ::DrawEdge;
		} draw_scene_button_edge;

		//
		// このクラスはレイヤーボタンの文字列の描画をフックします。
		//
		inline static struct {
			inline static BOOL WINAPI hook_proc(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCSTR text, UINT c, CONST INT* dx)
			{
//				MY_TRACE_FUNC("{/hex}, {/}, {/}, {/hex}", dc, x, y, options);

				auto part_id = WP_EXEDIT;
				auto state_id = 0;

				switch (::GetBkColor(dc))
				{
				case 0x00F0F0F0: state_id = EES_EVEN_LAYER_BUTTON_ACTIVE; break;
				case 0x00CCCCCC: state_id = EES_EVEN_LAYER_BUTTON_INACTIVE; break;
				}

				if (state_id != 0)
				{
					// タイムラインを縞模様にするための処理です。
					state_id = as_zebra(state_id);

					// レイヤーボタン用のピグメントを取得できた場合は
					if (auto pigment = palette.get(part_id, state_id))
					{
#if 0
						// 背景用のピグメントを取得できた場合は
						if (auto pigment = palette.get(WP_EXEDIT, 0))
						{
							// レイヤーボタンの背景矩形を算出します。
							auto rc2 = *rc;
							::InflateRect(&rc2, 1, 1);

							// レイヤーボタンの背景を塗りつぶします。
							paint::stylus.draw_rect(dc, &rc2, pigment);
						}

						// 背景を描画します。※ここで縁も同時に描画します。
						paint::stylus.draw_round_rect(dc, rc, pigment);
#else
						{
							// レイヤーボタンの背景矩形を算出します。
							auto rc2 = *rc;
							::InflateRect(&rc2, 1, 1);

							// レイヤーボタンの背景を塗りつぶします。
							paint::stylus.draw_rect(dc, &rc2, pigment);
						}
#endif
						// レイヤー名をワイド文字列に変換します。
						auto str = my::ws(std::string(text, c));

						// 記号より前にある文字列を取り出します。
						str = str.substr(0, str.find(L'#'));

						// 文字列が空の場合はデフォルトのレイヤー名にします。
						if (str.empty()) str = my::format(L"Layer {/}", drawing_layer_index + 1);

						// レイヤーボタンの文字列を描画します。
#if 1
						auto format = DWORD { DT_CENTER | DT_VCENTER | DT_SINGLELINE };

						return paint::stylus.d2d_draw_text(dc, rc,
							str.c_str(), (int)str.length(), format, pigment, FALSE);
#else
						// 既に背景は描画済みなので、
						// 背景を塗りつぶさないようにします。
						options &= ~ETO_OPAQUE;

						return paint::stylus.ext_text_out(dc, x, y,
							options, rc, str.c_str(), (UINT)str.length(), dx, pigment);
#endif
					}
				}

				return orig_proc(dc, x, y, options, rc, text, c, dx);
			}
			inline static decltype(&hook_proc) orig_proc = ::ExtTextOutA;
		} draw_layer_button_text;

		//
		// このクラスはレイヤーボタンのエッジの描画をフックします。
		//
		inline static struct {
			inline static BOOL WINAPI hook_proc(HDC dc, LPRECT rc, UINT edge, UINT flags)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}", dc, edge, flags);

				// 縁もdraw_layer_button_textで描画しているので、ここでは何もしません。
				return TRUE;
				return orig_proc(dc, rc, edge, flags);
			}
			inline static decltype(&hook_proc) orig_proc = ::DrawEdge;
		} draw_layer_button_edge;

		//
		// このクラスは主目盛りの描画をフックします。
		//
		inline static struct {
			inline static void CDECL hook_proc(HDC dc, int mx, int my, int lx, int ly, HPEN pen)
			{
//				MY_TRACE_FUNC("{/hex}, {/}, {/}, {/}, {/}, {/hex}", dc, mx, my, lx, ly, pen);

				// ピグメントを取得できた場合は
				if (auto pigment = palette.get(WP_EXEDIT, EES_SCALE_PRIMARY))
				{
					auto rc = RECT { mx, my, mx + 1, ly };

					return paint::stylus.draw_rect(dc, &rc, pigment), (void)0;
				}
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} draw_timeline_primary_scale;

		//
		// このクラスは短い目盛りの描画をフックします。
		//
		inline static struct {
			inline static void CDECL hook_proc(HDC dc, int mx, int my, int lx, int ly, HPEN pen)
			{
//				MY_TRACE_FUNC("{/hex}, {/}, {/}, {/}, {/}, {/hex}", dc, mx, my, lx, ly, pen);

				// ピグメントを取得できた場合は
				if (auto pigment = palette.get(WP_EXEDIT, EES_SCALE_SECONDARY))
				{
					auto rc = RECT { mx, my, mx + 1, ly };

					return paint::stylus.draw_rect(dc, &rc, pigment), (void)0;
				}
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} draw_timeline_secondary_scale;

		//
		// このクラスは時間文字列の描画をフックします。
		//
		inline static struct {
			inline static void CDECL hook_proc(HDC dc, LPCSTR text, int x, int y, int w, int h, int scroll_x)
			{
//				MY_TRACE_FUNC("{/hex}, {/}, {/}, {/}, {/}, {/}, {/}", dc, text, x, y, w, h, scroll_x);

				// ピグメントを取得できた場合は
				if (auto pigment = palette.get(WP_EXEDIT, EES_SCALE_PRIMARY))
				{
					// フォントをセットします。
					::SelectObject(dc, *font);

					// 時間文字列の描画位置を算出します。
					auto rc = RECT { x, y, x + w, y + h };
					{
						TEXTMETRIC tm = {};
						::GetTextMetrics(dc, &tm);

						::OffsetRect(&rc, tm.tmHeight / 4 + scroll_x, 0);
					}

					// 時間文字列をワイド文字列に変換します。
					auto str = my::ws(text);
#if 0
					auto options = UINT { ETO_CLIPPED };

					return paint::stylus.ext_text_out(dc, x, y, options, &rc,
						str.c_str(), (UINT)str.length(), nullptr, pigment), (void)0;
#else
					auto format = DWORD { DT_LEFT | DT_VCENTER | DT_SINGLELINE };

					return paint::stylus.d2d_draw_text(dc, &rc,
						str.c_str(), (int)str.length(), format, pigment, FALSE), (void)0;
#endif
				}

				return orig_proc(dc, text, x, y, w, h, scroll_x);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} draw_timeline_time;

		//
		// このクラスはレイヤー背景の描画をフックします。
		//
		inline static struct {
			inline static BOOL WINAPI hook_proc(HDC dc, LPCRECT rc, HBRUSH brush)
			{
//				MY_TRACE_FUNC("{/hex}, {/hex}", dc, brush);

				// レイヤーの背景色を取得できた場合は
				if (auto color = get_layer_background_color(); color != CLR_NONE)
				{
					// DCブラシにセットします。
					::SetDCBrushColor(dc, color);
				}

				// タイムラインを塗りつぶします。
				return fill_timeline_rect(dc, rc, (HBRUSH)::GetStockObject(DC_BRUSH));
			}
			inline static decltype(&hook_proc) orig_proc = ::FillRect;
		} fill_layer_background;

		//
		// このクラスはグループ制御などの描画をフックします。
		//
		inline static struct {
			inline static BOOL WINAPI hook_proc(HDC dc, LPCRECT rc, HBRUSH brush)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}", dc, brush);

				// ピグメントを取得できた場合は
				if (auto pigment = palette.get(WP_EXEDIT, EES_GROUP_BACKGROUND))
				{
					// レイヤーの背景色を取得できた場合は
					if (auto color = get_layer_background_color(); color != CLR_NONE)
					{
						// グループ範囲の色を取得します。
						auto group_color = pigment->background.get_win32_color();

						// グループ範囲のアルファを取得します。
						auto group_alpha = (float)hive.exedit.group.alpha / 100.0f;

						// ブレンド回数を取得します。
						auto blend_count = get_blend_count(dc);
						for (decltype(blend_count) i = 0; i < blend_count; i++)
						{
							// ブレンド回数分だけグループ範囲の色とブレンドします。
							color = blend(color, group_color, group_alpha);
						}
						// ブレンドしたカラーをDCブラシにセットします。
						::SetDCBrushColor(dc, color);
					}
				}

				// タイムラインを塗りつぶします。
				return fill_timeline_rect(dc, rc, brush);
			}
			inline static decltype(&hook_proc) orig_proc = ::FillRect;
		} fill_group_background;

		inline static void draw_line(HDC dc, HPEN pen, int state_id, int x, int y, int w, int h)
		{
			if (pen) ::SelectObject(dc, pen);

			auto rc = RECT { x, y, x + w, y + h };

			return paint::stylus.draw_rect(dc, &rc, palette, WP_EXEDIT, state_id), (void)0;
		}

		inline static struct {
			inline static void CDECL hook_proc(HDC dc, int mx, int my, int lx, int ly, HPEN pen)
			{
				MY_TRACE_FUNC("{/hex}, {/}, {/}, {/}, {/}, {/hex}", dc, mx, my, lx, ly, pen);

				return draw_line(dc, pen, EES_LAYER_LINE_LEFT, mx, my, 1, ly - my);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} draw_layer_left;

		inline static struct {
			inline static void CDECL hook_proc(HDC dc, int mx, int my, int lx, int ly, HPEN pen)
			{
				MY_TRACE_FUNC("{/hex}, {/}, {/}, {/}, {/}, {/hex}", dc, mx, my, lx, ly, pen);

				return draw_line(dc, pen, EES_LAYER_LINE_RIGHT, mx, my, 1, ly - my);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} draw_layer_right;

		inline static struct {
			inline static void CDECL hook_proc(HDC dc, int mx, int my, int lx, int ly, HPEN pen)
			{
				MY_TRACE_FUNC("{/hex}, {/}, {/}, {/}, {/}, {/hex}", dc, mx, my, lx, ly, pen);

				return draw_line(dc, pen, EES_LAYER_LINE_TOP, mx, my, lx - mx, 1);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} draw_layer_top;

		inline static struct {
			inline static void CDECL hook_proc(HDC dc, int mx, int my, int lx, int ly, HPEN pen)
			{
				MY_TRACE_FUNC("{/hex}, {/}, {/}, {/}, {/}, {/hex}", dc, mx, my, lx, ly, pen);

				return draw_line(dc, pen, EES_LAYER_LINE_BOTTOM, mx, my, lx - mx, 1);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} draw_layer_bottom;

		inline static struct {
			inline static void CDECL hook_proc(HDC dc, int mx, int my, int lx, int ly, HPEN pen)
			{
//				MY_TRACE_FUNC("{/hex}, {/}, {/}, {/}, {/}, {/hex}", dc, mx, my, lx, ly, pen);

				return draw_line(dc, pen, EES_LAYER_LINE_SEPARATOR, mx, my, 1, ly - my);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} draw_layer_separator;

		inline static struct {
			inline static void CDECL hook_proc(HDC dc, int layer_index, DWORD a3, DWORD a4, DWORD a5, DWORD a6, DWORD a7)
			{
//				MY_TRACE_FUNC("{/hex}, {/}, {/}, {/}, {/}, {/}, {/}", dc, layer_index, a3, a4, a5, a6, a7);

				// ここで現在描画しているレイヤーのインデックスを取得します。
				drawing_layer_index = layer_index;

				orig_proc(dc, layer_index, a3, a4, a5, a6, a7);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} draw_layer_background;

		//
		// このクラスはレイヤー名変更ダイアログのダイアログプロシージャをフックします。
		//
		inline static struct {
			inline static INT_PTR CALLBACK hook_proc(HWND hdlg, UINT message, WPARAM w_param, LPARAM l_param)
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
						auto code = HIWORD(w_param);
						auto id = LOWORD(w_param);
						auto sender = (HWND)l_param;

						MY_TRACE_FUNC("WM_COMMAND, {/hex16}, {/hex16}, {/hex}", code, id, sender);

						if (id == IDC_CHOOSE_COLOR)
						{
							const auto control_id = 171;

							auto str = my::get_dlg_item_text(hdlg, control_id);
							MY_TRACE_STR(str);

							auto color = get_color_from_layer_name(str.c_str());
							MY_TRACE_HEX(color);

							if (IDOK != magi.exin.show_color_dialog(0, &color, 2))
								break;

							str = str.substr(0, str.find(L'#')); // 記号より前にある文字列を取り出します。

							::SetDlgItemText(hdlg, control_id, my::format(_T("{/}#{/02x}{/02x}{/02x}"),
								str, GetRValue(color), GetGValue(color), GetBValue(color)).c_str());
						}

						break;
					}
				}

				return orig_proc(hdlg, message, w_param, l_param);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} layer_name_dialog_proc;

		//
		// このクラスはレイヤー名変更ダイアログを表示する関数をフックします。
		//
		inline static struct {
			inline static INT_PTR CDECL hook_proc(HINSTANCE instance, LPCSTR template_name, HWND parent, DLGPROC dlg_proc)
			{
				MY_TRACE_FUNC("{/hex}, {/}, {/hex}, {/hex}", instance, template_name, parent, dlg_proc);

				if (::lstrcmpiA(template_name, "GET_LAYER_NAME") == 0)
				{
					MY_TRACE("「レイヤー名」ダイアログをフックします\n");

					layer_name_dialog_proc.orig_proc = dlg_proc;

					return orig_proc(hive.instance, template_name, parent, layer_name_dialog_proc.hook_proc);
				}

				return orig_proc(instance, template_name, parent, dlg_proc);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} show_layer_name_dialog;
/*
		//
		// このクラスは拡張編集のカーソル用のペンを作成する関数をフックします。
		//
		inline static struct {
			inline static HPEN WINAPI hook_proc(int style, int width, COLORREF color)
			{
				if (style == PS_SOLID && width == 0 && color == RGB(0x00, 0xff, 0xff))
				{
					if (hive.exedit.cursor.style != -1) style = hive.exedit.cursor.style;
					if (hive.exedit.cursor.width != -1) width = hive.exedit.cursor.width;
					if (hive.exedit.cursor.color != CLR_NONE) color = hive.exedit.cursor.color;
				}

				return orig_proc(style, width, color);
			}
			inline static decltype(&hook_proc) orig_proc = ::CreatePen;
		} CreatePen;
*/
		//
		// このクラスは拡張編集ウィンドウをフックします。
		//
		struct exedit_window_t : my::Window
		{
			int32_t prev_selected_layer = -1;

			//
			// ウィンドウプロシージャです。
			//
			virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
			{
				switch (message)
				{
				case WM_LBUTTONDOWN:
					{
						MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

						// 選択レイヤーを描画しない場合は何もしません。
						if (!hive.exedit.flag_draw_selected_layer) break;

						// スコープ終了時(デフォルト処理の後)に実行します。
						my::scope_exit scope_exit([&]()
						{
							// 選択レイヤーが変更されていない場合は何もしません。
							if (prev_selected_layer == *exedit.selected_layer) return;

							// レイヤーフラグです。
							int32_t layer_flags[100] = {};

							// レイヤーフラグを立てます。
							if ((size_t)prev_selected_layer < std::size(layer_flags))
								layer_flags[prev_selected_layer] = TRUE;

							// 選択レイヤーを更新します。
							prev_selected_layer = *exedit.selected_layer;

							// レイヤーフラグを立てます。
							if ((size_t)prev_selected_layer < std::size(layer_flags))
								layer_flags[prev_selected_layer] = TRUE;

							// レイヤーを再描画します。
							magi.exin.redraw_layers(layer_flags);
						});

						return __super::on_wnd_proc(hwnd, message, w_param, l_param);
					}
				}

				return __super::on_wnd_proc(hwnd, message, w_param, l_param);
			}
		} exedit_window;
	} exedit;
}
