#pragma once

namespace apn::workspace
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"workspace";
		inline static constexpr auto c_display_name = L"ワークスペース化";

		inline static constexpr struct MainMenuItemIndex {
			inline static constexpr uint32_t c_file = 0;
			inline static constexpr uint32_t c_filter = 1;
			inline static constexpr uint32_t c_config = 2;
			inline static constexpr uint32_t c_edit = 3;
			inline static constexpr uint32_t c_profile = 4;
			inline static constexpr uint32_t c_view = 5;
			inline static constexpr uint32_t c_etc = 6;
		} c_main_menu_item_index;

		//
		// 範囲19800～19900のAviUtlウィンドウのコマンドIDを使用しています。
		//
		inline static constexpr struct CommandID {
			inline static constexpr uint32_t c_begin = 19800;
			inline static constexpr uint32_t c_end = c_begin + 100;
			inline static constexpr uint32_t c_show_extra_menu = c_begin + 0;
		} c_command_id;

		inline static constexpr struct ClassName {
			inline static constexpr auto c_sub_window = L"apn::workspace::sub_window";
			inline static constexpr auto c_sub_process = L"apn::workspace::sub_process";
		} c_class_name;

		inline static constexpr struct Message {
			inline static constexpr uint32_t c_post_init = WM_APP + 1;
			inline static constexpr uint32_t c_read_preference = WM_APP + 2;
			inline static constexpr uint32_t c_write_preference = WM_APP + 3;
			inline static constexpr uint32_t c_init_shuttle = WM_APP + 4;
			inline static constexpr uint32_t c_refresh_title = WM_APP + 5;
			inline static constexpr uint32_t c_update_layout_list = WM_APP + 6;
			inline static constexpr uint32_t c_show_extra_menu = WM_APP + 7;
		} c_message;

		inline static struct PreferenceFlag {
			inline static constexpr uint32_t c_config = 0x00000001;
			inline static constexpr uint32_t c_fluid = 0x00000002;
			inline static constexpr uint32_t c_default = c_config;
		} c_preference_flag;

		inline static struct LayoutListMode {
			inline static constexpr int32_t c_none = 0;
			inline static constexpr int32_t c_top = 1;
			inline static constexpr int32_t c_bottom = 2;
			inline static constexpr my::Label labels[] = {
				{ c_none, L"none" },
				{ c_top, L"top" },
				{ c_bottom, L"bottom" },
			};
		} c_layout_list_mode;

		struct Pane {
			inline static struct SplitMode {
				inline static constexpr int32_t c_none = 0;
				inline static constexpr int32_t c_vert = 1;
				inline static constexpr int32_t c_horz = 2;
				inline static constexpr my::Label labels[] = {
					{ c_none, L"none" },
					{ c_vert, L"vert" },
					{ c_horz, L"horz" },
				};
			} c_split_mode;

			inline static struct Origin {
				inline static constexpr int32_t c_top_left = 0;
				inline static constexpr int32_t c_bottom_right = 1;
				inline static constexpr my::Label labels[] = {
					{ c_top_left, L"top_left" },
					{ c_bottom_right, L"bottom_right" },
				};
			} c_origin;

			inline static struct CaptionMode {
				inline static constexpr int32_t c_hide = 0;
				inline static constexpr int32_t c_show = 1;
				inline static constexpr my::Label labels[] = {
					{ c_hide, L"hide" },
					{ c_show, L"show" },
				};
			} c_caption_mode;

			inline static struct CaptionLocation {
				inline static constexpr int32_t c_default = 0;
				inline static constexpr int32_t c_left = 1;
				inline static constexpr int32_t c_top = 2;
				inline static constexpr int32_t c_right = 3;
				inline static constexpr int32_t c_bottom = 4;
				inline static constexpr int32_t c_nowhere = 5;
				inline static constexpr my::Label labels[] = {
					{ c_default, L"default" },
					{ c_left, L"left" },
					{ c_top, L"top" },
					{ c_right, L"right" },
					{ c_bottom, L"bottom" },
					{ c_nowhere, L"nowhere" },
				};
			} c_caption_location;
		} pane;

		struct Tav {
			inline static constexpr int32_t c_icon_index = -2;

			inline static struct DisplayMode {
				inline static constexpr int32_t c_default = 0;
				inline static constexpr int32_t c_manual = 1;
				inline static constexpr int32_t c_semi_auto = 2;
				inline static constexpr int32_t c_auto = 3;
				inline static constexpr int32_t c_full_auto = 4;
				inline static constexpr my::Label labels[] = {
					{ c_default, L"default" },
					{ c_manual, L"manual" },
					{ c_semi_auto, L"semi_auto" },
					{ c_auto, L"auto" },
					{ c_full_auto, L"full_auto" },
				};
			} c_display_mode;

			inline static struct SelectMode {
				inline static constexpr int32_t c_default = 0;
				inline static constexpr int32_t c_click = 1;
				inline static constexpr int32_t c_hover = 2;
				inline static constexpr my::Label labels[] = {
					{ c_default, L"default" },
					{ c_click, L"click" },
					{ c_hover, L"hover" },
				};
			} c_select_mode;

			inline static struct StretchMode {
				inline static constexpr int32_t c_default = 0;
				inline static constexpr int32_t c_inside = 1;
				inline static constexpr int32_t c_outside = 2;
				inline static constexpr my::Label labels[] = {
					{ c_default, L"default" },
					{ c_inside, L"inside" },
					{ c_outside, L"outside" },
				};
			} c_stretch_mode;

			inline static struct Location {
				inline static constexpr int32_t c_default = 0;
				inline static constexpr int32_t c_left = 1;
				inline static constexpr int32_t c_top = 2;
				inline static constexpr int32_t c_right = 3;
				inline static constexpr int32_t c_bottom = 4;
				inline static constexpr my::Label labels[] = {
					{ c_default, L"default" },
					{ c_left, L"left" },
					{ c_top, L"top" },
					{ c_right, L"right" },
					{ c_bottom, L"bottom" },
				};
			} c_location;

			inline static struct NodeAlign {
				inline static constexpr int32_t c_default = 0;
				inline static constexpr int32_t c_near = 1;
				inline static constexpr int32_t c_far = 2;
				inline static constexpr int32_t c_center = 3;
				inline static constexpr my::Label labels[] = {
					{ c_default, L"default" },
					{ c_near, L"near" },
					{ c_far, L"far" },
					{ c_center, L"center" },
				};
			} c_node_align;

			//
			// 表示モードです。
			//
			int32_t display_mode = c_display_mode.c_default;

			//
			// 選択モードです。
			//
			int32_t select_mode = c_select_mode.c_default;

			//
			// 伸展モードです。
			//
			int32_t stretch_mode = c_stretch_mode.c_default;

			//
			// タブコントロールの配置位置です。
			//
			int32_t location = c_location.c_default;

			//
			// タブ項目の配置位置です。
			//
			int32_t node_align = c_node_align.c_default;
		} tav = {
			.display_mode = tav.c_display_mode.c_semi_auto,
			.select_mode = tav.c_select_mode.c_click,
			.stretch_mode = tav.c_stretch_mode.c_inside,
			.location = tav.c_location.c_top,
			.node_align = tav.c_node_align.c_near,
		};

		struct OrigWindow {
			DWORD ex_style = 0;
			std::wstring window_name;
		};

		struct SubProcess {
			struct Node {
				BOOL active = FALSE;
				std::wstring type;
				std::wstring name;
				std::wstring class_name;
				std::wstring window_name;
				std::unordered_set<std::wstring> exit_mode;
				std::wstring path;
			};
			std::vector<std::shared_ptr<Node>> collection;
		} sub_process;

		struct ShortcutKey {
			my::ShortcutKey show_caption = { L"z", L"shift win" };
		} shortcut_key;

		struct App {
			virtual BOOL read_preference() = 0;
			virtual BOOL write_preference() = 0;
			virtual BOOL import_layout() = 0;
			virtual BOOL export_layout() = 0;
			virtual BOOL change_layout(const std::wstring& file_name) = 0;
			virtual BOOL enable_fullscreen_preview(BOOL enable) = 0;
		} *app = nullptr;

		//
		// このアドインのインスタンスハンドルです。
		//
		HINSTANCE instance = nullptr;

		//
		// コンフィグのファイル名です。
		//
		std::wstring config_file_name;

		//
		// プリファレンスのファイル名です。
		//
		std::wstring preference_file_name;

		//
		// このアドインのメインウィンドウです。
		//
		HWND main_window = nullptr;

		//
		// ウィンドウ用のテーマハンドルです。
		//
		my::theme::unique_ptr<> theme;

		//
		// タブコントロール用のテーマハンドルです。
		//
		my::theme::unique_ptr<> theme_tav;

		//
		// aviutlウィンドウのウィンドウハンドルです。
		//
		HWND aviutl_window = nullptr;

		//
		// 拡張編集ウィンドウのウィンドウハンドルです。
		//
		HWND exedit_window = nullptr;

		//
		// 設定ダイアログのウィンドウハンドルです。
		//
		HWND setting_dialog = nullptr;

		//
		// 『スクリプトの設定』ダイアログのウィンドウハンドルです。
		//
		HWND script_param_dialog = nullptr;

		//
		// タブコントロールの項目の高さです。
		// ただし、垂直タブコントロールの場合は幅になります。
		//
		int32_t tab_height = 24;

		//
		// タブコントロールの項目の余白です。
		//
		int32_t tab_space = 6;

		//
		// キャプションの高さです。
		//
		int32_t caption_height = 24;

		//
		// セパレータの幅です。
		//
		int32_t border_width = 8;

		//
		// 背景の色です。
		//
		COLORREF fill_color = RGB(0x99, 0x99, 0x99);

		//
		// セパレータの色です。
		//
		COLORREF border_color = RGB(0xcc, 0xcc, 0xcc);

		//
		// アクティブなセパレータの色です。
		//
		COLORREF hot_border_color = RGB(0x00, 0x00, 0x00);

		//
		// アクティブなキャプションの背景の色です。
		//
		COLORREF active_caption_color = ::GetSysColor(COLOR_HIGHLIGHT);

		//
		// アクティブなキャプションの文字の色です。
		//
		COLORREF active_caption_text_color = RGB(0xff, 0xff, 0xff);

		//
		// 非アクティブなキャプションの背景の色です。
		//
		COLORREF inactive_caption_color = ::GetSysColor(COLOR_HIGHLIGHTTEXT);

		//
		// 非アクティブなキャプションの文字の色です。
		//
		COLORREF inactive_caption_text_color = RGB(0x00, 0x00, 0x00);

		//
		// TRUEの場合はテーマを使用して描画します。
		//
		BOOL use_theme = FALSE;

		//
		// TRUEの場合はマウスホイール時にスクロールを強制します。
		//
		BOOL scroll_force = FALSE;

		//
		// TRUEの場合は再生時にプレビューウィンドウを最大化します。
		//
		BOOL fullscreen_preview = FALSE;

		//
		// TRUEの場合は常にタブコントロールを表示します。
		//
		BOOL show_tab_force = FALSE;

		//
		// TRUEの場合はキーボードメッセージを拡張編集ウィンドウにバイパスします。
		//
		BOOL bypass_keyboard_message = FALSE;

		//
		// TRUEの場合は拡張編集ウィンドウでCtrlキーを離しても選択が解除されなくなります。
		//
		BOOL ignore_ctrl_key_up = FALSE;

		//
		// レイアウトリストの表示モードです。
		//
		int32_t layout_list_mode = c_layout_list_mode.c_none;

		//
		// TRUEの場合は『再生時最大化』機能が使用可能になります。
		//
		BOOL use_fullscreen_preview = FALSE;

		//
		// TRUEの場合はウィンドウ初期化処理を省略します。
		//
		BOOL omit_window_initialize = FALSE;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_display_name, hwnd, type);
		}
	} hive;
}
