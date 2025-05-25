#pragma once

namespace apn::item_align
{
	//
	// このクラスは指定されたレイヤーまで
	// タイムラインアイテムを下に詰めます。
	//
	struct MoveVertDown : Action
	{
		//
		// アクションを実行します。
		//
		virtual void execute() override
		{
			MY_TRACE_FUNC("");

			// レイヤー行数を取得します。
			auto row_count = std::max(1, hive.move_vert_layer);
			MY_TRACE_INT(row_count);

			// 基準位置(レイヤー)を取得します。
			// この位置までノードを詰めます。
			auto base_layer = bound.bottom_layer;
			MY_TRACE_INT(base_layer);

			// 基準位置(時間)を取得します。
			// この位置までノードを詰めます。
			auto base_time_start = get_base_time_start(bound.time_start);
			MY_TRACE_REAL(base_time_start);

			struct Cell {
				std::vector<std::shared_ptr<Node>> nodes;
			};

			struct Col {
				std::vector<Cell> cells;
			};

			std::vector<Col> cols;

			// 基準位置(レイヤー)から一番上のレイヤーまで走査します。
			for (auto primary_layer_set = base_layer;
				primary_layer_set >= bound.top_layer;
				primary_layer_set -= row_count)
			{
				// 列のプライマリインデックスです。
				auto primary_col_index = cols.size();

				// 行数分だけ走査します。
				for (auto row_index = 0; row_index < row_count; row_index++)
				{
					// レイヤーインデックスを取得します。
					auto secondary_layer_set = primary_layer_set - row_index;
					if (secondary_layer_set < bound.top_layer) break;

					// レイヤーを取得します。
					auto& layer = layers[secondary_layer_set];
					if (!layer) continue;
					MY_TRACE_INT(layer->layer_set);

					// 列のセカンダリインデックスです。
					auto secondary_col_index = primary_col_index;

					// カレントの中間点リーダーです。
					std::optional<int32_t> current_midpt_leader;

					// ノードを走査します。
					for (auto& node : layer->nodes)
					{
						// 選択ノードの場合は
						if (is_selected(node))
						{
							// アイテムを取得します。
							auto object = magi.exin.get_object(node->object_index);

							// カレントの中間点リーダーが存在する場合は
							if (current_midpt_leader.has_value())
							{
								// 中間点リーダーが存在しない、または
								// 中間点リーダーがカレントと異なる場合は
								if (object->index_midpt_leader < 0 ||
									object->index_midpt_leader != current_midpt_leader.value())
								{
									// 列のセカンダリインデックスを増やします。
									secondary_col_index++;
								}
							}

							MY_TRACE_INT(secondary_col_index);
							MY_TRACE_INT(row_index);

							// カレントの中間点リーダーを更新します。
							current_midpt_leader = object->index_midpt_leader;

							// 行を作成します。
							if (cols.size() < (size_t)secondary_col_index + 1)
								cols.resize(secondary_col_index + 1);

							// 行を取得します。
							auto& col = cols[secondary_col_index];

							// セルを作成します。
							if (col.cells.size() < (size_t)row_count)
								col.cells.resize(row_count);

							// セルを取得します。
							auto& cell = col.cells[row_index];

							// このノードをセルに追加します。
							cell.nodes.emplace_back(node);
						}
					}
				}
			}

			// 開始位置です。
			auto time_start = base_time_start;

			// 列を走査します。
			for (const auto& col : cols)
			{
				// 行のインデックスです。
				auto row_index = 0;

				// 終了位置です。
				auto time_end = to_time_end(time_start);

				// セルを走査します。
				for (const auto& cell : col.cells)
				{
					auto secondary_time_start = time_start;

					for (auto& node : cell.nodes)
					{
						// 開始位置にノードを移動します。
						auto range = node->before.time_end - node->before.time_start;
						node->after.layer_set = base_layer - row_index;
						node->after.time_start = secondary_time_start;
						node->after.time_end = secondary_time_start + range;
						MY_TRACE_REAL(node->after.time_start);
						MY_TRACE_REAL(node->after.time_end);

						// 開始位置と終了位置を更新します。
						secondary_time_start = to_time_start(node->after.time_end);
						time_end = std::max(time_end, node->after.time_end);
					}

					// 行のインデックスを増やします。
					row_index++;
				}

				// この列の最大の終了位置を次の開始位置に設定します。
				time_start = to_time_start(time_end);
			}

			return __super::execute();
		}
	};
}
