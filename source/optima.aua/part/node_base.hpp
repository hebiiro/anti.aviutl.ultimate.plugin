#pragma once

namespace apn::optima::part
{
	//
	// このクラスはパーツの基本クラスです。
	//
	struct NodeBase
	{
		virtual BOOL func_init(AviUtl::FilterPlugin* fp) = 0;
		virtual BOOL func_exit(AviUtl::FilterPlugin* fp) = 0;
		virtual BOOL func_window_init(AviUtl::FilterPlugin* fp) { return TRUE; }
		virtual BOOL func_window_exit(AviUtl::FilterPlugin* fp) { return TRUE; }
	};
}
