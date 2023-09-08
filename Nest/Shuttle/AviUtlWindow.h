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
			MY_TRACE(_T("AviUtlWindow::init(%s, 0x%08X)\n"), (LPCTSTR)name, hwnd);

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
			switch (message)
			{
			case WM_CLOSE:
				{
					MY_TRACE(_T("AviUtlWindow::onWndProc(WM_CLOSE)\n"));

					// AviUtlが終了しようとしているので設定を保存します。
					::SendMessage(hive.mainWindow, Hive::WindowMessage::WM_SAVE_CONFIG, 0, 0);

					break;
				}
			case WM_SETTEXT:
				{
					MY_TRACE(_T("AviUtlWindow::onWndProc(WM_SETTEXT)\n"));

					LPCSTR newText = (LPCSTR)lParam;

					// メインウィンドウのウィンドウテキストを更新します。

					AviUtl::EditHandle* editp = magi.auin.GetEditp();

					char fileName[MAX_PATH] = {};
					if (editp->frame_n)
					{
						::StringCbCopyA(fileName, sizeof(fileName), editp->project_filename);
						::PathStripPathA(fileName);

						if (strlen(fileName) == 0)
							::StringCbCopyA(fileName, sizeof(fileName), newText);
					}
					else
					{
						::StringCbCopyA(fileName, sizeof(fileName), "無題");
					}

					::StringCbCatA(fileName, sizeof(fileName), " - ");
					::StringCbCatA(fileName, sizeof(fileName), newText);

					::SetWindowTextA(hive.mainWindow, fileName);

					break;
				}
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}
	}; inline auto aviutlWindow = std::make_shared<AviUtlWindow>();
}
