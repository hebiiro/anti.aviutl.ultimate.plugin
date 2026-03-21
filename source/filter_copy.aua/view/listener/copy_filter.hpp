#pragma once

namespace apn::filter_copy::view::listener
{
	//
	// このクラスはビュー層のフィルタコピー処理用リスナーです。
	//
	inline struct copy_filter_t
	{
		//
		// コンストラクタです。
		//
		copy_filter_t() { copy_filter = this; }

		virtual LRESULT on_copy_filter(int32_t flag, BOOL cut) = 0;
	} *copy_filter = nullptr;
}
