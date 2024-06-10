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
			inline static constexpr uint32_t c_fullscreen_preview = c_begin + 0;
			inline static constexpr uint32_t c_show_config_dialog = c_begin + 1;
			inline static constexpr uint32_t c_import_layout = c_begin + 2;
			inline static constexpr uint32_t c_export_layout = c_begin + 3;
			inline static constexpr uint32_t c_create_sub_window = c_begin + 4;
			inline static constexpr uint32_t c_shuttle_begin = c_begin + 10;
			inline static constexpr uint32_t c_shuttle_end = c_end;
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

		my::theme::unique_ptr<> theme;
		HWND aviutl_window = nullptr;
		HWND exedit_window = nullptr;
		HWND setting_dialog = nullptr;
		HWND script_param_dialog = nullptr;

		COLORREF fill_color = RGB(0x99, 0x99, 0x99);
		COLORREF border_color = RGB(0xcc, 0xcc, 0xcc);
		COLORREF hot_border_color = RGB(0x00, 0x00, 0x00);
		COLORREF active_caption_color = ::GetSysColor(COLOR_HIGHLIGHT);
		COLORREF active_caption_text_color = RGB(0xff, 0xff, 0xff);
		COLORREF inactive_caption_color = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
		COLORREF inactive_caption_text_color = RGB(0x00, 0x00, 0x00);
		BOOL use_theme = FALSE;
		BOOL scroll_force = FALSE;
		BOOL fullscreen_preview = FALSE;
		BOOL show_tab_force = FALSE;
		BOOL bypass_keyboard_message = FALSE;
		int32_t layout_list_mode = c_layout_list_mode.c_none;

		BOOL use_fullscreen_preview = FALSE;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
