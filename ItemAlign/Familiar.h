#pragma once
#include "Hive.h"

namespace fgo::item_align
{
	//
	// このクラスはタイムラインアイテムの位置を変化させる処理を実行します。
	//
	struct Familiar
	{
		struct CommandID
		{
			static const int Shift = 1;
			static const int MoveToBegin = 2;
			static const int MoveToEnd = 3;
			static const int MoveBegin = 4;
			static const int MoveEnd = 5;
			static const int StretchBegin = 6;
			static const int StretchEnd = 7;
			static const int DeleteMidPt = 8;
		};

		struct Mover
		{
			int m_objectIndex;
			ExEdit::Object* m_object;
			int m_frame_begin;
			int m_frame_end;
		};

		AviUtl::EditHandle* m_editp = 0;
		AviUtl::FilterPlugin* m_fp = 0;
		int m_command = -1;

		std::map<ExEdit::Object*, Mover> m_moverMap;
		std::vector<int> m_selectObjects;

		Familiar()
		{
			m_fp = magi.fp;
			m_editp = magi.auin.GetEditp();
		}

		BOOL getSelectObjects()
		{
			MY_TRACE(_T("getSelectObjects()\n"));

			// 選択オブジェクトの数を取得する。
			int c = magi.auin.GetSelectedObjectCount();
			MY_TRACE_INT(c);

			// 選択オブジェクトが存在しないなら
			if (!c)
			{
				// カレントオブジェクトのインデックスを取得する。
				int objectIndex = magi.auin.GetCurrentObjectIndex();
				MY_TRACE_INT(objectIndex);

				if (objectIndex < 0)
					return FALSE;

				m_selectObjects.push_back(objectIndex);

				return TRUE;
			}

			for (int i = 0; i < c; i++)
			{
				// 選択オブジェクトのインデックスを取得する。
				int objectIndex = magi.auin.GetSelectedObject(i);
				MY_TRACE_INT(objectIndex);

				m_selectObjects.push_back(objectIndex);
			}

			return TRUE;
		}

		BOOL createMoverMap()
		{
			MY_TRACE_FUNC("");

			// 選択オブジェクトを取得できなかった場合は
			if (!getSelectObjects())
			{
				// エラーメッセージを出して終了します。
				hive.messageBox(L"選択アイテムがありません");

				return FALSE;
			}

			int stairFrame = hive.frame;
			int stairStep = 0;

			for (int objectIndex : m_selectObjects)
			{
				MY_TRACE_INT(objectIndex);
				if (objectIndex < 0) continue;

				// 選択オブジェクトを取得する。
				ExEdit::Object* object = magi.auin.GetObject(objectIndex);
				MY_TRACE_HEX(object);
				if (!object) continue;

				if (m_moverMap.find(object) != m_moverMap.end())
					continue; // すでに mover が作成済みならスキップする。

				int midptLeader = object->index_midpt_leader;
				MY_TRACE_INT(midptLeader);

				// 中間点があるなら
				if (midptLeader >= 0)
				{
					objectIndex = midptLeader; // 中間点元のオブジェクト ID を取得

					while (objectIndex >= 0)
					{
						// オブジェクトを取得する。
						ExEdit::Object* object = magi.auin.GetObject(objectIndex);
						MY_TRACE_HEX(object);
						if (!object) break;

						int midptLeader2 = object->index_midpt_leader;
						MY_TRACE_INT(midptLeader2);
						if (midptLeader2 != midptLeader) break;

						addMover(objectIndex, object, stairFrame * stairStep);

						objectIndex = magi.auin.GetNextObjectIndex(objectIndex);
					}
				}
				// 中間点がないなら
				else
				{
					addMover(objectIndex, object, stairFrame * stairStep);
				}

				stairStep++;
			}

			return TRUE;
		}

