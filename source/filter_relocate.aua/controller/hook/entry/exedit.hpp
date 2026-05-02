#pragma once

namespace apn::filter_relocate::controller::hook
{
	//
	// このクラスは拡張編集をフックします。
	//
	inline struct exedit_t : entry_t
	{
		//
		// 初期化処理を実行します。
		//
		virtual BOOL on_init() override
		{
			MY_TRACE_FUNC("");

			// APIフックを開始します。
			{
				my::hook::detours detours;
				my::hook::attach(create_undo, magi.exin.address.function.create_undo);
			}

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL on_exit() override
		{
			MY_TRACE_FUNC("");

			// APIフックを終了します。
			{
				my::hook::detours detours;
//				my::hook::detach(ExtTextOutW);
			}

			return TRUE;
		}

		//
		// このクラスは拡張編集の内部関数create_undo()をフックします。
		//
		struct {
			inline static void CDECL hook_proc(int32_t object_index, uint32_t flags)
			{
				MY_TRACE_FUNC("{/}, {/}", object_index, flags);

				// オブジェクトがアンドゥ除外対象の場合は何もしません。
				if (model::property.no_undos.contains(object_index)) return;

				return orig_proc(object_index, flags);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} create_undo;
	} exedit;
}
