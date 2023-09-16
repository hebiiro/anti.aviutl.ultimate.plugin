#pragma once
#include "Shuttle.h"

namespace fgo::nest
{
	//
	// このクラスは設定ダイアログ用のシャトルです。
	//
	struct SettingDialog : Shuttle
	{
		//
		// ターゲットにアタッチします。
		//
		void init(const _bstr_t& name, HWND hwnd)
		{
			MY_TRACE(_T("SettingDialog::init(%s, 0x%08X)\n"), (LPCTSTR)name, hwnd);

			__super::init(name, hwnd);
		}

		//
		// ターゲットウィンドウを表示します。
		//
		void showTargetWindow() override
		{
			// 設定ダイアログの表示は拡張編集に任せます。
		}

		//
		// ドッキングコンテナを作成して返します。
		//
		std::shared_ptr<Container> createDockContainer() override
		{
			DWORD dockStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

			return std::make_shared<ScrollContainer>(this, dockStyle);
		}

		//
		// フローティングコンテナを作成して返します。
		//
		std::shared_ptr<Container> createFloatContainer() override
		{
			DWORD floatStyle = WS_CAPTION | WS_SYSMENU | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
			floatStyle |= WS_THICKFRAME;

			return std::make_shared<ScrollContainer>(this, floatStyle);
		}

		//
		// コンテナのウィンドウプロシージャです。
		//
		LRESULT onContainerWndProc(Container* container, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, BOOL* skipDefault) override
		{
			switch (message)
			{
			case WM_RBUTTONDOWN:
				{
					::SetFocus(*this);

					// このまま WM_RBUTTONUP に処理を続けます。
				}
			case WM_RBUTTONUP:
				{
					// マウス座標を取得します。
					POINT point = LP2PT(lParam);

					// コンテナウィンドウの座標系から設定ダイアログの座標系へ変換します。
					::MapWindowPoints(hwnd, *this, &point, 1);

					// 設定ダイアログにメッセージを転送します。
					*skipDefault = TRUE;
					return ::SendMessage(*this, message, wParam, MAKELPARAM(point.x, point.y));
				}
			}

			return __super::onContainerWndProc(container, hwnd, message, wParam, lParam, skipDefault);
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
					MY_TRACE(_T("SettingDialog::onWndProc(WM_DESTROY, 0x%08X, 0x%08X)\n"), wParam, lParam);

					// このタイミングでサブクラス化を解除して、後始末処理を省略します。
					unsubclass();

					break;
				}
			case WM_PAINT:
				{
					// 「拡張編集」用の処理です。
					// カレントオブジェクトが存在しない場合に WM_PAINT を処理させてしまうとエラーになります。
					// よって、カレントオブジェクトをチェックし、存在しない場合は処理しないようにします。

					int objectIndex = magi.auin.GetCurrentObjectIndex();
//					MY_TRACE_INT(objectIndex);

					if (objectIndex < 0)
						return 0;

					break;
				}
			case WM_SIZE:
				{
					// 「patch.aul」用の処理です。
					// 設定ダイアログが高速描画されているときは
					// 親ウィンドウ(コンテナ)を手動で再描画する必要があります。
					::InvalidateRect(::GetParent(hwnd), 0, FALSE);

					break;
				}
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}
	}; inline auto settingDialog = std::make_shared<SettingDialog>();
}
