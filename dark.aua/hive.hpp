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

		inline static constexpr struct Message {
		} c_message;

		inline static constexpr struct ShadowMode {
			inline static constexpr int32_t c_normal = 0;
			inline static constexpr int32_t c_omit = 1;
			inline static constexpr my::Label labels[] = {
				{ c_normal, L"normal" },
				{ c_omit, L"omit" },
			};
		} c_shadow_mode;

		inline static constexpr struct RoundMode {
			inline static constexpr int32_t c_normal = 0;
			inline static constexpr int32_t c_omit = 1;
			inline static constexpr int32_t c_half = 2;
			inline static constexpr my::Label labels[] = {
				{ c_normal, L"normal" },
				{ c_omit, L"omit" },
				{ c_half, L"half" },
			};
		} c_round_mode;

		inline static constexpr struct StaticEdgeMode {
			inline static constexpr int32_t c_normal = 0;
			inline static constexpr int32_t c_omit = 1;
			inline static constexpr my::Label labels[] = {
				{ c_normal, L"normal" },
				{ c_omit, L"omit" },
			};
		} c_static_edge_mode;

		inline static constexpr struct FileDialogMode {
			inline static constexpr int32_t c_normal = 0;
			inline static constexpr int32_t c_omit = 1;
			inline static constexpr my::Label labels[] = {
				{ c_normal, L"normal" },
				{ c_omit, L"omit" },
			};
		} c_file_dialog_mode;

		inline static constexpr struct DpiScalingMode {
			inline static constexpr int32_t c_normal = 0;
			inline static constexpr int32_t c_fix = 1;
			inline static constexpr my::Label labels[] = {
				{ c_normal, L"normal" },
				{ c_fix, L"fix" },
			};
		} c_dpi_scaling_mode;

		struct Skin {
			std::wstring module_name;
			std::wstring display_name;
		};

		struct Scheme {
			std::wstring module_name;
			std::wstring display_name;
		};

		struct App {
			virtual BOOL init() = 0;
			virtual BOOL exit() = 0;
			virtual BOOL init_skin() = 0;
			virtual BOOL update_skin() = 0;
			virtual BOOL on_change_controls() = 0;
			virtual BOOL on_change_config() = 0;
			virtual BOOL on_change_assets() = 0;
			virtual uint32_t get_current_skin_id() = 0;
			virtual BOOL redraw() = 0;
			virtual BOOL deploy_darken_window_settings() = 0;
		} *app = 0;

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
		// スキンの丸みです。
		//
		int32_t ellipse = 5;

		//
		// スキンの縁幅です。
		//
		int32_t border_width = 1;

		//
		// スキンの配色です。
		//
		share::PairColor main_background_color = { RGB(10, 10, 10), RGB(250, 250, 250) };
		share::PairColor main_text_color = { RGB(10, 10, 10), RGB(250, 250, 250) };
		share::PairColor sub_background_color = { RGB(50, 30, 30), RGB(250, 50, 70) };
		share::PairColor sub_text_color = { RGB(10, 10, 10), RGB(250, 250, 250) };

		//
		// テキストの影に関するモードです。
		//
		int32_t shadow_mode = c_shadow_mode.c_normal;

		//
		// 図形の角に関するモードです。
		//
		int32_t round_mode = c_round_mode.c_normal;

		//
		// スタティックエッジの扱いに関するモードです。
		//
		int32_t static_edge_mode = c_static_edge_mode.c_omit;

		//
		// ファイル選択ダイアログの扱いに関するモードです。
		//
		int32_t file_dialog_mode = c_file_dialog_mode.c_omit;

		//
		// DPIスケーリングモードです。
		//
		int32_t dpi_scaling_mode = c_dpi_scaling_mode.c_normal;

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
		// カラーピッカーダイアログで使用するカスタムカラーの配列です。
		//
		COLORREF custom_colors[16] = {};

		//
		// レンダラを使用するかどうかのフラグです。
		// レンダラの使用を停止したい場合はTRUEに設定します。
		//
		thread_local inline static BOOL renderer_locked = FALSE;

		//
		// このクラスはフックする前のオリジナルのAPIを保持します。
		//
		struct {
			LRESULT (WINAPI *CallWindowProcInternal)(WNDPROC wnd_proc, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
			BOOL (WINAPI *Rectangle)(HDC dc, int left, int top, int right, int bottom) = 0;
			BOOL (WINAPI *FillRect)(HDC dc, LPCRECT rc, HBRUSH brush) = 0;
			BOOL (WINAPI *DrawFrame)(HDC dc, LPRECT rc, UINT width, UINT type) = 0;
			BOOL (WINAPI *DrawFrameControl)(HDC dc, LPRECT rc, UINT type, UINT state) = 0;
			BOOL (WINAPI *FrameRect)(HDC dc, LPCRECT rc, HBRUSH brush) = 0;
			BOOL (WINAPI *DrawEdge)(HDC dc, LPRECT rc, UINT edge, UINT flags) = 0;
			BOOL (WINAPI *DrawFocusRect)(HDC dc, LPCRECT rc) = 0;
			BOOL (WINAPI *DrawStateW)(HDC dc, HBRUSH fore, DRAWSTATEPROC cb, LPARAM lData, WPARAM wData, int x, int y, int cx, int cy, UINT flags) = 0;
			BOOL (WINAPI *ExtTextOutW)(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx) = 0;
			BOOL (WINAPI *PatBlt)(HDC dc, int x, int y, int w, int h, DWORD rop) = 0;

			HRESULT (WINAPI *DrawThemeParentBackground)(HWND hwnd, HDC dc, LPCRECT rc) = 0;
			HRESULT (WINAPI *DrawThemeBackground)(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, LPCRECT rc_clip) = 0;
			HRESULT (WINAPI *DrawThemeBackgroundEx)(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, const DTBGOPTS* options) = 0;
			HRESULT (WINAPI *DrawThemeText)(HTHEME theme, HDC dc, int part_id, int state_id, LPCWSTR text, int c, DWORD text_flags, DWORD text_flags2, LPCRECT rc) = 0;
			HRESULT (WINAPI *DrawThemeTextEx)(HTHEME theme, HDC dc, int part_id, int state_id, LPCWSTR text, int c, DWORD text_flags, LPRECT rc, const DTTOPTS* options) = 0;
			HRESULT (WINAPI *DrawThemeIcon)(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, HIMAGELIST image_list, int iImageIndex) = 0;
			HRESULT (WINAPI *DrawThemeEdge)(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT dest_rect, UINT edge, UINT flags, LPRECT content_rect) = 0;
		} orig;

		//
		// コンストラクタです。
		//
		Hive()
		{
			std::fill(std::begin(custom_colors), std::end(custom_colors), RGB(255, 255, 255));
		}

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = 0, int32_t type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
