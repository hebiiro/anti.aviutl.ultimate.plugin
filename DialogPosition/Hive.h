#pragma once

namespace fgo::dialog_position
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		HINSTANCE instance = 0;

		//
		// DLLの初期化処理です。
		//
		BOOL dll_init(HINSTANCE instance)
		{
			MY_TRACE_FUNC("0x%08X", instance);

			this->instance = instance;

			return TRUE;
		}

		//
		// DLLの後始末処理です。
		//
		BOOL dll_exit(HINSTANCE instance)
		{
			MY_TRACE_FUNC("0x%08X", instance);

			return TRUE;
		}
	} hive;
}
