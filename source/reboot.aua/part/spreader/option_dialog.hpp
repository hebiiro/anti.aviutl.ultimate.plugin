#pragma once

namespace apn::reboot::spreader
{
	//
	// このクラスはプレイヤー用のオプションダイアログです。
	//
	struct OptionDialog : my::Dialog2
	{
		//
		// ダイアログを作成して表示します。
		//
		int do_modal(HWND parent)
		{
			// ダイアログを作成します。
			create(hive.instance, MAKEINTRESOURCE(IDD_SPREADER_OPTION), parent);

			init_combobox(IDC_SPREADER_SIZE_MODE, _T("相対サイズを使用"), _T("絶対サイズを使用"));

			bind_combobox_index(IDC_SPREADER_SIZE_MODE, agit.size_mode);
			bind_int(IDC_SPREADER_ABSOLUTE_SIZE_W, agit.absolute_size.cx);
			bind_int(IDC_SPREADER_ABSOLUTE_SIZE_H, agit.absolute_size.cy);
			bind_int(IDC_SPREADER_RELATIVE_SIZE, agit.relative_size);
			for (int32_t i = 0; i < std::size(agit.thumbnail_scale); i++)
				bind_int(IDC_SPREADER_THUMBNAIL_SCALE_0 + i, agit.thumbnail_scale[i]);

			return do_modal2(parent);
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
					auto code = HIWORD(wParam);
					auto control_id = LOWORD(wParam);
					auto control = (HWND)lParam;

					MY_TRACE_FUNC("WM_COMMAND, {/hex16}, {/hex16}, {/hex}", code, control_id, control);

					break;
				}
			}

			return __super::on_dlg_proc(hwnd, message, wParam, lParam);
		}
	};
}
