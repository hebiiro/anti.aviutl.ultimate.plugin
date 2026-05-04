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
			virtual BOOL on_split_item_above() = 0;
		} *listener = {};

		//
		// コントロールを更新します。
		//
		virtual void on_update_controls() override
		{
			MY_TRACE_FUNC("");

			set_check(idc_flag_relocate_draw_filter, model::property.flag_relocate_draw_filter);
			set_check(idc_flag_up_two_layers, model::property.flag_up_two_layers);
		}

		//
		// コンフィグを更新します。
		//
		virtual void on_update_config() override
		{
			MY_TRACE_FUNC("");

			get_check(idc_flag_relocate_draw_filter, model::property.flag_relocate_draw_filter);
			get_check(idc_flag_up_two_layers, model::property.flag_up_two_layers);
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
			case idc_split_item_above: listener->on_split_item_above(); break;

			// チェックボックス
			case idc_flag_relocate_draw_filter:
			case idc_flag_up_two_layers:
				update_config(); break;
			}
		}
	} main_dialog;
}
