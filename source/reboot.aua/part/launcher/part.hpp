#pragma once

namespace apn::reboot::launcher
{
	//
	// このクラスはランチャーのパートです。
	//
	inline struct Launcher : Part
	{
		//
		// このパートの名前を返します。
		//
		virtual std::wstring on_get_name()
		{
			return L"launcher";
		}

		//
		// 初期化処理を実行します。
		//
		virtual BOOL on_init() override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL on_exit() override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// コンフィグを読み込みます。
		//
		virtual BOOL on_read(n_json& root) override
		{
			MY_TRACE_FUNC("");

			// デューティを読み込みます。
			get_child_nodes(root, "duty",
				[&](const n_json& duty_node, size_t i)
			{
				if (i >= std::size(agit.duties)) return FALSE;

				auto& duty = agit.duties[i];

				get_string(duty_node, "name", duty.name);
				get_string(duty_node, "verb", duty.verb);
				get_string(duty_node, "path", duty.path);
				get_string(duty_node, "args", duty.args);

				return TRUE;
			});

			return TRUE;
		}

		//
		// コンフィグを書き込みます。
		//
		virtual BOOL on_write(n_json& root) override
		{
			MY_TRACE_FUNC("");

			// デューティを書き込みます。
			set_child_nodes(root, "duty", agit.duties,
				[&](n_json& duty_node, const auto& duty, size_t i)
			{
				if (i >= std::size(agit.duties)) return FALSE;

				set_string(duty_node, "name", duty.name);
				set_string(duty_node, "verb", duty.verb);
				set_string(duty_node, "path", duty.path);
				set_string(duty_node, "args", duty.args);

				return TRUE;
			});

			return TRUE;
		}

		//
		// パートの状態を更新します。
		//
		virtual BOOL on_update(AddinDialogInterface* addin_dialog) override
		{
			MY_TRACE_FUNC("");

			// ダイアログのデューティ名を更新します。
			for (int32_t i = 0; i < std::size(agit.duties); i++)
			{
				const auto& duty = agit.duties[i];

				addin_dialog->set_text(IDC_LAUNCHER_DUTY_0 + i,
					duty.name.empty() ? my::format(_T("デューティ {}"), i) : duty.name);
			}

			return TRUE;
		}

		//
		// コマンドを実行します。
		//
		virtual BOOL on_command(AddinDialogInterface* addin_dialog, uint32_t code, uint32_t control_id, HWND control) override
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}, {:#010x}", code, control_id, control);

			int32_t index = (int32_t)control_id - IDC_LAUNCHER_DUTY_0;

			// このパート用のコントロールではない場合は何もしません。
			if (index < 0 || index >= std::size(agit.duties)) return FALSE;

			const auto& duty = agit.duties[index];

			if (duty.path.length())
			{
				share::shell_execute(duty.verb.c_str(), duty.path.c_str(), duty.args.c_str());
			}
			else
			{
				hive.message_box(_T("コマンドが指定されていません"));
			}

			return TRUE;
		}

		//
		// オプションを変更します。
		//
		virtual BOOL on_option(AddinDialogInterface* addin_dialog, uint32_t control_id, HWND control, const POINT& point) override
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}", control_id, control);

			int32_t index = (int32_t)control_id - IDC_LAUNCHER_DUTY_0;

			// このパート用のコントロールではない場合は何もしません。
			if (index < 0 || index >= std::size(agit.duties)) return FALSE;

			DutyOptionDialog duty_option_dialog;

			if (IDOK != duty_option_dialog.do_modal(*addin_dialog, index))
				return FALSE;

			on_update(addin_dialog);

			return TRUE;
		}
	} part;
}
