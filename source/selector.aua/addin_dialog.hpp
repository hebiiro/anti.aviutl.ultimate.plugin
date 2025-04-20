#pragma once

namespace apn::selector
{
	//
	// このクラスはアドインダイアログです。
	//
	inline struct AddinDialog : AddinDialogInterface
	{
		//
		// コンフィグを更新します。
		//
		virtual void on_update_config() override
		{
			MY_TRACE_FUNC("");

			// パートのコンフィグ更新処理を実行します。
			for (auto& part : Part::parts) part->on_update_config(this);
		}

		//
		// コントロールを更新します。
		//
		virtual void on_update_controls() override
		{
			MY_TRACE_FUNC("");

			// パートのコントロール更新処理を実行します。
			for (auto& part : Part::parts) part->on_update_controls(this);
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
			auto base_size = get_base_size();
			auto row_size = base_size + margin_value * 2;
			auto row = std::make_shared<RelativePos>(row_size);
			auto size_s = std::make_shared<RelativePos>(base_size * 3);
			auto size_m = std::make_shared<RelativePos>(base_size * 4);
			auto size_l = std::make_shared<RelativePos>(base_size * 6);
			auto half = std::make_shared<AbsolutePos>(1, 2);
			auto full = std::make_shared<AbsolutePos>(2, 2);

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, size_l, margin, ctrl(IDC_SHOW_CUSTOM_OBJECT_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, size_m, margin, ctrl(IDC_SHOW_CUSTOM_OBJECT_TREE));
				node->add_pane(c_axis.c_horz, c_align.c_left, size_m, margin, ctrl(IDC_SHOW_CUSTOM_OBJECT_LIST));
			}
		}

		//
		// ダイアログのコマンド処理です。
		//
		virtual void on_command(UINT code, UINT id, HWND control) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}", code, id, control);

			// パートのコマンド処理を実行します。
			for (auto& part : Part::parts) part->on_command(this, code, id, control);
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_CONTEXTMENU:
				{
					auto control = (HWND)wParam;
					auto id = ::GetDlgCtrlID(control);
					auto point = my::lp_to_pt(lParam);

					// パートのオプション処理を実行します。
					for (auto& part : Part::parts) part->on_option(this, id, control, point);

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} addin_dialog;
}
