#pragma once

namespace apn::note
{
	//
	// このクラスはリッチエディットコントロールです。
	//
	struct RichEdit : my::Window
	{
		inline static constexpr struct CommandID {
			inline static constexpr auto c_cut = 1;
			inline static constexpr auto c_copy = 2;
			inline static constexpr auto c_paste = 3;
			inline static constexpr auto c_erase = 4;
			inline static constexpr auto c_select_all = 5;
			inline static constexpr auto c_undo = 6;
			inline static constexpr auto c_redo = 7;
		} c_command_id;

		//
		// 初期化処理を実行します。
		//
		BOOL init(HWND parent)
		{
			MY_TRACE_FUNC("{:#010x}", parent);

			if (!__super::create(
				WS_EX_CLIENTEDGE,
				RICHEDIT_CLASSW,
				nullptr,
				WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_HSCROLL |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
				ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
				0, 0, 0, 0,
				parent, nullptr, hive.instance, nullptr))
			{
				hive.message_box(L"リッチエディットコントロールの作成に失敗しました");

				return FALSE;
			}

			// フォントを設定します。
			AviUtl::SysInfo si = {};
			magi.fp->exfunc->get_sys_info(nullptr, &si);
			::SendMessage(*this, WM_SETFONT, (WPARAM)si.hfont, FALSE);

			// イベントマスクを設定します。
			auto event_mask = ::SendMessage(*this, EM_GETEVENTMASK, 0, 0);
			event_mask |= ENM_MOUSEEVENTS;
			::SendMessage(*this, EM_SETEVENTMASK, 0, event_mask);

			// 言語オプションを設定します。
			auto lang_options = ::SendMessage(*this, EM_GETLANGOPTIONS, 0, 0); 
//			lang_options &= ~IMF_DUALFONT;
//			lang_options &= ~IMF_AUTOFONT;
//			lang_options &= ~IMF_AUTOFONTSIZEADJUST;
			lang_options |= IMF_UIFONTS;
			::SendMessage(*this, EM_SETLANGOPTIONS, 0, lang_options);

			// 余白を設定します。
			auto margin = RECT { 6, 6, 6, 6 };
			::SendMessage(*this, EM_SETRECT, TRUE, (LPARAM)&margin);
//			::SendMessage(*this, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(6, 6));

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return destroy();
		}

		//
		// 親ウィンドウに送られたリッチエディットコントロールのコマンドを処理します。
		//
		LRESULT on_command_reflection(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			auto code = HIWORD(wParam);
			auto id = LOWORD(wParam);
			auto control = (HWND)lParam;

			switch (id)
			{
			case c_command_id.c_cut: return ::SendMessage(*this, WM_CUT, 0, 0);
			case c_command_id.c_copy: return ::SendMessage(*this, WM_COPY, 0, 0);
			case c_command_id.c_paste: return ::SendMessage(*this, WM_PASTE, 0, 0);
			case c_command_id.c_erase: return ::SendMessage(*this, WM_KEYDOWN, VK_DELETE, 0);
			case c_command_id.c_select_all: return ::SendMessage(*this, EM_SETSEL, 0, -1);
			case c_command_id.c_undo: return ::SendMessage(*this, EM_UNDO, 0, 0);
			case c_command_id.c_redo: return ::SendMessage(*this, EM_REDO, 0, 0);
			}

			return 0;
		}

		//
		// 親ウィンドウに送られたリッチエディットコントロールの通知を処理します。
		//
		LRESULT on_notify_reflection(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			auto header = (NMHDR*)lParam;

			switch (header->code)
			{
			case EN_MSGFILTER:
				{
					auto msg_filter = (MSGFILTER*)lParam;

					switch (msg_filter->msg)
					{
					case WM_RBUTTONDOWN:
						{
							// リッチエディットコントロール用のコンテキストメニューを表示します。

							my::menu::unique_ptr<> menu(::CreatePopupMenu());

							::AppendMenuW(menu.get(), MF_STRING, c_command_id.c_undo, L"元に戻す(&U)");
							::AppendMenuW(menu.get(), MF_STRING, c_command_id.c_redo, L"やり直し(&R)");
							::AppendMenuW(menu.get(), MF_SEPARATOR, 0, nullptr);
							::AppendMenuW(menu.get(), MF_STRING, c_command_id.c_cut, L"切り取り(&T)");
							::AppendMenuW(menu.get(), MF_STRING, c_command_id.c_copy, L"コピー(&C)");
							::AppendMenuW(menu.get(), MF_STRING, c_command_id.c_paste, L"貼り付け(&P)");
							::AppendMenuW(menu.get(), MF_SEPARATOR, 0, nullptr);
							::AppendMenuW(menu.get(), MF_STRING, c_command_id.c_erase, L"削除(&D)");
							::AppendMenuW(menu.get(), MF_STRING, c_command_id.c_select_all, L"すべて選択(&A)");

							if (!::SendMessage(*this, EM_CANUNDO, 0, 0)) ::EnableMenuItem(menu.get(), c_command_id.c_undo, MF_DISABLED | MF_GRAYED);
							if (!::SendMessage(*this, EM_CANREDO, 0, 0)) ::EnableMenuItem(menu.get(), c_command_id.c_redo, MF_DISABLED | MF_GRAYED);

							auto pos = my::lp_to_pt(msg_filter->lParam);
							my::map_window_points(msg_filter->nmhdr.hwndFrom, nullptr, &pos);

							::TrackPopupMenuEx(menu.get(), 0, pos.x, pos.y, hwnd, nullptr);

							break;
						}
					}

					break;
				}
			}

			return 0;
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_CREATE:
				{
					MY_TRACE_FUNC("WM_CREATE, {:#010x}, {:#010x}", wParam, lParam);

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {:#010x}, {:#010x}", wParam, lParam);

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	};
}
