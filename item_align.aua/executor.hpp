#pragma once

namespace apn::item_align
{
	//
	// このクラスはタイムラインアイテムの位置を変化させる処理を実行します。
	//
	struct Executor
	{
		inline static constexpr struct CommandID
		{
			inline static constexpr int32_t c_shift = 1;
			inline static constexpr int32_t c_move_to_begin = 2;
			inline static constexpr int32_t c_move_to_end = 3;
			inline static constexpr int32_t c_move_begin = 4;
			inline static constexpr int32_t c_move_end = 5;
			inline static constexpr int32_t c_stretch_begin = 6;
			inline static constexpr int32_t c_stretch_end = 7;
			inline static constexpr int32_t c_delete_midpt = 8;
			inline static constexpr int32_t c_fix_bpm = 9;
		} command_id;

		//
		// レイヤーの最大数です。
		//
		inline static constexpr int32_t c_layer_max_size = 100;

		struct Mover
		{
			int32_t object_index;
			ExEdit::Object* object;
			int32_t frame_begin;
			int32_t frame_end;
		};

		AviUtl::EditHandle* editp = 0;
		AviUtl::FilterPlugin* fp = 0;
		int32_t command = -1;

		std::map<ExEdit::Object*, Mover> mover_map;
		std::vector<int32_t> selection;
		int32_t fps = 0;
		int32_t bpm = 0;
		double frame_per_time = 0;

		//
		// コンストラクタです。
		//
		Executor()
		{
			// よく使う変数を取得しておきます。
			fp = magi.fp;
			editp = magi.exin.get_editp();

			// ファイル情報を取得します。
			AviUtl::FileInfo fi = {};
			fp->exfunc->get_file_info(editp, &fi);

			// フレームレートを取得します。
			if (fi.video_rate != 0 && fi.video_scale != 0)
				fps = fi.video_rate / fi.video_scale;
			MY_TRACE_INT(fps);

			// BPMを取得します。
			bpm = magi.exin.get_bpm();
			MY_TRACE_INT(bpm);

			// 1区間当たりのフレーム数を取得します。
			if (bpm != 0)
				frame_per_time = fps / (bpm / 60.0 / 10000);
			MY_TRACE_REAL(frame_per_time);
		}

		//
		// 指定されたフレーム番号を修正して返します。
		//
		int32_t fix_frame(int32_t frame)
		{
			auto right_frame = frame_per_time;
			auto wrong_frame = ceil(right_frame);

			return (int32_t)ceil(frame * right_frame / wrong_frame);
		}

		//
		// 拡張編集の選択アイテムからセレクションを構築します。
		//
		BOOL create_selection()
		{
			MY_TRACE_FUNC("");

			// 選択アイテムの数を取得します。
			auto c = magi.exin.get_object_selection_count();
			MY_TRACE_INT(c);

			// 選択アイテムが存在しない場合は
			if (!c)
			{
				// カレントアイテムのインデックスを取得します。
				auto object_index = magi.exin.get_current_object_index();
				MY_TRACE_INT(object_index);
				if (object_index < 0) return FALSE;

				selection.push_back(object_index);

				return TRUE;
			}
			// 選択アイテムが存在する場合は
			else
			{
				for (decltype(c) i = 0; i < c; i++)
				{
					// 選択オブジェクトのインデックスを取得します。
					auto object_index = magi.exin.get_object_selection(i);
					MY_TRACE_INT(object_index);

					selection.push_back(object_index);
				}

				return TRUE;
			}
		}

		//
		// セレクションから移動予定のアイテム群を構築します。
		//
		BOOL create_mover_map()
		{
			MY_TRACE_FUNC("");

			// 選択オブジェクトを取得できなかった場合は
			if (!create_selection())
			{
				// エラーメッセージを出して終了します。
				hive.message_box(L"選択アイテムがありません");

				return FALSE;
			}

			auto stair_frame = hive.frame;
			auto stair_step = 0;

			for (auto object_index : selection)
			{
				MY_TRACE_INT(object_index);
				if (object_index < 0) continue;

				// 選択アイテムを取得します。
				auto object = magi.exin.get_object(object_index);
				MY_TRACE_HEX(object);
				if (!object) continue;

				if (mover_map.find(object) != mover_map.end())
					continue; // すでにmoverが作成済みならスキップします。

				auto midpt_leader = object->index_midpt_leader;
				MY_TRACE_INT(midpt_leader);

				// 中間点が存在する場合は
				if (midpt_leader >= 0)
				{
					object_index = midpt_leader;

					while (object_index >= 0)
					{
						// アイテムを取得します。
						auto object = magi.exin.get_object(object_index);
						MY_TRACE_HEX(object);
						if (!object) break;

						// アイテムの中間点先頭アイテムをチェックします。
						MY_TRACE_INT(object->index_midpt_leader);
						if (object->index_midpt_leader != midpt_leader) break;

						add_mover(object_index, object, stair_frame * stair_step);

						object_index = magi.exin.get_next_object_index(object_index);
					}
				}
				// 中間点が存在しない場合は
				else
				{
					add_mover(object_index, object, stair_frame * stair_step);
				}

				stair_step++;
			}

			return TRUE;
		}

