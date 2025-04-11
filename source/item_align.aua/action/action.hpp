#pragma once

namespace apn::item_align
{
	template <typename T>
	struct reverse_iteration_helper
	{
		using Collection = T;
		using iterator = Collection::reverse_iterator;
		using const_iterator = Collection::const_reverse_iterator;

		Collection& collection;

		reverse_iteration_helper(Collection& collection) : collection(collection) {}
		iterator begin(){ return collection.rbegin(); }
		const_iterator begin() const { return collection.rbegin(); }
		iterator end(){ return collection.rend(); }
		const_iterator end() const { return collection.rend(); }
	};

	template <typename T>
	inline auto reverse_iteration(T& collection) { return reverse_iteration_helper<T>(collection); }

	//
	// このクラスはタイムラインアイテムの位置を変化させる処理を実行します。
	//
	struct Action : Utils
	{
		//
		// このクラスはノードです。
		//
		struct Node
		{
			//
			// このクラスは編集前後の値です。
			//
			struct {
				int32_t layer_set;
				double time_start, time_end;
			} before, after;

			//
			// オブジェクトのインデックスです。
			//
			int32_t object_index;

			//
			// オブジェクトです。
			//
			ExEdit::Object* object;

			//
			// コンストラクタです。
			//
			Node(const Action& action, int32_t object_index, ExEdit::Object* object)
				: object_index(object_index)
				, object(object)
			{
				before.layer_set = object->layer_set;
				before.time_start = action.frame_to_time(object->frame_begin);
				before.time_end = action.frame_to_time(object->frame_end);
				after = before;
			}
		};

		//
		// このクラスはレイヤーです。
		//
		struct Layer
		{
			//
			// レイヤー番号です。
			//
			int32_t layer_set;

			//
			// ノードのコレクションです。
			//
			std::vector<std::shared_ptr<Node>> nodes;

			//
			// コンストラクタです。
			//
			explicit Layer(int32_t layer_set) : layer_set(layer_set) {}
		};

		//
		// このクラスは範囲です。
		//
		struct Bound
		{
			int32_t top_layer = INT_MAX;
			int32_t bottom_layer = INT_MIN;
			double time_start = DBL_MAX;
			double time_end = DBL_MIN;

			//
			// 指定されたノードが含まれるように範囲を拡張します。
			//
			void inflate(const std::shared_ptr<Node>& node)
			{
				// 一番上のレイヤーを取得します。
				top_layer = std::min(top_layer, node->before.layer_set);

				// 一番下のレイヤーを取得します。
				bottom_layer = std::max(bottom_layer, node->before.layer_set);

				// 最小の開始位置を取得します。
				time_start = std::min(time_start, node->before.time_start);

				// 最大の終了位置を取得します。
				time_end = std::max(time_end, node->before.time_end);
			}
		};

		//
		// ノードのコレクションです。
		//
		std::unordered_map<ExEdit::Object*, std::shared_ptr<Node>> nodes;

		//
		// レイヤーのコレクションです。
		//
		std::unordered_map<int32_t, std::shared_ptr<Layer>> layers;

		//
		// 選択ノードのコレクションです。
		//
		std::unordered_set<std::shared_ptr<Node>> selection;

		//
		// 選択ノードの範囲です。
		//
		Bound bound;

		//
		// コンストラクタです。
		//
		Action()
		{
			MY_TRACE_FUNC("");

			init_nodes();
			init_layers();
			init_selection();
		}

		//
		// 指定されたノードが選択ノードの場合はtrueを返します。
		//
		bool is_selected(const std::shared_ptr<Node>& node) const
		{
			return selection.contains(node);
		}

		//
		// オブジェクトからノードを取得して返します。
		// 内部的に使用されます。
		//
		std::shared_ptr<Node> get_node(ExEdit::Object* object)
		{
			auto it = nodes.find(object);
			if (it == nodes.end()) return nullptr;
			return it->second;
		}

