#pragma once

namespace fgo
{
	//
	// このクラスはこのプラグインのウィンドウです。
	//
	inline struct Spiritron : Tools::Window2, Magi::CommandID::Plugin
	{
		//
		// アドインの一覧を表示するためのリストボックスです。
		//
		Tools::Window listbox;

		//
		// 初期化を実行します。
		// 内部的に使用されます。
		//
		BOOL init(AviUtl::FilterPlugin* fp)
		{
			if (!subclass(fp->hwnd)) return FALSE;

			// アドイン一覧用のリストボックスを作成します。
			if (!listbox.create(
				WS_EX_CLIENTEDGE,
				WC_LISTBOX,
				_T("アドインの一覧"),
				WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
				WS_HSCROLL | WS_VSCROLL |
				LBS_HASSTRINGS | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | LBS_MULTICOLUMN | LBS_SORT,
				0, 0, 0, 0,
				*this, 0, 0, 0))
			{
				::MessageBox(*this, _T("アドイン一覧のリストボックスの作成に失敗しました"), _T("Ultimate"), MB_OK);
			}

			{
				// コントロールのフォントを設定します。

				AviUtl::SysInfo si = {};
				fp->exfunc->get_sys_info(0, &si);
				MY_TRACE_HEX(si.hfont);

				::SendMessageW(listbox, WM_SETFONT, (WPARAM)si.hfont, TRUE);
			}

			{
				// ウィンドウホルダー拡張を作成します。
				auto windowHolderExtension = std::make_shared<Tools::AviUtl::WindowHolderExtension>();

				// このウィンドウにウィンドウホルダー拡張を追加します。
				// これによって、クライアント領域にターゲット(他のウィンドウ)を保持できるようになります。
				addExtension(windowHolderExtension);

				// リストボックスをターゲットにしてリサイズします。
				windowHolderExtension->setTarget(listbox);
				windowHolderExtension->resize(*this);
			}

			{
				// リストボックスにアドインの一覧を構築します。

				for (const auto& pair : fate.servants.map)
				{
					auto servant = pair.second.servant;

					// リストボックスにアイテムを追加します。アイテム名はサーヴァントの表示名です。
					int index = ::SendMessageW(listbox, LB_ADDSTRING,
						0, (LPARAM)servant->get_servant_display_name());

					// アイテムデータにサーヴァントのポインタを設定します。
					::SendMessageW(listbox, LB_SETITEMDATA, index, (LPARAM)servant);
				}
			}

			{
				// AviUtlにメニューアイテムを追加します。
				// リストボックスからソート済みのサーヴァントを取得します。

				// アイテムの数を取得します。
				int c = ::SendMessageW(listbox, LB_GETCOUNT, 0, 0);
				for (int i = 0; i < c; i++)
				{
					// アイテムに関連付けられているサーヴァントを取得します。
					auto servant = (Servant*)::SendMessageW(listbox, LB_GETITEMDATA, i, 0);
					MY_TRACE_HEX(servant);

					// サーヴァントを取得できなかった場合は何もしません。
					if (!servant) continue;

					// メニューアイテム名を構築します。
					char name[MAX_PATH] = {};
					::StringCchPrintfA(name, std::size(name), "%ws", servant->get_servant_display_name());
					MY_TRACE_STR(name);

					// AviUtlにメニュー項目を追加します。
					fp->exfunc->add_menu_item(fp, name, *this, ID_ADDIN_BEGIN + i, 0, AviUtl::ExFunc::AddMenuItemFlag::None);
				}
			}

			return TRUE;
		}

		//
		// 後始末を実行します。
		// 内部的に使用されます。
		//
		BOOL exit()
		{
			return TRUE;
		}

		//
		// ウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, 0x%08X, 0x%08X", wParam, lParam);

					break;
				}
			case WM_NCDESTROY:
				{
					MY_TRACE_FUNC("WM_NCDESTROY, 0x%08X, 0x%08X", wParam, lParam);

					MY_TRACE_HWND(hwnd);

					break;
				}
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}

		BOOL func_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
		{
//			MY_TRACE_FUNC("0x%08X, 0x%08X, 0x%08X, 0x%08X", hwnd, message, wParam, lParam);

			switch (message)
			{
			case AviUtl::FilterPlugin::WindowMessage::Command:
				{
					MY_TRACE_FUNC("Command, 0x%08X, 0x%08X", wParam, lParam);

					if (wParam == 0 && lParam == 0) return TRUE; // AviUtlの再描画のみ行います。

					if (wParam >= ID_ADDIN_BEGIN && wParam < ID_ADDIN_END)
					{
						// コマンドからインデックスを取得します。
						int index = wParam - ID_ADDIN_BEGIN;
						MY_TRACE_INT(index);

						// コマンドのインデックスからサーヴァントを取得します。
						auto servant = (Servant*)::SendMessageW(listbox, LB_GETITEMDATA, index, 0);
						MY_TRACE_HEX(servant);

						// サーヴァントを取得できなかった場合は何もしません。
						if (!servant) break;

						// サーヴァントにアドインコマンドを送ります。
						return servant->on_window_command(hwnd, message, ID_ADDIN, lParam, editp, fp);
					}

					return fate.fire_window_command(hwnd, message, wParam, lParam, editp, fp);
				}
			case WM_COMMAND:
				{
					UINT code = HIWORD(wParam);
					HWND control = (HWND)lParam;

					switch (code)
					{
					case LBN_DBLCLK: // リストボックスでダブルクリックが発生しました。
						{
							// 選択アイテムのインデックスを取得します。
							int index = ::SendMessageW(listbox, LB_GETCURSEL, 0, 0);
							MY_TRACE_INT(index);

							// 選択アイテムのインデックスからサーヴァントを取得します。
							auto servant = (Servant*)::SendMessageW(listbox, LB_GETITEMDATA, index, 0);
							MY_TRACE_HEX(servant);

							// サーヴァントを取得できなかった場合は何もしません。
							if (!servant) break;

							// サーヴァントにアドインコマンドを送ります。
							return servant->on_window_command(hwnd, message, ID_ADDIN, lParam, editp, fp);
						}
					}

					break;
				}
			}

			return FALSE;
		}
	} spiritron;
}
