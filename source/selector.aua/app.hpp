#pragma once

namespace apn::selector
{
	//
	// このクラスはアプリケーションです。
	//
	inline struct App : AppInterface
	{
		//
		// コンフィグとコントロールを更新します。
		//
		virtual BOOL update() override
		{
//			read_custom_objects_from_exedit();
//			addin_dialog.update_list_control();

			return TRUE;
		}
	} app_impl;
}
