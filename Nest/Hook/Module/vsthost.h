#pragma once

namespace fgo::nest::hook
{
	//
	// このクラスはvsthost.aufをフックします。
	//
	struct vsthost
	{
		//
		// このクラスは::DialogBoxIndirectParamA()をフックします。
		//
		inline static struct
		{
			static INT_PTR WINAPI hook(HINSTANCE instance, LPCDLGTEMPLATEA dialogTemplate, HWND parent, DLGPROC dlgProc, LPARAM initParam)
			{
				MY_TRACE(_T("vsthost::DialogBoxIndirectParamA()\n"));

				HWND dummy = hive.createPopupWindow(parent);

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
				// vsthost_N.auf 内の ::DialogBoxIndirectParamA() をフックする。

				TCHAR fileName[MAX_PATH] = {};
				::StringCbPrintf(fileName, sizeof(fileName), _T("vsthost_%d.auf"), i + 1);
				MY_TRACE_TSTR(fileName);

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
