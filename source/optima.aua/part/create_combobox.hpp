#pragma once

namespace apn::optima
{
	//
	// このクラスはフィルタのコンボボックスのウィンドウプロシージャをフックします。
	//
	inline struct CreateComboBox : Part
	{
		//
		// コンストラクタです。
		//
		CreateComboBox()
		{
			MY_TRACE_FUNC("");

			part_manager.add(this);
		}

		//
		// 処理中のオブジェクトです。
		//
		inline static ObjectPtr object = {};

		//
		// 処理中のフィルタです。
		//
		inline static FilterPtr filter = {};

		//
		// 処理中のアイテムのインデックスです。
		//
		inline static int32_t item_index = 0;

		//
		// TRUEの場合はデフォルト処理をスキップします。
		//
		inline static BOOL skip_default = FALSE;

		//
		// オリジナルのウィンドウプロシージャです。
		//
		inline static WNDPROC orig_combobox_wnd_proc = nullptr;

		//
		// オリジナルのウィンドウプロシージャを実行します。
		//
		inline static LRESULT WINAPI call_combobox_wnd_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
		{
			return ::CallWindowProcA(orig_combobox_wnd_proc, hwnd, message, wparam, lparam);
		}

		//
		// ウィンドウプロシージャです。
		//
		inline static LRESULT CALLBACK combobox_wnd_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
		{
			switch (message)
			{
			case CB_RESETCONTENT:
				{
					MY_TRACE("{:#010x}, CB_RESETCONTENT\n", (uint32_t)hwnd);

					dump_filter_controls();

					// まず、変数をリセットします。
					object = {};
					filter = {};
					item_index = 0;
					skip_default = FALSE;

					if (hive.reuse_combobox)
					{
						// カレントオブジェクトを取得します。
						if (!object.init(magi.exin.get_current_object_index()))
							return call_combobox_wnd_proc(hwnd, message, wparam, lparam);

						// アニメーション効果を取得します。
						filter = object.get_animation_effect(hwnd);

						// アニメーション効果の場合は
						if (filter.get())
						{
							MY_TRACE("{:#010x} => アニメーション効果\n", (uint32_t)hwnd);

							// デフォルト処理をスキップして
							// コンボボックスのコンテンツを維持します。
							return CB_OKAY;
						}
						// アニメーション効果以外の場合は
						else
						{
							MY_TRACE("{:#010x} => アニメーション効果以外\n", (uint32_t)hwnd);

							// コンボボックスのカテゴリをリセットします。
							set_category_id(hwnd, 0);

							// デフォルト処理を実行します。
							return call_combobox_wnd_proc(hwnd, message, wparam, lparam);
						}
					}
					else
					{
						// デフォルト処理を実行します。
						return call_combobox_wnd_proc(hwnd, message, wparam, lparam);
					}
				}
			case CB_INSERTSTRING:
				{
					// アニメーション効果の場合は
					if (filter.get())
					{
						// 処理中のアイテムのインデックスを取得します。
						auto result = item_index++;

						// フラグが立っている場合はデフォルト処理をスキップします。
						if (skip_default) return result;

						MY_TRACE("{:#010x}, CB_INSERTSTRING, {}, {:#010x}, {}\n", (uint32_t)hwnd, result, lparam, (LPCSTR)lparam);

						// 先頭アイテムの場合は
						if (result == 0)
						{
							// フィルタのカテゴリを取得します。
							auto filter_category_id = animation_effect_manager.get_category_id(object.get(), filter.index());
							MY_TRACE("フィルタカテゴリ => {}\n", filter_category_id);

							// 選択アイテム(設定ダイアログ全体)ではなく、
							// 例えばコンボボックスの選択だけが変更された場合は
							if (hive.lock_count == 0)
							{
								auto combobox_index = find_combobox_index(hwnd);
								MY_TRACE("コンボボックスインデックス => {}\n", combobox_index);

								auto category_combobox = get_combobox(combobox_index + 1);
								MY_TRACE("カテゴリコンボボック => {:#010x}\n", (uint32_t)category_combobox);

								auto category_index = cb::get_cur_sel(category_combobox);
								MY_TRACE("カテゴリインデックス => {}\n", category_index);
#ifdef _DEBUG
								auto category_text = cb::get_text(category_combobox, -1);
								MY_TRACE("カテゴリ文字列 => {}\n", category_text);

								auto name = animation_effect_manager.get_name(object.get(), filter.index());
								MY_TRACE("アニメーション効果名 => {}\n", name);
#endif
								// カテゴリコンボボックスからフィルタカテゴリを取得します。
								filter_category_id = category_index + 1;
								MY_TRACE("フィルタカテゴリ => {}\n", filter_category_id);
							}

							// フィルタのカテゴリが設定されていて、
							// なおかつ最初のアイテム文字列がアニメーション効果名バッファの場合は
							if (filter_category_id > 2 && (LPCSTR)lparam == animation_effect_manager.table)
							{
								MY_TRACE("あとで再構築されるのでデフォルト処理をスキップします\n");

								// このコンボボックスをあとで再構築されるので
								// 今回はデフォルト処理をスキップするようにします。
								skip_default = TRUE;
								return result;
							}

							// コンボボックスのカテゴリを取得します。
							auto cached_category_id = get_category_id(hwnd);
							MY_TRACE("キャッシュカテゴリ => {}\n", cached_category_id);

							// キャッシュされたカテゴリが存在する場合は
							if (cached_category_id)
							{
								// カテゴリが同一の場合は
								if (filter_category_id == cached_category_id)
								{
									MY_TRACE("キャッシュが存在するのでデフォルト処理をスキップします\n");

									// コンボボックスを再構築する必要がないので
									// デフォルト処理をスキップするようにします。
									skip_default = TRUE;
									return result;
								}
							}

							// スキップしていたコンテンツリセット処理をここで実行します。
							call_combobox_wnd_proc(hwnd, CB_RESETCONTENT, 0, 0);

							// コンボボックスにカテゴリを関連付けます。
							set_category_id(hwnd, filter_category_id);
						}
					}

					return call_combobox_wnd_proc(hwnd, message, wparam, lparam);
				}
			}

			return call_combobox_wnd_proc(hwnd, message, wparam, lparam);
		}

		//
		// この関数は::CreateWindowExA()の呼び出しと置き換えられるフック関数です。
		//
		inline static HWND WINAPI hook_proc(
			decltype(&::CreateWindowExA) orig_proc,
			LPCSTR window_name, DWORD style, int x, int y, int w, int h,
			HWND parent, HMENU menu, HINSTANCE instance, LPVOID param)
		{
			MY_TRACE_FUNC("");

			auto hwnd = orig_proc(0, WC_COMBOBOXA, window_name, style, x, y, w, h, parent, menu, instance, param);
			orig_combobox_wnd_proc = (WNDPROC)::SetWindowLongPtrA(hwnd, GWLP_WNDPROC, (LONG_PTR)combobox_wnd_proc);
			return hwnd;
		}

		//
		// 初期化処理を実行します。
		// 拡張編集内のフィルタのコンボボックスを作成する部分のコードを書き換えます。
		//
		virtual BOOL func_init(AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");
/*
90            NOP
90            NOP
90            NOP
53            PUSH EBX
E8 func       CALL func
合計 9

$+2F1A9   |.  68 48480A10    |PUSH OFFSET 100A4848                    ; ASCII "COMBOBOX"
$+2F1AE   |.  6A 00          |PUSH 0
$+2F1B0   |.  FFD3           |CALL EBX
合計 9
*/
#pragma pack(push, 1)
			struct Code {
				uint8_t nop_1 = 0x90;
				uint8_t nop_2 = 0x90;
				uint8_t nop_3 = 0x90;
				uint8_t push_ebx = 0x53;
				uint8_t call = 0xe8;
				uint32_t hook_proc;
			} code;
			static_assert(sizeof(Code) == 9);
#pragma pack(pop)

			uint32_t address = magi.exin.exedit + 0x0002F1A9;
			code.hook_proc = (uint32_t)hook_proc - (address + sizeof(code));
			::WriteProcessMemory(::GetCurrentProcess(),
				(void*)address, &code, sizeof(code), nullptr);

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL func_exit(AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}
	} create_combobox;
}
