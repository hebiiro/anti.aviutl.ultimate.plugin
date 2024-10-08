#pragma once

namespace apn::item_align
{
	//
	// このクラスは指定された時間まで
	// タイムラインアイテムを右に詰めます。
	//
	struct AlignRight : Action
	{
		//
		// アクションを実行します。
		//
		virtual void execute() override
		{
			MY_TRACE_FUNC("");

			// 移動値(時間)を取得します。
			auto align_time = str_to_time(hive.align_time);
			MY_TRACE_REAL(align_time);

			// 基準位置(時間)を取得します。
			auto base_time_end = get_base_time_end(bound.time_end);
			MY_TRACE_REAL(base_time_end);

			// レイヤーを走査します。
			for (auto& pair : layers)
			{
				// レイヤーを取得します。
				auto& layer = pair.second;
				MY_TRACE_INT(layer->layer_set);

				// 終了位置です。この位置までノードを詰めます。
				auto time_end = base_time_end + align_time;
				MY_TRACE_REAL(time_end);

				// ノードを逆順に走査します。
				for (auto& node : reverse_iteration(layer->nodes))
				{
					// 選択ノードの場合は
					if (is_selected(node))
					{
						// 終了位置まで詰めます。
						auto range = node->before.time_end - node->before.time_start;
						node->after.time_start = time_end - range;
						node->after.time_end = time_end;
						MY_TRACE_REAL(node->after.time_start);
						MY_TRACE_REAL(node->after.time_end);

						// 終了位置を更新します。
						time_end = to_time_end(node->after.time_start);
						MY_TRACE_REAL(time_end);
					}
				}
			}

			return __super::execute();
		}
	};
}