		//
		// 指定されたアイテムを移動予定にします。
		//
		BOOL add_mover(int32_t object_index, auto object, int32_t frame_offset)
		{
			auto current_frame = magi.exin.get_exedit_current_frame();
			auto fixed_current_frame = fix_frame(current_frame);
			auto diff_current_frame = current_frame - fixed_current_frame;

			auto frame_begin = object->frame_begin;
			auto frame_end = object->frame_end;

			switch (command)
			{
			case command_id.c_shift:
				{
					frame_begin += frame_offset;
					frame_end += frame_offset;

					break;
				}
			case command_id.c_move_begin:
				{
					frame_begin = current_frame;
					frame_end = current_frame + (object->frame_end - object->frame_begin);

					break;
				}
			case command_id.c_move_end:
				{
					frame_end = current_frame - 1;
					frame_begin = current_frame - 1 - (object->frame_end - object->frame_begin);

					break;
				}
			case command_id.c_stretch_begin:
				{
					frame_begin = current_frame;

					break;
				}
			case command_id.c_stretch_end:
				{
					frame_end = current_frame - 1;

					break;
				}
			case command_id.c_fix_bpm:
				{
					if (hive.use_current_frame)
					{
						frame_begin += diff_current_frame;
						frame_end += diff_current_frame;

						frame_begin = fix_frame(frame_begin);
						frame_end = fix_frame(frame_end + 1) - 1;
					}
					else
					{
						frame_begin = fix_frame(frame_begin);
						frame_end = fix_frame(frame_end + 1) - 1;
					}


					break;
				}
			}

			if (frame_begin > frame_end)
				return FALSE;

			mover_map[object] = Mover(object_index, object, frame_begin, frame_end);

			return TRUE;
		}

		//
		// 移動予定アイテムが実際に移動可能かチェックします。
		//
		int32_t check_mover_map()
		{
			MY_TRACE_FUNC("");

			auto invalid_count = 0;

			for (const auto& pair : mover_map)
			{
				auto& mover = pair.second;

				// moverの開始位置が無効な場合は
				if (mover.frame_begin < 0)
				{
					invalid_count++;
					continue;
				}

				// アイテムの数を取得します。
				auto c = magi.exin.get_current_scene_object_count();
				MY_TRACE_INT(c);

				for (decltype(c) i = 0; i < c; i++)
				{
					// アイテムを取得します。
					auto object = magi.exin.get_sorted_object(i);
					MY_TRACE_HEX(object);
					if (!object) continue;

					// objectがmoverと同じ場合は
					if (object == mover.object)
						continue; // スキップします。

					// レイヤーが違う場合は
					if (object->layer_set != mover.object->layer_set)
						continue; // スキップします。

					// アイテムの位置を取得します。
					auto frame_begin = object->frame_begin;
					auto frame_end = object->frame_end;

					// アイテムに対応するmoverが存在する場合は
					auto it = mover_map.find(object);
					if (it != mover_map.end())
					{
						// moverから位置を取得します。
						frame_begin = it->second.frame_begin;
						frame_end = it->second.frame_end;
					}

					if (frame_end < mover.frame_begin)
						continue; // objectがmoverより完全に前にあるのでスキップします。

					if (mover.frame_end < frame_begin)
						continue; // objectがmoverより完全に後ろにあるのでスキップします。

					// objectとmoverは重なっています。
					invalid_count++;
					break;
				}
			}

			return invalid_count;
		}

