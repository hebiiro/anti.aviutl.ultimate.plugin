#pragma once

namespace apn::dark::hook
{
	//
	// このクラスは描画関係のフックを担当します。
	//
	inline struct Theme
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			my::hook::attach(DrawThemeBackground);
			my::hook::attach(DrawThemeBackgroundEx);
			my::hook::attach(DrawThemeText);
			my::hook::attach(DrawThemeTextEx);
#if 0
			my::hook::attach(DrawThemeParentBackground);
			my::hook::attach(DrawThemeIcon);
			my::hook::attach(DrawThemeEdge);
#endif
#if 0
			my::hook::attach(OpenThemeData);
			my::hook::attach(OpenThemeDataForDpi);
			my::hook::attach(OpenThemeDataEx);
			my::hook::attach(SetWindowTheme);
#endif
			if (DetourTransactionCommit() != NO_ERROR)
			{
				MY_TRACE("APIフックに失敗しました\n");

				return FALSE;
			}

			hive.orig.DrawThemeBackground = DrawThemeBackground.orig_proc;
			hive.orig.DrawThemeBackgroundEx = DrawThemeBackgroundEx.orig_proc;
			hive.orig.DrawThemeText = DrawThemeText.orig_proc;
			hive.orig.DrawThemeTextEx = DrawThemeTextEx.orig_proc;
#if 0
			hive.orig.DrawThemeParentBackground = DrawThemeParentBackground.orig_proc;
			hive.orig.DrawThemeIcon = DrawThemeIcon.orig_proc;
			hive.orig.DrawThemeEdge = DrawThemeEdge.orig_proc;
#endif
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
		// このクラスは::DrawThemeParentBackground()をフックします。
		//
		struct {
			inline static HRESULT WINAPI hook_proc(HWND hwnd, HDC dc, LPCRECT rc)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, ({/})",
					ret_addr(&hwnd), hwnd, dc, safe_string(rc));

				MY_TRACE_HWND(hwnd);

