#pragma once

namespace apn::filter_relocate::controller::action
{
	//
	// このクラスは上のアイテム(グループ制御)にフィルタを分離します。
	//
	struct split_item_above_t : base_t
	{
		//
		// 指定されたオブジェクトがグループ制御に変換できる場合はTRUEを返します。
		//
		inline static BOOL is_convertible(const auto& object)
		{
			// 制御オブジェクト(グループ制御など)の場合はFALSEを返します。
			if (has_flag(object->flag, decltype(object->flag)::Control)) return FALSE;

			// フィルタオブジェクト(図形など)の場合はFALSEを返します。
			if (has_flag(object->flag, decltype(object->flag)::Filter)) return FALSE;
/*
			// オブジェクトのフィルタを走査します。
			for (size_t i = 0; i < ExEdit::Object::MAX_FILTER; i++)
			{
				// フィルタが無効の場合はループを終了します。
				if (!object->filter_param[i].is_valid()) break;

				// 分離できないフィルタを持っている場合はFALSEを返します。
				if (!is_removable_filter(object->filter_param[i].id)) return FALSE;
			}
*/
			// それ以外の場合はTRUEを返します。
			return TRUE;
		}

		//
		// この仮想関数は指定されたオブジェクトが加工対象かどうか確認するために呼び出されます。
		//
		virtual BOOL on_is_valid_target(ExEdit::Object* object) override
		{
			// 音声オブジェクトではない場合は
			if (!has_flag(object->flag, ExEdit::Object::Flag::Sound))
			{
				// フィルタオブジェクト(図形など)の場合はTRUEを返します。
				if (has_flag(object->flag, ExEdit::Object::Flag::Filter)) return TRUE;

				// エフェクトオブジェクト(ぼかしなど)の場合はTRUEを返します。
				if (has_flag(object->flag, ExEdit::Object::Flag::Effect)) return TRUE;
			}

			// それ以外の場合はFALSEを返します。
			return FALSE;
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
				// 一つ上のレイヤーに作成します。
				auto new_layer = target_object->layer_set - 1;

				// 2つ上にずらす場合は
				if (model::property.flag_up_two_layers)
				{
					// グループ制御に変換できる場合は更に上のレイヤーに作成します。
					if (is_convertible(target_object)) new_layer--;
				}

				// アイテムの作成予定位置が有効かチェックします。
				check_new_item_pos(new_layer, target_object->frame_begin, target_object->frame_end);
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

					// グループ制御に変換できる場合は何もしません。
					if (is_convertible(object)) continue;

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

							// 描画フィルタをグループ制御に移す場合は
							if (model::property.flag_relocate_draw_filter)
							{
								// 標準描画または拡張描画をデフォルト値に戻します。

								// フィルタIDで分岐します。
								switch (object->filter_param[i].id)
								{
								// 標準描画の場合は
								case 10:
									{
										exo.write_string(after_section_name, "X", "0.0");
										exo.write_string(after_section_name, "Y", "0.0");
										exo.write_string(after_section_name, "Z", "0.0");
										exo.write_string(after_section_name, "拡大率", "100.00");
										exo.write_string(after_section_name, "回転", "0.0");

										break;
									}
								// 拡張描画の場合は
								case 11:
									{
										exo.write_string(after_section_name, "X", "0.0");
										exo.write_string(after_section_name, "Y", "0.0");
										exo.write_string(after_section_name, "Z", "0.0");
										exo.write_string(after_section_name, "拡大率", "100.00");
										exo.write_string(after_section_name, "X軸回転", "0.0");
										exo.write_string(after_section_name, "Y軸回転", "0.0");
										exo.write_string(after_section_name, "Z軸回転", "0.0");

										break;
									}
								}
							}
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

					// 加工対象のオブジェクトをグループ制御に変換できるかどうかのフラグを取得します。
					auto flag_is_convertible = is_convertible(object);

					// 新しいオブジェクトのセクションを作成します。
					{
						// セクション名を取得します。
						auto before_section_name = my::format("{/}", sorted_object_index);
						auto after_section_name = my::format("{/}", nb_written_object);

						// セクションをコピーします。
						exo.copy_section(before_section_name, after_section_name);

						// レイヤー番号を取得します。
						auto new_layer = exo.read_int(before_section_name, "layer");

						// レイヤー番号を上にずらします。
						new_layer--;

						// 2つ上にずらす場合は
						if (model::property.flag_up_two_layers)
						{
							// 加工対象のオブジェクトがグループ制御に変換できる場合は更に上にずらします。
							if (flag_is_convertible) new_layer--;
						}

						// レイヤー番号を書き換えます。
						exo.write_int(after_section_name, "layer", new_layer);
					}

					// グループ制御のセクションを作成します。
					{
						// セクション名を取得します。
						auto after_section_name = my::format("{/}.{/}", nb_written_object, nb_written_filter);

						// グループ制御のセクションを作成します。
						exo.create_section(after_section_name);

						// グループ制御の項目です。
						auto x = std::string("0.0");
						auto y = std::string("0.0");
						auto z = std::string("0.0");
						auto scale = std::string("100.00");
						auto rotate_x = std::string("0.0");
						auto rotate_y = std::string("0.0");
						auto rotate_z = std::string("0.0");

						// 描画フィルタをグループ制御に移す場合は
						if (model::property.flag_relocate_draw_filter)
						{
							// 加工対象のオブジェクトのフィルタを走査します。
							for (size_t i = 0; i < ExEdit::Object::MAX_FILTER; i++)
							{
								// フィルタが無効の場合はループを終了します。
								if (!object->filter_param[i].is_valid()) break;

								// フィルタIDで分岐します。
								switch (object->filter_param[i].id)
								{
								// 標準描画の場合は
								case 10:
									{
										// セクション名を取得します。
										auto before_section_name = my::format("{/}.{/}", sorted_object_index, i);

										x = exo.read_string(before_section_name, "X", x);
										y = exo.read_string(before_section_name, "Y", y);
										z = exo.read_string(before_section_name, "Z", z);
										scale = exo.read_string(before_section_name, "拡大率", scale);
										rotate_z = exo.read_string(before_section_name, "回転", rotate_z);

										break;
									}
								// 拡張描画の場合は
								case 11:
									{
										// セクション名を取得します。
										auto before_section_name = my::format("{/}.{/}", sorted_object_index, i);

										x = exo.read_string(before_section_name, "X", x);
										y = exo.read_string(before_section_name, "Y", y);
										z = exo.read_string(before_section_name, "Z", z);
										scale = exo.read_string(before_section_name, "拡大率", scale);
										rotate_x = exo.read_string(before_section_name, "X軸回転", rotate_x);
										rotate_y = exo.read_string(before_section_name, "Y軸回転", rotate_y);
										rotate_z = exo.read_string(before_section_name, "Z回転", rotate_z);

										break;
									}
								}
							}
						}

						// グループ制御の項目を作成します。
						exo.write_string(after_section_name, "_name", "グループ制御");
						exo.write_string(after_section_name, "X", x);
						exo.write_string(after_section_name, "Y", y);
						exo.write_string(after_section_name, "Z", z);
						exo.write_string(after_section_name, "拡大率", scale);
						exo.write_string(after_section_name, "X軸回転", rotate_x);
						exo.write_string(after_section_name, "Y軸回転", rotate_y);
						exo.write_string(after_section_name, "Z軸回転", rotate_z);
						exo.write_string(after_section_name, "上位グループ制御の影響を受ける", "0");
						exo.write_string(after_section_name, "同じグループのオブジェクトを対象にする", "1");

						// 書き込んだフィルタの数を増やします。
						nb_written_filter++;
					}

					// 加工対象のオブジェクトのフィルタを走査します。
					for (size_t i = 0; i < ExEdit::Object::MAX_FILTER; i++)
					{
						// フィルタが無効の場合はループを終了します。
						if (!object->filter_param[i].is_valid()) break;

						// グループ制御に変換できない場合は
						if (!flag_is_convertible)
						{
							// 先頭のフィルタまたは分離できないフィルタの場合は除外します。
							if (i == 0 || !is_removable_filter(object->filter_param[i].id)) continue;
						}

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
				// 元のオブジェクトを走査します。
				for (auto sorted_object_index : sorted_object_indexs)
				{
					// 元のオブジェクトを取得します。
					auto object = magi.exin.get_sorted_object(sorted_object_index);

					// 元のオブジェクトを削除します。
					erase_object(object);
				}
			}
		}
	};
}
