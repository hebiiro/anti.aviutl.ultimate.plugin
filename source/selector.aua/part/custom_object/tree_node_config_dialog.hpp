#pragma once

namespace apn::selector::custom_object
{
	//
	// このクラスはツリーノードの設定を変更するためのダイアログです。
	//
	struct TreeNodeConfigDialog : my::Dialog2
	{
		//
		// ツリーノードの設定です。
		//
		std::wstring name;
		std::wstring pattern;
		int32_t target = agit.c_column.c_tag;
		int32_t mode = agit.tree.c_mode.c_off;
		BOOL expanded = FALSE;

		//
		// コンストラクタです。
		//
		TreeNodeConfigDialog(HWND parent)
		{
			create(hive.instance, MAKEINTRESOURCE(IDD_TREE_NODE_CONFIG), parent);
		}

		//
		// ダイアログを表示します。
		//
		int do_modal(HWND parent)
		{
			init_combobox(IDC_TREE_NODE_CONFIG_TARGET,
				_T("スクリプト名"),
				_T("フォルダ名"),
				_T("ファイル名"),
				_T("ファイルタイム"),
				_T("作者"),
				_T("タグ"),
				_T("説明"),
				_T("ノート"));

			bind_text(IDC_TREE_NODE_CONFIG_NAME, name);
			bind_text(IDC_TREE_NODE_CONFIG_PATTERN, pattern);
			bind_combobox_index(IDC_TREE_NODE_CONFIG_TARGET, target);

			return __super::do_modal2(parent);
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

					break;
				}
			}

			return __super::on_dlg_proc(hwnd, message, wParam, lParam);
		}
	};
}
