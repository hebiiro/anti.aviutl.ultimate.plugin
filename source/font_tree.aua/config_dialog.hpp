#pragma once

namespace apn::font_tree
{
	//
	// このクラスはコンフィグダイアログです。
	//
	struct ConfigDialog : StdAddinDialog<IDD_CONFIG, my::layout::Window<my::Dialog2>>
	{
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
			auto s_ctrl = std::make_shared<RelativePos>(get_base_size() * 2 + margin_value * 2);
			auto m_ctrl = std::make_shared<RelativePos>(get_base_size() * 4 + margin_value * 2);
			auto l_ctrl = std::make_shared<RelativePos>(get_base_size() * 6 + margin_value * 2);
			auto half = std::make_shared<AbsolutePos>(1, 2);
			auto full = std::make_shared<AbsolutePos>(2, 2);

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_FAVORITE_STAT));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, m_ctrl, margin, ctrl(IDC_DISPLAY_NAME_FORMAT_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_DISPLAY_NAME_FORMAT));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, m_ctrl, margin, ctrl(IDC_SEPARATOR_FORMAT_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_SEPARATOR_FORMAT));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_PREVIEW_STAT));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_PREVIEW_ENABLED));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_PREVIEW_LEFT_SIDE));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, m_ctrl, margin, ctrl(IDC_PREVIEW_ITEM_SIZE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, s_ctrl, margin, ctrl(IDC_PREVIEW_ITEM_SIZE_W));
				node->add_pane(c_axis.c_horz, c_align.c_left, s_ctrl, margin, ctrl(IDC_PREVIEW_ITEM_SIZE_H));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, m_ctrl, margin, ctrl(IDC_PREVIEW_SAMPLE_FORMAT_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_PREVIEW_SAMPLE_FORMAT));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, s_ctrl, margin, ctrl(IDC_PREVIEW_FILL_COLOR_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, s_ctrl, margin, ctrl(IDC_PREVIEW_FILL_COLOR));
				node->add_pane(c_axis.c_horz, c_align.c_left, s_ctrl, margin, ctrl(IDC_PREVIEW_TEXT_COLOR_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, s_ctrl, margin, ctrl(IDC_PREVIEW_TEXT_COLOR));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, half, margin, ctrl(IDOK));
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDCANCEL));
			}

			::SetWindowPos(*this, nullptr, 0, 0, 360, 360, SWP_NOZORDER | SWP_NOMOVE);
		}

		//
		// ダイアログのコマンド処理です。
		//
		virtual void on_command(UINT code, UINT id, HWND control) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}", code, id, control);

			switch (id)
			{
			case IDC_PREVIEW_FILL_COLOR:
			case IDC_PREVIEW_TEXT_COLOR:
				{
					try
					{
						// カラー選択ダイアログを表示してカラーを取得します。
						auto color = magi.choose_color(*this, get_uint(id));

						// 取得した色をコントロールに適用します。
						set_uint(id, color);
						my::invalidate(control);

						// コンフィグを更新します。
						update_config();
					}
					catch (...)
					{
					}

					break;
				}
			}
		}

		//
		// ダイアログプロシージャです。
		//
		virtual INT_PTR on_dlg_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_DRAWITEM:
				{
					auto id = (uint32_t)wParam;

					switch (id)
					{
					case IDC_PREVIEW_FILL_COLOR:
					case IDC_PREVIEW_TEXT_COLOR:
						{
							auto dis = (DRAWITEMSTRUCT*)lParam;

							my::gdi::unique_ptr<HBRUSH> brush(
								::CreateSolidBrush(get_uint(id)));
							::FillRect(dis->hDC, &dis->rcItem, brush.get());

							return TRUE;
						}
					}

					break;
				}
			}

			return __super::on_dlg_proc(hwnd, message, wParam, lParam);
		}
	};
}
