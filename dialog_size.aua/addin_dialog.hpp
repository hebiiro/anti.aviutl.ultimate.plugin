#pragma once

namespace apn::dialog_size
{
	//
	// このクラスはアドインダイアログです。
	//
	inline struct AddinDialog : StdAddinDialog<IDD_MAIN_DIALOG>
	{
		//
		// コントロールを更新します。
		//
		virtual void on_update_controls() override
		{
			MY_TRACE_FUNC("");

			for (size_t i = 0; i < std::size(hive.custom_templates); i++)
				set_check(IDC_CUSTOM_TEMPLATE_0 + i, hive.custom_templates[i].active);
		}

		//
		// コンフィグを更新します。
		//
		virtual void on_update_config() override
		{
			MY_TRACE_FUNC("");

			for (size_t i = 0; i < std::size(hive.custom_templates); i++)
				get_check(IDC_CUSTOM_TEMPLATE_0 + i, hive.custom_templates[i].active);
		}

		//
		// ダイアログの初期化処理です。
		//
		virtual void on_init_dialog() override
		{
			MY_TRACE_FUNC("");

			using namespace my::layout;

			auto margin_value = 2;
			auto margin = RECT { margin_value, margin_value, margin_value, margin_value };
			auto row = std::make_shared<RelativePos>(get_base_size() + margin_value * 2);
			std::shared_ptr<AbsolutePos> q[4 + 1];
			for (auto i = 0; i < std::size(q); i++)
				q[i] = std::make_shared<AbsolutePos>(i, std::size(q) - 1);

			for (size_t i = 0; i < std::size(hive.custom_templates); i += 2)
			{
				auto id = IDC_CUSTOM_TEMPLATE_0 + i;

				set_text(id + 0, hive.custom_templates[i + 0].target_display_name.c_str());
				set_text(id + 1, hive.custom_templates[i + 1].target_display_name.c_str());

				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, q[2], margin, ctrl(id + 0));
				node->add_pane(c_axis.c_horz, c_align.c_left, q[4], margin, ctrl(id + 1));
			}
		}

		//
		// ダイアログのコマンド処理です。
		//
		virtual void on_command(UINT code, UINT id, HWND control) override
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}, {:#010x}", code, id, control);

			auto index = id - IDC_CUSTOM_TEMPLATE_0;
			if (index < std::size(hive.custom_templates)) update_config();
		}
	} addin_dialog;
}
