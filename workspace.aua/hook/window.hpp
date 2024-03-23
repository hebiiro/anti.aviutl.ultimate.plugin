#pragma once

namespace apn::workspace::hook
{
	//
	// このクラスはウィンドウの生成をフックします。
	//
	inline struct Window
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			auto user32 = ::GetModuleHandle(_T("user32.dll"));

			my::hook::attach(CreateWindowExA, ::GetProcAddress(user32, "CreateWindowExA"));
			my::hook::attach(CreateWindowExW, ::GetProcAddress(user32, "CreateWindowExW"));

			my::hook::attach(FindWindowA);
			my::hook::attach(FindWindowW);
			my::hook::attach(FindWindowExA);
			my::hook::attach(GetWindow);
			my::hook::attach(EnumThreadWindows);
			my::hook::attach(EnumWindows);
			my::hook::attach(SetWindowLongA);

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// 指定されたウィンドウがフック対象の場合はTRUEを返します。
		//
		inline static BOOL is_hook_target(HWND hwnd, const auto& _class_name, const auto& _window_name)
		{
			if (_class_name.empty() || _window_name.empty()) return FALSE;

			my::case_insensitive_tstring class_name(_class_name.c_str());
			my::case_insensitive_tstring window_name(_window_name.c_str());

			// ついでに主要ウィンドウのハンドルはここで取得しておきます。

			if (class_name == _T("AviUtl"))
			{
				if (window_name == _T("AviUtl"))
				{
					if (!hive.aviutl_window) hive.aviutl_window = hwnd;
				}
				else if (window_name == _T("拡張編集"))
				{
					if (!hive.exedit_window) hive.exedit_window = hwnd;
				}

				return TRUE;
			}
			else if (class_name == _T("ExtendedFilterClass"))
			{
				if (!hive.setting_dialog) hive.setting_dialog = hwnd;

				return TRUE;
			}

			return FALSE;
		}

		//
		// ウィンドウをフックします。
		//
		inline static HWND hook_window(HWND hwnd, const auto& class_name, const auto& window_name)
		{
			MY_TRACE_FUNC("{:#010x}, {}, {}", hwnd, class_name, window_name);

			// ウィンドウがフック対象かチェックします。
			if (is_hook_target(hwnd, class_name, window_name))
			{
				// ここで確保したメモリはメインウィンドウのc_init_shuttleで開放されます。
				auto orig = new Hive::OrigWindow();

				// ウィンドウ名を取得します。
				orig->window_name = window_name;

				// メインウィンドウにc_init_shuttleを送信します。
				::SendMessage(hive.main_window, hive.c_message.c_init_shuttle, (WPARAM)hwnd, (LPARAM)orig);
			}

			return hwnd;
		}

		//
		// 指定された文字列を大文字・小文字の区別のない文字列にして返します。
		//
		inline static my::tstring safe_string(auto str)
		{
			// 文字列がアトムの可能性があるのでチェックします。
			if ((DWORD)str <= 0x0000FFFF) return _T("");

			return my::ts(str);
		}

		//
		// このクラスは::CreateWindowExA()をフックします。
		//
		inline static struct {
			inline static HWND WINAPI hook_proc(DWORD ex_style, LPCSTR class_name, LPCSTR window_name,
				DWORD style, int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param)
			{
				auto hwnd = orig_proc(ex_style, class_name, window_name, style, x, y, w, h, parent, menu, instance, param);

				return hook_window(hwnd, safe_string(class_name), safe_string(window_name));
			}
			inline static decltype(&hook_proc) orig_proc = 0;
		} CreateWindowExA;

		//
		// このクラスは::CreateWindowExW()をフックします。
		//
		inline static struct {
			inline static HWND WINAPI hook_proc(DWORD ex_style, LPCWSTR class_name, LPCWSTR window_name,
				DWORD style, int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param)
			{
				auto hwnd = orig_proc(ex_style, class_name, window_name, style, x, y, w, h, parent, menu, instance, param);

				return hook_window(hwnd, safe_string(class_name), safe_string(window_name));
			}
			inline static decltype(&hook_proc) orig_proc = 0;
		} CreateWindowExW;

		//
		// このクラスは::FindWindowA()をフックします。
		//
		inline static struct {
			inline static HWND WINAPI hook_proc(LPCSTR class_name, LPCSTR window_name)
			{
				MY_TRACE_FUNC("{}, {}", class_name, window_name);

				// 「ショートカット再生」用の処理です。
				if (class_name && window_name && _stricmp(class_name, "AviUtl") == 0)
				{
					auto shuttle = shuttle_manager.get(my::ts(window_name));
					if (shuttle)
						return *shuttle;
				}

				return orig_proc(class_name, window_name);
			}
			inline static decltype(&hook_proc) orig_proc = ::FindWindowA;
		} FindWindowA;

