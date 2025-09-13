#pragma once

namespace apn::optima::part
{
#if 1
	//
	// このクラスはウィンドウの作成をフックします。
	//
	inline struct CreateWindowHook : Node
	{
		//
		// 初期化処理を実行します。
		//
		virtual BOOL func_init(AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			my::hook::attach(CreateWindowExA);

			if (DetourTransactionCommit() != NO_ERROR)
			{
				MY_TRACE("APIフックに失敗しました\n");

				return FALSE;
			}

			// ドロップダウン用のリストボックスを作成します。
			if (!ult_combobox::popup::listbox->create(
				WS_EX_TOOLWINDOW, _T("ComboLBox"), nullptr,
				WS_POPUP | WS_CLIPSIBLINGS | WS_BORDER |
				LBS_COMBOBOX | LBS_HASSTRINGS | LBS_SORT | LBS_NOTIFY,
				0, 0, 100, 100, nullptr, nullptr, hive.instance, nullptr))
			{
				MY_TRACE("ポップアップリストボックスの作成に失敗しました\n");

				return FALSE;
			}

			// リストボックスのコントロールIDをセットします。
			::SetWindowLongPtr(*ult_combobox::popup::listbox, GWLP_ID, 1000);

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL func_exit(AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// このクラスは::CreateWindowExA()をフックします。
		//
		inline static struct {
			inline static HWND WINAPI hook_proc(DWORD ex_style, LPCSTR class_name, LPCSTR window_name,
				DWORD style, int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param)
			{
				// 設定ダイアログ内のドロップダウンリスト形式のコンボボックスの場合は
				if (style == (WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_SORT) &&
					parent == magi.exin.get_setting_dialog() && ::lstrcmpiA(class_name, WC_COMBOBOXA) == 0)
				{
					// 自作コンボボックスの作成に成功した場合は
					if (auto my_combobox = ult_combobox::drop_down_list::ComboBox::create(
						0, WC_COMBOBOX, nullptr, style, x, y, w, h, parent, menu, instance, param))
					{
						// 自作コンボボックスを返します。
						return *my_combobox;
					}
				}

				// それ以外の場合はデフォルト処理を実行します。
				return orig_proc(ex_style, class_name, window_name, style, x, y, w, h, parent, menu, instance, param);
			}
			inline static decltype(&hook_proc) orig_proc = ::CreateWindowExA;
		} CreateWindowExA;
	} create_window_hook;
#endif
}
