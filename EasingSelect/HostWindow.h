#pragma once
#include "Hive.h"
#include "Hook.h"
#include "Gui.h"

namespace fgo::easing_select
{
	//
	// このクラスはホストプロセスのメインウィンドウです。
	//
	struct HostWindow : Tools::AviUtl::PluginWindow
	{
		//
		// フックです。
		//
		std::shared_ptr<Hook> hook;

		//
		// GUIプロセスです。
		//
		std::shared_ptr<Gui> gui;

		//
		// コンストラクタです。
		//
		HostWindow()
		{
			MY_TRACE_FUNC("");

			// ウィンドウを作成します。
			if (!createAsPlugin(
				hive.instance,
				magi.auin.GetAviUtlWindow(),
				hive.AppName,
				WS_EX_NOPARENTNOTIFY,
				WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				100, 100, 300, 300))
			{
				throw _T("ホストウィンドウの失敗しました");
			}

			hook = std::make_shared<Hook>();
			gui = std::make_shared<Gui>(*this);
		}

		//
		// デストラクタです。
		//
		~HostWindow()
		{
			MY_TRACE_FUNC("");
		}

		//
		// ウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case Share::EasingSelect::Message::FireNotify:
				{
					MY_TRACE_FUNC("Share::EasingSelect::Message::FireNotify, 0x%08X, 0x%08X", wParam, lParam);

					if (gui)
						::PostThreadMessage(gui->pi.dwThreadId, Share::EasingSelect::Message::Notify, 0, 0);

					break;
				}
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}
	};
}
