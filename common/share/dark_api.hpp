#pragma once

inline constexpr auto VSCLASS_DIALOG = VSCLASS_WINDOW;
inline constexpr auto VSCLASS_CTLCOLOR = VSCLASS_WINDOW;
inline constexpr auto VSCLASS_EXEDIT = VSCLASS_WINDOW;

enum MY_STATIC_PARTS {
	STAT_ETCHED_EDGE = 2,
};

enum MY_TRACKBAR_TRACK_STATES {
	TRS_BACKGROUND = 0,
	TRS_DISABLED = 2,
	TRS_SELECTED = 3,
};

enum CUSTOM_PARTS {
	WP_WINDOW_FACE = 51,
	WP_WINDOW_EDGE = 52,
	WP_CLIENT_EDGE = 53,
	WP_STATIC_EDGE = 54,
	WP_LIGHT_RAISED_EDGE = 56,
	WP_LIGHT_SUNKEN_EDGE = 57,
	WP_RAISED_EDGE = 58,
	WP_SUNKEN_EDGE = 59,
	WP_BUMP_EDGE = 60,
	WP_ETCHED_EDGE = 61,
	WP_SEPARATOR = 62,
	WP_EXEDIT = 63,
};

enum EXEDIT_STATES {
	EES_SELECTION_FILL = 1,
	EES_SELECTION_EDGE = 2,
	EES_SELECTION_BACKGROUND = 3,
	EES_SCALE_PRIMARY = 11,
	EES_SCALE_SECONDARY = 12,
	EES_SCENE_BUTTON_EDGE = 21,
	EES_SCENE_BUTTON = 22,
	EES_EVEN_LAYER_BUTTON_EDGE = 30,
	EES_ODD_LAYER_BUTTON_EDGE = 31,
	EES_EVEN_LAYER_BUTTON_ACTIVE = 32,
	EES_ODD_LAYER_BUTTON_ACTIVE = 33,
	EES_EVEN_LAYER_BUTTON_INACTIVE = 34,
	EES_ODD_LAYER_BUTTON_INACTIVE = 35,
	EES_EVEN_LAYER_BACKGROUND_ACTIVE = 36,
	EES_ODD_LAYER_BACKGROUND_ACTIVE = 37,
	EES_EVEN_LAYER_BACKGROUND_INACTIVE = 38,
	EES_ODD_LAYER_BACKGROUND_INACTIVE = 39,
	EES_LAYER_LINE_LEFT = 51,
	EES_LAYER_LINE_TOP = 52,
	EES_LAYER_LINE_RIGHT = 53,
	EES_LAYER_LINE_BOTTOM = 54,
	EES_LAYER_LINE_SEPARATOR = 55,
	EES_GROUP_BACKGROUND_ACTIVE_1 = 61,
	EES_GROUP_BACKGROUND_ACTIVE_2 = 62,
	EES_GROUP_BACKGROUND_ACTIVE_3 = 63,
	EES_GROUP_BACKGROUND_INACTIVE_1 = 64,
	EES_GROUP_BACKGROUND_INACTIVE_2 = 65,
	EES_GROUP_BACKGROUND_INACTIVE_3 = 66,
};

namespace Dark
{
	struct Stuff
	{
		struct Fill {
			inline static constexpr struct Mode {
				inline static constexpr int c_normal = 0;
				inline static constexpr int c_left = 1;
				inline static constexpr int c_top = 2;
				inline static constexpr int c_right = 3;
				inline static constexpr int c_bottom = 4;
				inline static constexpr int c_top_left = 5;
				inline static constexpr int c_top_right = 6;
				inline static constexpr int c_bottom_left = 7;
				inline static constexpr int c_bottom_right = 8;
				inline static constexpr int c_center = 9;
			} c_mode;

			COLORREF color = CLR_NONE;
			COLORREF sub_color = CLR_NONE;
			int mode = c_mode.c_normal;

			inline bool is_enabled() const { return color != CLR_NONE; }
		} fill;
		struct Border {
			COLORREF color = CLR_NONE;
			REAL width = 0;

			inline bool is_enabled() const { return color != CLR_NONE && width != 0; }
		} border;
		struct Text {
			COLORREF color = CLR_NONE;
			COLORREF shadow_color = CLR_NONE;
			COLORREF background_color = CLR_NONE;

			inline bool is_enabled() const { return color != CLR_NONE; }
		} text;
		struct Accent {
			COLORREF color = CLR_NONE;
			COLORREF sub_color = CLR_NONE;

