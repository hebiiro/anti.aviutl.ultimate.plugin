#pragma once

namespace apn::text_editor
{
	//
	// このクラスはフックを管理します。
	//
	inline struct HookManager
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			setting_dialog.init();

			// 拡張編集のベースアドレスを取得します。
			auto exedit = magi.exin.get_exedit();

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			my::hook::attach(init_text_item_controls, exedit + 0x8C310);

			if (DetourTransactionCommit() == NO_ERROR)
			{
				MY_TRACE("拡張編集のフックに成功しました\n");

				return TRUE;
			}
			else
			{
				MY_TRACE("拡張編集のフックに失敗しました\n");

				return FALSE;
			}
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			text_editbox.exit();
			setting_dialog.exit();

			return TRUE;
		}

		//
		// このクラスは拡張編集内のinit_text_item_controls()の呼び出しをフックします。
		//
		inline static struct {
			inline static int32_t hook_proc(uint32_t u1, uint32_t u2, uint32_t u3, uint32_t u4, uint32_t u5, ExEdit::Filter* filter)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/hex}, {/hex}, {/hex}", u1, u2, u3, u4, u5, filter);

				// 先にテキストアイテムのコントロールを作成させます。
				auto result = orig_proc(u1, u2, u3, u4, u5, filter);

				// テキストエディットボックスをフックします。
				text_editbox.init();

				return result;
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} init_text_item_controls;
	} hook_manager;
}