		//
		// 移動予定のアイテムを実際に移動させます。
		//
		BOOL apply_mover_map()
		{
			MY_TRACE_FUNC("");

			// 移動予定のアイテムが存在しない場合は
			if (mover_map.empty())
			{
				// エラーメッセージを出して終了します。
				hive.message_box(L"動かせるアイテムがありません");

				return FALSE;
			}

			// 移動予定のアイテムを実際に移動できるかチェックします。
			auto invalid_count = check_mover_map();

			// 動かせないアイテムが存在する場合は
			if (invalid_count)
			{
				// エラーメッセージを出して終了します。
				hive.message_box(std::format(L"{}個のアイテムの位置が無効で動かせません", invalid_count));

				return FALSE;
			}

			// アンドゥを作成します。

			magi.exin.push_undo();

			for (auto& pair : mover_map)
			{
				const auto& mover = pair.second;
				auto object_index = mover.object_index;

				magi.exin.create_undo(object_index, 8);
			}

			// 実際にアイテムを動かし、レイヤーを再計算します。

			int32_t flags[c_layer_max_size] = {};

			for (auto& pair : mover_map)
			{
				const auto& mover = pair.second;
				auto object = mover.object;

				object->frame_begin = mover.frame_begin;
				object->frame_end = mover.frame_end;

				flags[object->layer_set] = 1;
			}

			// 拡張編集を再描画します。
			magi.exin.redraw_layers(flags);

			// AviUtlを再描画します。
			magi.redraw();

			return TRUE;
		}

		// アイテムを左に動かします。
		BOOL move_begin_internal(BOOL is_move)
		{
			MY_TRACE_FUNC("{}", is_move);

			// セレクションを構築できなかった場合は
			if (!create_selection())
			{
				// エラーメッセージを出して終了します。
				hive.message_box(L"選択アイテムがありません");

				return FALSE;
			}

			// 新しいアンドゥを作成します。
			magi.exin.push_undo();

			// レイヤーを再描画するためのフラグです。
			int32_t flags[c_layer_max_size] = {};

			for (int32_t layer = 0; layer < c_layer_max_size; layer++)
			{
				std::vector<int32_t> objects;

				// アイテムの数を取得します。
				auto c = magi.exin.get_current_scene_object_count();
				MY_TRACE_INT(c);

				for (decltype(c) i = 0; i < c; i++)
				{
					// アイテムを取得します。
					auto object = magi.exin.get_sorted_object(i);
					MY_TRACE_HEX(object);
					if (!object) continue;

					if (layer != object->layer_set)
						continue;

					objects.push_back(i);
				}

				if (objects.empty())
					continue;

				flags[layer] = TRUE;

				// アイテムを時間順にソートします。
				std::sort(objects.begin(), objects.end(), [](auto a, auto b) {
					auto object_a = magi.exin.get_sorted_object(a);
					auto object_b = magi.exin.get_sorted_object(b);
					return object_a->frame_begin < object_b->frame_begin;
				});

				// 現在フレームを取得します。これが移動先になります。
				auto current_frame = fp->exfunc->get_frame(editp);

				for (auto object_index : objects)
				{
					// アイテムを取得します。
					auto object = magi.exin.get_sorted_object(object_index);

					// このアイテムが選択されているか調べます。
					auto result = std::find_if(selection.begin(), selection.end(), [object](auto x) {
						return object == magi.exin.get_object(x);
					});

					// このアイテムが選択されているなら
					if (result != selection.end())
					{
						// アイテムの移動量を算出します。
						auto move = current_frame - object->frame_begin;

						// アイテムの移動は前方に限定します。
						if (move < 0)
						{
							// このアイテムをアンドゥに加えます。
							magi.exin.create_undo(*result, 8);

							// アイテムを移動します。
							object->frame_begin += move;
							if (is_move) object->frame_end += move;
						}
					}

					// 移動先よりアイテムの終了位置が後方の場合は
					if (current_frame < object->frame_end + 1)
					{
						// 移動先を更新します。
						current_frame = object->frame_end + 1;
					}
				}
			}

			// 拡張編集を再描画します。
			magi.exin.redraw_layers(flags);

			// AviUtlを再描画します。
			magi.redraw();

			return TRUE;
		}

