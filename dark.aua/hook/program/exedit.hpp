#pragma once

namespace apn::dark::hook
{
	//
	// このクラスは拡張編集をフックします。
	//
	inline struct ExEdit
	{
		inline static HFONT* font = nullptr;
		inline static int drawing_layer_index = 0;
		inline static int (*ShowColorDialog)(DWORD u1, COLORREF* color, DWORD u3) = nullptr;

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
			if (auto layer = magi.exin.get_layer_setting(drawing_layer_index))
			{
				// ステートIDを取得します。
				auto state_id = 0;
				if (!(layer->flag & ::ExEdit::LayerSetting::Flag::UnDisp))
				{
					if (hive.use_layer_color)
					{
						// ユーザーカラーを取得します。
						auto user_color = CLR_NONE;
						if (hive.use_layer_color_multi)
							user_color = get_color_from_layer_index(drawing_layer_index);
						else if (layer->name)
							user_color = get_color_from_layer_name(my::ws(layer->name));

						// ユーザーカラーが有効なら使用します。
						if (user_color != CLR_NONE)
							return user_color;
					}

					state_id = EES_EVEN_LAYER_BACKGROUND_ACTIVE;
				}
				else
				{
					state_id = EES_EVEN_LAYER_BACKGROUND_INACTIVE;
				}

				state_id += drawing_layer_index % 2;

				// テーマカラーがあればそれを使用します。
				if (auto theme = skin::theme::manager.get_theme(VSCLASS_EXEDIT))
				{
					auto state = skin::theme::manager.get_state(theme, WP_EXEDIT, state_id);
					if (state && state->stuff.fill.color != CLR_NONE)
						return state->stuff.fill.color;
				}
			}

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
			if (hive.timeline_border_mode == hive.c_timeline_border_mode.c_omit)
			{
				auto rc_inflate = *rc;
				::InflateRect(&rc_inflate, 0, 1);
				return hive.orig.FillRect(dc, &rc_inflate, brush);
			}
			else
			{
				return hive.orig.FillRect(dc, rc, brush);
			}
		}

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			auto exedit = magi.exin.get_exedit();

			font = (HFONT*)(exedit + 0x00167D84);

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

			my::hook::attach_import(CreatePen, (HMODULE)exedit, "CreatePen");