		//
		// このクラスは::FindWindowW()をフックします。
		//
		inline static struct {
			inline static HWND WINAPI hook_proc(LPCWSTR class_name, LPCWSTR window_name)
			{
				MY_TRACE_FUNC("{}, {}", class_name, window_name);

				// 「PSDToolKit」の「送る」用の処理です。
				if (class_name && _wcsicmp(class_name, L"ExtendedFilterClass") == 0)
					return hive.setting_dialog;

				return orig_proc(class_name, window_name);
			}
			inline static decltype(&hook_proc) orig_proc = ::FindWindowW;
		} FindWindowW;

		//
		// このクラスは::FindWindowExA()をフックします。
		//
		inline static struct {
			inline static HWND WINAPI hook_proc(HWND parent, HWND child_after, LPCSTR class_name, LPCSTR window_name)
			{
				MY_TRACE_FUNC("{:#010x}, {:#010x}, {}, {}", parent, child_after, class_name, window_name);

				if (!parent && class_name)
				{
					// 「テキスト編集補助プラグイン」用の処理です。
					if (_stricmp(class_name, "ExtendedFilterClass") == 0)
					{
						return hive.setting_dialog;
					}
					// 「キーフレームプラグイン」用の処理です。
					else if (_stricmp(class_name, "AviUtl") == 0 && window_name)
					{
						auto shuttle = shuttle_manager.get(my::ts(window_name));
						if (shuttle)
						{
							MY_TRACE("{}を返します\n", window_name);

							return *shuttle;
						}
					}
				}

				return orig_proc(parent, child_after, class_name, window_name);
			}
			inline static decltype(&hook_proc) orig_proc = ::FindWindowExA;
		} FindWindowExA;

		//
		// このクラスは::GetWindow()をフックします。
		//
		inline static struct {
			inline static HWND WINAPI hook_proc(HWND hwnd, UINT cmd)
			{
//				MY_TRACE_FUNC("{:#010x}, {}", hwnd, cmd);
//				MY_TRACE_HWND(hwnd);

				if (cmd == GW_OWNER)
				{
					if (hwnd == hive.exedit_window)
					{
						// 拡張編集ウィンドウのオーナーウィンドウとしてAviUtlウィンドウを返すようにします。
						return hive.aviutl_window;
					}
					else if (hwnd == hive.setting_dialog)
					{
						// 設定ダイアログのオーナーウィンドウとして拡張編集ウィンドウを返すようにします。
						return hive.exedit_window;
					}

					if (Shuttle::get_pointer(hwnd))
					{
						// 「スクリプト並べ替え管理」「シークバー＋」などの一般的なプラグイン用の処理です。
						// シャトルのオーナーが要求されたときはAviUtlウィンドウを返すようにします。
						return hive.aviutl_window;
					}
				}

				return orig_proc(hwnd, cmd);
			}
			inline static decltype(&hook_proc) orig_proc = ::GetWindow;
		} GetWindow;

		//
		// このクラスは::EnumThreadWindows()をフックします。
		//
		inline static struct {
			inline static BOOL WINAPI hook_proc(DWORD thread_id, WNDENUMPROC enum_proc, LPARAM lParam)
			{
//				MY_TRACE_FUNC("{}, {:#010x}, {:#010x}", thread_id, enum_proc, lParam);

				// 「イージング設定時短プラグイン」用の処理です。
				if (thread_id == ::GetCurrentThreadId() && enum_proc && lParam)
				{
					// enum_proc()の中で::GetWindow()が呼ばれます。
					if (!enum_proc(hive.setting_dialog, lParam))
						return FALSE;
				}

				return orig_proc(thread_id, enum_proc, lParam);
			}
			inline static decltype(&hook_proc) orig_proc = ::EnumThreadWindows;
		} EnumThreadWindows;

		//
		// このクラスは::EnumWindows()をフックします。
		//
		inline static struct {
			inline static BOOL WINAPI hook_proc(WNDENUMPROC enum_proc, LPARAM lParam)
			{
//				MY_TRACE_FUNC("{:#010x}, {:#010x}", enum_proc, lParam);

				// 「拡張編集RAMプレビュー」用の処理です。
				if (enum_proc && lParam)
				{
					if (!enum_proc(hive.aviutl_window, lParam))
						return FALSE;
				}

				return orig_proc(enum_proc, lParam);
			}
			inline static decltype(&hook_proc) orig_proc = ::EnumWindows;
		} EnumWindows;

		//
		// このクラスは::SetWindowLongA()をフックします。
		//
		inline static struct {
			inline static LONG WINAPI hook_proc(HWND hwnd, int index, LONG new_long)
			{
//				MY_TRACE_FUNC("{:#010x}, {}, {:#010x}", hwnd, index, new_long);

				// 「拡張ツールバー」用の処理です。
				if (index == GWL_HWNDPARENT)
				{
					auto shuttle = Shuttle::get_pointer(hwnd);
					if (shuttle)
					{
						MY_TRACE("シャトルが取得できるウィンドウはHWNDPARENTを変更できないようにします\n");

						return 0;
					}
				}

				return orig_proc(hwnd, index, new_long);
			}
			inline static decltype(&hook_proc) orig_proc = ::SetWindowLongA;
		} SetWindowLongA;
	} window;
}
