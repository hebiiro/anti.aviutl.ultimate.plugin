#pragma once

namespace apn::filter_relocate::view
{
	//
	// このクラスはビュー層のメインダイアログです。
	//
	inline struct main_dialog_t : StdAddinDialog<idd_main>
	{
		//
		// このクラスはリスナーです。
		//
		struct listener_t {
			virtual BOOL on_split_item_below() = 0;
		} *listener = {};

		//
		// コントロールを更新します。
		//
		virtual void on_update_controls() override
		{
			MY_TRACE_FUNC("");
		}

		//
		// コンフィグを更新します。
		//
		virtual void on_update_config() override
		{
			MY_TRACE_FUNC("");
		}

		//
		// ダイアログの初期化処理です。
		//
		virtual void on_init_dialog() override
		{
			MY_TRACE_FUNC("");
		}

		//
		// ダイアログのコマンド処理です。
		//
		virtual void on_command(UINT code, UINT id, HWND control) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}", code, id, control);

			switch (id)
			{
			// ボタン
			case idc_split_item_below: listener->on_split_item_below(); break;
			}
		}
	} main_dialog;
}
