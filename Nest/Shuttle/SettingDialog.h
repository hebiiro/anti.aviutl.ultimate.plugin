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
			MY_TRACE_FUNC("%s, 0x%08X", (LPCTSTR)name, hwnd);

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
#if 0 // この処理を有効にしてもトラックバーなどのチラツキが発生します。
		//
		// ターゲットウィンドウに適用する新しいスタイルを返します。
		//
		DWORD onGetTargetNewStyle() override
		{
			return __super::onGetTargetNewStyle() | WS_CLIPCHILDREN;
		}
#endif
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
					MY_TRACE_FUNC("WM_SIZE, 0x%08X, 0x%08X", wParam, lParam);

					// 「patch.aul」用の処理です。
					// 設定ダイアログが高速描画されているときは
					// 親ウィンドウ(コンテナ)を手動で再描画する必要があります。
					::InvalidateRect(::GetParent(hwnd), 0, FALSE);

					break;
				}
			case WM_MOUSEWHEEL:
			case WM_MOUSEHWHEEL:
				{
					// マウスホイールでスクロールするために
					// 既定の処理をブロックし、コンテナウィンドウへバイパスします。
					return ::SendMessage(::GetParent(hwnd), message, wParam, lParam);
				}
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}
	}; inline auto settingDialog = std::make_shared<SettingDialog>();
}
