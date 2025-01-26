#pragma once

namespace apn::filer
{
	//
	// このクラスはファイラウィンドウです。
	// クライアントプロセスのファイラダイアログを保持します。
	//
	struct FilerWindow : my::aviutl::PluginWindow
	{
		//
		// ファイラウィンドウのコレクションです。
		//
		inline static std::vector<std::shared_ptr<FilerWindow>> collection;

		//
		// 指定された名前のファイラウィンドウを作成して返します。
		//
		inline static std::shared_ptr<FilerWindow> create_instance(LPCTSTR name, BOOL full)
		{
			MY_TRACE_FUNC("{}, {}", name, full);

			// ファイラウィンドウのインスタンスを作成します。
			auto filer_window = std::make_shared<FilerWindow>();

			// ファイラウィンドウの表示位置を算出します。
			auto rc = my::get_window_rect(magi.exin.get_aviutl_window());
			::InflateRect(&rc, -my::get_width(rc) / 5, -my::get_height(rc) / 5);

			// ファイラウィンドウを作成します。
			if (!filer_window->create_as_plugin(
				hive.instance, magi.exin.get_aviutl_window(), name,
				WS_EX_NOPARENTNOTIFY | WS_EX_TOOLWINDOW,
				WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				rc.left, rc.top, my::get_width(rc), my::get_height(rc)))
			{
				hive.message_box(L"{}(ファイラウィンドウ)の作成に失敗しました");

				return nullptr;
			}

			// ファイラウィンドウにカテゴリ名を設定します。
			::SetProp(*filer_window, L"aviutl.plugin.category_name", (HANDLE)hive.c_display_name);

			// クライアントプロセスにファイラウィンドウが作成されたことを通知します。
			hive.post_message(share::message::c_init_filer_window, (HWND)*filer_window, full);

			// ファイラウィンドウをコレクションに追加します。
			return collection.emplace_back(filer_window);
		}

		//
		// カレントオブジェクトに関連するファイルパスを取得します。
		//
		std::wstring get_item_file_name()
		{
			// カレントオブジェクトのインデックスを取得します。
			auto object_index = magi.exin.get_current_object_index();
			if (object_index < 0) return {};

			// カレントオブジェクトを取得します。
			auto object = magi.exin.get_object(object_index);
			if (!object) return {};

			// 先頭のフィルタを取得します。
			auto filter_index = 0;
			auto filter = magi.exin.get_filter(object, filter_index);
			if (!filter) return {};

			// 拡張データへのオフセットとサイズを取得します。
			auto offset = -1, size = -1;
			auto id = object->filter_param[filter_index].id;
			switch (id)
			{
			case 0: offset = 0; size = 260; break; // 動画ファイル
			case 1: offset = 4; size = 256; break; // 画像ファイル
			case 2: offset = 0; size = 260; break; // 音声ファイル
			case 6: offset = 0; size = 260; break; // 音声波形表示
			case 82: offset = 0; size = 260; break; // 動画ファイル合成
			case 83: offset = 4; size = 256; break; // 画像ファイル合成
			}
			if (size <= 0) return {}; // サイズを取得できなかった場合は何もしません。

			// 拡張データからファイル名を取得します。
			auto exdata = magi.exin.get_exdata(object, filter_index);
			auto file_name = (LPCSTR)(exdata + offset);

			// フォルダ名を取得して返します。
			return std::filesystem::path(file_name).parent_path();
		}

		//
		// ウィンドウ名を更新します。
		//
		void update_window_name(LPCTSTR name)
		{
			::SetWindowText(*this, name);
			::SetWindowText(get_target(), name);
		}

		//
		// ウィンドウ名を更新します。
		//
		void update_window_name()
		{
			auto name = my::get_window_text(*this);
			::SetWindowText(get_target(), name.c_str());
		}

		//
		// 設定を読み込みます。
		//
		void load(const n_json& node)
		{
			MY_TRACE_FUNC("");

			workspace::share::read_window_pos(node, "filer_window", *this);
		}

		//
		// 設定を保存します。
		//
		void save(n_json& node)
		{
			MY_TRACE_FUNC("");

			workspace::share::write_window_pos(node, "filer_window", *this);
		}