		//
		// 選択ノードを追加します。
		// 内部的に使用されます。
		//
		void select_node(const std::shared_ptr<Node>& node)
		{
			MY_TRACE_FUNC("");

			selection.insert(node);
			bound.inflate(node);
		}

		//
		// 選択ノードを追加します。
		// 内部的に使用されます。
		//
		void select_object(int32_t object_index)
		{
			MY_TRACE_FUNC("{/}", object_index);

			if (object_index < 0) return;
			auto object = magi.exin.get_object(object_index);
			if (!object) return;
			if (!(object->flag & ExEdit::Object::Flag::Exist)) return;

			auto midpt_leader = object->index_midpt_leader;
			MY_TRACE_INT(midpt_leader);

			// 中間点が存在しない場合は
			if (midpt_leader < 0)
			{
				// 引数で与えられたオブジェクトだけを選択します。
				if (auto node = get_node(object)) select_node(node);
			}
			// 中間点が存在する場合は
			else
			{
				// 中間点を構成するオブジェクトも選択します。

				object_index = midpt_leader;

				while (object_index >= 0)
				{
					// オブジェクトを取得します。
					auto object = magi.exin.get_object(object_index);
					MY_TRACE_HEX(object);
					if (!object) break;

					// オブジェクトの中間点先頭オブジェクトをチェックします。
					MY_TRACE_INT(object->index_midpt_leader);
					if (object->index_midpt_leader != midpt_leader) break;

					// この中間点オブジェクトを選択します。
					if (auto node = get_node(object)) select_node(node);

					// 次の中間点に移動します。
					object_index = magi.exin.get_next_object_index(object_index);
				}
			}
		}

		//
		// ノードのコレクションを初期化します。
		// 内部的に使用されます。
		//
		void init_nodes()
		{
			MY_TRACE_FUNC("");

			// カレントシーンを取得します。
			auto current_scene_set = magi.exin.get_current_scene_index();

			// オブジェクトの数を取得します。
			auto c = magi.exin.get_object_count();
			MY_TRACE_INT(c);

			// オブジェクトを走査します。
			for (decltype(c) i = 0; i < c; i++)
			{
				// オブジェクトを取得します。
				auto object = magi.exin.get_object(i);
				MY_TRACE_HEX(object);
				if (!object) continue;
				if (!(object->flag & ExEdit::Object::Flag::Exist)) continue;
				if (object->scene_set != current_scene_set) continue;

				// ノードのインスタンスを作成します。
				auto node = std::make_shared<Node>(*this, i, object);

				// ノードをコレクションに追加します。
				nodes[object] = node;

				// レイヤーを取得します。
				auto& layer = layers[object->layer_set];

				// レイヤーが取得できなかった場合は新規作成します。
				if (!layer) layer = std::make_shared<Layer>(object->layer_set);

				// ノードをレイヤーに追加します。
				layer->nodes.emplace_back(node);
			}
		}

		//
		// レイヤーのコレクションを初期化します。
		// 内部的に使用されます。
		//
		void init_layers()
		{
			MY_TRACE_FUNC("");

			// レイヤーを走査します。
			for (auto& pair : layers)
			{
				// レイヤーを取得します。
				auto& layer = pair.second;

				// レイヤー内のノードを時間順に並べ替えます。
				std::sort(std::begin(layer->nodes), std::end(layer->nodes),
					[](const auto& x, const auto& y)
				{
					if (x->before.time_start == y->before.time_start)
						return x->before.time_end < y->before.time_end;
					else
						return x->before.time_start < y->before.time_start;
				});
			}
		}