		BOOL addMover(int objectIndex, ExEdit::Object* object, int frame_offset)
		{
			int currentFrame = magi.auin.GetExEditCurrentFrame();

			int frame_begin = object->frame_begin;
			int frame_end = object->frame_end;

			switch (m_command)
			{
			case CommandID::Shift:
				{
					frame_begin += frame_offset;
					frame_end += frame_offset;

					break;
				}
			case CommandID::MoveBegin:
				{
					frame_begin = currentFrame;
					frame_end = currentFrame + (object->frame_end - object->frame_begin);

					break;
				}
			case CommandID::MoveEnd:
				{
					frame_end = currentFrame - 1;
					frame_begin = currentFrame - 1 - (object->frame_end - object->frame_begin);

					break;
				}
			case CommandID::StretchBegin:
				{
					frame_begin = currentFrame;

					break;
				}
			case CommandID::StretchEnd:
				{
					frame_end = currentFrame - 1;

					break;
				}
			}

			if (frame_begin > frame_end)
				return FALSE;

			m_moverMap[object] = Mover(objectIndex, object, frame_begin, frame_end);

			return TRUE;
		}

		int checkMoverMap()
		{
			MY_TRACE_FUNC("");

			int invalidCount = 0;

			for (auto& value : m_moverMap)
			{
				Mover& mover = value.second;

				if (mover.m_frame_begin < 0)
				{
					// mover が範囲外になっている。
					invalidCount++;
					continue;
				}

				// オブジェクトの数を取得する。
				int c = magi.auin.GetCurrentSceneObjectCount();
				MY_TRACE_INT(c);

				for (int i = 0; i < c; i++)
				{
					// オブジェクトを取得する。
					ExEdit::Object* object = magi.auin.GetSortedObject(i);
					MY_TRACE_HEX(object);
					if (!object) continue;

					// object が mover と同じなら
					if (object == mover.m_object)
						continue; // スキップする。

					// レイヤーが違うなら
					if (object->layer_set != mover.m_object->layer_set)
						continue; // スキップする。

					// オブジェクトのフレームを取得する。
					int frame_begin = object->frame_begin;
					int frame_end = object->frame_end;

					// mover があれば
					auto it = m_moverMap.find(object);
					if (it != m_moverMap.end())
					{
						// mover からフレームを取得する。
						frame_begin = it->second.m_frame_begin;
						frame_end = it->second.m_frame_end;
					}

					if (frame_end < mover.m_frame_begin)
						continue; // object が mover より完全に前にあるのでスキップする。

					if (mover.m_frame_end < frame_begin)
						continue; // object が mover より完全に後ろにあるのでスキップする。

					// object と mover は重なっている。
					invalidCount++;
					break;
				}
			}

			return invalidCount;
		}

		BOOL applyMoverMap()
		{
			MY_TRACE_FUNC("");

			if (m_moverMap.empty())
			{
				// エラーメッセージを出して終了します。
				hive.messageBox(L"動かせるアイテムがありません");

				return FALSE;
			}

			// オブジェクトを動かせるかチェックします。

			int invalidCount = checkMoverMap();

			// オブジェクトが重なっていて動かせないなら
			if (invalidCount)
			{
				// エラーメッセージを出して終了します。
				hive.messageBox(L"%d個のアイテムの位置が無効で動かせません", invalidCount);

				return FALSE;
			}

			// Undoを作成します。

			magi.auin.PushUndo();

			for (auto& value : m_moverMap)
			{
				const Mover& mover = value.second;
				int objectIndex = mover.m_objectIndex;

				magi.auin.CreateUndo(objectIndex, 8);
			}

			// 実際にオブジェクトを動かし、レイヤーを再計算します。

			int flags[100] = {};

			for (auto& value : m_moverMap)
			{
				const Mover& mover = value.second;
				ExEdit::Object* object = mover.m_object;

				object->frame_begin = mover.m_frame_begin;
				object->frame_end = mover.m_frame_end;

				flags[object->layer_set] = 1;
			}

			// 拡張編集を再描画します。
			magi.auin.RedrawLayers(flags);

			// AviUtlを再描画します。
			magi.redraw();

			return TRUE;
		}

