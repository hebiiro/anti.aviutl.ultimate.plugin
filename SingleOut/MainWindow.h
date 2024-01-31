#pragma once
#include "MainDialog.h"

namespace fgo::single_out
{
	//
	// このクラスはメインウィンドウです。
	// このウィンドウはプラグインウィンドウのように振る舞います。
	//
	inline struct MainWindow : Tools::AviUtl::PluginWindow
	{
		MainDialog mainDialog;

		//
		// 初期化を行います。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// プラグインウィンドウのようなウィンドウを作成します。
			return createAsPlugin(
				hive.instance,
				magi.auin.GetAviUtlWindow(),
				Hive::DisplayName,
				WS_EX_NOPARENTNOTIFY,
				WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				100, 100, 300, 300);
		}

		//
		// 後始末を行います。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return destroy();
		}

		//
		// ウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_CREATE:
				{
					MY_TRACE_FUNC("WM_CREATE, 0x%08X, 0x%08X", wParam, lParam);

					if (!mainDialog.create(hwnd))
					{
						MY_TRACE(_T("メインダイアログの作成に失敗しました\n"));

						return -1;
					}

					setTarget(mainDialog);
					resize();

					break;
				}
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}
	} mainWindow;
}
