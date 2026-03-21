#pragma once

namespace apn::filter_copy::view::listener
{
	//
	// このクラスはビュー層のフィルタ作成処理用リスナーです。
	//
	inline struct create_filter_t
	{
		//
		// コンストラクタです。
		//
		create_filter_t() { create_filter = this; }

		virtual LRESULT on_create_filter(uint32_t command_id) = 0;
	} *create_filter = nullptr;
}