		// アイテムを左に動かします。
		BOOL moveBeginInternal(BOOL isMove)
		{
			MY_TRACE_FUNC("%d", isMove);

			// 選択オブジェクトを取得できなかった場合は
			if (!getSelectObjects())
			{
				// エラーメッセージを出して終了します。
				hive.messageBox(L"選択アイテムがありません");

				return FALSE;
			}

			// レイヤーの最大数。
			const int LayerMaxSize = 100;

			// 新しい Undo を作成する。
			magi.auin.PushUndo();

			// レイヤーを再描画するためのフラグ。
			int redrawLayerFlags[LayerMaxSize] = {};

			for (int layer = 0; layer < LayerMaxSize; layer++)
			{
				std::vector<int> objects;

				// オブジェクトの数を取得する。
				int c = magi.auin.GetCurrentSceneObjectCount();
				MY_TRACE_INT(c);

				for (int i = 0; i < c; i++)
				{
					// オブジェクトを取得する。
					ExEdit::Object* object = magi.auin.GetSortedObject(i);
					MY_TRACE_HEX(object);
					if (!object) continue;

					if (layer != object->layer_set)
						continue;

					objects.push_back(i);
				}

				if (objects.empty())
					continue;

				redrawLayerFlags[layer] = TRUE;

				// オブジェクトを時間順にソートする。
				std::sort(objects.begin(), objects.end(), [](int a, int b) {
					ExEdit::Object* objectA = magi.auin.GetSortedObject(a);
					ExEdit::Object* objectB = magi.auin.GetSortedObject(b);
					return objectA->frame_begin < objectB->frame_begin;
				});

				// 移動先。
				int currentPos = m_fp->exfunc->get_frame(m_editp);

				for (int objectIndex : objects)
				{
					// オブジェクトを取得する。
					ExEdit::Object* object = magi.auin.GetSortedObject(objectIndex);

					// このオブジェクトが選択されているか調べる。
					auto result = std::find_if(m_selectObjects.begin(), m_selectObjects.end(), [object](int x) {
						return object == magi.auin.GetObject(x);
					});

					// このオブジェクトが選択されているなら
					if (result != m_selectObjects.end())
					{
						// オブジェクトの移動量を算出する。
						int move = currentPos - object->frame_begin;

						// オブジェクトの移動は前方に限定する。
						if (move < 0)
						{
							// このオブジェクトを Undo に加える。
							magi.auin.CreateUndo(*result, 8);

							// オブジェクトを移動する。
							object->frame_begin += move;
							if (isMove) object->frame_end += move;
						}
					}

					// 移動先よりオブジェクトの終了位置が後方なら
					if (currentPos < object->frame_end + 1)
					{
						// 移動先を更新する。
						currentPos = object->frame_end + 1;
					}
				}
			}

			// 拡張編集を再描画します。
			magi.auin.RedrawLayers(redrawLayerFlags);

			// AviUtlを再描画します。
			magi.redraw();

			return TRUE;
		}

