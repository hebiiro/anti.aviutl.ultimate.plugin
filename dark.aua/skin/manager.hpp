#pragma once

namespace apn::dark::skin
{
	//
	// このクラスはスキンを管理します。
	//
	inline struct Manager
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			if (!dwm.init()) return FALSE;
			if (!theme::manager.init()) return FALSE;

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			theme::manager.exit();
			dwm.exit();

			return TRUE;
		}

		//
		// 現在のコンフィグを(拡張編集に)適用します。
		//
		BOOL apply_config()
		{
			MY_TRACE_FUNC("");

			auto exedit = (my::addr_t)::GetModuleHandle(_T("exedit.auf"));

			if (hive.static_edge_mode == hive.c_static_edge_mode.c_omit)
			{
				for (auto button : hive.static_edge_buttons)
					my::modify_ex_style(button, WS_EX_STATICEDGE, 0);
			}
			else
			{
				for (auto button : hive.static_edge_buttons)
					my::modify_ex_style(button, 0, WS_EX_STATICEDGE);
			}

			{
				auto style = PS_SOLID;
				auto width = 0;
				auto color = RGB(0x00, 0xff, 0xff);

				if (config.exedit.xor_pen.style != -1 &&
					config.exedit.xor_pen.width != -1 &&
					config.exedit.xor_pen.color != CLR_NONE)
				{
					style = config.exedit.xor_pen.style;
					width = config.exedit.xor_pen.width;
					color = config.exedit.xor_pen.color;
				}

				auto new_pen = ::CreatePen(style, width, color);
				auto old_pen = my::tools::set_abs_addr(exedit + 0x1538B4, new_pen);
				::DeleteObject(old_pen);
			}

			if (auto theme = skin::theme::manager.get_theme(VSCLASS_EXEDIT))
			{
				auto state = skin::theme::manager.get_state(theme, WP_EXEDIT, EES_SELECTION_FILL);
				if (state && state->stuff.fill.color != CLR_NONE)
					my::tools::set_abs_addr(exedit + 0x0003807E, &state->stuff.fill.color);
			}

			if (auto theme = skin::theme::manager.get_theme(VSCLASS_EXEDIT))
			{
				auto state = skin::theme::manager.get_state(theme, WP_EXEDIT, EES_SELECTION_EDGE);
				if (state && state->stuff.fill.color != CLR_NONE)
					my::tools::set_abs_addr(exedit + 0x00038076, &state->stuff.fill.color);
			}

			if (auto theme = skin::theme::manager.get_theme(VSCLASS_EXEDIT))
			{
				auto state = skin::theme::manager.get_state(theme, WP_EXEDIT, EES_SELECTION_BACKGROUND);
				if (state && state->stuff.fill.color != CLR_NONE)
					my::tools::set_abs_addr(exedit + 0x00038087, &state->stuff.fill.color);
			}

			return TRUE;
		}
	} manager;
}
