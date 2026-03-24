#pragma once

namespace apn::text_split
{
	//
	// このクラスはコントローラ層のフック処理です。
	//
	inline struct hook_t
	{
		//
		// 初期化処理です。
		// 拡張編集の関数をフックします。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			my::hook::attach(create_undo, magi.exin.address.function.create_undo);

			return DetourTransactionCommit() == NO_ERROR;
		}

		//
		// 後始末処理です。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// このクラスは拡張編集の内部関数create_undo()をフックします。
		//
		struct {
			inline static void CDECL hook_proc(int32_t object_index, uint32_t flags)
			{
				MY_TRACE_FUNC("{/}, {/}", object_index, flags);

				// オブジェクトが除外対象の場合は何もしません。
				if (hive.exclude_objects.contains(object_index)) return;

				return orig_proc(object_index, flags);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} create_undo;
	} hook;
}
