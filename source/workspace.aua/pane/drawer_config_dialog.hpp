#pragma once

namespace apn::workspace
{
	//
	// このクラスはドロワーの設定を変更するためのダイアログです。
	//
	struct DrawerConfigDialog : my::Dialog
	{
		//
		// 編集対象のペインです。
		//
		std::shared_ptr<Pane> pane;

		//
		// 編集対象のドロワーのインデックスです。
		//
		size_t drawer_index;

		//
		// コンストラクタです。
		//
		DrawerConfigDialog(const std::shared_ptr<Pane>& pane, size_t drawer_index, HWND parent)
			: pane(pane)
			, drawer_index(drawer_index)
		{
			create(hive.instance, MAKEINTRESOURCE(IDD_DRAWER_CONFIG), parent);
		}

		//
		// ダイアログを表示します。
		//
		int do_modal()
		{
			init_combobox(IDC_TAV_DISPLAY_MODE, _T("デフォルト"), _T("手動"), _T("半自動"), _T("自動"), _T("全自動"));
			init_combobox(IDC_TAV_SELECT_MODE, _T("デフォルト"), _T("クリック"), _T("ホバー"));
			init_combobox(IDC_TAV_STRETCH_MODE, _T("デフォルト"), _T("内側"), _T("外側"));
			init_combobox(IDC_TAV_LOCATION, _T("デフォルト"), _T("左辺"), _T("上辺"), _T("右辺"), _T("下辺"));
			init_combobox(IDC_TAV_NODE_ALIGN, _T("デフォルト"), _T("左または上"), _T("右または下"), _T("中央"));

			auto drawer = pane->drawers[drawer_index];

			Hive::Tav tav;
			BOOL enabled = FALSE;
			BOOL inside = TRUE;
			if (drawer)
			{
				tav = *drawer;
				enabled = TRUE;
				inside = drawer->inside;
			}
			else
			{
				switch (drawer_index)
				{
				case Pane::c_drawer_index.c_top: tav.location = tav.c_location.c_top; break;
				case Pane::c_drawer_index.c_bottom: tav.location = tav.c_location.c_bottom; break;
				case Pane::c_drawer_index.c_left: tav.location = tav.c_location.c_left; break;
				case Pane::c_drawer_index.c_right: tav.location = tav.c_location.c_right; break;
				}
			}

			set_check(IDC_DRAWER_ENABLED, enabled);
			set_check(IDC_DRAWER_INSIDE, inside);
			set_combobox_index(IDC_TAV_DISPLAY_MODE, tav.display_mode);
			set_combobox_index(IDC_TAV_SELECT_MODE, tav.select_mode);
			set_combobox_index(IDC_TAV_STRETCH_MODE, tav.stretch_mode);
			set_combobox_index(IDC_TAV_LOCATION, tav.location);
			set_combobox_index(IDC_TAV_NODE_ALIGN, tav.node_align);

			auto result = __super::do_modal();
			if (result != IDOK) return result;

			get_check(IDC_DRAWER_ENABLED, enabled);
			get_check(IDC_DRAWER_INSIDE, inside);
			get_combobox_index(IDC_TAV_DISPLAY_MODE, tav.display_mode);
			get_combobox_index(IDC_TAV_SELECT_MODE, tav.select_mode);
			get_combobox_index(IDC_TAV_STRETCH_MODE, tav.stretch_mode);
			get_combobox_index(IDC_TAV_LOCATION, tav.location);
			get_combobox_index(IDC_TAV_NODE_ALIGN, tav.node_align);

			if (enabled)
			{
				if (!drawer)
					drawer = pane->create_drawer(drawer_index);

				(Hive::Tav&)*drawer = tav;

				drawer->inside = inside;
			}
			else
			{
				pane->erase_drawer(drawer_index);
			}

			return result;
		}
	};
}
