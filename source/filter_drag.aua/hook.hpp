#pragma once

namespace apn::filter_drag
{
	//
	// このクラスはフック処理を担当します。
	//
	inline struct HookManager
	{
		//
		// 初期化処理です。
		// 拡張編集の関数をフックします。
		// 拡張編集の定数を書き換えます。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			// 拡張編集のモジュールハンドルを取得します。
			auto exedit = magi.exin.get_exedit();

			::SetWindowSubclass(magi.exin.get_setting_dialog(), setting_dialog.subclass_proc, (UINT_PTR)this, 0);
			my::hook::attach(swap_filter, magi.exin.address.function.swap_filter);

			return DetourTransactionCommit() == NO_ERROR;
		}

		//
		// 後始末処理です。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		ObjectHolder src_object; // ドラッグ元のオブジェクトです。
		FilterHolder src_filter; // ドラッグ元のフィルタです。
		FilterHolder dst_filter; // ドラッグ先のフィルタです。
		BOOL is_filter_dragging = FALSE; // ドラッグ中か判定するためのフラグです。
		BOOL enable_move_filter = FALSE; // フィルタの移動処理を有効化します。

		struct CursorPos : POINT {
			CursorPos(HWND hwnd) {
				::GetCursorPos(this);
				::ScreenToClient(hwnd, this);
			}
		};

		//
		// ドラッグの開始処理です。
		//
		void start_drag(const Layout& layout)
		{
			MY_TRACE_FUNC("");

			// マウスをキャプチャーします。
			::SetCapture(layout.get_setting_dialog());

			// フラグを立てます。
			is_filter_dragging = TRUE;

			// ドラッグ元を表示します。
			if (aim_src.is_available())
				aim_src.show(layout, src_filter);

			// サイトを表示します。
			if (sight.is_available())
				sight.move(layout, src_filter, TRUE);
		}

		//
		// ドラッグの終了処理です。
		//
		void end_drag()
		{
			MY_TRACE_FUNC("");

			// フラグをリセットします。
			is_filter_dragging = FALSE;

			if (aim_src.is_available()) aim_src.hide();
			if (aim_dst.is_available()) aim_dst.hide();
			if (sight.is_available()) sight.hide();
		}

		//
		// ドラッグの更新処理です。
		//
		void update_drag(const Layout& layout)
		{
			MY_TRACE_FUNC("");

			if (aim_dst.is_available())
			{
				if (dst_filter != src_filter)
				{
					// ドラッグ先を表示します。
					aim_dst.show(layout, dst_filter);
				}
				else
				{
					// ドラッグ先の隠します。
					aim_dst.hide();
				}
			}

			// ドラッグ元の表示位置を更新します。
			if (aim_src.is_available()) aim_src.show(layout, src_filter);

			// サイトを動かします。
			if (sight.is_available()) sight.move(layout, dst_filter, FALSE);
		}

		LRESULT on_set_cursor(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			if ((HWND)wParam == hwnd && LOWORD(lParam) == HTCLIENT)
			{
				// ドラッグを開始できるかチェックします。
				if (!keyboard_hook.can_start_drag())
					return ::DefSubclassProc(hwnd, message, wParam, lParam);

				// 設定ダイアログのレイアウトを取得します。
				Layout layout(hwnd);

				// マウスカーソルの位置を取得します。
				CursorPos pos(hwnd);
//				MY_TRACE_POINT(pos);

				// オブジェクトを取得します。
				ObjectHolder object(magi.exin.get_current_object_index());
//				MY_TRACE_OBJECT_HOLDER(object);
				if (!object.is_valid()) return ::DefSubclassProc(hwnd, message, wParam, lParam);

				// マウスカーソルの位置に移動できるフィルタがあるかチェックします。
				FilterHolder filter = layout.get_src_filter(pos, object);
//				MY_TRACE_FILTER_HOLDER(filter);
				if (!filter.is_valid()) return ::DefSubclassProc(hwnd, message, wParam, lParam);

				// マウスカーソルを変更します。
				::SetCursor(::LoadCursor(nullptr, IDC_SIZENS));
				return TRUE;
			}

			return ::DefSubclassProc(hwnd, message, wParam, lParam);
		}

