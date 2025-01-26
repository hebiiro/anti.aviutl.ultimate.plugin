#pragma once

namespace apn::workspace::hook
{
	//
	// このクラスはフックを管理します。
	//
	inline struct Manager
	{
		//
		// DLL開始のタイミングで初期化処理を実行します。
		//
		BOOL dll_init()
		{
			MY_TRACE_FUNC("");

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			for (auto& entity : Entity::collection)
				entity->on_dll_init();

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
		// DLL終了のタイミングで後始末処理を実行します。
		//
		BOOL dll_exit()
		{
			MY_TRACE_FUNC("");

			for (auto& entity : Entity::collection)
				entity->on_dll_exit();

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

			for (auto& entity : Entity::collection)
				entity->on_init();

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

			for (auto& entity : Entity::collection)
				entity->on_exit();

			return TRUE;
		}
	} manager;
}