		//
		// ウィンドウ破壊時の最終処理を行います。
		//
		virtual void post_nc_destroy() override
		{
			MY_TRACE_FUNC("");

			std::shared_ptr<FilerWindow> keeper;

			// コレクション内のファイラウィンドウを検索します。
			auto it = std::find_if(collection.begin(), collection.end(),
				[this](const auto& x){ return x.get() == this; });
			if (it != collection.end())
			{
				// スマートポインタを取得しておきます。
				// これにより、このスコープ内でthisポインタが削除されないようになります。
				keeper = *it;

				// コレクションから取り除きます。
				collection.erase(it);

				// フィルタウィンドウが削除されたのでリフレッシュします。
				hive.app->refresh();
			}

			// ここでHWNDとの関連付けが解除されます。
			__super::post_nc_destroy();
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
					MY_TRACE_FUNC("WM_DESTROY, {:#010x}, {:#010x}", wParam, lParam);

					// ファイラダイアログの親ウィンドウを変更します。
					// これにより、ファイラウィンドウが終了したときに
					// ファイラダイアログまで終了してしまうことを防ぐことができます。
					auto dialog = get_target();
					::ShowWindow(dialog, SW_HIDE);
					::SetParent(dialog, 0);
					my::modify_style(dialog, WS_CHILD, WS_POPUP | WS_CAPTION | WS_THICKFRAME);

					// ファイラウィンドウの後始末が完了したことをファイラダイアログに通知します。
					// これにより、クライアントプロセス側でファイラダイアログを終了することができます。
					::PostMessage(dialog, share::message::c_exit_filer_window, 0, 0);

					break;
				}
			case WM_ACTIVATE:
				{
					MY_TRACE_FUNC("WM_ACTIVATE, {:#010x}, {:#010x}", wParam, lParam);

					if (wParam)
						hive.active_window = hwnd;

					break;
				}
			case WM_SETFOCUS:
				{
					MY_TRACE_FUNC("WM_SETFOCUS, {:#010x}, {:#010x}", wParam, lParam);

					hive.active_window = hwnd;

					break;
				}
			case share::message::c_init: // このメッセージはファイラダイアログの作成が完了したときに通知されます。
				{
					MY_TRACE_FUNC("c_init, {:#010x}, {:#010x}", wParam, lParam);

					auto dialog = (HWND)wParam;
					auto full = (BOOL)lParam;

					// ファイラダイアログを子ウィンドウスタイルにしてから子ウィンドウに変更します。
					my::modify_style(dialog, WS_POPUP | WS_CAPTION | WS_THICKFRAME, WS_CHILD);
					::SetParent(dialog, hwnd);
					::SetWindowPos(dialog, 0, 0, 0, 0, 0,
						SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

					// ファイラダイアログの作成が完了したので、ターゲットに指定し、リサイズします。
					set_target(dialog);
					resize();

					// ターゲットのウィンドウ名を更新します。
					update_window_name();

					break;
				}
			case share::message::c_get_file_name: // このメッセージはファイラダイアログで「取得」ボタンが押されたときに通知されます。
				{
					MY_TRACE_FUNC("c_get_file_name, {:#010x}, {:#010x}", wParam, lParam);

					// カレントオブジェクトに関連するファイル名を取得します。
					auto file_name = get_item_file_name();
					if (!file_name.empty())
					{
						// ファイル名を取得できた場合はダイアログに送信します。
						auto dialog = (HWND)wParam;
						auto folder_control = (HWND)lParam;
						::SendMessage(folder_control, WM_SETTEXT, 0, (LPARAM)file_name.c_str());
						::PostMessage(dialog, WM_COMMAND, IDOK, 0);
					}

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	};

	inline struct FilerWindowManager
	{
		//
		// ファイラウィンドウのコレクションです。
		//
		decltype(FilerWindow::collection)& collection;

		//
		// コンストラクタです。
		//
		FilerWindowManager()
			: collection(FilerWindow::collection)
		{
		}

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			FilerWindow::collection.clear();

			return TRUE;
		}

		//
		// 指定された名前のファイラウィンドウを作成して返します。
		//
		auto create_filer_window(LPCTSTR name, BOOL full)
		{
			return FilerWindow::create_instance(name, full);
		}

		//
		// ファイラウィンドウのマップを返します。
		//
		auto get_filers()
		{
			std::map<std::wstring, std::shared_ptr<FilerWindow>> filers;
			for (auto& filer_window : collection)
				filers[my::get_window_text(*filer_window)] = filer_window;
			return filers;
		}
	} filer_window_manager;
}