		// アイテムを右に動かします。
		BOOL move_end_internal(BOOL is_move)
		{
			MY_TRACE_FUNC("{}", is_move);

			// セレクションを構築できなかった場合は
			if (!create_selection())
			{
				// エラーメッセージを出して終了します。
				hive.message_box(L"選択アイテムがありません");

				return FALSE;
			}

			// 新しいアンドゥを作成します。
			magi.exin.push_undo();

			// レイヤーを再描画するためのフラグです。
			int32_t flags[c_layer_max_size] = {};

			for (int32_t layer = 0; layer < c_layer_max_size; layer++)
			{
				std::vector<int32_t> objects;

				// アイテムの数を取得します。
				auto c = magi.exin.get_current_scene_object_count();
				MY_TRACE_INT(c);

				for (decltype(c) i = 0; i < c; i++)
				{
					// アイテムを取得します。
					auto object = magi.exin.get_sorted_object(i);
					MY_TRACE_HEX(object);
					if (!object) continue;

					if (layer != object->layer_set)
						continue;

					objects.push_back(i);
				}

				if (objects.empty())
					continue;

				flags[layer] = TRUE;

				// アイテムを時間順にソートします。
				std::sort(objects.begin(), objects.end(), [](auto a, auto b) {
					auto object_a = magi.exin.get_sorted_object(a);
					auto object_b = magi.exin.get_sorted_object(b);
					return object_a->frame_begin > object_b->frame_begin;
				});

				// 現在フレームを取得します。これが移動先になります。
				auto current_frame = fp->exfunc->get_frame(editp);

				for (auto object_index : objects)
				{
					// アイテムを取得します。
					auto object = magi.exin.get_sorted_object(object_index);

					// このアイテムが選択されているか調べます。
					auto result = std::find_if(selection.begin(), selection.end(), [object](auto x) {
						return object == magi.exin.get_object(x);
					});

					// このアイテムが選択されている場合は
					if (result != selection.end())
					{
						// アイテムの移動量を算出します。
						auto move = current_frame - object->frame_end;

						// アイテムの移動は後方に限定します。
						if (move > 0)
						{
							// このアイテムをアンドゥに加えます。
							magi.exin.create_undo(*result, 8);

							// アイテムを移動します。
							if (is_move) object->frame_begin += move;
							object->frame_end += move;
						}
					}

					// 移動先よりアイテムの終了位置が前方の場合は
					if (current_frame > object->frame_begin - 1)
					{
						// 移動先を更新します。
						current_frame = object->frame_begin - 1;
					}
				}
			}

			// レイヤーを更新します。
			magi.exin.redraw_layers(flags);
#if 0
			{
				// 設定ダイアログを再描画します。

				auto object_index = magi.exin.get_current_object_index();
				MY_TRACE_INT(object_index);

				if (object_index >= 0)
				{
					// この処理が機能していません。
					magi.exin.DrawSettingDialog(object_index);
					::InvalidateRect(magi.exin.get_setting_dialog(), 0, TRUE);
				}
			}
#endif
			// AviUtlを再描画します。
			magi.redraw();

			return TRUE;
		}

		BOOL move_current_frame()
		{
			MY_TRACE_FUNC("");

			// セレクションを構築できなかった場合は
			if (!create_selection())
			{
				// エラーメッセージを出して終了します。
				hive.message_box(L"選択アイテムがありません");

				return FALSE;
			}

			auto frame = -1;

			for (auto object_index : selection)
			{
				MY_TRACE_INT(object_index);
				if (object_index < 0) continue;

				// 選択アイテムを取得します。
				auto object = magi.exin.get_object(object_index);
				MY_TRACE_HEX(object);
				if (!object) continue;

				if (command == command_id.c_move_to_begin)
				{
					if (frame < 0)
						frame = object->frame_begin;
					else
						frame = std::min(frame, object->frame_begin);
				}
				else if (command == command_id.c_move_to_end)
				{
					if (frame < 0)
						frame = object->frame_end + 1;
					else
						frame = std::max(frame, object->frame_end + 1);
				}
			}

			// フレーム位置が無効の場合は
			if (frame < 0)
			{
				// エラーメッセージを出して終了します。
				hive.message_box(L"現在フレームを動かせませんでした");

				return FALSE;
			}

			// 現在フレームを変更します。
			fp->exfunc->set_frame(editp, frame);

			// AviUtlを再描画します。
			magi.redraw();

			return TRUE;
		}

