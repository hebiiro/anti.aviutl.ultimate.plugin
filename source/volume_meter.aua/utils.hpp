#pragma once

namespace apn::volume_meter
{
	//
	// 生のポインタをユニークポインタに変換して返します。
	//
	template <typename T>
	inline std::unique_ptr<T> to_unique(T* p)
	{
		return std::unique_ptr<T>(p);
	}
}
