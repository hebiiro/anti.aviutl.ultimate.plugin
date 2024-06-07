#pragma once

namespace apn::item_align
{
	//
	// このクラスは指定された時間まで
	// タイムラインアイテムを左に寄せます。
	//
	struct AlignLeft : Action
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
			auto base_time_start = get_base_time_start(bound.time_start);
			MY_TRACE_REAL(base_time_start);

			// レイヤーを走査します。
			for (auto& pair : layers)
			{
				// レイヤーを取得します。
				auto& layer = pair.second;
				MY_TRACE_INT(layer->layer_set);

				// 開始位置です。この位置までノードを詰めます。
				auto time_start = base_time_start + align_time;
				MY_TRACE_REAL(time_start);

				// ノードを走査します。
				for (auto& node : layer->nodes)
				{
					// 選択ノードの場合は
					if (is_selected(node))
					{
						// 開始位置まで詰めます。
						auto range = node->before.time_end - node->before.time_start;
						node->after.time_start = time_start;
						node->after.time_end = time_start + range;
						MY_TRACE_REAL(node->after.time_start);
						MY_TRACE_REAL(node->after.time_end);

						// 開始位置を更新します。
						time_start = to_time_start(node->after.time_end);
						MY_TRACE_REAL(time_start);
					}
				}
			}

			return __super::execute();
		}
	};
}
