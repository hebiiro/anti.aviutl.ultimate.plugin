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

			// コマンドを読み込みます。
			read_child_nodes(root, "duty",
				[&](const n_json& duty_node, size_t i)
			{
				if (i >= std::size(agit.duties)) return FALSE;

				auto& duty = agit.duties[i];

				read_string(duty_node, "name", duty.name);
				read_string(duty_node, "verb", duty.verb);
				read_string(duty_node, "path", duty.path);
				read_string(duty_node, "args", duty.args);

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

			// コマンドを書き込みます。
			write_child_nodes(root, "duty", agit.duties,
				[&](n_json& duty_node, const auto& duty, size_t i)
			{
				if (i >= std::size(agit.duties)) return FALSE;

				write_string(duty_node, "name", duty.name);
				write_string(duty_node, "verb", duty.verb);
				write_string(duty_node, "path", duty.path);
				write_string(duty_node, "args", duty.args);

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

			// ダイアログのコマンド名を更新します。
			for (int32_t i = 0; i < std::size(agit.duties); i++)
			{
				const auto& duty = agit.duties[i];

				addin_dialog->set_text(IDC_LAUNCHER_COMMAND_0 + i,
					duty.name.empty() ? my::format(_T("コマンド {/}"), i) : duty.name);
			}

			return TRUE;
		}

		//
		// コマンドを実行します。
		//
		virtual BOOL on_command(AddinDialogInterface* addin_dialog, uint32_t code, uint32_t control_id, HWND control) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}", code, control_id, control);

			int32_t index = (int32_t)control_id - IDC_LAUNCHER_COMMAND_0;

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
			MY_TRACE_FUNC("{/hex}, {/hex}", control_id, control);

			int32_t index = (int32_t)control_id - IDC_LAUNCHER_COMMAND_0;

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
