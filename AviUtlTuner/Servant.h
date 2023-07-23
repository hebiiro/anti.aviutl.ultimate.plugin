#pragma once

namespace aut
{
	//
	// このクラスはサーヴァントのスーパークラスです。
	// 基本的に契約したマスターに呼び出された時に処理を行います。
	//
	struct Servant
	{
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
