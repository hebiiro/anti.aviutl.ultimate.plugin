#pragma once

namespace apn::workspace::hook
{
	//
	// このクラスはvsthost.aufをフックします。
	//
	inline struct {
		//
		// 他のポップアップウィンドウの土台となるポップアップウィンドウを作成します。
		//
		inline static HWND create_popup_window(HWND parent)
		{
			MY_TRACE_FUNC("{:#010x}", parent);

			const auto c_class_name = _T("apn::workspace::popup_window");

			WNDCLASS wc = {};
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
			wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = hive.instance;
			wc.lpszClassName = c_class_name;
			::RegisterClass(&wc);

			return ::CreateWindowEx(
				0,
				c_class_name,
				c_class_name,
				WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				parent, 0, hive.instance, 0);
		}

		//
		// このクラスは::DialogBoxIndirectParamA()をフックします。
		//
		inline static struct {
			inline static INT_PTR WINAPI hook_proc(HINSTANCE instance, LPCDLGTEMPLATEA dlg_template, HWND parent, DLGPROC dlg_proc, LPARAM init_param)
			{
				MY_TRACE_FUNC("");

				auto dummy = create_popup_window(parent);

				auto active_window = ::GetActiveWindow();
				::EnableWindow(parent, FALSE);
				auto result = orig_proc(instance, dlg_template, dummy, dlg_proc, init_param);
				::EnableWindow(parent, TRUE);
				::SetActiveWindow(active_window);

				::DestroyWindow(dummy);

				return result;
			}
			inline static decltype(&hook_proc) orig_proc = 0;
		} DialogBoxIndirectParamA;

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			for (auto i = 0; i < 10; i++)
			{
				// vsthost_N.aufのファイル名を取得します。
				auto file_name = std::format(_T("vsthost_{}.auf"), i + 1);
				MY_TRACE_STR(file_name);

				// vsthost_N.aufのモジュールハンドルを取得します。
				auto vsthost = ::GetModuleHandle(file_name.c_str());
				MY_TRACE_HEX(vsthost);
				if (!vsthost) break;

				// vsthost_N.auf内の::DialogBoxIndirectParamA()をフックします。
				my::hook::attach_import(DialogBoxIndirectParamA, vsthost, "DialogBoxIndirectParamA");
				MY_TRACE_HEX(DialogBoxIndirectParamA.orig_proc);
			}

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
	} vsthost;
}
