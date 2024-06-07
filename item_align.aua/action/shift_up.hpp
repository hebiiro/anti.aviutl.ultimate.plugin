#pragma once

namespace apn::item_align
{
	//
	// このクラスはタイムラインアイテムを下から順番にずらします。
	//
	struct ShiftUp : Action
	{
		//
		// アクションを実行します。
		//
		virtual void execute() override
		{
			MY_TRACE_FUNC("");

			// ずらす量(時間)を取得します。
			auto shift_time = str_to_time(hive.shift_time);
			MY_TRACE_REAL(shift_time);

			// 基準レイヤーを取得します。
			auto base_layer = bound.bottom_layer;
			MY_TRACE_INT(base_layer);

			// 選択ノードを走査します。
			for (auto& node : selection)
			{
				// 増幅量です。
				auto amplitude = base_layer - node->object->layer_set;
				if (amplitude <= 0) continue;
				MY_TRACE_INT(amplitude);

				// 位置をずらします。
				node->after.time_start = node->before.time_start + shift_time * amplitude;
				node->after.time_end = node->before.time_end + shift_time * amplitude;
			}

			return __super::execute();
		}
	};
}
