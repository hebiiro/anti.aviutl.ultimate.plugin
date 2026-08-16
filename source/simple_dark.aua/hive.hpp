#pragma once

namespace apn::dark
{
	enum EXEDIT_STATES {
		EES_EVEN_LAYER_BACKGROUND_SELECTED = 70,
		EES_ODD_LAYER_BACKGROUND_SELECTED = 71,
		EES_GROUP_BACKGROUND = 72,
		EES_FRAME_CURSOR = 73,
	};

	namespace
	{
		//
		// このクラスは::PolyPatBlt()の引数です。
		//
		struct PATRECT {
			int x, y, w, h; HBRUSH brush;
		};
	}

	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct hive_t
	{
		inline static constexpr struct message_t {
			//
			// 初期化後のウィンドウメッセージです。
			//
			inline static const auto c_post_init = ::RegisterWindowMessageW(L"apn::simple_dark::post_init");
		} c_message;

		inline static constexpr auto c_name = L"simple_dark";
		inline static constexpr auto c_display_name = L"簡易ダークモード化";

		//
		// このアドインです。
		//
		Addin* addin = nullptr;

		//
		// このアドインのインスタンスハンドルです。
		//
		HINSTANCE instance = nullptr;

		//
		// 黒窓(ダミー)のインスタンスハンドルです。
		//
		HINSTANCE darken_window = nullptr;

		//
		// このアドインのメインウィンドウです。
		//
		HWND main_window = nullptr;

		//
		// テーマ用のウィンドウです。
		//
		HWND theme_window = nullptr;

		//
		// アセットのファイル名です。
		//
		std::wstring assets_file_name;

		//
		// コンフィグのファイル名です。
		//
		std::wstring config_file_name;

		//
		// 現在表示されているコモンダイアログの数です。
		//
		ULONG comdlg32_visible_count = {};

		//
		// このクラスはダークモード化の設定です。
		//
		struct dark_t
		{
			//
			// スタイルのファイル名です。
			//
			std::wstring style_file_name;

			//
			// カスタムカラーのファイル名です。
			//
			std::wstring custom_color_file_name;

			//
			// TRUEの場合はコモンダイアログをダークモード化から除外します。
			//
			BOOL flag_exclude_comdlg32 = FALSE;
		} dark;

		//
		// このクラスはDirect2Dの設定です。
		//
		struct d2d_t
		{
			//
			// TRUEの場合はDirect2Dを使用して図形を描画します。
			//
			BOOL flag_figure = FALSE;

			//
			// TRUEの場合はDirect2Dを使用して文字列を描画します。
			//
			BOOL flag_text = FALSE;
		} d2d;

		//
		// このクラスはスクロールバーの設定です。
		//
		struct scrollbar_t
		{
			//
			// TRUEの場合は矢印をボタンのように描画します。
			//
			BOOL flag_arrow_as_button = FALSE;

			//
			// TRUEの場合はつまみのグリッパーを描画します。
			//
			BOOL flag_has_gripper = FALSE;

			//
			// スクロールバーの縮小率です。%単位です。
			//
			int32_t reduction = 50;
		} scrollbar;

		//
		// このクラスはチェックボックスの設定です。
		//
		struct checkbox_t
		{
			//
			// TRUEの場合はチェックボックスを反転して描画します。
			//
			BOOL flag_reversed = FALSE;
		} checkbox;

		//
		// このクラスは丸みの設定です。
		//
		struct round_t
		{
			//
			// TRUEの場合は矩形の角を丸めます。
			//
			BOOL flag_use = TRUE;

			//
			// 丸角のサイズです。%単位です。
			//
			int32_t size = 20;
		} round;

		//
		// このクラスは縁の設定です。
		//
		struct border_t
		{
			//
			// TRUEの場合は縁を3Dのように描画します。
			//
			BOOL flag_3d_edge = FALSE;

			//
			// 縁の幅です。1/10px単位です。
			//
			int32_t width = 5;
		} border;

		//
		// このクラスはグラデーションの設定です。
		//
		struct gradient_t
		{
			//
			// TRUEの場合は背景をグラデーションで描画します。
			//
			BOOL flag_use = TRUE;

			//
			// TRUEの場合はグラデーション終了色にアルファを適用します。
			//
			BOOL flag_end_alpha = TRUE;

			//
			// グラデーション終了色のアルファです。%単位です。
			//
			int32_t end_alpha = 60;
		} gradient;

		//
		// このクラスは影の設定です。
		//
		struct shadow_t
		{
			//
			// TRUEの場合はテキストの影を描画します。
			//
			BOOL flag_use = TRUE;

			//
			// TRUEの場合は影をぼかします。
			//
			BOOL flag_blur = TRUE;

			//
			// 影のオフセット(位置)です。1/10px単位です。
			//
			POINT offset = { 10, 10 };

			//
			// ぼかしの範囲です。1/10px単位です。
			//
			int32_t size = 15;
		} shadow;

		//
		// このクラスは拡張編集の設定です。
		//
		struct exedit_t
		{
			inline static constexpr struct layer_border_t {
				inline static const auto k_both = 0; // 通常通り、上下両方(2px)の境界線を描画します。
				inline static const auto k_top_only = 1; // 上辺だけ(1px)の境界線を描画します。
				inline static const auto k_none = 2; // 境界線を描画しません。
			} k_layer_border;

			struct group_t {
				int32_t alpha = 20;
			} group;

			struct cursor_t {
				int32_t style = PS_SOLID;
				int32_t width = 2;
			} cursor;

			//
			// TRUEの場合はボタンの縁を描画します。
			//
			BOOL flag_draw_button_edge = FALSE;

			//
			// レイヤーの境界線の描画モードです。
			//
			int32_t layer_border = k_layer_border.k_both;

			//
			// TRUEの場合はタイムラインを縞模様に描画します。
			//
			BOOL flag_as_zebra = TRUE;

			//
			// TRUEの場合はタイムラインの選択レイヤーを描画します。
			//
			BOOL flag_draw_selected_layer = FALSE;

			//
			// TRUEの場合はユーザー指定のレイヤーカラーを使用します。
			//
			BOOL flag_use_layer_color = TRUE;

			//
			// TRUEの場合はユーザー指定のレイヤーカラーを複数行に適用します。
			//
			BOOL flag_use_layer_color_multi = FALSE;

			//
			// スタティックエッジを持つボタンのリストです。
			//
			std::vector<HWND> static_edge_buttons;
		} exedit;

		//
		// レンダラを使用するかどうかのフラグです。
		// レンダラの使用を停止したい場合はTRUEに設定します。
		//
		thread_local inline static BOOL is_renderer_locked = FALSE;

		//
		// メインスレッドのIDです。
		//
		const DWORD main_thread_id = ::GetCurrentThreadId();

		//
		// 現在のスレッドが有効の場合はTRUEを返します。
		//
		BOOL is_valid_thread() const
		{
			// レンダラーがロックされている場合は無効です。
			if (is_renderer_locked) return FALSE;

			// メインスレッドではない場合は無効です。
			if (main_thread_id != ::GetCurrentThreadId()) return FALSE;

			// コモンダイアログを除外する場合は
			if (dark.flag_exclude_comdlg32)
			{
				// コモンダイアログが表示されていない場合は有効です。
				return comdlg32_visible_count == 0;
			}

			// このスレッドは有効です。
			return TRUE;
		}

		//
		// このクラスはフックする前のオリジナルのAPIを保持します。
		//
		struct orig_t
		{
			LRESULT (WINAPI *CallWindowProcWInternal)(void* u1, WNDPROC wnd_proc, HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);
			decltype(&::Rectangle) Rectangle;
			decltype(&::FillRect) FillRect;
			BOOL (WINAPI *DrawFrame)(HDC dc, LPRECT rc, UINT width, UINT type);
			decltype(&::DrawFrameControl) DrawFrameControl;
			decltype(&::FrameRect) FrameRect;
			decltype(&::DrawEdge) DrawEdge;
			decltype(&::DrawFocusRect) DrawFocusRect;
			decltype(&::DrawStateW) DrawStateW;
			decltype(&::GrayStringW) GrayStringW;
			decltype(&::DrawTextW) DrawTextW;
			decltype(&::DrawTextExW) DrawTextExW;
			decltype(&::DrawMenuBar) DrawMenuBar;
			decltype(&::ExtTextOutW) ExtTextOutW;
			decltype(&::PatBlt) PatBlt;
			BOOL (WINAPI *PolyPatBlt)(HDC dc, DWORD rop, const PATRECT* rects, int nb_rects, ULONG reserved);
			decltype(&::GetSysColor) GetSysColor;
			decltype(&::GetSysColorBrush) GetSysColorBrush;

			decltype(&::GetThemeColor) GetThemeColor;
			decltype(&::DrawThemeParentBackground) DrawThemeParentBackground;
			decltype(&::DrawThemeBackground) DrawThemeBackground;
			decltype(&::DrawThemeBackgroundEx) DrawThemeBackgroundEx;
			decltype(&::DrawThemeText) DrawThemeText;
			decltype(&::DrawThemeTextEx) DrawThemeTextEx;
			decltype(&::DrawThemeIcon) DrawThemeIcon;
			decltype(&::DrawThemeEdge) DrawThemeEdge;
		} orig = {};

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING) {
			if (!hwnd) hwnd = theme_window;
			return ::MessageBoxW(hwnd, text.c_str(), c_display_name, type);
		}
	} hive;
}