		//
		// 編集対象を初期化します。
		// 内部的に使用されます。
		//
		void init_selection()
		{
			MY_TRACE_FUNC("");

			// 選択オブジェクトの数を取得します。
			auto c = magi.exin.get_object_selection_count();
			MY_TRACE_INT(c);

			// 選択オブジェクトが存在する場合は
			if (c)
			{
				// 選択オブジェクトを走査します。
				for (decltype(c) i = 0; i < c; i++)
				{
					// 選択オブジェクトを選択します。
					select_object(magi.exin.get_object_selection(i));
				}
			}
			// 選択オブジェクトが存在しない場合は
			else
			{
				// カレントオブジェクトを選択します。
				select_object(magi.exin.get_current_object_index());
			}
		}

		//
		// アクションを実行します。
		//
		virtual void execute()
		{
			MY_TRACE_FUNC("");

			try
			{
				check();
				apply();
			}
			catch (const std::wstring& error)
			{
				hive.message_box(error);
			}
		}

		//
		// 選択オブジェクトを実際に移動します。
		//
		virtual void move_objects()
		{
			// 選択ノードを走査します。
			for (const auto& mover : selection)
			{
				// オブジェクトを取得します。
				auto object = mover->object;

				// オブジェクトの値を書き換えます。
				object->layer_disp = mover->after.layer_set;
				object->layer_set = mover->after.layer_set;
				object->frame_begin = time_to_frame(mover->after.time_start);
				object->frame_end = time_to_frame(mover->after.time_end);
			}
		}

		//
		// アクションを適用可能かどうかチェックします。
		//
		void check()
		{
			MY_TRACE_FUNC("");

			std::wstring error;

			// 選択ノードを走査します。
			for (const auto& mover : selection)
			{
				if (mover->after.time_start < 0)
				{
					std::format_to(std::back_inserter(error),
						L"レイヤー{}のオブジェクトの開始位置が不正な値になっています\n",
						mover->after.layer_set + 1);

					continue;
				}

				if (mover->after.time_end < 0)
				{
					std::format_to(std::back_inserter(error),
						L"レイヤー{}のオブジェクトの終了位置が不正な値になっています\n",
						mover->after.layer_set + 1);

					continue;
				}

				if (mover->after.time_start >= mover->after.time_end)
				{
					std::format_to(std::back_inserter(error),
						L"レイヤー{}のオブジェクトの長さが不正な値になっています\n",
						mover->after.layer_set + 1);

					continue;
				}

				// すべてのノードを走査します。
				for (const auto& pair : nodes)
				{
					// ノードを取得します。
					const auto& node = pair.second;

					// ノードが選択ノードの移動を妨げているかどうか確認します。

					// ノードと選択ノードが同じオブジェクトを保有している場合は何もしません。
					if (node->object == mover->object) continue;

					// ノードと選択ノードが異なるシーンに存在している場合は何もしません。
					if (node->object->scene_set != mover->object->scene_set) continue;

					// ノードと選択ノードが異なるレイヤーに存在している場合は何もしません。
					if (node->object->layer_set != mover->after.layer_set) continue;

					// ノードと選択ノードが重なっている場合は
					if (node->after.time_start <= mover->after.time_end &&
						node->after.time_end >= mover->after.time_start)
					{
						error += my::format(L"レイヤー{/}で[{/}]のオブジェクトが交差しています\n",
							mover->after.layer_set + 1, time_to_str(mover->after.time_start, mover->after.time_end));
					}
				}
			}

			if (!error.empty()) throw error;
		}

		//
		// 選択ノードのオブジェクトを実際に移動させます。
		//
		void apply()
		{
			MY_TRACE_FUNC("");

			// 選択オブジェクトが存在しない場合は
			if (selection.empty())
				throw std::wstring(L"オブジェクトを選択してください");

			// アンドゥを作成します。
			{
				magi.exin.push_undo();

				for (const auto& mover : selection)
					magi.exin.create_undo(mover->object_index, 0x00);
			}

			// 実際にオブジェクトを移動します。
			move_objects();

			// 拡張編集を再描画します。
			magi.exin.update_object_table();
			magi.exin.invalidate();

			// AviUtlを再描画します。
			magi.redraw();
		}
	};
}
