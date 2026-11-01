#pragma once

namespace apn::item_list
{
	//
	// フィルタの名前を返します。
	//
	inline LPCSTR get_filter_name(ExEdit::Object* object, int filter_index)
	{
		if (filter_index < 0)
		{
			if (object->dispname[0])
				return object->dispname;

			filter_index = 0;
		}

		auto filter = magi.exin.get_filter(object, filter_index);
		if (!filter) return "";

		auto id = object->filter_param[filter_index].id;
		if (id == 79) // アニメーション効果
		{
			auto exdata = magi.exin.get_exdata(object, filter_index);
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
		else if (id == 80) // カスタムオブジェクト
		{
			auto exdata = magi.exin.get_exdata(object, filter_index);
			auto name = (LPCSTR)(exdata + 0x04);
			if (!name[0]) name = "カスタムオブジェクト";
			return name;
		}
		else
		{
			return filter->name;
		}
	}
}
