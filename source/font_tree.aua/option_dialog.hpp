#pragma once

namespace apn::font_tree
{
	//
	// このクラスはオプションダイアログです。
	//
	struct OptionDialog : my::Dialog2
	{
		//
		// ダイアログを作成して表示します。
		//
		int do_modal(HWND parent)
		{
			// ダイアログを作成します。
			create(hive.instance, MAKEINTRESOURCE(IDD_OPTION), parent);

			init_combobox(IDC_PREVIEW_MODE, _T("プレビューを表示しない"), _T("プレビューを左側に表示"), _T("プレビューを右側に表示"));

			bind_text(IDC_DISPLAY_NAME_FORMAT, hive.favorite.display_name_format);
			bind_text(IDC_SEPARATOR_FORMAT, hive.favorite.separator_format);

			bind_combobox_index(IDC_PREVIEW_MODE, hive.preview.mode);
			bind_text(IDC_PREVIEW_SAMPLE_FORMAT, hive.preview.sample_format);
			bind_int(IDC_PREVIEW_ITEM_SIZE_W, hive.preview.item_size.cx);
			bind_int(IDC_PREVIEW_ITEM_SIZE_H, hive.preview.item_size.cy);
			bind_int(IDC_PREVIEW_FILL_COLOR, hive.preview.fill_color);
			bind_int(IDC_PREVIEW_TEXT_COLOR, hive.preview.text_color);

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

					switch (control_id)
					{
					case IDC_PREVIEW_FILL_COLOR:
					case IDC_PREVIEW_TEXT_COLOR:
						{
							try
							{
								// カラー選択ダイアログを表示してカラーを取得します。
								auto color = magi.choose_color(*this, get_uint(control_id));

								// 取得した色をコントロールに適用します。
								set_uint(control_id, color);

								// コントロールを再描画します。
								my::invalidate(control);
							}
							catch (...)
							{
							}

							break;
						}
					}

					break;
				}
			case WM_DRAWITEM:
				{
					auto control_id = (uint32_t)wParam;

					switch (control_id)
					{
					case IDC_PREVIEW_FILL_COLOR:
					case IDC_PREVIEW_TEXT_COLOR:
						{
							auto dis = (DRAWITEMSTRUCT*)lParam;

							my::gdi::unique_ptr<HBRUSH> brush(
								::CreateSolidBrush(get_uint(control_id)));
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
