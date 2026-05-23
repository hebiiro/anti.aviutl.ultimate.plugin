#pragma once

namespace apn::filter_copy::model
{
	//
	// このクラスはモデルのプロパティ(固体データ)です。
	//
	inline struct property_t
	{
		my::Mutex mutex { nullptr, FALSE, L"aviutl_filter_copy" };
	} property;
}
