#pragma once

namespace apn::dark
{
	//
	// このクラスは拡張編集の状態を管理します。
	//
	inline struct exedit_state_t : entry_t::node_t
	{
		//
		// 選択フレーム範囲のデータです。
		//
		struct frame_range_select_t {
			my::addr_t address = 0;
			int state_id = 0;
			COLORREF* orig = nullptr;
			COLORREF color = CLR_NONE;
		} frame_range_selects[3] = {};

		//
		// 初期化処理を実行します。
		//
		virtual BOOL on_init() override
		{
			MY_TRACE_FUNC("");

			// 拡張編集のベースアドレスを取得します。
			auto exedit = (my::addr_t)::GetModuleHandle(_T("exedit.auf"));
			if (!exedit) return FALSE;

			// 選択フレーム範囲のデータをセットします。
			{
				frame_range_selects[0].address = exedit + 0x0003807E;
				frame_range_selects[0].state_id = EES_SELECTION_FILL;
				frame_range_selects[0].orig = *(COLORREF**)frame_range_selects[0].address;

				frame_range_selects[1].address = exedit + 0x00038076;
				frame_range_selects[1].state_id = EES_SELECTION_EDGE;
				frame_range_selects[1].orig = *(COLORREF**)frame_range_selects[1].address;

				frame_range_selects[2].address = exedit + 0x00038087;
				frame_range_selects[2].state_id = EES_SELECTION_BACKGROUND;
				frame_range_selects[2].orig = *(COLORREF**)frame_range_selects[2].address;
			}

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
		// 拡張編集の状態を更新します。
		//
		BOOL update()
		{
			MY_TRACE_FUNC("");

			// 拡張編集のベースアドレスを取得します。
			auto exedit = (my::addr_t)::GetModuleHandle(_T("exedit.auf"));
			if (!exedit) return FALSE;

			{
				// ボタン用のマテリアルを使用します。
				const auto& palette = kuro::paint::button_material.palette;

				// ボタンのピグメントを取得します。
				auto pigment = palette.get(BP_PUSHBUTTON, PBS_NORMAL);

				kuro::hook::icon.manager.clear();
				kuro::hook::icon.editor.change_color(L"", RGB(0, 0, 0), pigment->text.get_win32_color());
				kuro::hook::icon.editor.change_color(L"EXEDIT_ICON_MA", CLR_NONE, CLR_NONE); // マウスのアイコンは変更しません
				kuro::hook::icon.editor.change_color(L"EXEDIT_ICON_SP", CLR_NONE, CLR_NONE); // スポイトのアイコンは変更しません
			}

			// ボタンを走査します。
			for (auto button : hive.exedit.static_edge_buttons)
			{
				// スタティックエッジを描画する場合は
				if (hive.exedit.flag_draw_button_edge)
				{
					// スタティックエッジを付与します。
					my::modify_ex_style(button, 0, WS_EX_STATICEDGE);
				}
				else
				{
					// スタティックエッジを取り除きます。
					my::modify_ex_style(button, WS_EX_STATICEDGE, 0);
				}

				// ウィンドウレイアウトを更新します。
				::SetWindowPos(button, nullptr, 0, 0, 0, 0,
					SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
			}

			// 拡張編集用のマテリアルパレットを使用します。
			const auto& palette = kuro::paint::exedit_material.palette;

			{
				auto style = PS_SOLID;
				auto width = 0;
				auto color = RGB(0x00, 0xff, 0xff);

				// ピグメントを取得できた場合はペンの色をセットします。
				if (auto pigment = palette.get(WP_EXEDIT, EES_FRAME_CURSOR))
					color = pigment->background.get_win32_color();

				// ペンの属性をセットします。
				if (hive.exedit.cursor.style >= 0) style = hive.exedit.cursor.style;
				if (hive.exedit.cursor.width >= 0) width = hive.exedit.cursor.width;

				// ペンを新しく作り直します。
				auto new_pen = ::CreatePen(style, width, color);
				auto old_pen = my::tools::set_abs_addr(exedit + 0x1538B4, new_pen);
				::DeleteObject(old_pen);
			}

			// 選択フレーム範囲のデータを走査します。
			for (auto& frame_range_select : frame_range_selects)
			{
				// ピグメントを取得できた場合は
				if (auto pigment = palette.get(WP_EXEDIT, frame_range_select.state_id))
				{
					// 選択フレーム範囲のデータを走査します。
					frame_range_select.color = pigment->background.get_win32_color();

					// 色が有効の場合はメモリアドレスを書き換えます。
					if (frame_range_select.color != CLR_NONE)
					{
						my::tools::set_abs_addr(frame_range_select.address, &frame_range_select.color);

						continue;
					}
				}

				// 本来ならここで元のメモリアドレスに戻さなければならない。
				my::tools::set_abs_addr(frame_range_select.address, &frame_range_select.orig);
			}

			return TRUE;
		}
	} exedit_state;
}
