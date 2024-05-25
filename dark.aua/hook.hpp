#pragma once

namespace apn::dark
{
	//
	// このクラスはフックを管理します。
	//
	inline struct HookManager
	{
		//
		// 先制して初期化処理を実行します。
		//
		BOOL on_pre_init()
		{
			MY_TRACE_FUNC("");

			if (!hook::draw.init()) return FALSE;
			if (!hook::theme.init()) return FALSE;
			if (!hook::icon.init()) return FALSE;
			if (!hook::window.init()) return FALSE;
			if (!hook::comdlg.init()) return FALSE;
//			if (!hook::program.init()) return FALSE;
			if (!hook::double_buffer.init()) return FALSE;

			return TRUE;
		}

		//
		// 最後の後始末処理を実行します。
		//
		BOOL on_post_exit()
		{
			MY_TRACE_FUNC("");

			hook::double_buffer.exit();
			hook::program.exit();
			hook::comdlg.exit();
			hook::window.exit();
			hook::icon.exit();
			hook::theme.exit();
			hook::draw.exit();

			return TRUE;
		}

		//
		// 初期化処理を実行します。
		//
		BOOL on_init()
		{
			MY_TRACE_FUNC("");

			if (!hook::exedit.init()) return FALSE;
			if (!hook::loudness.init()) return FALSE;

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL on_exit()
		{
			MY_TRACE_FUNC("");

			hook::loudness.exit();
			hook::exedit.exit();

			return TRUE;
		}
	} hook_manager;
}
