#pragma once

namespace apn::item_list
{
	//
	// このクラスはインプレースエディットボックスです。
	//
	inline struct inplace_edit_t : my::Window
	{
		//
		// 編集先のリストビューです。
		//
		HWND listview = {};

		//
		// このクラスはインプレース編集用の変数を保持します。
		//
		struct state_t {
			int item = -1;
			int sub_item = -1;
			std::shared_ptr<list_t::node_t> node;
		} state;

		//
		// インプレースエディットボックスを初期化します。
		//
		BOOL init(HWND listview, HWND hwnd)
		{
			MY_TRACE_FUNC("");

			// 編集先のリストビューをセットします。
			this->listview = listview;

			// インプレースエディットボックスをサブクラス化します。
			if (!__super::subclass(hwnd)) return FALSE;

			// インプレースエディットボックスのウィンドウスタイルを変更します。
			my::modify_style(*this, 0, WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

			return TRUE;
		}

		//
		// インプレース編集中の場合はTRUEを返します。
		//
		BOOL is_inplace_editing()
		{
			// インプレースエディットボックスが
			// 表示されている場合はTRUEを返します。
			return ::IsWindowVisible(*this);
		}

		//
		// インプレース編集の状態をリセットします。
		//
		BOOL reset_inplace_edit()
		{
			// インプレースエディットボックスを非表示にします。
			::ShowWindow(*this, SW_HIDE);

			// インプレース編集の状態をリセットします。
			state = {};

			return TRUE;
		}

		//
		// インプレース編集を開始します。
		//
		BOOL begin_inplace_edit(int32_t index, int32_t sub_item)
		{
			MY_TRACE_FUNC("{/}, {/}", index, sub_item);

			// 編集不可のサブ項目の場合は何もしません。
			if (!list.k_column.k_datas[sub_item].flag_editable) return FALSE;

			// インプレース編集中の場合は
			if (is_inplace_editing())
			{
				// 既存のインプレース編集を終了します。
				end_inplace_edit();
			}

			// インデックスが無効の場合は何もしません。
			if ((size_t)index >= finder.nodes.size()) return FALSE;

			// インプレース編集の状態を設定します。
			state = { index, sub_item, finder.nodes[index] };

			// サブ項目の矩形を取得します。
			auto rc = RECT {}; rc.top = state.sub_item;
			SendMessage(listview, LVM_GETSUBITEMRECT, (WPARAM)index, (LPARAM)&rc);
			my::map_window_points(listview, *this, &rc);

			// エディットボックスの位置を変更します。
			::SetWindowPos(*this, HWND_TOP,
				rc.left, rc.top, my::get_width(rc), my::get_height(rc), 0);

			// エディットボックスを表示します。
			::ShowWindow(*this, SW_SHOW);

			// エディットボックスにフォーカスを与えます。
			::SetFocus(*this);

			// エディットボックスのテキストを変更します。
			::SetWindowText(*this,
				state.node->get_text(state.sub_item).c_str());

			// エディットボックスのテキストを選択状態にします。
			::SendMessage(*this, EM_SETSEL, 0, -1);

			return TRUE;
		}

		//
		// インプレース編集を終了します。
		//
		BOOL end_inplace_edit()
		{
			MY_TRACE_FUNC("");

			// インプレース編集中ではない場合はFALSEを返します。
			if (!is_inplace_editing()) return FALSE;

			// エディットボックスに入力されているテキストを取得します。
			auto text = my::get_window_text(*this);

			// ノードのテキストを変更します。
			state.node->set_text(state.sub_item, text);

			// サブ項目のテキストを更新します。
			ListView_SetItemText(listview, state.item, state.sub_item, text.data());

			// インプレース編集の状態をリセットします。
			return reset_inplace_edit();
		}

		//
		// インプレース編集を中止します。
		//
		BOOL cancel_inplace_edit()
		{
			MY_TRACE_FUNC("");

			// インプレース編集中ではない場合はFALSEを返します。
			if (!is_inplace_editing()) return FALSE;

			// インプレース編集の状態をリセットします。
			return reset_inplace_edit();
		}

		//
		// インプレースエディットボックスのウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
			switch (message)
			{
			case WM_KILLFOCUS:
				{
					MY_TRACE_FUNC("WM_KILLFOCUS, {/hex}, {/hex}", w_param, l_param);

					// エディットボックスがフォーカスを失ったので
					// インプレース編集を終了します。
					end_inplace_edit();

					break;
				}
			case WM_CHAR:
				{
					MY_TRACE_FUNC("WM_CHAR, {/hex}, {/hex}", w_param, l_param);

					switch (w_param)
					{
					case VK_RETURN:
						{
							// リターンキーが押されたので
							// インプレース編集を終了します。
							end_inplace_edit();

							return 0;
						}
					case VK_ESCAPE:
						{
							// エスケープキーが押されたので
							// インプレース編集をキャンセルします。
							cancel_inplace_edit();

							return 0;
						}
					}

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}
	} inplace_edit;
}
