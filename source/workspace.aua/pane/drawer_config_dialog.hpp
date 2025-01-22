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
			// ダイアログを作成します。
			create(hive.instance, MAKEINTRESOURCE(IDD_DRAWER_CONFIG), parent);
		}

		//
		// ダイアログを表示します。
		//
		int do_modal()
		{
			// ダイアログコントロールを初期化します。
			init_combobox(IDC_TAV_DISPLAY_MODE, _T("デフォルト"), _T("手動"), _T("半自動"), _T("自動"), _T("全自動"));
			init_combobox(IDC_TAV_SELECT_MODE, _T("デフォルト"), _T("クリック"), _T("ホバー"));
			init_combobox(IDC_TAV_STRETCH_MODE, _T("デフォルト"), _T("内側"), _T("外側"));
			init_combobox(IDC_TAV_LOCATION, _T("デフォルト"), _T("左辺"), _T("上辺"), _T("右辺"), _T("下辺"));
			init_combobox(IDC_TAV_NODE_ALIGN, _T("デフォルト"), _T("左または上"), _T("右または下"), _T("中央"));

			// インデックスからドロワーを取得します。
			auto drawer = pane->drawers[drawer_index];

			// ドロワーの設定を格納する変数です。
			Hive::Tav tav;
			BOOL enabled = FALSE;
			BOOL inside = TRUE;

			// ドロワーが存在する場合は
			if (drawer)
			{
				// ドロワーの設定を取得します。
				tav = *drawer;
				enabled = TRUE;
				inside = drawer->inside;
			}
			// ドロワーが存在しない場合は
			else
			{
				// デフォルトの設定を取得します。
				switch (drawer_index)
				{
				case Pane::c_drawer_index.c_top: tav.location = tav.c_location.c_top; break;
				case Pane::c_drawer_index.c_bottom: tav.location = tav.c_location.c_bottom; break;
				case Pane::c_drawer_index.c_left: tav.location = tav.c_location.c_left; break;
				case Pane::c_drawer_index.c_right: tav.location = tav.c_location.c_right; break;
				}
			}

			// ダイアログコントロールに値を適用します。
			set_check(IDC_DRAWER_ENABLED, enabled);
			set_check(IDC_DRAWER_INSIDE, inside);
			set_combobox_index(IDC_TAV_DISPLAY_MODE, tav.display_mode);
			set_combobox_index(IDC_TAV_SELECT_MODE, tav.select_mode);
			set_combobox_index(IDC_TAV_STRETCH_MODE, tav.stretch_mode);
			set_combobox_index(IDC_TAV_LOCATION, tav.location);
			set_combobox_index(IDC_TAV_NODE_ALIGN, tav.node_align);

			// ダイアログを表示します。
			auto result = __super::do_modal();
			if (result != IDOK) return result;

			// ダイアログコントロールから値を取得します。
			get_check(IDC_DRAWER_ENABLED, enabled);
			get_check(IDC_DRAWER_INSIDE, inside);
			get_combobox_index(IDC_TAV_DISPLAY_MODE, tav.display_mode);
			get_combobox_index(IDC_TAV_SELECT_MODE, tav.select_mode);
			get_combobox_index(IDC_TAV_STRETCH_MODE, tav.stretch_mode);
			get_combobox_index(IDC_TAV_LOCATION, tav.location);
			get_combobox_index(IDC_TAV_NODE_ALIGN, tav.node_align);

			// ドロワーを有効にする場合は
			if (enabled)
			{
				// ドロワーが既存ではない場合は
				if (!drawer)
				{
					// ドロワーを新規作成します。
					drawer = pane->create_drawer(drawer_index);
				}

				// ドロワーに設定を適用します。
				(Hive::Tav&)*drawer = tav;
				drawer->inside = inside;

				// ドロワーの設定が更新されたので再描画します。
				my::invalidate(*drawer);
			}
			// ドロワーを無効にする場合は
			else
			{
				// ペインからドロワーを削除します。
				pane->erase_drawer(drawer_index);
			}

			return result;
		}
	};
}