		LRESULT on_l_button_down(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			// ドラッグを開始できるかチェックします。
			if (!keyboard_hook.can_start_drag())
				return ::DefSubclassProc(hwnd, message, wParam, lParam);

			// 設定ダイアログのレイアウトを取得します。
			Layout layout(hwnd);

			// マウスカーソルの位置を取得します。
			CursorPos pos(hwnd);
			MY_TRACE_POINT(pos);

			// オブジェクトを取得します。
			src_object = ObjectHolder(magi.exin.get_current_object_index());
			MY_TRACE_OBJECT_HOLDER(src_object);
			if (!src_object.is_valid()) return ::DefSubclassProc(hwnd, message, wParam, lParam);

			// マウスカーソルの位置にあるドラッグ元フィルタを取得します。
			src_filter = layout.get_src_filter(pos, src_object);
			MY_TRACE_FILTER_HOLDER(src_filter);
			dst_filter = src_filter;
			if (!src_filter.is_valid()) return ::DefSubclassProc(hwnd, message, wParam, lParam);

			MY_TRACE("フィルタのドラッグを開始します\n");

			// ドラッグを開始します。
			start_drag(hwnd);

			return ::DefSubclassProc(hwnd, message, wParam, lParam);
		}

		LRESULT on_l_button_up(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			// ドラッグを終了する前にフィルタをドラッグ中だったかチェックします。
			auto is_dragging = ::GetCapture() == hwnd && is_filter_dragging;

			// ドラッグを終了します。
			end_drag();

			// ドラッグ中の場合はフィルタの移動を行います。
			if (is_dragging)
			{
				::ReleaseCapture(); // ここでWM_CAPTURECHANGEDが呼ばれます。

				MY_TRACE_FILTER_HOLDER(src_filter);
				MY_TRACE_FILTER_HOLDER(dst_filter);

				// オブジェクトを取得します。
				ObjectHolder object(magi.exin.get_current_object_index());
				MY_TRACE_OBJECT_HOLDER(object);
				if (!object.is_valid() || object != src_object)
				{
					MY_TRACE("ドラッグ開始時のオブジェクトではないのでフィルタの移動を中止します\n");
					return ::DefSubclassProc(hwnd, message, wParam, lParam);
				}

				// フィルタを取得します。
				FilterHolder filter(src_object, src_filter.get_filter_index());
				MY_TRACE_FILTER_HOLDER(filter);
				if (!filter.is_valid() || filter != src_filter)
				{
					MY_TRACE("ドラッグ開始時のフィルタではないのでフィルタの移動を中止します\n");
					return ::DefSubclassProc(hwnd, message, wParam, lParam);
				}

				auto src_filter_index = src_filter.get_filter_index();
				MY_TRACE_INT(src_filter_index);

				auto dst_filter_index = dst_filter.get_filter_index();
				MY_TRACE_INT(dst_filter_index);

				// フィルタのインデックスの差分を取得します。
				auto sub = dst_filter_index - src_filter_index;
				MY_TRACE_INT(sub);

				if (sub != 0)
				{
					MY_TRACE("ドラッグ先にフィルタを移動します\n");

					hook_manager.enable_move_filter = TRUE;
					::SendMessage(hwnd, WM_COMMAND, sub < 0 ? 1116 : 1117, 0);
					hook_manager.enable_move_filter = FALSE;
				}
			}

			return ::DefSubclassProc(hwnd, message, wParam, lParam);
		}

