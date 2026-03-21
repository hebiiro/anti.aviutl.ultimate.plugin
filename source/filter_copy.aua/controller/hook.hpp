#pragma once

namespace apn::filter_copy::controller
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

			my::hook::attach(add_alias, magi.exin.address.function.add_alias);
			my::hook::attach(unknown1, magi.exin.address.function.unknown1);

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
		// このクラスは拡張編集の内部関数add_alias()をフックします。
		//
		struct {
			inline static BOOL CDECL hook_proc(LPCSTR file_name, BOOL flag1, BOOL flag2, int32_t object_index)
			{
				MY_TRACE_FUNC("{/}, {/}, {/}, {/}", file_name, flag1, flag2, object_index);

				return controller::command::paste_filter.on_add_alias(orig_proc, file_name, flag1, flag2, object_index);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} add_alias;

		//
		// このクラスは拡張編集の内部関数unknown1()をフックします。
		//
		struct {
			inline static void CDECL hook_proc(int32_t object_index, int32_t filter_index)
			{
				MY_TRACE_FUNC("{/}, {/}", object_index, filter_index);

				return controller::command::create_filter.on_unknown1(orig_proc, object_index, filter_index);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} unknown1;
	} hook;
}
