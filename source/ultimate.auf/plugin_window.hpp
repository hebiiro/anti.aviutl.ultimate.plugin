#pragma once

namespace apn
{
	//
	// このクラスはこのプラグインのウィンドウです。
	//
	inline struct PluginWindow : my::Window
	{
		inline static constexpr struct Message {
			inline static constexpr uint32_t c_update_config_file = WM_APP + 2024;
		} c_message;

		//
		// アドインの一覧を表示するためのリストボックスです。
		//
		my::Window listbox;

		//
		// 初期化を実行します。
		// 内部的に使用されます。
		//
		BOOL init(AviUtl::FilterPlugin* fp)
		{
			MY_TRACE_FUNC("");

			if (!subclass(fp->hwnd)) return FALSE;

			// クライアント矩形を取得します。
			auto rc = my::get_client_rect(*this);

			// アドイン一覧用のリストボックスを作成します。
			if (!listbox.create(
				WS_EX_CLIENTEDGE,
				WC_LISTBOX,
				_T("アドインの一覧"),
				WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
				WS_HSCROLL | WS_VSCROLL |
				LBS_HASSTRINGS | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | LBS_MULTICOLUMN | LBS_SORT,
				rc.left, rc.top, my::get_width(rc), my::get_height(rc),
				*this, nullptr, nullptr, nullptr))
			{
				hive.message_box(_T("アドイン一覧リストボックスの作成に失敗しました"));

				return FALSE;
			}

			{
				// コントロールのフォントを設定します。

				AviUtl::SysInfo si = {};
				fp->exfunc->get_sys_info(nullptr, &si);
				MY_TRACE_HEX(si.hfont);

				::SendMessageW(listbox, WM_SETFONT, (WPARAM)si.hfont, TRUE);
			}

			{
				// リストボックスの項目の幅を設定します。

				// 項目の高さを取得します。
				auto height = ::SendMessageW(listbox, LB_GETITEMHEIGHT, 0, 0);
				MY_TRACE_INT(height);

				// 項目の幅を項目の高さの10倍に設定します。
				::SendMessageW(listbox, LB_SETCOLUMNWIDTH, height * 10, 0);
			}

			{
				// リストボックスにアドインの一覧を構築します。

				for (const auto& pair : addin_manager.addins.map)
				{
					const auto& addin = pair.second.addin;

					// リストボックスにアイテムを追加します。アイテム名はアドインの表示名です。
					auto index = (int)::SendMessageW(listbox, LB_ADDSTRING,
						0, (LPARAM)addin->get_addin_display_name());

					// アイテムデータにアドインのポインタを設定します。
					::SendMessageW(listbox, LB_SETITEMDATA, index, (LPARAM)addin);
				}
			}

			{
				// AviUtlにメニューアイテムを追加します。
				// リストボックスからソート済みのアドインを取得します。

				// アイテムの数を取得します。
				auto c = (int)::SendMessageW(listbox, LB_GETCOUNT, 0, 0);
				for (decltype(c) i = 0; i < c; i++)
				{
					// アイテムに関連付けられているアドインを取得します。
					auto addin = (Addin*)::SendMessageW(listbox, LB_GETITEMDATA, i, 0);
					MY_TRACE_HEX(addin);

					// アドインを取得できなかった場合は何もしません。
					if (!addin) continue;

					// メニューアイテム名を構築します。
					auto item_name = my::hs(addin->get_addin_display_name());
					MY_TRACE_STR(item_name);

					// AviUtlにメニュー項目を追加します。
					fp->exfunc->add_menu_item(fp, item_name.c_str(), *this,
						magi.c_command_id.c_addin.c_begin + i, 0, AviUtl::ExFunc::AddMenuItemFlag::None);
				}
			}

			{
				// コンフィグファイルを監視するサブスレッドを作成します。
				DWORD tid = 0;
				::CreateThread(nullptr, 0,
					[](LPVOID param) -> DWORD
					{
						auto folder_name = std::filesystem::path(magi.get_config_file_name(L""));
						MY_TRACE_STR(folder_name);
						std::vector<BYTE> buffer(1024);
						DWORD returned = 0;
						my::handle::unique_ptr<> file(::CreateFile(
							folder_name.c_str(),
							FILE_LIST_DIRECTORY,
							FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
							nullptr,
							OPEN_EXISTING,
							FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
							nullptr));
						MY_TRACE_HEX(file.get());
						while (::ReadDirectoryChangesW(
							file.get(),
							buffer.data(), buffer.size(),
							TRUE,
//							FILE_NOTIFY_CHANGE_FILE_NAME |
//							FILE_NOTIFY_CHANGE_DIR_NAME |
//							FILE_NOTIFY_CHANGE_SIZE |
							FILE_NOTIFY_CHANGE_LAST_WRITE,
							&returned,
							nullptr,
							nullptr))
						{
							MY_TRACE("コンフィグファイルの変更を検出しました\n");

							if (::IsWindow(plugin_window))
							{
								auto fni = (FILE_NOTIFY_INFORMATION*)buffer.data();
								auto length = fni->FileNameLength / sizeof(WCHAR);
								auto file_name = new WCHAR[length + 1];
								memcpy(file_name, fni->FileName, fni->FileNameLength);
								file_name[length] = L'\0';

								// 同じ通知が2回発生する場合があるので、
								// ファイルサイズをチェックして回避します。
								my::handle::unique_ptr<> file(::CreateFileW((folder_name / file_name).c_str(),
									GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr));
								auto file_size = ::GetFileSize(file.get(), nullptr);
								if (file_size)
									::PostMessage(plugin_window, c_message.c_update_config_file, 0, (LPARAM)file_name);
								else
									delete[] file_name;
							}
						}

						return 0;
					},
					nullptr, 0, &tid);
				MY_TRACE_INT(tid);
			}

			return TRUE;
		}

		//
		// 後始末を実行します。
		// 内部的に使用されます。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {/hex}, {/hex}", wParam, lParam);

					break;
				}
			case WM_NCDESTROY:
				{
					MY_TRACE_FUNC("WM_NCDESTROY, {/hex}, {/hex}", wParam, lParam);

					MY_TRACE_HWND(hwnd);

					break;
				}
			case WM_SIZE:
				{
					MY_TRACE_FUNC("WM_SIZE, {/hex}, {/hex}", wParam, lParam);

					// クライアント矩形を取得します。
					auto rc = my::get_client_rect(*this);

					// リストボックスをリサイズします。
					::SetWindowPos(listbox, nullptr,
						rc.left, rc.top, my::get_width(rc), my::get_height(rc), SWP_NOZORDER);

					break;
				}
			case c_message.c_update_config_file:
				{
					MY_TRACE_FUNC("c_message.c_update_config_file, {/hex}, {/hex}", wParam, lParam);

					auto file_name = (LPCWSTR)lParam;
					addin_manager.fire_update_config_file(file_name);
					delete[] file_name;

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		BOOL func_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
		{
//			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/hex}", hwnd, message, wParam, lParam);

			switch (message)
			{
			case AviUtl::FilterPlugin::WindowMessage::Command:
				{
					MY_TRACE_FUNC("Command, {/hex}, {/hex}", wParam, lParam);

					if (wParam == 0 && lParam == 0) return TRUE; // AviUtlの再描画のみ行います。

					if (wParam >= magi.c_command_id.c_addin.c_begin && wParam < magi.c_command_id.c_addin.c_end)
					{
						// コマンドからインデックスを取得します。
						auto index = wParam - magi.c_command_id.c_addin.c_begin;
						MY_TRACE_INT(index);

						// コマンドのインデックスからアドインを取得します。
						auto addin = (Addin*)::SendMessageW(listbox, LB_GETITEMDATA, index, 0);
						MY_TRACE_HEX(addin);

						// アドインを取得できなかった場合は何もしません。
						if (!addin) break;

						// アドインにコマンドを送ります。
						return addin->on_window_command(hwnd, message, magi.c_command_id.c_addin.c_command, lParam, editp, fp);
					}

					return addin_manager.fire_window_command(hwnd, message, wParam, lParam, editp, fp);
				}
			case WM_COMMAND:
				{
					auto code = HIWORD(wParam);
					auto control = (HWND)lParam;

					switch (code)
					{
					case LBN_DBLCLK: // リストボックスでダブルクリックが発生しました。
						{
							// 選択アイテムのインデックスを取得します。
							auto index = (int)::SendMessageW(listbox, LB_GETCURSEL, 0, 0);
							MY_TRACE_INT(index);

							// 選択アイテムのインデックスからアドインを取得します。
							auto addin = (Addin*)::SendMessageW(listbox, LB_GETITEMDATA, index, 0);
							MY_TRACE_HEX(addin);

							// アドインを取得できなかった場合は何もしません。
							if (!addin) break;

							// アドインにコマンドを送ります。
							return addin->on_window_command(hwnd, message, magi.c_command_id.c_addin.c_command, lParam, editp, fp);
						}
					}

					break;
				}
			}

			return FALSE;
		}
	} plugin_window;
}
