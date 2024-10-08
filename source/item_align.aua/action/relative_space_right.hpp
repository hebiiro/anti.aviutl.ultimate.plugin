#pragma once

namespace apn::item_align
{
	//
	// このクラスは指定された間隔(相対値)が空くように
	// タイムラインアイテムを右に移動します。
	//
	struct RelativeSpaceRight : Action
	{
		//
		// アクションを実行します。
		//
		virtual void execute() override
		{
			MY_TRACE_FUNC("");

			// ずらす量(時間)を取得します。
			auto shift_time = str_to_time(hive.relative_space_time);
			MY_TRACE_REAL(shift_time);

			// 基準位置(時間)を取得します。
			auto base_time_end = get_base_time_end(bound.time_end);
			MY_TRACE_REAL(base_time_end);

			// レイヤーを走査します。
			for (auto& pair : layers)
			{
				// レイヤーを取得します。
				auto& layer = pair.second;
				MY_TRACE_INT(layer->layer_set);

				// 移動幅です。
				auto amplitude = 0;

				// カレントの中間点リーダーです。
				std::optional<int32_t> current_midpt_leader;

				// ノードを走査します。
				for (auto& node : layer->nodes)
				{
					// 選択ノードの場合は
					if (is_selected(node))
					{
						// カレントの中間点リーダーが存在する場合は
						if (current_midpt_leader)
						{
							// 中間点リーダーが存在しない、または
							// 中間点リーダーがカレントと異なる場合は
							if (node->object->index_midpt_leader < 0 ||
								node->object->index_midpt_leader != current_midpt_leader.value())
							{
								// 移動幅を増やします。
								amplitude++;
							}
						}

						// カレントの中間点リーダーを更新します。
						current_midpt_leader = node->object->index_midpt_leader;

						MY_TRACE_INT(amplitude);

						// 位置をずらします。
						node->after.time_start = node->before.time_start + shift_time * amplitude;
						node->after.time_end = node->before.time_end + shift_time * amplitude;
					}
				}
			}

			return __super::execute();
		}
	};
}
