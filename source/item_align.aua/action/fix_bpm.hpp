#pragma once

namespace apn::item_align
{
	//
	// このクラスはBPMずれを修正します。
	//
	struct FixBpm : Action
	{
		//
		// 指定されたフレーム番号を修正して返します。
		//
		int32_t fix_frame(int32_t frame)
		{
			auto right_frame = frame_per_time;
			auto wrong_frame = ceil(right_frame);

			return (int32_t)ceil(frame * right_frame / wrong_frame);
		}

		//
		// 選択オブジェクトを移動します。
		//
		virtual void move_objects() override
		{
			MY_TRACE_FUNC("");

			auto current_frame = magi.exin.get_exedit_current_frame();
			auto fixed_current_frame = fix_frame(current_frame);
			auto diff_current_frame = current_frame - fixed_current_frame;

			// 選択ノードを走査します。
			for (auto& mover : selection)
			{
				// アイテムを取得します。
				auto object = magi.exin.get_object(mover->object_index);

				auto frame_begin = object->frame_begin;
				auto frame_end = object->frame_end;

				if (hive.use_current_frame)
				{
					frame_begin += diff_current_frame;
					frame_end += diff_current_frame;
				}

				// 位置をずらします。
				object->frame_begin = fix_frame(frame_begin);
				object->frame_end = frame_end = fix_frame(frame_end + 1) - 1;

				magi.exin.create_undo(mover->object_index, 0x00);
			}
		}
	};
}