			if (DetourTransactionCommit() != NO_ERROR)
			{
				MY_TRACE("APIフックに失敗しました\n");

				return FALSE;
			}

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// このクラスはタイムラインのヘッダーの描画をフックします。
		//
		inline static struct {
			inline static BOOL WINAPI hook_proc(HDC dc, LPCRECT rc, HBRUSH brush)
			{
				MY_TRACE_FUNC("{:#010x}, {:#010x}", dc, brush);

				if (auto theme = skin::theme::manager.get_theme(VSCLASS_EXEDIT))
				{
					if (python.call_draw_figure(magi.exin.get_exedit_window(), theme, dc, WP_EXEDIT, 0, rc))
						return TRUE;
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
				MY_TRACE_FUNC("{:#010x}, {}, {}, {:#010x}", dc, x, y, options);

				if (auto theme = skin::theme::manager.get_theme(VSCLASS_EXEDIT))
				{
					auto str = my::ws(std::string(text, c));

					if (python.call_text_out(magi.exin.get_exedit_window(), theme, dc, WP_EXEDIT, EES_SCENE_BUTTON, x, y, options, rc, str.c_str(), str.length(), dx))
						return TRUE;
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
				MY_TRACE_FUNC("{:#010x}, {:#010x}, {:#010x}", dc, edge, flags);

				if (auto theme = skin::theme::manager.get_theme(VSCLASS_EXEDIT))
				{
					if (python.call_draw_figure(magi.exin.get_exedit_window(), theme, dc, WP_EXEDIT, EES_SCENE_BUTTON_EDGE, rc))
						return TRUE;
				}

				return orig_proc(dc, rc, edge,flags);
			}
			inline static decltype(&hook_proc) orig_proc = ::DrawEdge;
		} draw_scene_button_edge;

		//
		// このクラスはレイヤーボタンの文字列の描画をフックします。
		//
		inline static struct {
			inline static BOOL WINAPI hook_proc(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCSTR text, UINT c, CONST INT* dx)
			{
//				MY_TRACE_FUNC("{:#010x}, {}, {}, {:#010x}", dc, x, y, options);

				if (auto theme = skin::theme::manager.get_theme(VSCLASS_EXEDIT))
				{
					auto state_id = 0;

					switch (::GetBkColor(dc))
					{
					case 0x00F0F0F0: state_id = EES_EVEN_LAYER_BUTTON_ACTIVE; break;
					case 0x00CCCCCC: state_id = EES_EVEN_LAYER_BUTTON_INACTIVE; break;
					}

					if (state_id != 0)
					{
						state_id += drawing_layer_index % 2;

						auto str = my::ws(std::string(text, c));
						str = str.substr(0, str.find(L'#')); // 記号より前にある文字列を取り出します。

						// 文字列が空の場合はデフォルトのレイヤー名にします。
						if (str.empty()) str = std::format(L"Layer {}", drawing_layer_index + 1);

						// レイヤーボタンの文字列を描画します。
#if 1
						if (python.call_draw_text(magi.exin.get_exedit_window(), theme, dc, WP_EXEDIT, state_id,
							str.c_str(), str.length(), DT_CENTER | DT_VCENTER | DT_SINGLELINE, rc))
							return TRUE;
#else
						if (python.call_text_out(magi.exin.get_exedit_window(), theme, dc, WP_EXEDIT, state_id,
							x, y, options, rc, str.c_str(), str.length(), dx))
							return TRUE;
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
				MY_TRACE_FUNC("{:#010x}, {:#010x}, {:#010x}", dc, edge, flags);

				if (auto theme = skin::theme::manager.get_theme(VSCLASS_EXEDIT))
				{
					auto state_id = EES_EVEN_LAYER_BUTTON_EDGE + drawing_layer_index % 2;

					if (python.call_draw_figure(magi.exin.get_exedit_window(), theme, dc, WP_EXEDIT, state_id, rc))
						return TRUE;
				}

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
//				MY_TRACE_FUNC("{:#010x}, {}, {}, {}, {}, {:#010x}", dc, mx, my, lx, ly, pen);

				if (auto theme = skin::theme::manager.get_theme(VSCLASS_EXEDIT))
				{
					auto state = skin::theme::manager.get_state(theme, WP_EXEDIT, EES_SCALE_PRIMARY);
					if (state && state->stuff.fill.color != CLR_NONE)
					{
						auto rc = RECT { mx, my, mx + 1, ly };
						painter::fill_rect(dc, &rc, state->stuff.fill.color);
					}
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
//				MY_TRACE_FUNC("{:#010x}, {}, {}, {}, {}, {:#010x}", dc, mx, my, lx, ly, pen);

				if (auto theme = skin::theme::manager.get_theme(VSCLASS_EXEDIT))
				{
					auto state = skin::theme::manager.get_state(theme, WP_EXEDIT, EES_SCALE_SECONDARY);
					if (state && state->stuff.fill.color != CLR_NONE)
					{
						auto rc = RECT { mx, my, mx + 1, ly };
						painter::fill_rect(dc, &rc, state->stuff.fill.color);
					}
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
//				MY_TRACE_FUNC("{:#010x}, {}, {}, {}, {}, {}, {}", dc, text, x, y, w, h, scroll_x);

				if (auto theme = skin::theme::manager.get_theme(VSCLASS_EXEDIT))
				{
					::SelectObject(dc, *font);

					TEXTMETRIC tm = {};
					::GetTextMetrics(dc, &tm);

					auto rc = RECT { x, y, x + w, y + h };
					::OffsetRect(&rc, tm.tmHeight / 4 + scroll_x, 0);

					auto str = my::ws(text);
#if 1
					auto options = UINT {};
					if (rc.left > 0) options |= ETO_CLIPPED;

					if (python.call_text_out(magi.exin.get_exedit_window(), theme, dc, WP_EXEDIT, EES_SCALE_PRIMARY,
						rc.left, rc.top, options, &rc, str.c_str(), str.length(), nullptr)) return;
#else
					if (python.call_draw_text(magi.exin.get_exedit_window(), theme, dc, WP_EXEDIT, EES_SCALE_PRIMARY,
						str.c_str(), str.length(), DT_LEFT | DT_VCENTER | DT_SINGLELINE, &rc)) return;
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
//				MY_TRACE_FUNC("{:#010x}, {:#010x}", dc, brush);

				// レイヤーの背景色を取得します。
				auto color = get_layer_background_color();
				if (color != CLR_NONE)
				{
					// カラーを適用します。
					::SetDCBrushColor(dc, color);
				}
#if 0
				if (auto theme = skin::theme::manager.get_theme(VSCLASS_EXEDIT))
				{
					auto state_id = 0;

					switch (painter::get_brush_color(brush))
					{
					case 0x00ffffff: state_id = EES_EVEN_LAYER_BACKGROUND_ACTIVE; break;
					case 0x00cccccc: state_id = EES_EVEN_LAYER_BACKGROUND_INACTIVE; break;
					}

					if (state_id != 0)
					{
						state_id += drawing_layer_index % 2;

						if (python.call_draw_figure(current_state->hwnd, theme, dc, WP_EXEDIT, state_id, rc))
							return TRUE;
					}
				}
#endif
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
				MY_TRACE_FUNC("{:#010x}, {:#010x}", dc, brush);

				// レイヤーの背景色を取得します。
				auto color = get_layer_background_color();
				if (color != CLR_NONE)
				{
					// ブレンド回数を取得します。
					auto blend_count = get_blend_count(dc);
					for (decltype(blend_count) i = 0; i < blend_count; i++)
					{
						// ブレンド回数分だけグループカラーとブレンドします。
						color = blend(color, skin::config.exedit.group.color, skin::config.exedit.group.alpha);
					}
					// ブレンドしたカラーを適用します。
					::SetDCBrushColor(dc, color);
				}

				return fill_timeline_rect(dc, rc, brush);
			}
			inline static decltype(&hook_proc) orig_proc = ::FillRect;
		} fill_group_background;

		inline static void draw_line(HDC dc, HPEN pen, int state_id, int x, int y, int w, int h)
		{
			if (pen) ::SelectObject(dc, pen);

			if (auto theme = skin::theme::manager.get_theme(VSCLASS_EXEDIT))
			{
				auto state = skin::theme::manager.get_state(theme, WP_EXEDIT, state_id);
				if (state && state->stuff.fill.color != CLR_NONE)
				{
					my::gdi::selector brush_selector(dc, state->get_fill_brush());

					::PatBlt(dc, x, y, w, h, PATCOPY);
				}
			}
		}

		inline static struct {
			inline static void CDECL hook_proc(HDC dc, int mx, int my, int lx, int ly, HPEN pen)
			{
				MY_TRACE_FUNC("{:#010x}, {}, {}, {}, {}, {:#010x}", dc, mx, my, lx, ly, pen);

				return draw_line(dc, pen, EES_LAYER_LINE_LEFT, mx, my, 1, ly - my);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} draw_layer_left;

		inline static struct {
			inline static void CDECL hook_proc(HDC dc, int mx, int my, int lx, int ly, HPEN pen)
			{
				MY_TRACE_FUNC("{:#010x}, {}, {}, {}, {}, {:#010x}", dc, mx, my, lx, ly, pen);

				return draw_line(dc, pen, EES_LAYER_LINE_RIGHT, mx, my, 1, ly - my);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} draw_layer_right;

		inline static struct {
			inline static void CDECL hook_proc(HDC dc, int mx, int my, int lx, int ly, HPEN pen)
			{
				MY_TRACE_FUNC("{:#010x}, {}, {}, {}, {}, {:#010x}", dc, mx, my, lx, ly, pen);

				return draw_line(dc, pen, EES_LAYER_LINE_TOP, mx, my, lx - mx, 1);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} draw_layer_top;

		inline static struct {
			inline static void CDECL hook_proc(HDC dc, int mx, int my, int lx, int ly, HPEN pen)
			{
				MY_TRACE_FUNC("{:#010x}, {}, {}, {}, {}, {:#010x}", dc, mx, my, lx, ly, pen);

				return draw_line(dc, pen, EES_LAYER_LINE_BOTTOM, mx, my, lx - mx, 1);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} draw_layer_bottom;

		inline static struct {
			inline static void CDECL hook_proc(HDC dc, int mx, int my, int lx, int ly, HPEN pen)
			{
//				MY_TRACE_FUNC("{:#010x}, {}, {}, {}, {}, {:#010x}", dc, mx, my, lx, ly, pen);

				return draw_line(dc, pen, EES_LAYER_LINE_SEPARATOR, mx, my, 1, ly - my);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} draw_layer_separator;

		inline static struct {
			inline static void CDECL hook_proc(HDC dc, int layer_index, DWORD a3, DWORD a4, DWORD a5, DWORD a6, DWORD a7)
			{
//				MY_TRACE_FUNC("{:#010x}, {}, {}, {}, {}, {}, {}", dc, layer_index, a3, a4, a5, a6, a7);

				// ここで現在描画しているレイヤーのインデックスを取得します。
				drawing_layer_index = layer_index;

				// Pythonから値にアクセスできるようにします。
				try
				{
					python.get_context().attr("exedit").attr("drawing_layer_index") = drawing_layer_index;
				}
				catch (const std::exception& error)
				{
					MY_TRACE_STR(error.what());

					std::cout << error.what() << std::endl;
				}

				orig_proc(dc, layer_index, a3, a4, a5, a6, a7);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} draw_layer_background;

		inline static struct {
			inline static INT_PTR CALLBACK hook_proc(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
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
						auto code = HIWORD(wParam);
						auto id = LOWORD(wParam);
						auto sender = (HWND)lParam;

						MY_TRACE_FUNC("WM_COMMAND, {:#04x}, {:#04x}, {:#010x}", code, id, sender);

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

							::SetDlgItemText(hdlg, control_id, std::format(_T("{}#{:02x}{:02x}{:02x}"),
								str, GetRValue(color), GetGValue(color), GetBValue(color)).c_str());
						}

						break;
					}
				}

				return orig_proc(hdlg, message, wParam, lParam);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} layer_name_dialog_proc;

		inline static struct {
			inline static INT_PTR CDECL hook_proc(HINSTANCE instance, LPCSTR template_name, HWND parent, DLGPROC dlg_proc)
			{
				MY_TRACE_FUNC("{:#010x}, {}, {:#010x}, {:#010x}", instance, template_name, parent, dlg_proc);

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

		inline static struct {
			inline static HPEN WINAPI hook_proc(int style, int width, COLORREF color)
			{
				if (style == PS_SOLID && width == 0 && color == RGB(0x00, 0xff, 0xff))
				{
					if (skin::config.exedit.xor_pen.style != -1) style = skin::config.exedit.xor_pen.style;
					if (skin::config.exedit.xor_pen.width != -1) width = skin::config.exedit.xor_pen.width;
					if (skin::config.exedit.xor_pen.color != CLR_NONE) color = skin::config.exedit.xor_pen.color;
				}

				return orig_proc(style, width, color);
			}
			inline static decltype(&hook_proc) orig_proc = ::CreatePen;
		} CreatePen;
	} exedit;
}
