#pragma once

namespace fgo
{
	//
	// このクラスはサーヴァントのスーパークラスです。
	//
	struct Servant
	{
		//
		// サーヴァントの識別名を返します。
		//
		virtual LPCWSTR get_servant_name() = 0;

		//
		// 初期化のタイミングで呼び出されます。
		//
		virtual BOOL on_init() = 0;

		//
		// 後始末のタイミングで呼び出されます。
		//
		virtual BOOL on_exit() = 0;
	};
}
