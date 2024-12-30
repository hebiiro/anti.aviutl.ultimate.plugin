#pragma once

namespace apn::dialog_size
{
	//
	// このクラスはフックを管理します。
	//
	inline struct HookManager
	{
		HINSTANCE aviutl = nullptr;
		HINSTANCE exedit = nullptr;

		//
		// カスタマイズされたダイアログのインスタンスを返します。
		//
		inline static HINSTANCE customize(HINSTANCE instance)
		{
			return hive.instance;
		}

		//
		// カスタマイズされたダイアログのテンプレート名を返します。
		//
		inline static std::string customize(LPCSTR template_name)
		{
			std::string s(template_name);
			auto it = std::find_if(std::begin(hive.custom_templates), std::end(hive.custom_templates),
				[s](const auto& custom_template) { return s == custom_template.target_template_name; });
			if (it != std::end(hive.custom_templates))
			{
				if (it->active)
					return s + "_CUSTOM";
			}

			return s;
		}

		//
		// 与えられたインスタンスがカスタマイズ対象の場合はTRUEを返します。
		//
		inline BOOL is_target(HINSTANCE instance)
		{
			if (instance == aviutl) return TRUE;
			if (instance == exedit) return TRUE;

			return FALSE;
		}

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			aviutl = ::GetModuleHandle(nullptr);
			exedit = ::GetModuleHandle(_T("exedit.auf"));

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			auto user32 = ::GetModuleHandle(_T("user32.dll"));
			MY_TRACE_HEX(user32);

			my::hook::attach(DialogBoxParamA, ::GetProcAddress(user32, "DialogBoxParamA"));
			my::hook::attach(CreateDialogParamA, ::GetProcAddress(user32, "CreateDialogParamA"));

			auto comdlg32 = ::GetModuleHandle(_T("comdlg32.dll"));
			MY_TRACE_HEX(comdlg32);

			my::hook::attach(GetOpenFileNameA, ::GetProcAddress(comdlg32, "GetOpenFileNameA"));
			my::hook::attach(GetSaveFileNameA, ::GetProcAddress(comdlg32, "GetSaveFileNameA"));

			if (DetourTransactionCommit() == NO_ERROR)
			{
				MY_TRACE("拡張編集のフックに成功しました\n");

				return TRUE;
			}
			else
			{
				MY_TRACE("拡張編集のフックに失敗しました\n");

				return FALSE;
			}
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
		// このクラスは::DialogBoxParamA()をフックします。
		//
		inline static struct {
			inline static INT_PTR WINAPI hook_proc(HINSTANCE instance, LPCSTR template_name, HWND parent, DLGPROC dlg_proc, LPARAM init_param)
			{
				const auto do_customize = [&]() {
					return orig_proc(customize(instance), customize(template_name).c_str(), parent, dlg_proc, init_param);
				};

				if ((DWORD)template_name <= 0x0000FFFF)
				{
					MY_TRACE_FUNC("{:#010x}, {}", instance, template_name);
				}
				else
				{
					MY_TRACE_FUNC("{:#010x}, {}", instance, template_name);

					if (hook_manager.is_target(instance))
					{
						MY_TRACE("ダイアログを置き換えます\n");

						if (::lstrcmpA(template_name, "NEW_FILE") == 0)
						{
							// `新規プロジェクトの作成`ダイアログをフックします。
							decltype(new_file_dialog)::Hooker hooker(new_file_dialog);
							return do_customize();
						}
						else if (::lstrcmpA(template_name, "SAVE_OBJECT") == 0)
						{
							// `エイリアスの作成`ダイアログをフックします。
							decltype(save_object_dialog)::Hooker hooker(save_object_dialog);
							return do_customize();
						}

						return do_customize();
					}
				}

				return orig_proc(instance, template_name, parent, dlg_proc, init_param);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} DialogBoxParamA;

		//
		// このクラスは::CreateDialogParamA()をフックします。
		//
		inline static struct {
			inline static HWND WINAPI hook_proc(HINSTANCE instance, LPCSTR template_name, HWND parent, DLGPROC dlg_proc, LPARAM init_param)
			{
				if ((DWORD)template_name <= 0x0000FFFF)
				{
					MY_TRACE_FUNC("{:#010x}, {}", instance, template_name);
				}
				else
				{
					MY_TRACE_FUNC("{:#010x}, {}", instance, template_name);

					if (hook_manager.is_target(instance))
					{
						MY_TRACE("ダイアログを置き換えます\n");

						return orig_proc(customize(instance), customize(template_name).c_str(), parent, dlg_proc, init_param);
					}
				}

				return orig_proc(instance, template_name, parent, dlg_proc, init_param);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} CreateDialogParamA;

		//
		// このクラスは::GetOpenFileNameA()をフックします。
		//
		inline static struct {
			inline static BOOL WINAPI hook_proc(LPOPENFILENAMEA ofn)
			{
				MY_TRACE_FUNC("");

				if (ofn->hInstance == hook_manager.aviutl)
				{
					MY_TRACE("ダイアログを置き換えます\n");

					ofn->hInstance = customize(ofn->hInstance);
				}

				return orig_proc(ofn);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} GetOpenFileNameA;

		//
		// このクラスは::GetSaveFileNameA()をフックします。
		//
		inline static struct {
			inline static BOOL WINAPI hook_proc(LPOPENFILENAMEA ofn)
			{
				MY_TRACE_FUNC("");

				if (ofn->hInstance == hook_manager.aviutl)
				{
					MY_TRACE("ダイアログを置き換えます\n");

					ofn->hInstance = customize(ofn->hInstance);
				}

				return orig_proc(ofn);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} GetSaveFileNameA;
	} hook_manager;
}
