#pragma once

namespace fgo::nest::hook
{
	//
	// このクラスはvsthost.aufをフックします。
	//
	struct vsthost
	{
		//
		// 他のポップアップウィンドウの土台となるポップアップウィンドウを作成します。
		//
		static HWND createPopupWindow(HWND parent)
		{
			MY_TRACE_FUNC("0x%08X", parent);

			const LPCTSTR ClassName = _T("PopupWindow");

			WNDCLASS wc = {};
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
			wc.hCursor = ::LoadCursor(0, IDC_ARROW);
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = hive.instance;
			wc.lpszClassName = ClassName;
			::RegisterClass(&wc);

			HWND hwnd = ::CreateWindowEx(
				0,
				ClassName,
				ClassName,
				WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				parent, 0, hive.instance, 0);

			return hwnd;
		}

		//
		// このクラスは::DialogBoxIndirectParamA()をフックします。
		//
		inline static struct
		{
			static INT_PTR WINAPI hook(HINSTANCE instance, LPCDLGTEMPLATEA dialogTemplate, HWND parent, DLGPROC dlgProc, LPARAM initParam)
			{
				MY_TRACE_FUNC("");

				HWND dummy = createPopupWindow(parent);

				HWND activeWindow = ::GetActiveWindow();
				::EnableWindow(parent, FALSE);
				INT_PTR result = orig(instance, dialogTemplate, dummy, dlgProc, initParam);
				::EnableWindow(parent, TRUE);
				::SetActiveWindow(activeWindow);

				::DestroyWindow(dummy);

				return result;
			}
			inline static decltype(&hook) orig = 0;
		} DialogBoxIndirectParamA;

		//
		// vsthost.auf用のフックを初期化します。
		//
		static BOOL init()
		{
			for (int i = 0; i < 10; i++)
			{
				// vsthost_N.aufのファイル名を取得します。
				TCHAR fileName[MAX_PATH] = {};
				::StringCchPrintf(fileName, std::size(fileName), _T("vsthost_%d.auf"), i + 1);
				MY_TRACE_TSTR(fileName);

				// vsthost_N.aufのモジュールハンドルを取得します。
				HMODULE vsthost = ::GetModuleHandle(fileName);
				MY_TRACE_HEX(vsthost);
				if (!vsthost) break;

				// vsthost_N.auf内の::DialogBoxIndirectParamA()をフックします。
				Tools::attach_import(DialogBoxIndirectParamA, vsthost, "DialogBoxIndirectParamA");
				MY_TRACE_HEX(DialogBoxIndirectParamA.orig);
			}

			return TRUE;
		}
	};
}