		// アイテムを右に動かします。
		BOOL moveEndInternal(BOOL isMove)
		{
			MY_TRACE_FUNC("%d", isMove);

			// 選択オブジェクトを取得できなかった場合は
			if (!getSelectObjects())
			{
				// エラーメッセージを出して終了します。
				hive.messageBox(L"選択アイテムがありません");

				return FALSE;
			}

			// レイヤーの最大数。
			const int LayerMaxSize = 100;

			// 新しい Undo を作成する。
			magi.auin.PushUndo();

			// レイヤーを再描画するためのフラグ。
			int redrawLayerFlags[LayerMaxSize] = {};

			for (int layer = 0; layer < LayerMaxSize; layer++)
			{
				std::vector<int> objects;

				// オブジェクトの数を取得する。
				int c = magi.auin.GetCurrentSceneObjectCount();
				MY_TRACE_INT(c);

				for (int i = 0; i < c; i++)
				{
					// オブジェクトを取得する。
					ExEdit::Object* object = magi.auin.GetSortedObject(i);
					MY_TRACE_HEX(object);
					if (!object) continue;

					if (layer != object->layer_set)
						continue;

					objects.push_back(i);
				}

				if (objects.empty())
					continue;

				redrawLayerFlags[layer] = TRUE;

				// オブジェクトを時間順にソートする。
				std::sort(objects.begin(), objects.end(), [](int a, int b) {
					ExEdit::Object* objectA = magi.auin.GetSortedObject(a);
					ExEdit::Object* objectB = magi.auin.GetSortedObject(b);
					return objectA->frame_begin > objectB->frame_begin;
				});

				// 移動先。
				int currentPos = m_fp->exfunc->get_frame(m_editp);

				for (int objectIndex : objects)
				{
					// オブジェクトを取得する。
					ExEdit::Object* object = magi.auin.GetSortedObject(objectIndex);

					// このオブジェクトが選択されているか調べる。
					auto result = std::find_if(m_selectObjects.begin(), m_selectObjects.end(), [object](int x) {
						return object == magi.auin.GetObject(x);
					});

					// このオブジェクトが選択されているなら
					if (result != m_selectObjects.end())
					{
						// オブジェクトの移動量を算出する。
						int move = currentPos - object->frame_end;

						// オブジェクトの移動は後方に限定する。
						if (move > 0)
						{
							// このオブジェクトを Undo に加える。
							magi.auin.CreateUndo(*result, 8);

							// オブジェクトを移動する。
							if (isMove) object->frame_begin += move;
							object->frame_end += move;
						}
					}

					// 移動先よりオブジェクトの終了位置が前方なら
					if (currentPos > object->frame_begin - 1)
					{
						// 移動先を更新する。
						currentPos = object->frame_begin - 1;
					}
				}
			}

			// レイヤーを更新する。
			magi.auin.RedrawLayers(redrawLayerFlags);

			{
				// 設定ダイアログを再描画する。

				int objectIndex = magi.auin.GetCurrentObjectIndex();
				MY_TRACE_INT(objectIndex);

				if (objectIndex >= 0)
				{
					// ここが機能していない。
					magi.auin.DrawSettingDialog(objectIndex);
					::InvalidateRect(magi.auin.GetSettingDialog(), 0, TRUE);
				}
			}

			// AviUtlを再描画します。
			magi.redraw();

			return TRUE;
		}

		BOOL moveCurrentFrame()
		{
			MY_TRACE_FUNC("");

			// 選択オブジェクトを取得できなかった場合は
			if (!getSelectObjects())
			{
				// エラーメッセージを出して終了します。
				hive.messageBox(L"選択アイテムがありません");

				return FALSE;
			}

			int frame = -1;

			for (int objectIndex : m_selectObjects)
			{
				MY_TRACE_INT(objectIndex);
				if (objectIndex < 0) continue;

				// 選択オブジェクトを取得します。
				ExEdit::Object* object = magi.auin.GetObject(objectIndex);
				MY_TRACE_HEX(object);
				if (!object) continue;

				if (m_command == CommandID::MoveToBegin)
				{
					if (frame < 0)
						frame = object->frame_begin;
					else
						frame = std::min(frame, object->frame_begin);
				}
				else if (m_command == CommandID::MoveToEnd)
				{
					if (frame < 0)
						frame = object->frame_end + 1;
					else
						frame = std::max(frame, object->frame_end + 1);
				}
			}

			// フレームが無効なら
			if (frame < 0)
			{
				// エラーメッセージを出して終了します。
				hive.messageBox(L"現在フレームを動かせませんでした");

				return FALSE;
			}

//			magi.auin.SetExEditCurrentFrame(frame);
//			::InvalidateRect(magi.auin.GetExEditWindow(), 0, FALSE);
			m_fp->exfunc->set_frame(m_editp, frame);
			magi.redraw();

			return TRUE;
		}

		//
		// 選択アイテムを階段状にずらします。
		//
		BOOL shift()
		{
			MY_TRACE_FUNC("");

			m_command = CommandID::Shift;

			// オブジェクトを動かしたあとの状態を構築する。
			createMoverMap();

			// 実際にオブジェクトを動かします。
			return applyMoverMap();
		}

		//
		// 現在フレームを選択アイテムの開始位置に移動します。
		//
		BOOL moveToBegin()
		{
			MY_TRACE_FUNC("");

			m_command = CommandID::MoveToBegin;

			return moveCurrentFrame();
		}

