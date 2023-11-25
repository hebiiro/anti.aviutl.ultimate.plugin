#pragma once
#include "Shuttle.h"

namespace fgo::nest
{
	//
	// このクラスはAviUtlウィンドウ用のシャトルです。
	//
	struct AviUtlWindow : Shuttle
	{
		//
		// ターゲットにアタッチします。
		//
		void init(const _bstr_t& name, HWND hwnd)
		{
			MY_TRACE_FUNC("%s, 0x%08X", (LPCTSTR)name, hwnd);

			__super::init(name, hwnd);

			// フローティングコンテナのアイコンを設定します。
			HICON icon = (HICON)::GetClassLong(*this, GCL_HICON);
			::SendMessage(*floatContainer, WM_SETICON, ICON_SMALL, (LPARAM)icon);
			::SendMessage(*floatContainer, WM_SETICON, ICON_BIG, (LPARAM)icon);

			// メインウィンドウのアイコンを設定します。
			::SetClassLong(hive.mainWindow, GCL_HICON, (LONG)icon);
			::SetClassLong(hive.mainWindow, GCL_HICONSM, (LONG)icon);
		}

		//
		// ターゲットウィンドウを表示します。
		//
		void showTargetWindow() override
		{
			// ターゲットウィンドウが表示状態なら何もしません。
			if (::IsWindowVisible(*this)) return;

			// AviUtlウィンドウは普通に表示します。
			::ShowWindow(*this, SW_SHOW);
		}

		//
		// ターゲットのウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
//			MY_TRACE_FUNC("0x%08X, 0x%08X, 0x%08X, 0x%08X", hwnd, message, wParam, lParam);

			switch (message)
			{
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, 0x%08X, 0x%08X", wParam, lParam);

					// このタイミングでサブクラス化を解除して、後始末処理を省略します。
					unsubclass();

					break;
				}
			case WM_CLOSE:
				{
					MY_TRACE_FUNC("WM_CLOSE, 0x%08X, 0x%08X", wParam, lParam);

					// AviUtlが終了しようとしているので設定を保存します。
					::SendMessage(hive.mainWindow, Hive::WindowMessage::WM_SAVE_CONFIG, 0, 0);

					break;
				}
			case WM_SETTEXT:
				{
					MY_TRACE_FUNC("WM_SETTEXT, 0x%08X, 0x%08X", wParam, lParam);

					LRESULT lr = __super::onWndProc(hwnd, message, wParam, lParam);

					// メインウィンドウのウィンドウテキストを更新します。
					::SendMessage(hive.mainWindow, Hive::WindowMessage::WM_REFRESH_TITLE, 0, 0);

					return lr;
				}
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}
	}; inline auto aviutlWindow = std::make_shared<AviUtlWindow>();
}
