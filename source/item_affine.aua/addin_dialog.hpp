#pragma once

namespace apn::item_affine
{
	//
	// このクラスはアドインダイアログです。
	//
	inline struct addin_dialog_t : StdAddinDialog<idd_main>
	{
		//
		// コントロールを更新します。
		//
		virtual void on_update_controls() override
		{
			MY_TRACE_FUNC("");

			set_int(idc_pos_mul_num, hive.pos.mul.num);
			set_int(idc_pos_mul_den, hive.pos.mul.den);
			set_int(idc_pos_add_num, hive.pos.add.num);
			set_int(idc_pos_add_den, hive.pos.add.den);
			set_int(idc_pos_base_num, hive.pos.base.num);
			set_int(idc_pos_base_den, hive.pos.base.den);
			set_int(idc_pos_exp_num, hive.pos.exp.num);
			set_int(idc_pos_exp_den, hive.pos.exp.den);
			set_combobox_index(idc_pos_target, hive.pos.target);
			set_combobox_index(idc_pos_orig, hive.orig);

			set_int(idc_len_mul_num, hive.len.mul.num);
			set_int(idc_len_mul_den, hive.len.mul.den);
			set_int(idc_len_add_num, hive.len.add.num);
			set_int(idc_len_add_den, hive.len.add.den);
			set_int(idc_len_base_num, hive.len.base.num);
			set_int(idc_len_base_den, hive.len.base.den);
			set_int(idc_len_exp_num, hive.len.exp.num);
			set_int(idc_len_exp_den, hive.len.exp.den);
			set_combobox_index(idc_len_target, hive.len.target);
		}

		//
		// コンフィグを更新します。
		//
		virtual void on_update_config() override
		{
			MY_TRACE_FUNC("");

			get_int(idc_pos_mul_num, hive.pos.mul.num);
			get_int(idc_pos_mul_den, hive.pos.mul.den);
			get_int(idc_pos_add_num, hive.pos.add.num);
			get_int(idc_pos_add_den, hive.pos.add.den);
			get_int(idc_pos_base_num, hive.pos.base.num);
			get_int(idc_pos_base_den, hive.pos.base.den);
			get_int(idc_pos_exp_num, hive.pos.exp.num);
			get_int(idc_pos_exp_den, hive.pos.exp.den);
			get_combobox_index(idc_pos_target, hive.pos.target);
			get_combobox_index(idc_pos_orig, hive.orig);

			get_int(idc_len_mul_num, hive.len.mul.num);
			get_int(idc_len_mul_den, hive.len.mul.den);
			get_int(idc_len_add_num, hive.len.add.num);
			get_int(idc_len_add_den, hive.len.add.den);
			get_int(idc_len_base_num, hive.len.base.num);
			get_int(idc_len_base_den, hive.len.base.den);
			get_int(idc_len_exp_num, hive.len.exp.num);
			get_int(idc_len_exp_den, hive.len.exp.den);
			get_combobox_index(idc_len_target, hive.len.target);
		}

		//
		// ダイアログの初期化処理です。
		//
		virtual void on_init_dialog() override
		{
			MY_TRACE_FUNC("");

			init_combobox(idc_pos_target,
				_T("開始位置のみ"),
				_T("終了位置のみ"),
				_T("両方"));

			init_combobox(idc_len_target,
				_T("開始位置のみ"),
				_T("終了位置のみ"),
				_T("両方"));

			init_combobox(idc_pos_orig,
				_T("シーン左端"),
				_T("シーン右端"),
				_T("選択アイテム左端"),
				_T("選択アイテム右端"),
				_T("現在フレーム"));
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
			case idc_pos_execute: app->pos_execute(); break;
			case idc_len_execute: app->len_execute(); break;

			// エディットボックス
			case idc_pos_mul_num:
			case idc_pos_mul_den:
			case idc_pos_add_num:
			case idc_pos_add_den:
			case idc_pos_base_num:
			case idc_pos_base_den:
			case idc_pos_exp_num:
			case idc_pos_exp_den:
			case idc_len_mul_num:
			case idc_len_mul_den:
			case idc_len_add_num:
			case idc_len_add_den:
			case idc_len_base_num:
			case idc_len_base_den:
			case idc_len_exp_num:
			case idc_len_exp_den:
				if (code == EN_CHANGE) update_config(); break;

			// コンボボックス
			case idc_pos_target:
			case idc_len_target:
			case idc_pos_orig:
				if (code == CBN_SELCHANGE) update_config(); break;
			}
		}
	} addin_dialog;
}
