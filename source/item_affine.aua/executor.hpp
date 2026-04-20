#pragma once

namespace apn::item_affine
{
	//
	// このクラスは選択アイテムの位置を変更します。
	// エラーの場合はstd::wstring型の例外を発生させます。
	//
	struct executor_t
	{
		//
		// このクラスはアフィン変換後のアイテム位置を保持します。
		//
		struct mover_t {
			int32_t frame_begin = {}, frame_end = {};
		};

		//
		// aviutlの変数です。
		//
		AviUtl::FilterPlugin* fp = nullptr;
		AviUtl::EditHandle* editp = nullptr;

		//
		// アイテム位置を変換するときの原点です。
		//
		int32_t orig_pos = {};

		//
		// 指定されたエラーメッセージの例外を発生させます。
		//
		[[noreturn]] inline static void throw_wstring(const std::wstring& error) { throw error; }

		//
		// コンストラクタです。
		//
		executor_t()
			: fp(magi.auin.get_filter_plugin(magi.fp, "拡張編集"))
			, editp(magi.exin.get_editp())
		{
			if (!fp || !editp) throw_wstring(L"拡張編集を取得できませんでした");
		}

		//
		// この関数は指定された位置にアフィン変換を適用して返します。
		//
		inline static int32_t pos_transform(const affine_t& affine, int32_t pos, int32_t fix)
		{
			auto temp = (double)(pos + fix);

			// 累乗が有効の場合は
			if (affine.base.num && affine.base.den && affine.exp.num && affine.exp.den)
			{
				// 基数を算出します。
				auto base = (double)affine.base.num / affine.base.den;

				// 指数を算出します。
				auto exp = (double)affine.exp.num / affine.exp.den;

				// 累乗して乗算します。
				temp = base * pow(temp / base, exp);
			}

			// 乗算値が有効の場合は乗算します。
			if (affine.mul.num && affine.mul.den)
				temp *= (double)affine.mul.num / affine.mul.den;

			// 加算値が有効の場合は加算します。
			if (affine.add.num && affine.add.den)
				temp += (double)affine.add.num / affine.add.den;

			return (int32_t)temp - fix;
		}

		//
		// この関数は指定された長さにアフィン変換を適用して返します。
		//
		inline static int32_t len_transform(const affine_t& affine, int32_t len)
		{
			return pos_transform(affine, len, 1);
		}

		//
		// 選択アイテムの位置を変更します。
		//
		void pos_execute()
		{
			MY_TRACE_FUNC("");

			// 選択アイテムの位置を変更します。
			return execute([&](const auto& object)
			{
				//
				// この関数はフラグが有効の場合にだけ指定された位置にアフィン変換を適用して返します。
				//
				constexpr auto transform = [](BOOL flag, int32_t pos, int32_t fix)
				{
					if (!flag) return pos;

					return pos_transform(hive.pos, pos, fix);
				};

				// 処理対象かどうかのフラグを取得します。
				auto flag_begin = (hive.pos.target != hive.pos.c_target.c_end);
				auto flag_end = (hive.pos.target != hive.pos.c_target.c_begin);

				// 処理対象オブジェクトのアイテム位置にアフィン変換を適用して返します。
				return mover_t {
					transform(flag_begin, object->frame_begin, 0 - orig_pos),
					transform(flag_end, object->frame_end, 1 - orig_pos),
				};
			});
		}

