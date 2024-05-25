#pragma once

namespace apn::dark::hook
{
	//
	// このクラスはウィンドウ関係のフックを担当します。
	//
	inline struct Window
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// ダークエグゼの場合はフックする必要がないので何もしません。
			if (::GetModuleHandle(_T("aviutl_dark.exe"))) return TRUE;

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			my::hook::attach(CreateWindowExA);
			my::hook::attach(CreateWindowExW);
			my::hook::attach(CreateDialogParamA);
			my::hook::attach(CreateDialogParamW);
			my::hook::attach(CreateDialogIndirectParamA);
			my::hook::attach(CreateDialogIndirectParamW);
			my::hook::attach(DialogBoxParamA);
			my::hook::attach(DialogBoxParamW);
			my::hook::attach(DialogBoxIndirectParamA);
			my::hook::attach(DialogBoxIndirectParamW);

			if (DetourTransactionCommit() != NO_ERROR)
			{
				MY_TRACE("APIフックに失敗しました\n");

				return FALSE;
			}

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// このクラスは::CreateWindowExA()をフックします。
		//
		struct {
			inline static HWND WINAPI hook_proc(DWORD ex_style, LPCSTR class_name, LPCSTR window_name, DWORD style,
				int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param)
			{
				MY_TRACE_FUNC("{}, {}", safe_string(class_name), safe_string(window_name));

				ActCtx::Activator activator;

				return orig_proc(
					ex_style, class_name, window_name, style,
					x, y, w, h, parent, menu, instance, param);
			}
			inline static decltype(&hook_proc) orig_proc = ::CreateWindowExA;
		} CreateWindowExA;

		//
		// このクラスは::CreateWindowExW()をフックします。
		//
		struct {
			inline static HWND WINAPI hook_proc(DWORD ex_style, LPCWSTR class_name, LPCWSTR window_name, DWORD style,
				int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param)
			{
				MY_TRACE_FUNC("{}, {}", safe_string(class_name), safe_string(window_name));

				ActCtx::Activator activator;

				return orig_proc(
					ex_style, class_name, window_name, style,
					x, y, w, h, parent, menu, instance, param);
			}
			inline static decltype(&hook_proc) orig_proc = ::CreateWindowExW;
		} CreateWindowExW;

		//
		// このクラスは::CreateDialogParamA()をフックします。
		//
		struct {
			inline static HWND WINAPI hook_proc(
				_In_opt_ HINSTANCE instance,
				_In_ LPCSTR template_name,
				_In_opt_ HWND parent,
				_In_opt_ DLGPROC dlg_proc,
				_In_ LPARAM init_param)
			{
				MY_TRACE_FUNC("");

				ActCtx::Activator activator;

				return orig_proc(instance, template_name, parent, dlg_proc, init_param);
			}
			inline static decltype(&hook_proc) orig_proc = ::CreateDialogParamA;
		} CreateDialogParamA;

		//
		// このクラスは::CreateDialogParamW()をフックします。
		//
		struct {
			inline static HWND WINAPI hook_proc(
				_In_opt_ HINSTANCE instance,
				_In_ LPCWSTR template_name,
				_In_opt_ HWND parent,
				_In_opt_ DLGPROC dlg_proc,
				_In_ LPARAM init_param)
			{
				MY_TRACE_FUNC("");

				ActCtx::Activator activator;

				return orig_proc(instance, template_name, parent, dlg_proc, init_param);
			}
			inline static decltype(&hook_proc) orig_proc = ::CreateDialogParamW;
		} CreateDialogParamW;

		//
		// このクラスは::CreateDialogIndirectParamA()をフックします。
		//
		struct {
			inline static HWND WINAPI hook_proc(
				_In_opt_ HINSTANCE instance,
				_In_ LPCDLGTEMPLATEA lpTemplate,
				_In_opt_ HWND parent,
				_In_opt_ DLGPROC dlg_proc,
				_In_ LPARAM init_param)
			{
				MY_TRACE_FUNC("");

				ActCtx::Activator activator;

				return orig_proc(instance, lpTemplate, parent, dlg_proc, init_param);
			}
			inline static decltype(&hook_proc) orig_proc = ::CreateDialogIndirectParamA;
		} CreateDialogIndirectParamA;

		//
		// このクラスは::CreateDialogIndirectParamW()をフックします。
		//
		struct {
			inline static HWND WINAPI hook_proc(
				_In_opt_ HINSTANCE instance,
				_In_ LPCDLGTEMPLATEW lpTemplate,
				_In_opt_ HWND parent,
				_In_opt_ DLGPROC dlg_proc,
				_In_ LPARAM init_param)
			{
				MY_TRACE_FUNC("");

				ActCtx::Activator activator;

				return orig_proc(instance, lpTemplate, parent, dlg_proc, init_param);
			}
			inline static decltype(&hook_proc) orig_proc = ::CreateDialogIndirectParamW;
		} CreateDialogIndirectParamW;

		//
		// このクラスは::DialogBoxParamA()をフックします。
		//
		struct {
			inline static INT_PTR WINAPI hook_proc(
				_In_opt_ HINSTANCE instance,
				_In_ LPCSTR template_name,
				_In_opt_ HWND parent,
				_In_opt_ DLGPROC dlg_proc,
				_In_ LPARAM init_param)
			{
				MY_TRACE_FUNC("");

				ActCtx::Activator activator;

				return orig_proc(instance, template_name, parent, dlg_proc, init_param);
			}
			inline static decltype(&hook_proc) orig_proc = ::DialogBoxParamA;
		} DialogBoxParamA;

		//
		// このクラスは::DialogBoxParamW()をフックします。
		//
		struct {
			inline static INT_PTR WINAPI hook_proc(
				_In_opt_ HINSTANCE instance,
				_In_ LPCWSTR template_name,
				_In_opt_ HWND parent,
				_In_opt_ DLGPROC dlg_proc,
				_In_ LPARAM init_param)
			{
				MY_TRACE_FUNC("");

				ActCtx::Activator activator;

				return orig_proc(instance, template_name, parent, dlg_proc, init_param);
			}
			inline static decltype(&hook_proc) orig_proc = ::DialogBoxParamW;
		} DialogBoxParamW;

		//
		// このクラスは::DialogBoxIndirectParamA()をフックします。
		//
		struct {
			inline static INT_PTR WINAPI hook_proc(
				_In_opt_ HINSTANCE instance,
				_In_ LPCDLGTEMPLATEA lpTemplate,
				_In_opt_ HWND parent,
				_In_opt_ DLGPROC dlg_proc,
				_In_ LPARAM init_param)
			{
				MY_TRACE_FUNC("");

				ActCtx::Activator activator;

				return orig_proc(instance, lpTemplate, parent, dlg_proc, init_param);
			}
			inline static decltype(&hook_proc) orig_proc = ::DialogBoxIndirectParamA;
		} DialogBoxIndirectParamA;

		//
		// このクラスは::DialogBoxIndirectParamW()をフックします。
		//
		struct {
			inline static INT_PTR WINAPI hook_proc(
				_In_opt_ HINSTANCE instance,
				_In_ LPCDLGTEMPLATEW lpTemplate,
				_In_opt_ HWND parent,
				_In_opt_ DLGPROC dlg_proc,
				_In_ LPARAM init_param)
			{
				MY_TRACE_FUNC("");

				ActCtx::Activator activator;

				return orig_proc(instance, lpTemplate, parent, dlg_proc, init_param);
			}
			inline static decltype(&hook_proc) orig_proc = ::DialogBoxIndirectParamW;
		} DialogBoxIndirectParamW;
	} window;
}