		//
		// 現在フレームを選択アイテムの終了位置に移動します。
		//
		BOOL moveToEnd()
		{
			MY_TRACE_FUNC("");

			m_command = CommandID::MoveToEnd;

			return moveCurrentFrame();
		}

		//
		// アイテムの開始位置を動かします。終了位置もそれに連動して動かします。
		//
		BOOL moveBegin()
		{
			MY_TRACE_FUNC("");

			m_command = CommandID::MoveBegin;

			return moveBeginInternal(TRUE);
		}

		//
		// アイテムの終了位置を動かします。開始位置もそれに連動して動かします。
		//
		BOOL moveEnd()
		{
			MY_TRACE_FUNC("");

			m_command = CommandID::MoveEnd;

			return moveEndInternal(TRUE);
		}

		//
		// アイテムの開始位置を左に伸ばします。終了位置は動かしません。
		//
		BOOL stretchBegin()
		{
			MY_TRACE_FUNC("");

			m_command = CommandID::StretchBegin;

			return moveBeginInternal(FALSE);
		}

		//
		// アイテムの終了位置を右に伸ばします。開始位置は動かしません。
		//
		BOOL stretchEnd()
		{
			MY_TRACE_FUNC("");

			m_command = CommandID::StretchEnd;

			return moveEndInternal(FALSE);
		}

		//
		// 現在フレームに最も近い中間点を削除します。
		//
		BOOL deleteMidPt()
		{
			MY_TRACE_FUNC("");

			m_command = CommandID::DeleteMidPt;

			int currentFrame = m_fp->exfunc->get_frame(m_editp);

			int objectIndex = magi.auin.GetCurrentObjectIndex();
			MY_TRACE_INT(objectIndex);
			if (objectIndex < 0) return FALSE;

			// 選択オブジェクトを取得する。
			ExEdit::Object* object = magi.auin.GetObject(objectIndex);
			MY_TRACE_HEX(object);
			if (!object) return FALSE;

			int midptLeader = object->index_midpt_leader;
			MY_TRACE_INT(midptLeader);

			// 中間点がないなら何もしない。
			if (midptLeader < 0) return FALSE;

			int layerIndex = object->layer_disp;
			int nearObjectIndex = -1;
			int nearFrameBegin = -1;

			objectIndex = midptLeader; // 中間点元のオブジェクト ID を取得

			while (objectIndex >= 0)
			{
				if (objectIndex != midptLeader)
				{
					// オブジェクトを取得する。
					ExEdit::Object* object = magi.auin.GetObject(objectIndex);
					MY_TRACE_HEX(object);
					if (!object) break;

					// 中間点の先頭が異なる場合はループ終了。
					int midptLeader2 = object->index_midpt_leader;
					MY_TRACE_INT(midptLeader2);
					if (midptLeader2 != midptLeader) break;

					// 最初のオブジェクトなら
					if (nearFrameBegin == -1)
					{
						nearObjectIndex = objectIndex;
						nearFrameBegin = object->frame_begin;
					}
					// 最初以外のオブジェクトなら
					else
					{
						// 現在フレームとの距離を取得する。
						int distance = abs(currentFrame - nearFrameBegin);
						int distance2 = abs(currentFrame - object->frame_begin);

						// ニアオブジェクトより距離が近いなら
						if (distance2 < distance)
						{
							// ニアオブジェクトを更新する。
							nearObjectIndex = objectIndex;
							nearFrameBegin = object->frame_begin;
						}
					}
				}

				objectIndex = magi.auin.GetNextObjectIndex(objectIndex);
			}

			if (nearObjectIndex < 0) return FALSE;

			magi.auin.PushUndo();
			magi.auin.DeleteMidPoint(nearObjectIndex, 0);
			magi.auin.RedrawLayer(layerIndex);
//			magi.auin.DrawSettingDialog(midptLeader);
			::InvalidateRect(magi.auin.GetSettingDialog(), 0, FALSE);

			return TRUE;
		}
	};
}