		//
		// 選択アイテムの長さを変更します。
		//
		void len_execute()
		{
			MY_TRACE_FUNC("");

			// 選択アイテムの長さを変更します。
			return execute([&](const auto& object)
			{
				// アイテムの長さを取得します。
				const auto len = object->frame_end - object->frame_begin;

				// 新しいアイテムの長さを算出します。
				const auto new_len = len_transform(hive.len, len);

				// 処理対象で分岐します。
				switch (hive.len.target)
				{
				// 開始位置だけを変更する場合は
				case hive.len.c_target.c_begin:
					{
						// 開始位置だけを変更して返します。
						return mover_t {
							object->frame_end - new_len,
							object->frame_end,
						};
					}
				// 終了位置だけを変更する場合は
				case hive.len.c_target.c_end:
					{
						// 終了位置だけを変更して返します。
						return mover_t {
							object->frame_begin,
							object->frame_begin + new_len,
						};
					}
				// それ以外の場合は
				default:
					{
						// 新しい開始位置を算出します。
						auto new_begin = (object->frame_begin + object->frame_end + 1 - new_len) / 2;

						// 開始位置と終了位置を変更して返します。
						return mover_t {
							new_begin,
							new_begin + new_len,
						};
					}
				}
			});
		}

		//
		// 選択アイテムの位置または長さを変更します。
		//
		void execute(auto func)
		{
			MY_TRACE_FUNC("");

			// 選択オブジェクトのインデックスの配列です。
			std::vector<int32_t> selected_object_indexs;
			{
				// 選択オブジェクトの数を取得します。
				auto c = magi.exin.get_object_selection_count();

				// 選択オブジェクトが存在する場合は
				if (c)
				{
					// 選択オブジェクトを配列に追加します。
					for (decltype(c) i = 0; i < c; i++)
						selected_object_indexs.emplace_back(magi.exin.get_object_selection(i));
				}
				// 選択オブジェクトが存在しない場合は
				else
				{
					// カレントオブジェクトを選択オブジェクトとみなして配列に追加します。
					auto current_object_index = magi.exin.get_current_object_index();
					if (current_object_index >= 0)
						selected_object_indexs.emplace_back(current_object_index);
				}

				// 選択オブジェクトが存在しない場合は
				if (selected_object_indexs.empty()) throw_wstring(L"アイテムを選択してください");
			}

			// 選択オブジェクトの最も左と最も右の位置です。
			struct bounds_t {
				int32_t begin = INT_MAX;
				int32_t end = 0;
			} bounds;

			{
				// 選択オブジェクトを処理します。
				for (auto selected_object_index : selected_object_indexs)
				{
					// 選択オブジェクトを取得します。
					auto selected_object = magi.exin.get_object(selected_object_index);

					// 選択オブジェクトの最も左と最も右の位置を取得します。
					bounds.begin = std::min(bounds.begin, selected_object->frame_begin);
					bounds.end = std::max(bounds.end, selected_object->frame_end + 1);
				}
			}

			// 原点を取得します。
			orig_pos = [&]() {
				switch (hive.orig) {
				case hive.c_orig.c_scene_begin: return 0;
				case hive.c_orig.c_scene_end: return fp->exfunc->get_frame_n(editp);
				case hive.c_orig.c_item_begin: return bounds.begin;
				case hive.c_orig.c_item_end: return bounds.end;
				case hive.c_orig.c_current_frame: return fp->exfunc->get_frame(editp);
				default: throw_wstring(L"原点が無効です");
				}
			}();

			// 処理対象のオブジェクトのインデックスのコレクションです。
			std::unordered_map<int32_t, mover_t> movers;
			{
				// 選択オブジェクトを処理します。
				for (auto selected_object_index : selected_object_indexs)
				{
					// 既に処理対象になっている場合は除外します。
					if (movers.contains(selected_object_index)) continue;

					// 選択オブジェクトを取得します。
					auto selected_object = magi.exin.get_object(selected_object_index);

					// 中間点を持たない場合は
					if (selected_object->index_midpt_leader < 0)
					{
						// 選択オブジェクトを処理対象に追加します。
						movers.emplace(selected_object_index, mover_t());
					}
					// 中間点を持つ場合は
					else
					{
						// 中間点オブジェクトを走査します。
						for (auto midpt_object_index = selected_object->index_midpt_leader;
							midpt_object_index > 0; magi.exin.get_next_object_index(midpt_object_index))
						{
							// 中間点オブジェクトを処理対象に追加します。
							movers.emplace(midpt_object_index, mover_t());
						}
					}
				}

				if (movers.empty()) throw_wstring(L"処理対象のオブジェクトが存在しません");
			}

			// アフィン変換後のアイテム位置を算出します。
			{
				// 処理対象オブジェクトを処理します。
				for (auto& pair : movers)
				{
					// 処理対象オブジェクトを取得します。
					auto object = magi.exin.get_object(pair.first);

					// アイテム位置を変換します。
					pair.second = func(object);

					// アイテム位置が不正の場合は例外を送信します。
					if (pair.second.frame_begin < 0 || pair.second.frame_end < 0)
						throw_wstring(L"アイテムの位置が負数になっています");
					if (pair.second.frame_begin > pair.second.frame_end)
						throw_wstring(L"アイテムの長さが不正になっています");
				}
			}

			// アフィン変換後のアイテム位置が有効かどうか確認します。
			{
				// ソート済みオブジェクトを処理します。
				for (auto i = 0; i < magi.exin.get_sorted_object_count(); i++)
				{
					// ソート済みオブジェクトを取得します。
					auto sorted_object = magi.exin.get_sorted_object(i);

					// オブジェクトのインデックスを取得します。
					auto sorted_object_index = magi.exin.get_object_index(sorted_object);

					// 処理対象オブジェクトを走査します。
					for (auto pair : movers)
					{
						// ソート済みオブジェクトと同一の場合は除外します。
						if (pair.first == sorted_object_index) continue;

						// 処理対象オブジェクトを取得します。
						auto object = magi.exin.get_object(pair.first);

						// レイヤーが異なる場合は除外します。
						if (object->layer_set != sorted_object->layer_set) continue;

						// ソート済みオブジェクトの変換後位置を取得します。
						auto mover = mover_t { sorted_object->frame_begin, sorted_object->frame_end };
						if (auto it = movers.find(sorted_object_index); it != movers.end()) mover = it->second;

						// ソート済みオブジェクトと処理対象オブジェクトが交差しない場合は除外します。
						if (pair.second.frame_begin > mover.frame_end) continue;
						if (pair.second.frame_end < mover.frame_begin) continue;

						// 変換後オブジェクトが交差しているので例外を送信します。
						throw_wstring(my::format(
							L"レイヤー:{/} フレーム:{/}~{/}でアイテムが交差しています",
							object->layer_set + 1, mover.frame_begin, mover.frame_end));
					}
				}
			}

			// アフィン変換後のアイテム位置を適用します。
			{
				// 新しいアンドゥブロックを作成します。
				magi.exin.push_undo();

				// 最終フレーム位置です。
				auto last_frame_number = int32_t {};

				// 処理対象オブジェクトを処理します。
				for (auto pair : movers)
				{
					// 処理対象オブジェクトを取得します。
					auto object = magi.exin.get_object(pair.first);

					// 処理対象オブジェクトをアンドゥの対象にします。
					magi.exin.create_undo(pair.first, 1);

					// 処理対象オブジェクトにアフィン変換を適用します。
					object->frame_begin = pair.second.frame_begin;
					object->frame_end = pair.second.frame_end;

					// 最終フレーム位置を更新します。
					last_frame_number = std::max(last_frame_number, object->frame_end);
				}

				// 最終フレーム位置が有効の場合は
				if (last_frame_number > magi.exin.get_exedit_frame_number())
				{
					// 最終フレーム位置を更新します。
					magi.exin.set_frame_number(last_frame_number, editp, fp);
				}

				// オブジェクトテーブルを更新して、タイムラインを再描画します。
				magi.exin.update_object_table();
				magi.exin.redraw_timeline();

				// カレントオブジェクトを再設定します。
				magi.exin.set_current_object(magi.exin.get_current_object_index());

				// aviutlを再描画します。
				magi.redraw();
			}
		}
	};
}
