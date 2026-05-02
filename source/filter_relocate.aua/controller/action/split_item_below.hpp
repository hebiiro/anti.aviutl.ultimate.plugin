#pragma once

namespace apn::filter_relocate::controller::action
{
	//
	// このクラスは下のアイテムにフィルタを分離します。
	//
	struct split_item_below_t : base_t
	{
		//
		// 指定されたフィルタが分離できる場合はTRUEを返します。
		//
		inline static BOOL is_removable_filter(auto filter_id)
		{
			if (filter_id == 10) return FALSE; // 標準描画
			if (filter_id == 11) return FALSE; // 拡張描画
			if (filter_id == 12) return FALSE; // 標準再生

			return TRUE;
		}

		//
		// この仮想関数はタイムラインを加工する前に呼び出されます。
		// アイテム位置が重なってしまうなどの理由で
		// 加工の準備が完了しなかった場合は例外を発生させて処理を中断させます。
		//
		virtual void on_prepare() override
		{
			// 加工対象のオブジェクトを走査します。
			for (auto sorted_object_index : sorted_object_indexs)
			{
				// 加工対象のオブジェクトを取得します。
				auto target_object = magi.exin.get_sorted_object(sorted_object_index);

				// アイテムを作成予定のレイヤー番号です。
				auto new_layer = target_object->layer_set + 1;

				// アイテムを作成予定の位置です。
				auto new_item = create_item(target_object);

				// 作成予定位置に既にアイテムが存在する場合は
				if (placement.layers[new_layer].intersect(new_item))
				{
					// 例外を送信します。
					throw_wstring(my::format(
						L"レイヤー:{/} フレーム:{/}~{/}でアイテムが交差しています",
						new_layer + 1, new_item.frame_begin, new_item.frame_end));
				}
			}
		}

		//
		// この仮想関数はタイムラインを加工するときに呼び出されます。
		//
		virtual void on_edit() noexcept override
		{
			// 書き込んだオブジェクトの数です。
			auto nb_written_object = int32_t {};

			// まず、フィルタ分離後のオブジェクトをexoファイルに書き込みます。
			{
				// 加工対象のオブジェクトを走査します。
				for (auto sorted_object_index : sorted_object_indexs)
				{
					// 加工対象のオブジェクトを取得します。
					auto object = magi.exin.get_sorted_object(sorted_object_index);

					// 書き込んだフィルタの数です。
					auto nb_written_filter = int32_t {};

					// 新しいオブジェクトのセクションを作成します。
					{
						// セクション名を取得します。
						auto before_section_name = my::format("{/}", sorted_object_index);
						auto after_section_name = my::format("{/}", nb_written_object);

						// セクションをコピーします。
						exo.copy_section(before_section_name, after_section_name);
					}

					// 加工対象のオブジェクトのフィルタを走査します。
					for (size_t i = 0; i < ExEdit::Object::MAX_FILTER; i++)
					{
						// フィルタが無効の場合はループを終了します。
						if (!object->filter_param[i].is_valid()) break;

						// 先頭のフィルタ以外で尚且つフィルタが分離できる場合は除外します。
						if (i != 0 && is_removable_filter(object->filter_param[i].id)) continue;

						// 新しいフィルタのセクションを作成します。
						{
							// セクション名を取得します。
							auto before_section_name = my::format("{/}.{/}", sorted_object_index, i);
							auto after_section_name = my::format("{/}.{/}", nb_written_object, nb_written_filter);

							// セクションをコピーします。
							exo.copy_section(before_section_name, after_section_name);
						}

						// 書き込んだフィルタの数を増やします。
						nb_written_filter++;
					}

					// 書き込んだオブジェクトの数を増やします。
					nb_written_object++;
				}
			}

			// 次に、フィルタ分離先のオブジェクトをexoファイルに書き込みます。
			{
				// 加工対象のオブジェクトを走査します。
				for (auto sorted_object_index : sorted_object_indexs)
				{
					// 加工対象のオブジェクトを取得します。
					auto object = magi.exin.get_sorted_object(sorted_object_index);

					// 書き込んだフィルタの数です。
					auto nb_written_filter = int32_t {};

					// 新しいオブジェクトのセクションを作成します。
					{
						// セクション名を取得します。
						auto before_section_name = my::format("{/}", sorted_object_index);
						auto after_section_name = my::format("{/}", nb_written_object);

						// セクションをコピーします。
						exo.copy_section(before_section_name, after_section_name);

						// レイヤー番号を書き換えます。
						auto layer = exo.read_int(before_section_name, "layer");
						exo.write_int(after_section_name, "layer", layer + 1);
					}

					// 加工対象のオブジェクトのフィルタを走査します。
					// ただし、先頭のフィルタは除外します。
					for (size_t i = 0; i < ExEdit::Object::MAX_FILTER; i++)
					{
						// フィルタが無効の場合はループを終了します。
						if (!object->filter_param[i].is_valid()) break;

						// 先頭のフィルタまたはフィルタが分離できない場合は除外します。
						if (i == 0 || !is_removable_filter(object->filter_param[i].id)) continue;

						// 新しいフィルタのセクションを作成します。
						{
							// セクション名を取得します。
							auto before_section_name = my::format("{/}.{/}", sorted_object_index, i);
							auto after_section_name = my::format("{/}.{/}", nb_written_object, nb_written_filter);

							// セクションをコピーします。
							exo.copy_section(before_section_name, after_section_name);
						}

						// 書き込んだフィルタの数を増やします。
						nb_written_filter++;
					}

					// 書き込んだオブジェクトの数を増やします。
					nb_written_object++;
				}
			}

			// 最後に元のオブジェクトを削除します。
			{
				// erase_object()内でupdate_object_table()が呼ばれるので
				// ソート済みオブジェクトは走査できません。
				// なので、予め全ての削除予定のオブジェクトのインデックスを取得します。
				std::vector<int32_t> object_indexs;

				// 加工対象のオブジェクトを走査します。
				for (auto sorted_object_index : sorted_object_indexs)
				{
					// 加工対象のオブジェクトを取得します。
					auto object = magi.exin.get_sorted_object(sorted_object_index);

					// 元のオブジェクトを削除します。
					object_indexs.emplace_back(magi.exin.get_object_index(object));
				}

				// オブジェクトを走査します。
				for (auto object_index : object_indexs)
				{
					// 元のオブジェクトを削除します。
					erase_object(object_index);
				}
			}
		}
	};
}
