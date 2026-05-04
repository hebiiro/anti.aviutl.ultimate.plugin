#pragma once

namespace apn::filter_relocate::controller::action
{
	//
	// このクラスはアクションの基本クラスです。
	// エラーの場合はstd::wstring型の例外を発生させます。
	//
	struct base_t
	{
		//
		// このクラスはタイムラインの全アイテムの位置を保持します。
		//
		struct placement_t {
			//
			// このクラスはレイヤー毎のアイテムの位置を保持します。
			//
			struct layer_t {
				//
				// このクラスはアイテムの位置を保持します。
				//
				struct item_t {
					int32_t frame_begin = {}, frame_end = {};
				};

				//
				// アイテムのコレクションです。
				//
				std::vector<item_t> items;

				//
				// 指定されたアイテム位置に既にアイテムが存在する場合はTRUEを返します。
				//
				BOOL intersect(const item_t& arg_item)
				{
					// このレイヤーにあるアイテムを走査します。
					for (const auto& item : items)
					{
						// 指定されたアイテムが完全に左に存在する場合は何もしません。
						if (arg_item.frame_end < item.frame_begin) continue;

						// 指定されたアイテムが完全に右に存在する場合は何もしません。
						if (arg_item.frame_begin > item.frame_end) continue;

						// それ以外の場合はアイテム同士が交差しているのでTRUEを返します。
						return TRUE;
					}

					// アイテム同士が交差していなかったのでFALSEを返します。
					return FALSE;
				}
			};

			//
			// レイヤーのコレクションです。
			//
			std::unordered_map<int32_t, layer_t> layers;
		} placement;

		//
		// 使用するファイルパスを管理します。
		//
		path_t path;

		//
		// 使用するexoファイルを管理します。
		//
		exo_t exo;

		//
		// aviutlの変数です。
		//
		AviUtl::FilterPlugin* fp = nullptr;
		AviUtl::EditHandle* editp = nullptr;

		//
		// 加工対象のソート済みオブジェクトのインデックスのコレクションです。
		//
		std::vector<int32_t> sorted_object_indexs;

		//
		// 削除予定のオブジェクト(のインデックス)のコレクションです。
		//
		std::vector<int32_t> objects_to_erase;

		//
		// 指定されたエラーメッセージの例外を発生させます。
		//
		[[noreturn]] inline static void throw_wstring(const std::wstring& error) { throw error; }

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
		// この仮想関数は指定されたオブジェクトが加工対象かどうか確認するために呼び出されます。
		//
		virtual BOOL on_is_valid_target(ExEdit::Object* object) = 0;

		//
		// この仮想関数はタイムライン加工の準備をするときに呼び出されます。
		// アイテム位置が重なってしまうなどで、準備が完了しなかった場合は例外を発生させます。
		//
		virtual void on_prepare() = 0;

		//
		// この仮想関数はタイムラインを加工するときに呼び出されます。
		//
		virtual void on_edit() noexcept = 0;

		//
		// コンストラクタです。
		//
		base_t()
			: path()
			, exo(path)
			, fp(magi.auin.get_filter_plugin(magi.fp, "拡張編集"))
			, editp(magi.exin.get_editp())
		{
			if (!fp || !editp) throw_wstring(L"拡張編集を取得できませんでした");
		}

