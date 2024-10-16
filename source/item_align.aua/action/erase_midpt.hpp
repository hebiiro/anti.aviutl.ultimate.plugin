#pragma once

namespace apn::item_align
{
	//
	// このクラスは現在位置の近くにある中間点を削除します。
	//
	struct EraseMidpt : Action
	{
		//
		// アクションを実行します。
		//
		virtual void execute() override
		{
			MY_TRACE_FUNC("");

			// 現在位置を取得します。
			auto current_frame = fp->exfunc->get_frame(editp);

			// 選択オブジェクトのインデックスを取得します。
			auto object_index = magi.exin.get_current_object_index();
			MY_TRACE_INT(object_index);
			if (object_index < 0) return;

			// 選択オブジェクトを取得します。
			auto object = magi.exin.get_object(object_index);
			MY_TRACE_HEX(object);
			if (!object) return;

			// 中間点リーダーを取得します。
			auto midpt_leader = object->index_midpt_leader;
			MY_TRACE_INT(midpt_leader);

			// 中間点が存在しない場合は何もしません。
			if (midpt_leader < 0) return;

			auto layer_set = object->layer_disp;
			auto near_object_index = -1;
			auto near_frame_begin = -1;

			object_index = midpt_leader;

			while (object_index >= 0)
			{
				if (object_index != midpt_leader)
				{
					// オブジェクトを取得します。
					auto object = magi.exin.get_object(object_index);
					MY_TRACE_HEX(object);
					if (!object) break;

					// 中間点の先頭が異なる場合はループを終了します。
					MY_TRACE_INT(object->index_midpt_leader);
					if (object->index_midpt_leader != midpt_leader) break;

					// 最初に処理されるオブジェクトの場合は
					if (near_frame_begin == -1)
					{
						// このオブジェクトを削除予定の中間点に設定します。
						near_object_index = object_index;
						near_frame_begin = object->frame_begin;
					}
					// それ以外の場合は
					else
					{
						// このオブジェクトが削除予定の中間点より現在フレームに近い場合は
						if (abs(current_frame - object->frame_begin) <
							abs(current_frame - near_frame_begin))
						{
							// このオブジェクトを削除予定の中間点に設定します。
							near_object_index = object_index;
							near_frame_begin = object->frame_begin;
						}
					}
				}

				object_index = magi.exin.get_next_object_index(object_index);
			}

			if (near_object_index < 0) return;

			magi.exin.push_undo();
			magi.exin.erase_midpt(near_object_index, 0);
			magi.exin.redraw_layer(layer_set);
			my::invalidate(magi.exin.get_setting_dialog());
		}
	};
}
