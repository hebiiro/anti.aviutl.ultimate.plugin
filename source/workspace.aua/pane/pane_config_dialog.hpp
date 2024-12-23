#pragma once

namespace apn::workspace
{
	//
	// このクラスはペインの設定を変更するためのダイアログです。
	//
	struct PaneConfigDialog : my::Dialog
	{
		//
		// 編集対象のペインです。
		//
		std::shared_ptr<Pane> pane;

		//
		// コンストラクタです。
		//
		PaneConfigDialog(const std::shared_ptr<Pane>& pane, HWND parent)
			: pane(pane)
		{
			create(hive.instance, MAKEINTRESOURCE(IDD_PANE_CONFIG), parent);
		}

		//
		// ペインのサイズ(エディットボックス)を更新します。
		//
		void update_pane_size_editbox()
		{
			auto origin = get_combobox_index(IDC_PANE_ORIGIN);
			auto border_pos = get_int(IDC_PANE_BORDER_POS);

			auto top_left = int32_t {};
			auto bottom_right = int32_t {};

			auto w = (pane->split_mode == pane->c_split_mode.c_vert) ?
				my::get_width(pane->position) : my::get_height(pane->position);

			if (origin == pane->c_origin.c_top_left)
			{
				top_left = border_pos;
				bottom_right = w - (border_pos + hive.border_width);
			}
			else
			{
				top_left = w - (border_pos - hive.border_width);
				bottom_right = border_pos;
			}

			set_int(IDC_PANE_TOP_LEFT, top_left);
			set_int(IDC_PANE_BOTTOM_RIGHT, bottom_right);
		}

		//
		// ダイアログを表示します。
		//
		int do_modal()
		{
			auto x = pane->position.left;
			auto y = pane->position.top;
			auto w = my::get_width(pane->position);
			auto h = my::get_height(pane->position);

			init_combobox(IDC_PANE_ORIGIN, _T("左上"), _T("右下"));
			init_combobox(IDC_CAPTION_MODE, _T("非表示"), _T("表示"));
			init_combobox(IDC_CAPTION_LOCATION, _T("左辺"), _T("上辺"), _T("右辺"), _T("下辺"));
			init_combobox(IDC_TAV_DISPLAY_MODE, _T("デフォルト"), _T("手動"), _T("半自動"), _T("自動"), _T("全自動"));
			init_combobox(IDC_TAV_SELECT_MODE, _T("デフォルト"), _T("クリック"), _T("ホバー"));
			init_combobox(IDC_TAV_STRETCH_MODE, _T("デフォルト"), _T("内側"), _T("外側"));
			init_combobox(IDC_TAV_LOCATION, _T("デフォルト"), _T("左辺"), _T("上辺"), _T("右辺"), _T("下辺"));
			init_combobox(IDC_TAV_NODE_ALIGN, _T("デフォルト"), _T("左または上"), _T("右または下"), _T("中央"));

			set_int(IDC_PANE_X, x);
			set_int(IDC_PANE_Y, y);
			set_int(IDC_PANE_W, w);
			set_int(IDC_PANE_H, h);
			set_combobox_index(IDC_PANE_ORIGIN, pane->origin);
			set_combobox_index(IDC_CAPTION_MODE, pane->caption_mode);
			set_combobox_index(IDC_CAPTION_LOCATION, pane->caption_location - 1);
			set_int(IDC_PANE_BORDER_POS, pane->border);
			set_combobox_index(IDC_TAV_DISPLAY_MODE, pane->tav.display_mode);
			set_combobox_index(IDC_TAV_SELECT_MODE, pane->tav.select_mode);
			set_combobox_index(IDC_TAV_STRETCH_MODE, pane->tav.stretch_mode);
			set_combobox_index(IDC_TAV_LOCATION, pane->tav.location);
			set_combobox_index(IDC_TAV_NODE_ALIGN, pane->tav.node_align);

			auto result = __super::do_modal();
			if (result != IDOK) return result;

			auto origin = pane->origin;
			auto caption_mode = pane->caption_mode;
			auto caption_location = pane->caption_location;
			auto border = pane->border;
			get_combobox_index(IDC_PANE_ORIGIN, origin);
			get_combobox_index(IDC_CAPTION_MODE, caption_mode);
			get_combobox_index(IDC_CAPTION_LOCATION, caption_location);
			get_int(IDC_PANE_BORDER_POS, border);
			get_combobox_index(IDC_TAV_DISPLAY_MODE, pane->tav.display_mode);
			get_combobox_index(IDC_TAV_SELECT_MODE, pane->tav.select_mode);
			get_combobox_index(IDC_TAV_STRETCH_MODE, pane->tav.stretch_mode);
			get_combobox_index(IDC_TAV_LOCATION, pane->tav.location);
			get_combobox_index(IDC_TAV_NODE_ALIGN, pane->tav.node_align);

			pane->set_origin(origin);
			pane->set_caption_mode(caption_mode);
			pane->set_caption_location(caption_location + 1);
			pane->border = border;

			return result;
		}

		//
		// ダイアログプロシージャです。
		//
		virtual INT_PTR on_dlg_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_COMMAND:
				{
					auto id = LOWORD(wParam);
					auto code = HIWORD(wParam);
					auto control = (HWND)lParam;

					switch (id)
					{
					case IDC_PANE_ORIGIN:
						{
							if (code == CBN_SELCHANGE)
								update_pane_size_editbox();

							break;
						}
					case IDC_PANE_BORDER_POS:
						{
							if (code == EN_CHANGE)
								update_pane_size_editbox();

							break;
						}
					}

					break;
				}
			}

			return __super::on_dlg_proc(hwnd, message, wParam, lParam);
		}
	};
}
