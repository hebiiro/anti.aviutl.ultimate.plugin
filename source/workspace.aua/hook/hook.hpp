﻿#pragma once

namespace apn::workspace
{
	//
	// このクラスはフックを管理します。
	//
	inline struct HookManager
	{
		//
		// 先行して初期化処理を実行します。
		//
		BOOL pre_init()
		{
			MY_TRACE_FUNC("");

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			hook::aviutl.init();
			hook::menu.init();
			hook::window.init();
			hook::get_message.init();
			hook::move_cursor.init();
			hook::hide_window.init();

			if (DetourTransactionCommit() == NO_ERROR)
			{
				MY_TRACE("APIフックに成功しました\n");

				return TRUE;
			}
			else
			{
				MY_TRACE("APIフックに失敗しました\n");

				return FALSE;
			}
		}

		//
		// 最後の後始末処理を実行します。
		//
		BOOL post_exit()
		{
			MY_TRACE_FUNC("");

			hook::hide_window.exit();
			hook::move_cursor.exit();
			hook::get_message.exit();
			hook::window.exit();
			hook::menu.exit();
			hook::aviutl.exit();

			return TRUE;
		}

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			hook::exedit.init();
			hook::vsthost.init();
			hook::color_palette.init();
			hook::extoolbar.init();
			hook::z_ram_preview.init();

			if (DetourTransactionCommit() == NO_ERROR)
			{
				MY_TRACE("APIフックに成功しました\n");

				return TRUE;
			}
			else
			{
				MY_TRACE("APIフックに失敗しました\n");

				return FALSE;
			}
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			hook::z_ram_preview.exit();
			hook::extoolbar.exit();
			hook::color_palette.exit();
			hook::vsthost.exit();
			hook::exedit.exit();

			return TRUE;
		}
	} hook_manager;
}
