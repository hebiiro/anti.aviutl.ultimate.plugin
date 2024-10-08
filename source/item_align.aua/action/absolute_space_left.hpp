﻿#pragma once

namespace apn::item_align
{
	//
	// このクラスは指定された間隔(絶対値)が空くように
	// タイムラインアイテムを左に向かって配置します。
	//
	struct AbsoluteSpaceLeft : Action
	{
		//
		// アクションを実行します。
		//
		virtual void execute() override
		{
			MY_TRACE_FUNC("");

			// ずらす量(時間)を取得します。
			auto shift_time = str_to_time(hive.absolute_space_time);
			MY_TRACE_REAL(shift_time);

			// 基準位置(時間)を取得します。
			auto base_time_end = get_base_time_end(bound.time_end);
			MY_TRACE_REAL(base_time_end);

			// レイヤーを走査します。
			for (auto& pair : layers)
			{
				// レイヤーを取得します。
				auto& layer = pair.second;

				// 移動幅です。
				auto amplitude = 0;

				// 追加の移動幅です。
				auto add = 0.0;

				// カレントの中間点リーダーです。
				std::optional<int32_t> current_midpt_leader;

				// ノードを逆順に走査します。
				for (auto& node : reverse_iteration(layer->nodes))
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

								// 追加の移動幅をリセットします。
								add = 0.0;
							}
						}

						// カレントの中間点リーダーを更新します。
						current_midpt_leader = node->object->index_midpt_leader;

						MY_TRACE_INT(amplitude);
						MY_TRACE_REAL(add);

						// 位置をずらします。
						auto range = node->before.time_end - node->before.time_start;
						node->after.time_start = base_time_end - shift_time * amplitude - add - range;
						node->after.time_end = base_time_end - shift_time * amplitude - add;

						MY_TRACE_REAL(node->after.time_start);
						MY_TRACE_REAL(node->after.time_end);

						// 追加の移動幅を増やします。
						add += to_time_start(range);
					}
				}
			}

			return __super::execute();
		}
	};
}
