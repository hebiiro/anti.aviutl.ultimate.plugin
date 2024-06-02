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

		inline static constexpr struct WindowMessage {
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

		struct SubWindow {
			inline static constexpr LPCTSTR c_class_name = _T("apn::workspace::sub_window");
		};

		struct SubProcess {
			inline static constexpr LPCTSTR c_class_name = _T("apn::workspace::sub_process");
			BOOL dock = FALSE;
			std::wstring run;
		};

		struct PSDToolKit : SubProcess {
			inline static constexpr LPCTSTR c_name = _T("PSDToolKit(外部)");
		} psdtoolkit;

		struct Bouyomisan : SubProcess {
			inline static constexpr LPCTSTR c_name = _T("棒読みさん(外部)");
		} bouyomisan;

		struct Console : SubProcess {
			inline static constexpr LPCTSTR c_name = _T("コンソール");
		} console;

		struct ShortcutKey {
			my::ShortcutKey show_caption = { L"z", L"shift win" };
		} shortcut_key;

		struct App {
			virtual BOOL read_preference() = 0;
			virtual BOOL write_preference() = 0;
			virtual BOOL import_layout() = 0;
			virtual BOOL export_layout() = 0;
			virtual BOOL change_layout(const std::wstring& file_name) = 0;
		} *app = 0;

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
		BOOL maximum_play = FALSE;
		BOOL show_tab_force = FALSE;
		int32_t layout_list_mode = c_layout_list_mode.c_none;

		BOOL use_maximum_play = FALSE;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
