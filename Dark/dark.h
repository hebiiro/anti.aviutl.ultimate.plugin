#pragma once

namespace Dark
{
	struct ColorSet
	{
		COLORREF fillColor;
		COLORREF edgeColor;
		COLORREF textBkColor;
		COLORREF textForeColor;
		COLORREF textBackColor;

		struct Type
		{
			static const int fillColor = 0;
			static const int edgeColor = 1;
			static const int textBkColor = 2;
			static const int textForeColor = 3;
			static const int textBackColor = 4;
		};
	};

	//
	// ダークモードプラグインのAPIを管理します。
	//
	struct Api
	{
		UINT (WINAPI* dark_getCurrentId)() = 0;
		ColorSet* (WINAPI* dark_getNamedColor)(LPCWSTR name) = 0;

		//
		// ダークモードプラグインのエクスポート関数を取得します。
		// 最初に1回だけ呼び出してください。
		//
		BOOL init()
		{
			// Dark.auaを取得します。
			HMODULE dark = ::GetModuleHandleA("Dark.aua");

			// Dark.auaが取得できなかった場合はDarkenWindow.aulを取得します。
			if (!dark) dark = ::GetModuleHandleA("DarkenWindow.aul");

			// どちらも取得できなかった場合は失敗します。
			if (!dark) return FALSE;

			// dark_getCurrentIdを取得します。
			dark_getCurrentId = (decltype(dark_getCurrentId))::GetProcAddress(dark, "dark_getCurrentId");
			if (!dark_getCurrentId) return FALSE; // エクスポート関数を取得できなかった場合は失敗します。

			// dark_getNamedColorを取得します。
			dark_getNamedColor = (decltype(dark_getNamedColor))::GetProcAddress(dark, "dark_getNamedColor");
			if (!dark_getNamedColor) return FALSE; // エクスポート関数を取得できなかった場合は失敗します。

			return TRUE;
		}

		//
		// ダークモードプラグインが有効ならTRUEを返します。
		//
		BOOL isEnabled()
		{
			return !!dark_getNamedColor;
		}

		//
		// 現在のスキンのIDを返します。
		//
		UINT getCurrentId()
		{
			if (!dark_getCurrentId) return 0;

			return dark_getCurrentId();
		}

		//
		// 指定された名前のカラーセットを返します。
		// 指定された名前のカラーセットが存在しない場合は0を返します。
		//
		ColorSet* getNamedColor(LPCWSTR name)
		{
			if (!dark_getNamedColor) return 0;

			return dark_getNamedColor(name);
		}
	};

	//
	// ダークモードプラグインを管理します。
	//
	struct Module : Api {
		HMODULE dark = 0;

		//
		// ダークモードプラグインをロードします。
		// 外部プロセスから最初に1回だけ呼び出してください。
		// hostWindowはAviUtlプロセス内のウィンドウを指定してください。
		// clientWindowは現在のプロセスのウィンドウを指定してください。
		//
		BOOL init(HWND hostWindow, HWND clientWindow)
		{
			DWORD pid = 0;
			DWORD tid = ::GetWindowThreadProcessId(hostWindow, &pid);
			HANDLE host = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
			if (!host) return FALSE;

			HMODULE modules[1024] = {};
			DWORD c = 0;
			if (::EnumProcessModules(host, modules, sizeof(modules), &c)) {
				c /= sizeof(HMODULE);
				for (DWORD i = 0; i < c; i++) {
					char fileName[MAX_PATH] = {};
					::GetModuleFileNameExA(host, modules[i], fileName, std::size(fileName));
					if (::StrStrIA(fileName, "Dark.aua")) {
						HMODULE dark = ::LoadLibraryExA(fileName, 0, LOAD_WITH_ALTERED_SEARCH_PATH);
						BOOL (WINAPI* dark_init)(HWND hwnd) =
							(decltype(dark_init))::GetProcAddress(dark, "dark_init");
						if (!dark_init) continue;
						dark_init(clientWindow);
						this->dark = dark;
						break;
					}
					else if (::StrStrIA(fileName, "DarkenWindow.aul")) {
						HMODULE dark = ::LoadLibraryExA(fileName, 0, LOAD_WITH_ALTERED_SEARCH_PATH);
						void (WINAPI* DarkenWindow_init)(HWND hwnd) =
							(decltype(DarkenWindow_init))::GetProcAddress(dark, "DarkenWindow_init");
						if (!DarkenWindow_init) continue;
						DarkenWindow_init(clientWindow);
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
