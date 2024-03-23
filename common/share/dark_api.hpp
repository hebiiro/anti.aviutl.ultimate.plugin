#pragma once

namespace Dark
{
	struct Stuff
	{
		COLORREF fill_color = CLR_NONE;
		COLORREF edge_color = CLR_NONE;
		int edge_width = 0;
		int edge_round = 0;
		COLORREF text_color = CLR_NONE;
		COLORREF text_shadow_color = CLR_NONE;
		COLORREF text_background_color = CLR_NONE;
/*
		struct Type
		{
			inline static constexpr int c_fill_color = 0;
			inline static constexpr int c_edge_color = 1;
			inline static constexpr int c_edge_width = 2;
			inline static constexpr int c_edge_round = 3;
			inline static constexpr int c_text_color = 4;
			inline static constexpr int c_text_shadow_color = 5;
			inline static constexpr int c_text_background_color = 6;
		};
*/
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
	};
}