		LRESULT on_mouse_move(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			// フィルタをドラッグ中かチェックします。
			if (::GetCapture() == hwnd && is_filter_dragging)
			{
				// オブジェクトを取得します。
				ObjectHolder object(magi.exin.get_current_object_index());
//				MY_TRACE_OBJECT_HOLDER(object);
				if (!object.is_valid() || object != src_object)
				{
					MY_TRACE("ドラッグ開始時のオブジェクトではないのでドラッグを中止します\n");
					::ReleaseCapture(); end_drag();
					return ::DefSubclassProc(hwnd, message, wParam, lParam);
				}

				// フィルタを取得します。
				FilterHolder filter(src_object, src_filter.get_filter_index());
//				MY_TRACE_FILTER_HOLDER(filter);
				if (!filter.is_valid() || filter != src_filter)
				{
					MY_TRACE("ドラッグ開始時のフィルタではないのでドラッグを中止します\n");
					::ReleaseCapture(); end_drag();
					return ::DefSubclassProc(hwnd, message, wParam, lParam);
				}

				// 設定ダイアログのレイアウトを取得します。
				Layout layout(hwnd);

				// マウスカーソルの位置を取得します。
				CursorPos pos(hwnd);
//				MY_TRACE_POINT(pos);

				FilterHolder old_dst_filter = dst_filter;

				// マウスカーソルの位置にあるドラッグ元フィルタを取得します。
				dst_filter = layout.get_dst_filter(pos, src_object);
				if (!dst_filter.is_valid()) dst_filter = src_filter;
//				MY_TRACE_FILTER_HOLDER(dst_filter);

				if (dst_filter != old_dst_filter)
				{
					// ドラッグを更新します。
					update_drag(layout);
				}
			}

			return ::DefSubclassProc(hwnd, message, wParam, lParam);
		}

		LRESULT on_capture_changed(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			// ドラッグを終了します。
			end_drag();

			return ::DefSubclassProc(hwnd, message, wParam, lParam);
		}

		//
		// 移動元フィルタを移動先フィルタの場所に移動します。
		//
		void move_filter(int32_t object_index)
		{
			auto src_filter_index = hook_manager.src_filter.get_filter_index();
			MY_TRACE_INT(src_filter_index);

			auto dst_filter_index = hook_manager.dst_filter.get_filter_index();
			MY_TRACE_INT(dst_filter_index);

			// フィルタのインデックスの差分を取得します。
			auto sub = dst_filter_index - src_filter_index;
			MY_TRACE_INT(sub);

			if (sub < 0)
			{
				// フィルタを上に移動します。
				for (int32_t i = sub; i < 0; i++)
					swap_filter.orig_proc(object_index, src_filter_index--, -1);
			}
			else
			{
				// フィルタを下に移動します。
				for (int32_t i = sub; i > 0; i--)
					swap_filter.orig_proc(object_index, src_filter_index++, 1);
			}
		}

		//
		// このクラスは設定ダイアログをフックします。
		//
		struct {
			inline static LRESULT WINAPI subclass_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR subclass_id, DWORD_PTR ref_data)
			{
//				MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/hex}", hwnd, message, wParam, lParam);

				switch (message)
				{
				case WM_SETCURSOR: return hook_manager.on_set_cursor(hwnd, message, wParam, lParam);
				case WM_LBUTTONDOWN: return hook_manager.on_l_button_down(hwnd, message, wParam, lParam);
				case WM_LBUTTONUP: return hook_manager.on_l_button_up(hwnd, message, wParam, lParam);
				case WM_MOUSEMOVE: return hook_manager.on_mouse_move(hwnd, message, wParam, lParam);
				case WM_CAPTURECHANGED: return hook_manager.on_capture_changed(hwnd, message, wParam, lParam);
				}

				return ::DefSubclassProc(hwnd, message, wParam, lParam);
			}
		} setting_dialog;

		//
		// このクラスは拡張編集の内部関数swap_filter()をフックします。
		//
		struct {
			inline static void CDECL hook_proc(int32_t object_index, int32_t filter_index, int32_t relative_index)
			{
				if (hook_manager.enable_move_filter)
					hook_manager.move_filter(object_index);
				else
					orig_proc(object_index, filter_index, relative_index);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} swap_filter;
	} hook_manager;
}
