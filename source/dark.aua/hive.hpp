#pragma once

namespace apn::dark
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"dark";
		inline static constexpr auto c_display_name = L"ダークモード化";

		struct Skin {
			std::wstring module_name;
			std::wstring display_name;
		};

		struct Scheme {
			std::wstring module_name;
			std::wstring display_name;
		};

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
		// コンフィグのファイル名です。
		//
		std::wstring config_file_name;

		//
		// アセットのフォルダ名です。
		//
		std::wstring assets_folder_name;

		//
		// このアドインのメインウィンドウです。
		//
		HWND main_window = nullptr;

		//
		// テーマ用のウィンドウです。
		//
		HWND theme_window = nullptr;

		//
		// スタティックエッジを持つボタンのリストです。
		//
		std::vector<HWND> static_edge_buttons;

		//
		// 選択可能なスキンのリストです。
		//
		std::vector<std::shared_ptr<Skin>> skin_list;

		//
		// カレントスキンの名前です。
		//
		std::wstring skin_name;

		//
		// 選択可能なスキームのリストです。
		//
		std::vector<std::shared_ptr<Scheme>> scheme_list;

		//
		// カレントスキームの名前です。
		//
		std::wstring scheme_name;

		//
		// TRUEの場合はダークモードにします。
		// FALSEの場合はライトモードにします。
		//
		bool dark_mode = true;

		//
		// スキンの角の丸みです。
		//
		int32_t ellipse = 5;

		//
		// スキンの縁の幅です。
		//
		int32_t border_width = 1;

		//
		// スキンの影の濃度です。
		//
		int32_t shadow_density = 80;

		//
		// スクロールバーの縮小率です。
		//
		int32_t scrollbar_reduction = 50;

		//
		// ユーザー定義ダークモードスキン用のカラーセットです。
		//
		share::ColorSet dark_color[3] = {
			{ RGB(128, 128, 128), RGB(128, 128, 128), RGB(128, 128, 128) },
			{ RGB(128, 128, 128), RGB(128, 128, 128), RGB(128, 128, 128) },
			{ RGB(128, 128, 128), RGB(128, 128, 128), RGB(128, 128, 128) },
		};

		//
		// ユーザー定義ライトモードスキン用のカラーセットです。
		//
		share::ColorSet light_color[3] = {
			{ RGB(128, 128, 128), RGB(128, 128, 128), RGB(128, 128, 128) },
			{ RGB(128, 128, 128), RGB(128, 128, 128), RGB(128, 128, 128) },
			{ RGB(128, 128, 128), RGB(128, 128, 128), RGB(128, 128, 128) },
		};

		//
		// TRUEの場合はテキストの影を描画します。
		//
		BOOL draw_shadow = TRUE;

		//
		// TRUEの場合は矩形の角を丸めます。
		//
		BOOL as_round = TRUE;

		//
		// TRUEの場合はグラデーションを描画します。
		//
		BOOL draw_gradation = TRUE;

		//
		// TRUEの場合はボタンの縁を描画します。
		//
		BOOL draw_button_edge = FALSE;

		//
		// TRUEの場合はタイムラインの境界線を描画します。
		//
		BOOL draw_border = TRUE;

		//
		// TRUEの場合はタイムラインを縞模様に描画します。
		//
		BOOL as_zebra = TRUE;

		//
		// TRUEの場合はファイル選択ダイアログもダークモード化します。
		//
		BOOL apply_file_dialog = FALSE;

		//
		// TRUEの場合はDPIスケーリングを補正します。
		//
		BOOL fix_dpi_scaling = FALSE;

		//
		// TRUEの場合はユーザー指定のレイヤーカラーを使用します。
		//
		BOOL use_layer_color = TRUE;

		//
		// TRUEの場合はユーザー指定のレイヤーカラーを複数行に適用します。
		//
		BOOL use_layer_color_multi = FALSE;

		//
		// TRUEの場合は__pycache__フォルダを作成しません。
		//
		BOOL dont_write_bytecode = FALSE;

		//
		// レンダラを使用するかどうかのフラグです。
		// レンダラの使用を停止したい場合はTRUEに設定します。
		//
		thread_local inline static BOOL renderer_locked = FALSE;

		//
		// 現在処理中のビジュアルスタイルです。
		//
		thread_local inline static struct {
			HTHEME theme;
			int part_id;
			int state_id;
			inline void set(HTHEME theme, int part_id, int state_id) {
				this->theme = theme;
				this->part_id = part_id;
				this->state_id = state_id;
			}
		} current_processing_vs = { nullptr, 0, 0 };

		//
		// このクラスはフックする前のオリジナルのAPIを保持します。
		//
		struct {
			LRESULT (WINAPI *CallWindowProcInternal)(WNDPROC wnd_proc, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) = nullptr;
			BOOL (WINAPI *Rectangle)(HDC dc, int left, int top, int right, int bottom) = nullptr;
			BOOL (WINAPI *FillRect)(HDC dc, LPCRECT rc, HBRUSH brush) = nullptr;
			BOOL (WINAPI *DrawFrame)(HDC dc, LPRECT rc, UINT width, UINT type) = nullptr;
			BOOL (WINAPI *DrawFrameControl)(HDC dc, LPRECT rc, UINT type, UINT state) = nullptr;
			BOOL (WINAPI *FrameRect)(HDC dc, LPCRECT rc, HBRUSH brush) = nullptr;
			BOOL (WINAPI *DrawEdge)(HDC dc, LPRECT rc, UINT edge, UINT flags) = nullptr;
			BOOL (WINAPI *DrawFocusRect)(HDC dc, LPCRECT rc) = nullptr;
			BOOL (WINAPI *DrawStateW)(HDC dc, HBRUSH fore, DRAWSTATEPROC cb, LPARAM lData, WPARAM wData, int x, int y, int cx, int cy, UINT flags) = nullptr;
			BOOL (WINAPI *ExtTextOutW)(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx) = nullptr;
			BOOL (WINAPI *PatBlt)(HDC dc, int x, int y, int w, int h, DWORD rop) = nullptr;

			HRESULT (WINAPI *DrawThemeParentBackground)(HWND hwnd, HDC dc, LPCRECT rc) = nullptr;
			HRESULT (WINAPI *DrawThemeBackground)(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, LPCRECT rc_clip) = nullptr;
			HRESULT (WINAPI *DrawThemeBackgroundEx)(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, const DTBGOPTS* options) = nullptr;
			HRESULT (WINAPI *DrawThemeText)(HTHEME theme, HDC dc, int part_id, int state_id, LPCWSTR text, int c, DWORD text_flags, DWORD text_flags2, LPCRECT rc) = nullptr;
			HRESULT (WINAPI *DrawThemeTextEx)(HTHEME theme, HDC dc, int part_id, int state_id, LPCWSTR text, int c, DWORD text_flags, LPRECT rc, const DTTOPTS* options) = nullptr;
			HRESULT (WINAPI *DrawThemeIcon)(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, HIMAGELIST image_list, int iImageIndex) = nullptr;
			HRESULT (WINAPI *DrawThemeEdge)(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT dest_rect, UINT edge, UINT flags, LPRECT content_rect) = nullptr;
		} orig;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
