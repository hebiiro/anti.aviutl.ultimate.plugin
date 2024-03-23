#pragma once

namespace apn::editbox_tweaker
{
	//
	// このクラスはエディットボックスを微調整します。
	//
	inline struct EditBoxTweaker : Addin
	{
		//
		// この仮想関数は、このアドインの識別名が必要なときに呼ばれます。
		//
		virtual LPCWSTR get_addin_name() override
		{
			return hive.c_name;
		}

		//
		// この仮想関数は、このアドインの表示名が必要なときに呼ばれます。
		//
		virtual LPCWSTR get_addin_display_name() override
		{
			return hive.c_display_name;
		}

		//
		// この仮想関数は、初期化のタイミングで呼ばれます。
		//
		virtual BOOL on_init() override
		{
			MY_TRACE_FUNC("");

			if (!config_io.init()) return FALSE;
			if (!config_io.read()) MY_TRACE("コンフィグの読み込みに失敗しました\n");
			if (!hook_manager.init()) return FALSE;

			return TRUE;
		}

		//
		// この仮想関数は、後始末のタイミングで呼ばれます。
		//
		virtual BOOL on_exit() override
		{
			MY_TRACE_FUNC("");

			hook_manager.exit();
			config_io.write();
			config_io.exit();

			return TRUE;
		}
	} addin;
}