				return orig_proc(hwnd, dc, rc);
			}
			inline static decltype(&hook_proc) orig_proc = ::DrawThemeParentBackground;
		} DrawThemeParentBackground;

		//
		// このクラスは::DrawThemeBackground()をフックします。
		//
		struct {
			inline static HRESULT WINAPI hook_proc(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, LPCRECT rc_clip)
			{
				hive.current_processing_vs.set(theme, part_id, state_id);
				if (auto renderer = theme::manager.get_renderer(theme))
					return renderer->on_draw_theme_background(theme, dc, part_id, state_id, rc, rc_clip);

				MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), ({/})",
					ret_addr(&theme), theme, part_id, state_id, safe_string(rc), safe_string(rc_clip));

				return orig_proc(theme, dc, part_id, state_id, rc, rc_clip);
			}
			inline static decltype(&hook_proc) orig_proc = ::DrawThemeBackground;
		} DrawThemeBackground;

		//
		// このクラスは::DrawThemeBackgroundEx()をフックします。
		//
		struct {
			inline static HRESULT WINAPI hook_proc(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, const DTBGOPTS* options)
			{
				hive.current_processing_vs.set(theme, part_id, state_id);
				if (auto renderer = theme::manager.get_renderer(theme))
					return renderer->on_draw_theme_background_ex(theme, dc, part_id, state_id, rc, options);

				MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), {/hex}",
					ret_addr(&theme), theme, part_id, state_id, safe_string(rc), options);

				return orig_proc(theme, dc, part_id, state_id, rc, options);
			}
			inline static decltype(&hook_proc) orig_proc = ::DrawThemeBackgroundEx;
		} DrawThemeBackgroundEx;

		//
		// このクラスは::DrawThemeText()をフックします。
		//
		struct {
			inline static HRESULT WINAPI hook_proc(HTHEME theme, HDC dc, int part_id, int state_id, LPCWSTR text, int c, DWORD text_flags, DWORD text_flags2, LPCRECT rc)
			{
				hive.current_processing_vs.set(theme, part_id, state_id);
				if (auto renderer = theme::manager.get_renderer(theme))
					return renderer->on_draw_theme_text(theme, dc, part_id, state_id, text, c, text_flags, text_flags2, rc);

				MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/hex}, {/hex}, ({/})",
					ret_addr(&theme), theme, part_id, state_id, safe_string(text, c), text_flags, text_flags2, safe_string(rc));

				return orig_proc(theme, dc, part_id, state_id, text, c, text_flags, text_flags2, rc);
			}
			inline static decltype(&hook_proc) orig_proc = ::DrawThemeText;
		} DrawThemeText;

		//
		// このクラスは::DrawThemeTextEx()をフックします。
		//
		struct {
			inline static HRESULT WINAPI hook_proc(HTHEME theme, HDC dc, int part_id, int state_id, LPCWSTR text, int c, DWORD text_flags, LPRECT rc, const DTTOPTS* options)
			{
				hive.current_processing_vs.set(theme, part_id, state_id);
				if (auto renderer = theme::manager.get_renderer(theme))
					return renderer->on_draw_theme_text_ex(theme, dc, part_id, state_id, text, c, text_flags, rc, options);

				MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/hex}, ({/}), {/hex}",
					ret_addr(&theme), theme, part_id, state_id, safe_string(text, c), text_flags, safe_string(rc), options);

				return orig_proc(theme, dc, part_id, state_id, text, c, text_flags, rc, options);
			}
			inline static decltype(&hook_proc) orig_proc = ::DrawThemeTextEx;
		} DrawThemeTextEx;

		//
		// このクラスは::DrawThemeIcon()をフックします。
		//
		struct {
			inline static HRESULT WINAPI hook_proc(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, HIMAGELIST image_list, int image_index)
			{
				hive.current_processing_vs.set(theme, part_id, state_id);
				if (auto renderer = theme::manager.get_renderer(theme))
					return renderer->on_draw_theme_icon(theme, dc, part_id, state_id, rc, image_list, image_index);

				MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), {/hex}, {/}",
					ret_addr(&theme), theme, part_id, state_id, safe_string(rc), image_list, image_index);

				return orig_proc(theme, dc, part_id, state_id, rc, image_list, image_index);
			}
			inline static decltype(&hook_proc) orig_proc = ::DrawThemeIcon;
		} DrawThemeIcon;

		//
		// このクラスは::DrawThemeEdge()をフックします。
		//
		struct {
			inline static HRESULT WINAPI hook_proc(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT dest_rect, UINT edge, UINT flags, LPRECT content_rect)
			{
				hive.current_processing_vs.set(theme, part_id, state_id);
				if (auto renderer = theme::manager.get_renderer(theme))
					return renderer->on_draw_theme_edge(theme, dc, part_id, state_id, dest_rect, edge, flags, content_rect);

				MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), {/hex}, {/hex}",
					ret_addr(&theme), theme, part_id, state_id, safe_string(dest_rect), edge, flags);

				return orig_proc(theme, dc, part_id, state_id, dest_rect, edge, flags, content_rect);
			}
			inline static decltype(&hook_proc) orig_proc = ::DrawThemeEdge;
		} DrawThemeEdge;

		//
		// このクラスは::OpenThemeData()をフックします。
		//
		struct {
			inline static HTHEME WINAPI hook_proc(HWND hwnd, LPCWSTR class_list)
			{
				auto theme = orig_proc(hwnd, class_list);
				MY_TRACE_FUNC("{/hex}, {/} => {/hex}", hwnd, class_list, theme);
				return theme;
			}
			inline static decltype(&hook_proc) orig_proc = ::OpenThemeData;
		} OpenThemeData;

		//
		// このクラスは::OpenThemeDataForDpi()をフックします。
		//
		struct {
			inline static HTHEME WINAPI hook_proc(HWND hwnd, LPCWSTR class_list, UINT dpi)
			{
				auto theme = orig_proc(hwnd, class_list, dpi);
				MY_TRACE_FUNC("{/hex}, {/}, {/} => {/hex}", hwnd, class_list, dpi, theme);
				return theme;
			}
			inline static decltype(&hook_proc) orig_proc = ::OpenThemeDataForDpi;
		} OpenThemeDataForDpi;

		//
		// このクラスは::OpenThemeDataEx()をフックします。
		//
		struct {
			inline static HTHEME WINAPI hook_proc(HWND hwnd, LPCWSTR class_list, DWORD flags)
			{
				auto theme = orig_proc(hwnd, class_list, flags);
				MY_TRACE_FUNC("{/hex}, {/}, {/hex} => {/hex}", hwnd, class_list, flags, theme);
				return theme;
			}
			inline static decltype(&hook_proc) orig_proc = ::OpenThemeDataEx;
		} OpenThemeDataEx;

		//
		// このクラスは::SetWindowTheme()をフックします。
		//
		struct {
			inline static HRESULT WINAPI hook_proc(HWND hwnd, LPCWSTR sub_app_name, LPCWSTR sub_id_list)
			{
				MY_TRACE_FUNC("{/hex}, {/}, {/}", hwnd, sub_app_name, sub_id_list);

				return orig_proc(hwnd, sub_app_name, sub_id_list);
			}
			inline static decltype(&hook_proc) orig_proc = ::SetWindowTheme;
		} SetWindowTheme;
	} theme;
}
