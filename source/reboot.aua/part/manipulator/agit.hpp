#pragma once

namespace apn::reboot::manipulator
{
	//
	// このクラスはマニピュレータのアジトです。
	//
	inline struct Agit
	{
		//
		// TRUEの場合はヒープを最適化します。
		//
		BOOL optimize_heap = TRUE;

		//
		// TRUEの場合はメモリを掃除します。
		//
		BOOL clean_memory = TRUE;
	} agit;
}
