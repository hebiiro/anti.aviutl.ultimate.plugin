#pragma once

namespace apn::filter_copy
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
			my::hook::attach(add_alias, magi.exin.address.function.add_alias);
			my::hook::attach(unknown1, magi.exin.address.function.unknown1);

			// 設定ダイアログのコンテキストメニューを拡張します。
			for (auto i = 0; i < magi.exin.get_setting_dialog_menu_count(); i++)
			{
				auto menu = magi.exin.get_setting_dialog_menu(i);
				auto sub_menu = ::GetSubMenu(menu, 0);

				if (i == 2)
				{
					::AppendMenu(sub_menu, MF_SEPARATOR, 0, nullptr);
					::AppendMenu(sub_menu, MF_STRING, magi.c_command_id.c_setting_dialog.c_create_clone, _T("完全な複製を下に作成"));
					::AppendMenu(sub_menu, MF_STRING, magi.c_command_id.c_setting_dialog.c_create_same_above, _T("同じフィルタ効果を上に作成"));
					::AppendMenu(sub_menu, MF_STRING, magi.c_command_id.c_setting_dialog.c_create_same_below, _T("同じフィルタ効果を下に作成"));
				}

				::AppendMenu(sub_menu, MF_SEPARATOR, 0, nullptr);
				::AppendMenu(sub_menu, MF_STRING, magi.c_command_id.c_setting_dialog.c_cut_filter, _T("このフィルタを切り取り"));
				::AppendMenu(sub_menu, MF_STRING, magi.c_command_id.c_setting_dialog.c_cut_filter_above, _T("このフィルタ以上を切り取り"));
				::AppendMenu(sub_menu, MF_STRING, magi.c_command_id.c_setting_dialog.c_cut_filter_below, _T("このフィルタ以下を切り取り"));
				::AppendMenu(sub_menu, MF_STRING, magi.c_command_id.c_setting_dialog.c_copy_filter, _T("このフィルタをコピー"));
				::AppendMenu(sub_menu, MF_STRING, magi.c_command_id.c_setting_dialog.c_copy_filter_above, _T("このフィルタ以上をコピー"));
				::AppendMenu(sub_menu, MF_STRING, magi.c_command_id.c_setting_dialog.c_copy_filter_below, _T("このフィルタ以下をコピー"));
				::AppendMenu(sub_menu, MF_STRING, magi.c_command_id.c_setting_dialog.c_paste_filter, _T("フィルタを貼り付け"));
			}

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

		//
		// このクラスは前処理です。
		// 同じ処理を何度か繰り返すので、事前に変数をキャッシュして準備しておきます。
		//
		struct Prep {
			int32_t object_index = -1;
			ExEdit::Object* object = nullptr;
			std::string temp_folder_path;
			DWORD pid = 0;

			BOOL init()
			{
				// カレントオブジェクトのインデックスを取得します。
				object_index = magi.exin.get_current_object_index();
				MY_TRACE_INT(object_index);
				if (object_index < 0) return FALSE;

				// オブジェクトを取得すします。
				object = magi.exin.get_object(object_index);
				MY_TRACE_HEX(object);
				if (!object) return FALSE;

				// テンポラリフォルダのパスを取得します。
				temp_folder_path = my::hs(my::get_temp_path());
				MY_TRACE_STR(temp_folder_path);

				// カレントプロセスのIDを取得します。
				pid = ::GetCurrentProcessId();
				MY_TRACE_INT(pid);

				return TRUE;
			}
		};

		//
		// このクラスはオーダーです。
		//
		struct Order {
			struct Filter {
				int32_t index; // フィルタのインデックス。
				std::string file_name; // エイリアスのファイル名。
			};
			ExEdit::Object::Flag flag; // オブジェクトのフラグ。
			std::vector<Filter> filters; // オーダーされたフィルタのコレクション。

			//
			// オーダーを初期化します。
			//
			BOOL init(const Prep& prep)
			{
				MY_TRACE_FUNC("");

				flag = prep.object->flag;
				filters.clear();
				filters.reserve(ExEdit::Object::MAX_FILTER);

				return TRUE;
			}

			//
			// 指定されたフィルタのコピーをオーダーします。
			// 指定されたフィルタのエイリアスを一時ファイルに保存し、
			// ファイル名をコレクションに追加します。
			//
			BOOL add(const Prep& prep, int32_t filter_index)
			{
				MY_TRACE_FUNC("{}", filter_index);

				if (filter_index == 0)
					return FALSE; // 先頭のフィルタはコピーしません。

				if (!magi.exin.is_moveable(prep.object, filter_index))
					return FALSE; // 移動不可能なフィルタはコピーしません。

				// フィルタを取得します。
				auto filter = magi.exin.get_filter(prep.object, filter_index);
				if (!filter) return FALSE;

				// 一時ファイルのファイル名を構築します。
				auto temp_file_name = std::format(
					"{}\\aviutl_filter_copy_{}_{}.exa",
					prep.temp_folder_path, prep.pid, filter_index);
				MY_TRACE_STR(temp_file_name);

				// 一時ファイルにフィルタのエイリアスを保存します。
				if (!magi.exin.save_filter_alias(prep.object_index, filter_index, temp_file_name.c_str()))
				{
					MY_TRACE("magi.exin.save_filter_alias()が失敗しました\n");

					return FALSE;
				}

				// フィルタをコレクションに追加します。
				filters.emplace_back(filter_index, temp_file_name);

				return TRUE;
			}
		} order;

		//
		// オブジェクトのタイプを返します。
		//
		inline static int32_t get_type(uint32_t flag)
		{
			if (flag & 0x00020000) {
				if (flag & 0x00050000)
					return 1; // 音声メディアオブジェクト
				else
					return 2; // 音声フィルタオブジェクト
			} else {
				if (flag & 0x00050000)
					return 3; // 映像メディアオブジェクト＆グループ制御
				else {
					if (flag & 0x00080000)
						return 4; // カメラ制御＆時間制御
					else
						return 5; // 映像フィルタオブジェクト
				}
			}
		}

		//
		// 指定されたフィルタのコピーを発注します。
		//
		BOOL copy_filter(int32_t filter_index, int32_t flag, BOOL cut)
		{
			// 前準備を行います。
			Prep prep;
			if (!prep.init()) return FALSE;
			if (!order.init(prep)) return FALSE;

			// 戻り値です。
			BOOL ret_value = FALSE;

			switch (flag)
			{
			case 0:
				{
					// 指定されたフィルタのコピーを発注します。
					ret_value |= order.add(prep, filter_index);

					break;
				}
			case -1:
				{
					// 指定されたフィルタより上にあるフィルタのコピーを発注します。
					for (int32_t i = 0; i <= filter_index; i++)
						ret_value |= order.add(prep, i);

					break;
				}
			case 1:
				{
					// 指定されたフィルタより下にあるフィルタのコピーを発注します。
					for (int32_t i = filter_index; i < ExEdit::Object::MAX_FILTER; i++)
						ret_value |= order.add(prep, i);

					break;
				}
			}

			if (cut) // カットする場合は、ここでフィルタを削除します。
			{
				// カレントオブジェクトのインデックスを取得します。
				auto object_index = magi.exin.get_current_object_index();
				MY_TRACE_INT(object_index);
				if (object_index < 0) return FALSE;

				// オブジェクトを取得します。
				auto object = magi.exin.get_object(object_index);
				MY_TRACE_HEX(object);
				if (!object) return FALSE;

				// 中間点がある場合は先頭オブジェクトのインデックスを使用します。
				if (object->index_midpt_leader != -1)
					object_index = object->index_midpt_leader;

				magi.exin.push_undo();
				magi.exin.create_undo(object_index, 1);
				for (int32_t i = order.filters.size() - 1; i >= 0; i--)
					magi.exin.erase_filter(object_index, order.filters[i].index);
				magi.exin.redraw_setting_dialog(object_index);
				magi.exin.hide_controls();
				magi.exin.show_controls(magi.exin.get_current_object_index());

				// これを実行しないとプレビューが更新されません。
				magi.redraw();
			}

			return ret_value;
		}

		//
		// 独自のエイリアス読み込み処理を行うためのフラグです。
		// magi.exin.add_alias()のフックから参照されます。
		//
		BOOL flag_paste_filter = FALSE;

		//
		// カレントオブジェクトにフィルタを貼り付けます。
		//
		BOOL paste_filter()
		{
			MY_TRACE_FUNC("");

			// カレントオブジェクトのインデックスを取得します。
			auto object_index = magi.exin.get_current_object_index();
			MY_TRACE_INT(object_index);
			if (object_index < 0) return FALSE;

			// オブジェクトを取得します。
			auto object = magi.exin.get_object(object_index);
			MY_TRACE_HEX(object);
			if (!object) return FALSE;

			if (object->filter_param[0].id == 93) // 93 == 時間制御
				return FALSE; // 「時間制御」には貼り付けられるフィルタがないので何もしません。

			// オブジェクトのタイプを取得します。
			auto src_type = get_type((uint32_t)order.flag);
			auto dst_type = get_type((uint32_t)object->flag);
			MY_TRACE_INT(src_type);
			MY_TRACE_INT(dst_type);

			if (src_type != dst_type)
				return FALSE; // オブジェクトのタイプが異なる場合は何もしません。

			flag_paste_filter = TRUE;
			// この中でmagi.exin.add_alias()が呼ばれるので、フラグを立ててフックします。
			// 1036はエイリアスを追加するコマンド、0はエイリアスのインデックスです。
			::SendMessage(magi.exin.get_exedit_window(), WM_COMMAND, 1036, 0);
			flag_paste_filter = FALSE;

			return TRUE;
		}

		//
		// カレントフィルタの複製方法です。
		//
		uint32_t create_filter_command_id = 0;

		//
		// カレントフィルタの複製を作成します。
		//
		void create_filter(int32_t orig_object_index, int32_t new_filter_index)
		{
			MY_TRACE_FUNC("{}, {}", orig_object_index, new_filter_index);

			auto object_index = orig_object_index;
			MY_TRACE_INT(object_index);

			auto midpt_leader = magi.exin.get_object(object_index)->index_midpt_leader;
			MY_TRACE_INT(midpt_leader);
			if (midpt_leader >= 0)
				object_index = midpt_leader; // 中間点がある場合は中間点元のオブジェクトインデックスを取得します。

			while (object_index >= 0)
			{
				// オブジェクトインデックスを取得します。
				MY_TRACE_INT(object_index);
				if (object_index < 0) break;

				// オブジェクトを取得します。
				auto object = magi.exin.get_object(object_index);
				MY_TRACE_HEX(object);
				if (!object) break;

				auto midpt_leader2 = object->index_midpt_leader;
				MY_TRACE_INT(midpt_leader2);
				if (midpt_leader2 != midpt_leader) break;

				// コピー元フィルタのインデックスを取得します。
				auto src_filter_index = magi.exin.get_current_filter_index();
				MY_TRACE_INT(src_filter_index);
				if (src_filter_index < 0) break;

				// コピー先フィルタのインデックスを取得します。
				auto dst_filter_index = new_filter_index;
				MY_TRACE_INT(dst_filter_index);
				if (dst_filter_index < 0) break;

				// コピー元フィルタを取得します。
				auto src_filter = magi.exin.get_filter(object, src_filter_index);
				MY_TRACE_HEX(src_filter);
				if (!src_filter) break;

				// コピー先フィルタを取得します。
				auto dst_filter = magi.exin.get_filter(object, dst_filter_index);
				MY_TRACE_HEX(dst_filter);
				if (!dst_filter) break;

				if (create_filter_command_id == magi.c_command_id.c_setting_dialog.c_create_clone)
				{
					// 拡張データをコピーします。
					auto src_filter_exdata = magi.exin.get_exdata(object, src_filter_index);
					auto dst_filter_exdata = magi.exin.get_exdata(object, dst_filter_index);
					memcpy(dst_filter_exdata, src_filter_exdata, src_filter->exdata_size);

					// トラックデータをコピーします。
					for (auto i = 0; i < src_filter->track_n; i++)
					{
						auto src_track_index = object->filter_param[src_filter_index].track_begin + i;
						auto dst_track_index = object->filter_param[dst_filter_index].track_begin + i;
						object->track_value_left[dst_track_index] = object->track_value_left[src_track_index];
						object->track_value_right[dst_track_index] = object->track_value_right[src_track_index];
						object->track_mode[dst_track_index] = object->track_mode[src_track_index];
						object->track_param[dst_track_index] = object->track_param[src_track_index];
					}

					// チェックデータをコピーします。
					for (auto i = 0; i < src_filter->check_n; i++)
					{
						auto src_check_index = object->filter_param[src_filter_index].check_begin + i;
						auto dst_check_index = object->filter_param[dst_filter_index].check_begin + i;
						object->check_value[dst_check_index] = object->check_value[src_check_index];
					}
				}

				if (midpt_leader < 0) break;

				object_index = magi.exin.get_next_object_index(object_index);
			}

			// コピー元フィルタのインデックスを取得します。
			auto src_filter_index = magi.exin.get_current_filter_index();
			MY_TRACE_INT(src_filter_index);
			if (src_filter_index < 0) return;

			// コピー先フィルタのインデックスを取得します。
			auto dst_filter_index = new_filter_index;
			MY_TRACE_INT(dst_filter_index);
			if (dst_filter_index < 0) return;

			switch (create_filter_command_id)
			{
			case magi.c_command_id.c_setting_dialog.c_create_same_above:
				{
					// コピー元のすぐ上に移動します。
					auto c = dst_filter_index - src_filter_index;
					for (auto i = 0; i < c; i++)
						magi.exin.swap_filter(orig_object_index, dst_filter_index--, -1);

					break;
				}
			case magi.c_command_id.c_setting_dialog.c_create_clone:
			case magi.c_command_id.c_setting_dialog.c_create_same_below:
				{
					// コピー元のすぐ下に移動します。
					auto c = dst_filter_index - src_filter_index - 1;
					for (auto i = 0; i < c; i++)
						magi.exin.swap_filter(orig_object_index, dst_filter_index--, -1);

					break;
				}
			}
		}

		//
		// このクラスは設定ダイアログをフックします。
		//
		struct {
			inline static LRESULT WINAPI subclass_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR subclass_id, DWORD_PTR ref_data)
			{
				switch (message)
				{
				case WM_COMMAND:
					{
						switch (wParam)
						{
						case magi.c_command_id.c_setting_dialog.c_create_clone:
						case magi.c_command_id.c_setting_dialog.c_create_same_above:
						case magi.c_command_id.c_setting_dialog.c_create_same_below:
							{
								// オブジェクトを取得します。
								ObjectHolder object(magi.exin.get_current_object_index());
								MY_TRACE_OBJECT_HOLDER(object);
								if (!object.is_valid()) break;

								// フィルタを取得します。
								FilterHolder filter(object, magi.exin.get_current_filter_index());
								MY_TRACE_FILTER_HOLDER(filter);
								if (!filter.is_valid()) break;
								MY_TRACE_STR(filter.get_filter()->name);
								MY_TRACE_HEX(filter.get_filter()->flag);

								// フィルタが複製できるものかどうかチェックします。
								if (!filter.is_moveable()) break;

								// フィルタIDを取得します。
								auto filter_id = object.get_object()->filter_param[filter.get_filter_index()].id;
								MY_TRACE_HEX(filter_id);
								if (filter_id < 0) break;

								// フィルタを作成するコマンドを発行します。
								hook_manager.create_filter_command_id = wParam;
								auto result = ::DefSubclassProc(hwnd, message, 2000 + filter_id, lParam);
								hook_manager.create_filter_command_id = 0;
								return result;
							}
						case magi.c_command_id.c_setting_dialog.c_cut_filter:
							{
								MY_TRACE_FUNC("magi.c_command_id.c_setting_dialog.c_cut_filter");

								auto filter_index = magi.exin.get_current_filter_index();
								if (filter_index >= 0)
									hook_manager.copy_filter(filter_index, 0, TRUE);
								break;
							}
						case magi.c_command_id.c_setting_dialog.c_cut_filter_above:
							{
								MY_TRACE_FUNC("magi.c_command_id.c_setting_dialog.c_cut_filter_above");

								auto filter_index = magi.exin.get_current_filter_index();
								if (filter_index >= 0)
									hook_manager.copy_filter(filter_index, -1, TRUE);
								break;
							}
						case magi.c_command_id.c_setting_dialog.c_cut_filter_below:
							{
								MY_TRACE_FUNC("magi.c_command_id.c_setting_dialog.c_cut_filter_below");

								auto filter_index = magi.exin.get_current_filter_index();
								if (filter_index >= 0)
									hook_manager.copy_filter(filter_index, 1, TRUE);
								break;
							}
						case magi.c_command_id.c_setting_dialog.c_copy_filter:
							{
								MY_TRACE_FUNC("magi.c_command_id.c_setting_dialog.c_copy_filter");

								auto filter_index = magi.exin.get_current_filter_index();
								if (filter_index >= 0)
									hook_manager.copy_filter(filter_index, 0, FALSE);
								break;
							}
						case magi.c_command_id.c_setting_dialog.c_copy_filter_above:
							{
								MY_TRACE_FUNC("magi.c_command_id.c_setting_dialog.c_copy_filter_above");

								auto filter_index = magi.exin.get_current_filter_index();
								if (filter_index >= 0)
									hook_manager.copy_filter(filter_index, -1, FALSE);
								break;
							}
						case magi.c_command_id.c_setting_dialog.c_copy_filter_below:
							{
								MY_TRACE_FUNC("magi.c_command_id.c_setting_dialog.c_copy_filter_below");

								auto filter_index = magi.exin.get_current_filter_index();
								if (filter_index >= 0)
									hook_manager.copy_filter(filter_index, 1, FALSE);
								break;
							}
						case magi.c_command_id.c_setting_dialog.c_paste_filter:
							{
								MY_TRACE_FUNC("magi.c_command_id.c_setting_dialog.c_paste_filter");

								hook_manager.paste_filter();
								break;
							}
						}

						break;
					}
				}

				return ::DefSubclassProc(hwnd, message, wParam, lParam);
			}
		} setting_dialog;

		//
		// このクラスは拡張編集の内部関数add_alias()をフックします。
		//
		struct {
			inline static BOOL CDECL hook_proc(LPCSTR file_name, BOOL flag1, BOOL flag2, int32_t object_index)
			{
				MY_TRACE_FUNC("{}, {}, {}, {}", file_name, flag1, flag2, object_index);

				if (!hook_manager.flag_paste_filter) // フラグが立っていない場合はデフォルトの処理を行います。
					return orig_proc(file_name, flag1, flag2, object_index);

				// オブジェクトを取得します。
				auto object = magi.exin.get_object(object_index);
				MY_TRACE_HEX(object);
				if (!object) return FALSE;

				// カレントフィルタを取得します。
				auto filter_index = magi.exin.get_current_filter_index();
				MY_TRACE_INT(filter_index);
				if (filter_index < 0) return FALSE;

				// カメラ制御のフラグを取得しておきます。
				auto camera = object->flag & ExEdit::Object::Flag::Camera;

				auto insert_pos = filter_index; // フィルタを挿入する位置です。
				auto ret_value = FALSE; // 戻り値です。

				for (const auto& filter : hook_manager.order.filters)
				{
					MY_TRACE_STR(filter.file_name.c_str());

					// フィルタを末尾に追加します。
					auto result = orig_proc(filter.file_name.c_str(), flag1, flag2, object_index);

					auto c = magi.exin.get_moveable_filter_count(object);

					// 末尾に追加されたフィルタを挿入位置まで移動します。
					for (int32_t i = c - 1; i > insert_pos + 1; i--)
					{
						auto filter = magi.exin.get_filter(object, i);

						magi.exin.swap_filter(object_index, i, -1);
					}

					insert_pos++;
					ret_value = TRUE;
				}

				// カメラ制御のフラグを復元します。
				object->flag |= camera;

				return ret_value;
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} add_alias;

		//
		// このクラスは拡張編集の内部関数unknown1()をフックします。
		//
		struct {
			inline static void CDECL hook_proc(int32_t object_index, int32_t filter_index)
			{
				MY_TRACE_FUNC("{}, {}", object_index, filter_index);

				orig_proc(object_index, filter_index);

				if (hook_manager.create_filter_command_id)
					hook_manager.create_filter(object_index, filter_index);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} unknown1;
	} hook_manager;
}
