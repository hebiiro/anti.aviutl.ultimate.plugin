#pragma once

namespace apn::workspace
{
	//
	// このクラスは設定ダイアログ用のシャトルです。
	//
	struct SettingDialog : Shuttle
	{
		//
		// ターゲットにアタッチします。
		//
		void init(const std::wstring& name, HWND hwnd)
		{
			MY_TRACE_FUNC("{}, {:#010x}", name, hwnd);

			__super::init(name, hwnd);
		}

		//
		// ターゲットウィンドウを表示します。
		//
		virtual void show_target_window() override
		{
			// 設定ダイアログの表示は拡張編集に任せます。
		}

		//
		// ドッキングコンテナを作成して返します。
		//
		virtual std::shared_ptr<Container> create_dock_container() override
		{
			DWORD dock_style = WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

			return std::make_shared<ScrollContainer>(this, dock_style);
		}

		//
		// フローティングコンテナを作成して返します。
		//
		virtual std::shared_ptr<Container> create_float_container() override
		{
			DWORD float_style = WS_CAPTION | WS_SYSMENU | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
			float_style |= WS_THICKFRAME;

			return std::make_shared<ScrollContainer>(this, float_style);
		}
#if 0 // この処理を有効にしてもトラックバーなどのチラツキが発生します。
		//
		// ターゲットウィンドウに適用する新しいスタイルを返します。
		//
		virtual DWORD on_get_target_new_style() override
		{
			return __super::on_get_target_new_style() | WS_CLIPCHILDREN;
		}
#endif
		//
		// ターゲットのウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
//			MY_TRACE_FUNC("{:#010x}, {:#010x}, {:#010x}, {:#010x}", hwnd, message, wParam, lParam);

			switch (message)
			{
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {:#010x}, {:#010x}", wParam, lParam);

					// このタイミングでサブクラス化を解除して、後始末処理を省略します。
					unsubclass();

					break;
				}
			case WM_PAINT:
				{
					// 「拡張編集」用の処理です。
					// カレントオブジェクトが存在しない場合にWM_PAINTを処理させてしまうとエラーになります。
					// よって、カレントオブジェクトをチェックし、存在しない場合は処理しないようにします。

					auto object_index = magi.exin.get_current_object_index();
//					MY_TRACE_INT(object_index);

					if (object_index < 0)
					{
						my::PaintDC dc(hwnd);
						auto rc = my::get_client_rect(hwnd);
						auto brush = (HBRUSH)::SendMessage(hwnd, WM_CTLCOLORDLG, (WPARAM)(HDC)dc, (LPARAM)hwnd);
						::FillRect(dc, &rc, brush);
						return 0;
					}

					break;
				}
			case WM_SIZE:
				{
					MY_TRACE_FUNC("WM_SIZE, {:#010x}, {:#010x}", wParam, lParam);

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

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	}; inline auto setting_dialog = std::make_shared<SettingDialog>();
}
