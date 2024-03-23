#pragma once

namespace apn::font_preview
{
	//
	// このクラスは描画を担当します。
	//
	inline struct Paint
	{
		inline static constexpr struct Mode {
			inline static constexpr int32_t c_auto = 0;
			inline static constexpr int32_t c_system = 1;
			inline static constexpr int32_t c_theme = 2;
			inline static constexpr int32_t c_custom = 3;
			inline static constexpr my::Label labels[] = {
				{ c_auto, L"auto" },
				{ c_system, L"system" },
				{ c_theme, L"theme" },
				{ c_custom, L"custom" },
			};
		} c_mode;

		inline static constexpr struct Style {
			inline static constexpr int32_t c_normal = 0;
			inline static constexpr int32_t c_select = 1;
			inline static constexpr int32_t c_max_size = 2;
			inline static constexpr my::Label labels[] = {
				{ c_normal, L"normal" },
				{ c_select, L"select" },
			};
		} c_style;

		//
		// ダークモード用APIです。
		//
		Dark::Api dark;

		//
		// テーマのハンドルです。
		//
		std::unique_ptr<
			typename std::remove_pointer<HTHEME>::type,
			decltype([](HTHEME x){ ::CloseThemeData(x); })> theme;

		//
		// 描画モードです。
		//
		int32_t mode = c_mode.c_auto;

		//
		// 描画パレットの配列です。
		//
		struct {
			struct {
				COLORREF background, sample, font_name;
			} color;
		} palette[c_style.c_max_size] = {
			{ { RGB(0x00, 0x00, 0x00), RGB(0xcc, 0xcc, 0xcc), RGB(0xff, 0xff, 0xff) } }, // 通常の描画色です。
			{ { RGB(0x66, 0x66, 0x66), RGB(0xcc, 0xcc, 0xcc), RGB(0xff, 0xff, 0xff) } }, // 選択の描画色です。
		};

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// ダークモード用APIを初期化します。
			dark.init();

			// AviUtlウィンドウからメニューのテーマを取得します。
			theme.reset(::OpenThemeData(magi.fp->hwnd_parent, VSCLASS_MENU));

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			theme.reset();

			return TRUE;
		}

		//
		// テーマを使用して描画する場合はTRUEを返します。
		//
		BOOL use_theme()
		{
			if (mode == c_mode.c_theme) return TRUE;
			else if (mode == c_mode.c_auto) return dark.is_available();
			else return FALSE;
		}

		//
		// 指定されたスタイルで背景を描画します。
		//
		void draw_background(HDC dc, LPCRECT rc, int style)
		{
			if (use_theme())
			{
				{
					auto part_id = MENU_BARBACKGROUND;
					auto state_id = MB_ACTIVE;

					::DrawThemeBackground(theme.get(), dc, part_id, state_id, rc, 0);
				}

				{
					auto part_id = MENU_BARITEM;
					auto state_id = (style == c_style.c_normal) ? MBI_NORMAL : MBI_HOT;

					::DrawThemeBackground(theme.get(), dc, part_id, state_id, rc, 0);
				}
			}
			else
			{
				auto color = (mode == c_mode.c_custom) ? palette[style].color.background :
					::GetSysColor((style == c_style.c_normal) ? COLOR_WINDOW : COLOR_HIGHLIGHT);

				::FillRect(dc, rc, my::gdi::unique_ptr<HBRUSH>(::CreateSolidBrush(color)).get());
			}
		}

		//
		// 指定されたスタイルでサンプル文字列を描画します。
		//
		void draw_sample(HDC dc, LPCWSTR text, int c, LPRECT rc, UINT format, int style)
		{
			if (use_theme())
			{
				auto part_id = MENU_BARITEM;
				auto state_id = (style == c_style.c_normal) ? MBI_NORMAL : MBI_HOT;

				::DrawThemeText(theme.get(), dc, part_id, state_id, text, c, format, 0, rc);
			}
			else
			{
				auto color = (mode == c_mode.c_custom) ? palette[style].color.sample :
					::GetSysColor((style == c_style.c_normal) ? COLOR_WINDOWTEXT : COLOR_HIGHLIGHTTEXT);

				auto bk_mode = ::SetBkMode(dc, TRANSPARENT);
				::SetTextColor(dc, color);
				::DrawText(dc, text, c, rc, format);
				::SetBkMode(dc, bk_mode);
			}
		}

		//
		// 指定されたスタイルでフォント名を描画します。
		//
		void draw_font_name(HDC dc, LPCWSTR text, int c, LPRECT rc, UINT format, int style)
		{
			if (use_theme())
			{
				auto part_id = MENU_BARITEM;
				auto state_id = (style == c_style.c_normal) ? MBI_NORMAL : MBI_HOT;

				::DrawThemeText(theme.get(), dc, part_id, state_id, text, c, format, 0, rc);
			}
			else
			{
				auto color = (mode == c_mode.c_custom) ? palette[style].color.font_name :
					::GetSysColor((style == c_style.c_normal) ? COLOR_WINDOWTEXT : COLOR_HIGHLIGHTTEXT);

				auto bk_mode = ::SetBkMode(dc, TRANSPARENT);
				::SetTextColor(dc, color);
				::DrawText(dc, text, c, rc, format);
				::SetBkMode(dc, bk_mode);
			}
		}
	} paint;
}
