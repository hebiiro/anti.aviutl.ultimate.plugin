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
		} c_message;

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
		} *app = 0;

		//
		// このアドインのインスタンスハンドルです。
		//
		HINSTANCE instance = 0;

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
		HWND main_window = 0;

		my::theme::unique_ptr<> theme;
		HWND aviutl_window = 0;
		HWND exedit_window = 0;
		HWND setting_dialog = 0;
		HWND script_param_dialog = 0;

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

		BOOL use_maximum_play = FALSE;

		//
		// メッセージボックスを表示します。
		//
		int message_box(const std::wstring& text, HWND hwnd = 0, int type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
