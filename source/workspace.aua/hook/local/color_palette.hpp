#pragma once

namespace apn::workspace::hook
{
	//
	// このクラスはcolor_palette.aufをフックします。
	//
	inline struct : Entity {
		//
		// このクラスは::CreateDialogParamA()をフックします。
		// マイパレットが作成されるとき、マイパレット用シャトルを作成します。
		//
		inline static struct {
			inline static HWND WINAPI hook_proc(HINSTANCE instance, LPCSTR template_name, HWND parent, DLGPROC dlg_proc, LPARAM init_param)
			{
				MY_TRACE_FUNC("");

				auto hwnd = orig_proc(instance, template_name, parent, dlg_proc, init_param);

				auto window_name = my::get_window_text(hwnd);
				MY_TRACE_STR(window_name);

				if (window_name == _T("マイパレット"))
				{
					// マイパレットダイアログ用のシャトルを作成します。
					auto shuttle = std::make_shared<Shuttle>();
					shuttle->init(window_name, hwnd);
				}

				return hwnd;
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} CreateDialogParamA;

		//
		// このクラスは::ShowWindow()をフックします。
		// カラーパレットがドッキングしているマイパレットを表示するのを防ぎます。
		//
		inline static struct {
			inline static BOOL WINAPI hook_proc(HWND hwnd, int show_cmd)
			{
				MY_TRACE_FUNC("{/hex}, {/}", hwnd, show_cmd);

				if (show_cmd)
				{
					auto shuttle = shuttle_manager.get(L"マイパレット");
					if (shuttle && *shuttle == hwnd)
					{
						MY_TRACE_INT(shuttle->is_docking());

						// マイパレットがドッキング中の場合は
						if (shuttle->is_docking())
							return FALSE; // マイパレットを表示しません。
					}
				}

				return orig_proc(hwnd, show_cmd);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} ShowWindow;

		//
		// 初期化処理を実行します。
		//
		virtual BOOL on_init() override
		{
			MY_TRACE_FUNC("");

			auto color_palette = ::GetModuleHandle(_T("color_palette.auf"));
			MY_TRACE_HEX(color_palette);
			if (!color_palette) return FALSE;

			// color_palette.auf内の::CreateDialogParamA()をフックします。
			my::hook::attach_import(CreateDialogParamA, color_palette, "CreateDialogParamA");
			MY_TRACE_HEX(CreateDialogParamA.orig_proc);

			// color_palette.auf内の::ShowWindow()をフックします。
			my::hook::attach_import(ShowWindow, color_palette, "ShowWindow");
			MY_TRACE_HEX(ShowWindow.orig_proc);

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
	} color_palette;
}
