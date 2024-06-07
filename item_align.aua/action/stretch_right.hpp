#pragma once

namespace apn::item_align
{
	//
	// このクラスは指定された時間まで
	// タイムラインアイテムを右に伸ばします。
	//
	struct StretchRight : Action
	{
		//
		// アクションを実行します。
		//
		virtual void execute() override
		{
			MY_TRACE_FUNC("");

			// 伸縮量(相対値)を取得します。
			auto stretch_time = str_to_time(hive.stretch_time);
			MY_TRACE_REAL(stretch_time);

			// 基準値(時間)を取得します。
			auto base_time_end = get_base_time_end(bound.time_end);
			MY_TRACE_REAL(base_time_end);

			// レイヤーを走査します。
			for (auto& pair : layers)
			{
				// レイヤーを取得します。
				auto& layer = pair.second;
				MY_TRACE_INT(layer->layer_set);

				// 終了位置です。この位置までノードを伸ばします。
				auto time_end = base_time_end + stretch_time;
				MY_TRACE_REAL(time_end);

				// ノードを逆順に走査します。
				for (auto& node : reverse_iteration(layer->nodes))
				{
					// 選択ノードの場合は
					if (is_selected(node))
					{
						// 終了位置を変更しても範囲が不正にならない場合は
						if (node->after.time_start < time_end)
						{
							// 終了位置を伸ばします。
							node->after.time_end = time_end;
							MY_TRACE_REAL(node->after.time_end);
						}
					}

					// 終了位置を更新します。
					time_end = std::min(time_end, to_time_end(node->before.time_start));
					MY_TRACE_REAL(time_end);
				}
			}

			return __super::execute();
		}
	};
}