		//
		// 選択アイテムを階段状にずらします。
		//
		BOOL shift_selection()
		{
			MY_TRACE_FUNC("");

			command = command_id.c_shift;

			// 移動予定アイテムを構築します。
			create_mover_map();

			// 実際にアイテムを移動します。
			return apply_mover_map();
		}

		//
		// 現在フレームを選択アイテムの開始位置に移動します。
		//
		BOOL move_current_frame_to_start()
		{
			MY_TRACE_FUNC("");

			command = command_id.c_move_to_begin;

			return move_current_frame();
		}

		//
		// 現在フレームを選択アイテムの終了位置に移動します。
		//
		BOOL move_current_frame_to_end()
		{
			MY_TRACE_FUNC("");

			command = command_id.c_move_to_end;

			return move_current_frame();
		}

		//
		// アイテムの開始位置を動かします。終了位置もそれに連動して動かします。
		//
		BOOL move_selection_start()
		{
			MY_TRACE_FUNC("");

			command = command_id.c_move_begin;

			return move_begin_internal(TRUE);
		}

		//
		// アイテムの終了位置を動かします。開始位置もそれに連動して動かします。
		//
		BOOL move_selection_end()
		{
			MY_TRACE_FUNC("");

			command = command_id.c_move_end;

			return move_end_internal(TRUE);
		}

		//
		// アイテムの開始位置を左に伸ばします。終了位置は動かしません。
		//
		BOOL stretch_selection_start()
		{
			MY_TRACE_FUNC("");

			command = command_id.c_stretch_begin;

			return move_begin_internal(FALSE);
		}

		//
		// アイテムの終了位置を右に伸ばします。開始位置は動かしません。
		//
		BOOL stretch_selection_end()
		{
			MY_TRACE_FUNC("");

			command = command_id.c_stretch_end;

			return move_end_internal(FALSE);
		}

		//
		// 現在フレームに最も近い中間点を削除します。
		//
		BOOL delete_midpt()
		{
			MY_TRACE_FUNC("");

			command = command_id.c_delete_midpt;

			auto current_frame = fp->exfunc->get_frame(editp);

			auto object_index = magi.exin.get_current_object_index();
			MY_TRACE_INT(object_index);
			if (object_index < 0) return FALSE;

			// 選択アイテムを取得します。
			auto object = magi.exin.get_object(object_index);
			MY_TRACE_HEX(object);
			if (!object) return FALSE;

			auto midpt_leader = object->index_midpt_leader;
			MY_TRACE_INT(midpt_leader);

			// 中間点が存在しない場合は何もしません。
			if (midpt_leader < 0) return FALSE;

			auto layer_index = object->layer_disp;
			auto near_object_index = -1;
			auto near_frame_begin = -1;

			object_index = midpt_leader;

			while (object_index >= 0)
			{
				if (object_index != midpt_leader)
				{
					// アイテムを取得します。
					auto object = magi.exin.get_object(object_index);
					MY_TRACE_HEX(object);
					if (!object) break;

					// 中間点の先頭が異なる場合はループを終了します。
					MY_TRACE_INT(object->index_midpt_leader);
					if (object->index_midpt_leader != midpt_leader) break;

					// 最初に処理されるアイテムの場合は
					if (near_frame_begin == -1)
					{
						// このアイテムを削除予定の中間点に設定します。
						near_object_index = object_index;
						near_frame_begin = object->frame_begin;
					}
					// それ以外の場合は
					else
					{
						// このアイテムが削除予定の中間点より現在フレームに近い場合は
						if (abs(current_frame - object->frame_begin) <
							abs(current_frame - near_frame_begin))
						{
							// このアイテムを削除予定の中間点に設定します。
							near_object_index = object_index;
							near_frame_begin = object->frame_begin;
						}
					}
				}

				object_index = magi.exin.get_next_object_index(object_index);
			}

			if (near_object_index < 0) return FALSE;

			magi.exin.push_undo();
			magi.exin.erase_midpt(near_object_index, 0);
			magi.exin.redraw_layer(layer_index);
			::InvalidateRect(magi.exin.get_setting_dialog(), 0, FALSE);

			return TRUE;
		}

		//
		// 選択アイテムのBPMずれを修正します。
		//
		BOOL fix_selection_bpm()
		{
			MY_TRACE_FUNC("");

			command = command_id.c_fix_bpm;

			// 移動予定アイテムを構築します。
			create_mover_map();

			// 実際にアイテムを移動します。
			return apply_mover_map();
		}
	};
}
