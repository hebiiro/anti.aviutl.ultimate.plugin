#pragma once

namespace apn::item_wave::item_cache
{
	//
	// このクラスはアイテムキャッシュのコンテキストです。
	//
	struct Context
	{
		//
		// アイテムインデックスのコレクションです。
		//
		std::vector<int32_t> object_indexes;

		//
		// 再描画予定のレイヤーです。
		//
		int32_t redraw_layers[100] = {};

		//
		// 再描画予定のレイヤーを追加します。
		//
		BOOL add_redraw_layer(int32_t object_index)
		{
			// アイテムを取得します。
			auto object = magi.exin.get_object(object_index);

			// アイテムの表示レイヤーが無効の場合はFALSEを返します。
			if ((size_t)object->layer_disp >= std::size(redraw_layers)) return FALSE;

			// 再描画フラグを立ててTRUEを返します。
			return redraw_layers[object->layer_disp] = TRUE;
		}
	};
}