		void prepare()
		{
			// 選択オブジェクトのコレクションです。
			std::unordered_set<ExEdit::Object*> selected_objects;

			//
			// この関数をは指定されたオブジェクトをコレクションに追加します。
			// 同時に、中間点もコレクションに追加します。
			//
			const auto emplace_selected_object = [&](int32_t object_index)
			{
				// オブジェクトを取得します。
				auto object = magi.exin.get_object(object_index);

				// オブジェクトがコレクションに追加済みの場合は何もしません。
				if (selected_objects.contains(object)) return;

				// オブジェクトをコレクションに追加します。
				selected_objects.emplace(object);

				// オブジェクトの中間点を走査します。
				for (auto midpt = object->index_midpt_leader;
					midpt >= 0; midpt = magi.exin.get_next_object_index(midpt))
				{
					// 中間点をコレクションに追加します。
					selected_objects.emplace(magi.exin.get_object(midpt));
				}
			};

			// 選択オブジェクトをコレクションに追加します。
			{
				// 選択オブジェクトの数を取得します。
				auto c = magi.exin.get_object_selection_count();

				// 選択オブジェクトが存在する場合は
				if (c)
				{
					// 選択オブジェクトをコレクションに追加します。
					for (decltype(c) i = 0; i < c; i++)
						emplace_selected_object(magi.exin.get_object_selection(i));
				}
				// 選択オブジェクトが存在しない場合は
				else
				{
					// カレントオブジェクトのインデックスを取得します。
					auto current_object_index = magi.exin.get_current_object_index();

					// カレントオブジェクトを選択オブジェクトとみなしてコレクションに追加します。
					if (current_object_index >= 0)
						emplace_selected_object(current_object_index);
				}

				// 選択オブジェクトが存在しない場合は
				if (selected_objects.empty()) throw_wstring(L"アイテムを選択してください");
			}

			// 現在のシーンの全アイテム位置を取得しておきます。
			{
				// ソート済みオブジェクトの数を取得します。
				auto c = magi.exin.get_sorted_object_count();

				// ソート済みオブジェクトを走査します。
				for (decltype(c) i = 0; i < c; i++)
				{
					// ソート済みオブジェクトを取得します。
					auto object = magi.exin.get_sorted_object(i);

					// プレースメントにアイテム位置を格納します。
					placement.layers[object->layer_set].items.emplace_back(
						object->frame_begin, object->frame_end);

					// ここで加工対象(のソート済みオブジェクトのインデックス)を取得します。
					{
						// ソート済みオブジェクトが選択オブジェクトの場合は
						if (selected_objects.contains(object))
						{
							// ソート済みオブジェクトが加工対象の場合は
							if (on_is_valid_target(object))
							{
								// コレクションに追加します。
								sorted_object_indexs.emplace_back(i);
							}
						}
					}
				}

				// 加工対象のオブジェクトが存在しない場合は
				if (sorted_object_indexs.empty()) throw_wstring(L"加工対象のオブジェクトが存在しません");
			}

			// サブクラスにタイムラインを加工する準備をさせます。
			// アイテム位置が重なっているなどの理由で
			// 準備が完了しなかった場合は例外が発生します。
			on_prepare();
		}

		//
		// 選択アイテムを基準にタイムラインを加工します。
		//
		void execute()
		{
			MY_TRACE_FUNC("");

			// カレントオブジェクトのインデックスを取得しておきます。
			auto current_object_index = magi.exin.get_current_object_index();

			// タイムラインを加工する準備をします。
			prepare();

			// 新しいアンドゥブロックを作成します。
			magi.exin.push_undo();

			// スコープ終了時の処理です。
			my::scope_exit scope_exit([&]()
			{
				// 除外オブジェクトを消去します。
				model::property.no_undos.clear();

				// オブジェクトテーブルを更新して、タイムラインを再描画します。
				magi.exin.update_object_table();
				magi.exin.redraw_timeline();

				// カレントオブジェクトを再設定します。
				magi.exin.set_current_object(current_object_index);

				// aviutlを再描画します。
				magi.redraw();
			});

			// ここでフィルタを分離したり結合したりします。
			on_edit();

			// 削除予定のオブジェクトを実際に削除します。
			{
				// 削除予定のオブジェクトを走査します。
				for (auto object_index : objects_to_erase)
				{
					// オブジェクトを削除します。
					magi.exin.erase_object(object_index);

					// アンドゥ対象から除外します。
					model::property.no_undos.insert(object_index);
				}
			}

			// 加工後exoファイルを作成します。
			if (!exo.after_file.write_file(path.after_file_name))
				throw_wstring(L"exoファイルの作成に失敗しました");

			// 加工後exoファイルを読み込んでアイテムを配置します。
			auto frame = magi.exin.load_exo_internal(path.after_file_name.c_str(), 0, 0);
		}

		//
		// 指定されたアイテム位置が無効の場合は例外を発生させます。
		//
		inline auto check_new_item_pos(int32_t layer, int32_t frame_begin, int32_t frame_end)
		{
			// 作成予定のレイヤー位置が無効の場合は
			if (layer < 0 || layer >= 100)
			{
				// 例外を送信します。
				throw_wstring(my::format(
					L"レイヤー位置:{/}が無効です",
					layer + 1));
			}

			// 作成予定位置に既にアイテムが存在する場合は
			if (placement.layers[layer].intersect({ frame_begin, frame_end }))
			{
				// 例外を送信します。
				throw_wstring(my::format(
					L"レイヤー:{/} フレーム:{/}~{/}でアイテムが交差しています",
					layer + 1, frame_begin, frame_end));
			}
		}

		//
		// 指定されたオブジェクトを削除します。
		//
		inline void erase_object(ExEdit::Object* object)
		{
			// 削除予定のオブジェクトとしてコレクションに追加します。
			objects_to_erase.emplace_back(magi.exin.get_object_index(object));
		}
	};
}