			inline bool is_enabled() const { return color != CLR_NONE; }
		} accent;
		struct Etc {
			REAL ellipse = 0;
			int alpha = 255;
		} etc;
	};

	//
	// このクラスはダークモードプラグインのAPIを管理します。
	//
	struct Api
	{
		UINT (WINAPI* dark_get_current_id)() = 0;
		Stuff* (WINAPI* dark_get_stuff)(LPCWSTR name) = 0;

		//
		// ダークモードプラグインのエクスポート関数を取得します。
		// 最初に1回だけ呼び出してください。
		//
		BOOL init()
		{
			// アドインを取得します。
			HMODULE dark = ::GetModuleHandleA("dark.aua");

			// アドインが取得できなかった場合はプラグインを取得します。
			if (!dark) dark = ::GetModuleHandleA("DarkenWindow.aul");

			// どちらも取得できなかった場合は失敗します。
			if (!dark) return FALSE;

			// dark_get_current_idを取得します。
			dark_get_current_id = (decltype(dark_get_current_id))::GetProcAddress(dark, "dark_get_current_id");
			if (!dark_get_current_id) return FALSE; // エクスポート関数を取得できなかった場合は失敗します。

			// dark_get_stuffを取得します。
			dark_get_stuff = (decltype(dark_get_stuff))::GetProcAddress(dark, "dark_get_stuff");
			if (!dark_get_stuff) return FALSE; // エクスポート関数を取得できなかった場合は失敗します。

			return TRUE;
		}

		//
		// ダークモードプラグインが利用可能ならTRUEを返します。
		//
		BOOL is_available()
		{
			return !!dark_get_stuff;
		}

		//
		// 現在のスキンのIDを返します。
		//
		UINT get_current_id()
		{
			if (!dark_get_current_id) return 0;

			return dark_get_current_id();
		}

		//
		// 指定された名前のカラーセットを返します。
		// 指定された名前のカラーセットが存在しない場合は0を返します。
		//
		Stuff* get_stuff(LPCWSTR name)
		{
			if (!dark_get_stuff) return 0;

			return dark_get_stuff(name);
		}
	};

	//
	// このクラスはAviUtl外(サブプロセス)でのダークモードプラグインを管理します。
	//
	struct Module : Api
	{
		HMODULE dark = 0;

		//
		// ダークモードプラグインをロードします。
		// 外部プロセスから最初に1回だけ呼び出してください。
		// host_windowはAviUtlプロセス内のウィンドウを指定してください。
		// client_windowは現在のプロセスのウィンドウを指定してください。
		//
		BOOL init(HWND host_window, HWND client_window)
		{
			DWORD pid = 0;
			DWORD tid = ::GetWindowThreadProcessId(host_window, &pid);
			HANDLE host = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
			if (!host) return FALSE;

			HMODULE modules[1024] = {};
			DWORD c = 0;
			if (::EnumProcessModules(host, modules, sizeof(modules), &c)) {
				c /= sizeof(HMODULE);
				for (DWORD i = 0; i < c; i++) {
					char file_name[MAX_PATH] = {};
					::GetModuleFileNameExA(host, modules[i], file_name, std::size(file_name));
					if (::StrStrIA(file_name, "dark.aua")) {
						HMODULE dark = ::LoadLibraryExA(file_name, 0, LOAD_WITH_ALTERED_SEARCH_PATH);
						BOOL (WINAPI* dark_init)(HWND hwnd) =
							(decltype(dark_init))::GetProcAddress(dark, "dark_init");
						if (!dark_init) continue;
						dark_init(client_window);
						this->dark = dark;
						break;
					}
					else if (::StrStrIA(file_name, "DarkenWindow.aul")) {
						HMODULE dark = ::LoadLibraryExA(file_name, 0, LOAD_WITH_ALTERED_SEARCH_PATH);
						void (WINAPI* DarkenWindow_init)(HWND hwnd) =
							(decltype(DarkenWindow_init))::GetProcAddress(dark, "DarkenWindow_init");
						if (!DarkenWindow_init) continue;
						DarkenWindow_init(client_window);
						this->dark = dark;
						break;
					}
				}
			}

			::CloseHandle(host);

			if (!this->dark) return FALSE;

			return __super::init();
		}

		//
		// ダークモードプラグインをアンロードします。
		// 外部プロセスから最後に1回だけ呼び出してください。
		//
		BOOL exit()
		{
			if (!dark) return FALSE;

			BOOL (WINAPI* dark_exit)() =
				(decltype(dark_exit))::GetProcAddress(dark, "dark_exit");

			return dark_exit();
		}
	};
}
