#pragma once

#define MY_TRACE_FILTER_HOLDER(filter) \
do { \
	MY_TRACE(#filter _T(" = {}\n"), filter.get_filter_index()); \
} while (false)

namespace apn
{
	struct FilterHolder
	{
		ObjectHolder object;
		int32_t filter_index = -1;
		ExEdit::Filter* filter = nullptr;

		FilterHolder()
		{
		}

		FilterHolder(const ObjectHolder& object, int32_t filter_index)
			: object(object)
		{
			if (this->object.is_valid())
			{
				this->filter_index = filter_index;
				if (this->filter_index >= 0)
				{
					this->filter = magi.exin.get_filter(this->object.get_object(), this->filter_index);
					if (!this->filter)
						this->filter_index = -1;
				}
			}
		}

		const ObjectHolder& get_object() const
		{
			return object;
		}

		int32_t get_filter_index() const
		{
			return filter_index;
		}

		ExEdit::Filter* get_filter() const
		{
			return filter;
		}

		BOOL is_valid() const
		{
			if (filter_index < 0) return FALSE;
			if (!filter) return FALSE;
			return TRUE;
		}

		bool operator==(const FilterHolder& x) const
		{
			return object == x.object && filter_index == x.filter_index && filter == x.filter;
		}

		bool operator!=(const FilterHolder& x) const
		{
			return !operator==(x);
		}

		BOOL is_moveable() const
		{
			auto id = object.get_object()->filter_param[filter_index].id;
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

		LPCSTR get_name() const
		{
			auto object_index = get_object().get_object_index();
			auto midpt_leader = get_object().get_object()->index_midpt_leader;
			MY_TRACE_INT(midpt_leader);
			if (midpt_leader >= 0) object_index = midpt_leader;

			ObjectHolder object(object_index);

			auto id = object.get_object()->filter_param[filter_index].id;
			if (id == 79) // アニメーション効果
			{
				auto exdata = magi.exin.get_exdata(object.get_object(), filter_index);
				auto name = (LPCSTR)(exdata + 0x04);
				if (!name[0])
				{
					auto type = *(WORD*)(exdata + 0);
					MY_TRACE_HEX(type);

					auto filter = *(WORD*)(exdata + 2);
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
				return filter->name;
			}
		}
	};
}
