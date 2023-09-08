#pragma once

#define MY_TRACE_FILTER_HOLDER(filter) \
do { \
	MY_TRACE(_T(#filter) _T(" = %d\n"), filter.getFilterIndex()); \
} while (0)

namespace fgo
{
	struct FilterHolder
	{
		ObjectHolder m_object;
		int m_filterIndex;
		ExEdit::Filter* m_filter;

		FilterHolder()
			: m_filterIndex(-1)
			, m_filter(0)
		{
		}

		FilterHolder(ObjectHolder object, int filterIndex)
			: m_object(object)
			, m_filterIndex(-1)
			, m_filter(0)
		{
			if (m_object.isValid())
			{
				this->m_filterIndex = filterIndex;
				if (m_filterIndex >= 0)
				{
					m_filter = magi.auin.GetFilter(m_object.getObject(), m_filterIndex);
					if (!m_filter)
						m_filterIndex = -1;
				}
			}
		}

		const ObjectHolder& getObject() const
		{
			return m_object;
		}

		int getFilterIndex() const
		{
			return m_filterIndex;
		}

		ExEdit::Filter* getFilter() const
		{
			return m_filter;
		}

		BOOL isValid() const
		{
			if (m_filterIndex < 0) return FALSE;
			if (!m_filter) return FALSE;
			return TRUE;
		}

		bool operator==(const FilterHolder& x) const
		{
			return m_object == x.m_object && m_filterIndex == x.m_filterIndex && m_filter == x.m_filter;
		}

		bool operator!=(const FilterHolder& x) const
		{
			return !operator==(x);
		}

		BOOL isMoveable() const
		{
			int id = m_object.getObject()->filter_param[m_filterIndex].id;
			switch (id)
			{
			case 0x00: // 動画ファイル
			case 0x01: // 画像ファイル
			case 0x02: // 音声ファイル
			case 0x03: // テキスト
			case 0x04: // 図形
			case 0x05: // フレームバッファ
			case 0x06: // 音声波形
			case 0x07: // シーン
			case 0x08: // シーン(音声)
			case 0x09: // 直前オブジェクト
			case 0x0A: // 標準描画
			case 0x0B: // 拡張描画
			case 0x0C: // 標準再生
			case 0x0D: // パーティクル出力
			case 0x50: // カスタムオブジェクト
			case 0x5D: // 時間制御
			case 0x5E: // グループ制御
			case 0x5F: // カメラ制御
				{
					return FALSE;
				}
			}
			return TRUE;
		}

		LPCSTR getName() const
		{
			int objectIndex = getObject().getObjectIndex();
			int midptLeader = getObject().getObject()->index_midpt_leader;
			MY_TRACE_INT(midptLeader);
			if (midptLeader >= 0) objectIndex = midptLeader;

			ObjectHolder object(objectIndex);

			int id = object.getObject()->filter_param[m_filterIndex].id;
			if (id == 79) // アニメーション効果
			{
				BYTE* exdata = magi.auin.GetExdata(object.getObject(), m_filterIndex);
				LPCSTR name = (LPCSTR)(exdata + 0x04);
				if (!name[0])
				{
					WORD type = *(WORD*)(exdata + 0);
					MY_TRACE_HEX(type);

					WORD filter = *(WORD*)(exdata + 2);
					MY_TRACE_HEX(filter);

					switch (type)
					{
					case 0x00: name = "震える"; break;
					case 0x01: name = "振り子"; break;
					case 0x02: name = "弾む"; break;
					case 0x03: name = "座標の拡大縮小(個別オブジェクト)"; break;
					case 0x04: name = "画面外から登場"; break;
					case 0x05: name = "ランダム方向から登場"; break;
					case 0x06: name = "拡大縮小して登場"; break;
					case 0x07: name = "ランダム間隔で落ちながら登場"; break;
					case 0x08: name = "弾んで登場"; break;
					case 0x09: name = "広がって登場"; break;
					case 0x0A: name = "起き上がって登場"; break;
					case 0x0B: name = "何処からともなく登場"; break;
					case 0x0C: name = "反復移動"; break;
					case 0x0D: name = "座標の回転(個別オブジェクト)"; break;
					case 0x0E: name = "立方体(カメラ制御)"; break;
					case 0x0F: name = "球体(カメラ制御)"; break;
					case 0x10: name = "砕け散る"; break;
					case 0x11: name = "点滅"; break;
					case 0x12: name = "点滅して登場"; break;
					case 0x13: name = "簡易変形"; break;
					case 0x14: name = "簡易変形(カメラ制御)"; break;
					case 0x15: name = "リール回転"; break;
					case 0x16: name = "万華鏡"; break;
					case 0x17: name = "円形配置"; break;
					case 0x18: name = "ランダム配置"; break;
					default: name = "アニメーション効果"; break;
					}
				}
				return name;
			}
			else
			{
				return m_filter->name;
			}
		}
	};
}
