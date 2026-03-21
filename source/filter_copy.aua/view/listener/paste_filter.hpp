#pragma once

namespace apn::filter_copy::view::listener
{
	//
	// このクラスはビュー層のフィルタ貼り付け処理用リスナーです。
	//
	inline struct paste_filter_t
	{
		//
		// コンストラクタです。
		//
		paste_filter_t() { paste_filter = this; }

		virtual LRESULT on_paste_filter() = 0;
	} *paste_filter = nullptr;
}
