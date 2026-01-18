#pragma once

namespace apn::filter_drag
{
	//
	// このクラスはアドインダイアログです。
	//
	inline struct AddinDialog : StdAddinDialog<idd_main>
	{
		//
		// コントロールを更新します。
		//
		virtual void on_update_controls() override
		{
			MY_TRACE_FUNC("");

			set_int(idc_gripper_height, hive.gripper_height);
			set_check(idc_show_aim_src, aim_src.available);
			set_check(idc_show_aim_dst, aim_dst.available);
			set_check(idc_show_sight, sight.config.available);
			set_check(idc_use_shift, keyboard_hook.use_shift);
			set_check(idc_use_ctrl, keyboard_hook.use_ctrl);
			set_check(idc_use_alt, keyboard_hook.use_alt);
			set_check(idc_use_win, keyboard_hook.use_win);
		}

		//
		// コンフィグを更新します。
		//
		virtual void on_update_config() override
		{
			MY_TRACE_FUNC("");

			get_int(idc_gripper_height, hive.gripper_height);
			get_check(idc_show_aim_src, aim_src.available);
			get_check(idc_show_aim_dst, aim_dst.available);
			get_check(idc_show_sight, sight.config.available);
			get_check(idc_use_shift, keyboard_hook.use_shift);
			get_check(idc_use_ctrl, keyboard_hook.use_ctrl);
			get_check(idc_use_alt, keyboard_hook.use_alt);
			get_check(idc_use_win, keyboard_hook.use_win);
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
			// エディットボックス
			case idc_gripper_height:
				if (code == EN_CHANGE) update_config(); break;

			// チェックボックス
			case idc_show_aim_src:
			case idc_show_aim_dst:
			case idc_show_sight:
			case idc_use_shift:
			case idc_use_ctrl:
			case idc_use_alt:
			case idc_use_win:
				update_config(); break;
			}
		}
	} addin_dialog;
}
