#pragma once

namespace apn::optima::part
{
	//
	// このクラスは拡張編集のset_current_object()をフックします。
	//
	inline struct SetCurrentObjectHook : Node
	{
		//
		// 初期化処理を実行します。
		//
		virtual BOOL func_init(AviUtl::FilterPlugin* fp) override
		{
			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			my::hook::attach(SetCurrentObject, magi.exin.exedit + 0x305E0);

			DetourTransactionCommit();

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL func_exit(AviUtl::FilterPlugin* fp) override
		{
			return TRUE;
		}

		//
		// このクラスは拡張編集のSetCurrentObject()をフックします。
		//
		inline static struct {
			inline static void CDECL hook_proc(int32_t object_index)
			{
				MY_TRACE_FUNC("{/}", object_index);

				my::scope_exit scope_exit(
					[start_time = ::timeGetTime()]()
				{
					auto elapsed_time = ::timeGetTime() - start_time;
					::SetWindowText(hive.main_window, my::format(
						L"{/}ミリ秒でアイテムを切り替えました\n", elapsed_time).c_str());
				});

				return orig_proc(object_index);
			}
			inline static decltype(&hook_proc) orig_proc = {};
		} SetCurrentObject;
	} set_current_object_hook;
}
