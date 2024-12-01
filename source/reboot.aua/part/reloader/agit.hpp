#pragma once

namespace apn::reboot::reloader
{
	//
	// このクラスはリローダーのアジトです。
	//
	inline struct Agit
	{
		//
		// TRUEの場合は上書き保存してからaviutlを再起動します。
		//
		BOOL with_write = TRUE;
	} agit;
}
