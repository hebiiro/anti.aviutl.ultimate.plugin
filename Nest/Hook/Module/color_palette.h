#pragma once

namespace fgo::nest::hook
{
	//
	// このクラスはcolor_palette.aufをフックします。
	//
	struct color_palette
	{
		//
		// このクラスは::CreateDialogParamA()をフックします。
		// マイパレットが作成されるとき、マイパレット用シャトルを作成します。
		//
		inline static struct
		{
			static HWND WINAPI hook(HINSTANCE instance, LPCSTR templateName, HWND parent, DLGPROC dlgProc, LPARAM initParam)
			{
				MY_TRACE(_T("color_palette::CreateDialogParamA()\n"));

				HWND hwnd = orig(instance, templateName, parent, dlgProc, initParam);

				TCHAR windowName[MAX_PATH] = {};
				::GetWindowText(hwnd, windowName, MAX_PATH);
				MY_TRACE_TSTR(windowName);

				if (_tcsicmp(windowName, _T("マイパレット")) == 0)
				{
					// マイパレットダイアログ用のシャトルを作成します。
					auto shuttle = std::make_shared<Shuttle>();
					shuttle->init(windowName, hwnd);
				}

				return hwnd;
			}
			inline static decltype(&hook) orig = 0;
		} CreateDialogParamA;

		//
		// このクラスは::ShowWindow()をフックします。
		// カラーパレットがドッキングしているマイパレットを表示するのを防ぎます。
		//
		inline static struct
		{
			static BOOL WINAPI hook(HWND hwnd, int cmdShow)
			{
				MY_TRACE(_T("color_palette::ShowWindow(0x%08X, %d)\n"), hwnd, cmdShow);

				if (cmdShow)
				{
					auto shuttle = shuttleManager.get(L"マイパレット");
					if (shuttle && *shuttle == hwnd)
					{
						MY_TRACE_HEX(shuttle->getListener());

						// マイパレットがドッキング中なら
						if (shuttle->getListener())
							return FALSE; // マイパレットを表示しません。
					}
				}

				return orig(hwnd, cmdShow);
			}
			inline static decltype(&hook) orig = 0;
		} ShowWindow;

		//
		// color_palette.auf用のフックを初期化します。
		//
		static BOOL init()
		{
			HMODULE color_palette = ::GetModuleHandle(_T("color_palette.auf"));
			MY_TRACE_HEX(color_palette);
			if (!color_palette) return FALSE;

			// color_palette.auf内の::CreateDialogParamA()をフックします。
			Tools::attach_import(CreateDialogParamA, color_palette, "CreateDialogParamA");
			MY_TRACE_HEX(CreateDialogParamA.orig);

			// color_palette.auf内の::ShowWindow()をフックします。
			Tools::attach_import(ShowWindow, color_palette, "ShowWindow");
			MY_TRACE_HEX(ShowWindow.orig);

			return TRUE;
		}
	};
}
