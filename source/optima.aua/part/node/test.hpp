#pragma once

namespace apn::optima::part
{
#if 0
	//
	// テスト用コードです。
	// このクラスはテスト用ウィンドウです。
	//
	struct TestWindow : my::Window
	{
		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
			auto class_name = my::get_class_name(hwnd);
			auto message_name = my::message_to_string(message);

			MY_TRACE("{/hex}, {/}, {/}, w_param = {/hex}, l_param = {/hex}\n", hwnd, class_name, message_name, w_param, l_param);

			auto lr = __super::on_wnd_proc(hwnd, message, w_param, l_param);

			MY_TRACE("{/hex}, {/}, {/}, w_param = {/hex}, l_param = {/hex} => {/hex}を返しました\n", hwnd, class_name, message_name, w_param, l_param, lr);

			return lr;
		}
	};

	//
	// テスト用コードです。
	// このクラスは設定ダイアログのサブクラスです。
	//
	inline struct SettingDialog : TestWindow
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// 設定ダイアログをサブクラス化します。
			subclass(magi.exin.get_setting_dialog());

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			// 設定ダイアログのサブクラス化を解除します。
			unsubclass();

			return TRUE;
		}
	} setting_dialog;

	//
	// テスト用コードです。
	// このクラスはドロップダウンリストボックスのサブクラスです。
	//
	struct ComboLBox : TestWindow
	{
	};

	//
	// テスト用コードです。
	// このクラスはコンボボックスのサブクラスです。
	//
	struct ComboBox : TestWindow
	{
		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
			switch (message)
			{
			case WM_CREATE:
				{
					auto listbox = new ComboLBox();
					ComboLBox::Hooker hooker(*listbox);
					return __super::on_wnd_proc(hwnd, message, w_param, l_param);
				}
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}
	};

	//
	// このクラスはテスト用のパーツです。
	//
	inline struct TestPart : Node
	{
		//
		// 初期化処理を実行します。
		//
		virtual BOOL func_init(AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");
#if 1
			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			my::hook::attach(CreateWindowExA);
			my::hook::attach(CreateWindowExW);

			if (DetourTransactionCommit() != NO_ERROR)
			{
				MY_TRACE("APIフックに失敗しました\n");

				return FALSE;
			}
#endif
			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL func_exit(AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// ウィンドウ作成時の初期化処理を実行します。
		//
		virtual BOOL func_window_init(AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			setting_dialog.init();

			return TRUE;
		}

		//
		// ウィンドウ破壊時の後始末処理を実行します。
		//
		virtual BOOL func_window_exit(AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			setting_dialog.exit();

			return TRUE;
		}

		//
		// このクラスは::CreateWindowExA()をフックします。
		//
		inline static struct {
			inline static HWND WINAPI hook_proc(DWORD ex_style, LPCSTR class_name, LPCSTR window_name,
				DWORD style, int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param)
			{
				if (::lstrcmpiA(class_name, WC_COMBOBOXA) == 0)
				{
					auto window = new ComboBox();
					ComboBox::Hooker hooker(*window);
					return orig_proc(ex_style, class_name, window_name, style, x, y, w, h, parent, menu, instance, param);
				}
#if 0
				else if (::lstrcmpiA(class_name, "ComboLBox") == 0)
				{
					auto window = new ComboLBox();
					ComboBox::Hooker hooker(*window);
					return orig_proc(ex_style, class_name, window_name, style, x, y, w, h, parent, menu, instance, param);
				}
#endif
				// それ以外の場合はデフォルト処理を実行します。
				return orig_proc(ex_style, class_name, window_name, style, x, y, w, h, parent, menu, instance, param);
			}
			inline static decltype(&hook_proc) orig_proc = ::CreateWindowExA;
		} CreateWindowExA;

		//
		// このクラスは::CreateWindowExW()をフックします。
		//
		inline static struct {
			inline static HWND WINAPI hook_proc(DWORD ex_style, LPCWSTR class_name, LPCWSTR window_name,
				DWORD style, int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param)
			{
				if (::lstrcmpiW(class_name, WC_COMBOBOXW) == 0)
				{
					auto window = new ComboBox();
					ComboBox::Hooker hooker(*window);
					return orig_proc(ex_style, class_name, window_name, style, x, y, w, h, parent, menu, instance, param);
				}
#if 0
				else if (::lstrcmpiW(class_name, L"ComboLBox") == 0)
				{
					auto window = new ComboLBox();
					ComboBox::Hooker hooker(*window);
					return orig_proc(ex_style, class_name, window_name, style, x, y, w, h, parent, menu, instance, param);
				}
#endif
				// それ以外の場合はデフォルト処理を実行します。
				return orig_proc(ex_style, class_name, window_name, style, x, y, w, h, parent, menu, instance, param);
			}
			inline static decltype(&hook_proc) orig_proc = ::CreateWindowExW;
		} CreateWindowExW;
	} test_part;
#endif
}
