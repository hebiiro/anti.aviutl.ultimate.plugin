﻿#pragma once

namespace fgo::dark
{
	//
	// このクラスはUIにカスタムテーマを適用できるようにします。
	//
	inline struct Dark : Servant
	{
		//
		// この仮想関数は、このサーヴァントの識別名が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_name() override
		{
			return L"Dark";
		}

		//
		// この仮想関数は、このサーヴァントの表示名が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_display_name() override
		{
			return L"ダーク";
		}

		//
		// この仮想関数は、DLLの初期化のタイミングで呼ばれます。
		//
		BOOL on_dll_init(HINSTANCE instance) override
		{
			MY_TRACE_FUNC("0x%08X", instance);

			return actctx.init(instance);
		}

		//
		// この仮想関数は、DLLの後始末のタイミングで呼ばれます。
		//
		BOOL on_dll_exit(HINSTANCE instance) override
		{
			MY_TRACE_FUNC("");

			return actctx.exit();
		}

		//
		// この仮想関数は、初期化のタイミングで呼ばれます。
		//
		BOOL on_init() override
		{
			if (::GetModuleHandleW(L"DarkenWindow.aul"))
			{
				::MessageBoxW(magi.fp->hwnd,
					L"DarkとDarkenWindow.aulが競合しています"
					L"\nどちらかを除外してからAviUtlを再起動してください",
					get_servant_display_name(), MB_OK | MB_ICONWARNING);
			}

			if (!load()) return FALSE;
			if (!init()) return FALSE;
			return TRUE;
		}

		//
		// この仮想関数は、後始末のタイミングで呼ばれます。
		//
		BOOL on_exit() override
		{
			if (!save()) return FALSE;
			if (!exit()) return FALSE;
			return TRUE;
		}

		//
		// コンフィグファイルのフルパスを返します。
		//
		inline static std::wstring getConfigFileName()
		{
			return magi.getConfigFileName(L"Dark.ini");
		}

		//
		// コンフィグファイル名を取得し、設定を読み込みます。
		//
		BOOL load()
		{
			return load(getConfigFileName().c_str());
		}

		//
		// コンフィグファイルから設定を読み込みます。
		//
		BOOL load(LPCWSTR path)
		{
			return TRUE;
		}

		//
		// コンフィグファイル名を取得し、設定を保存します。
		//
		BOOL save()
		{
			return save(getConfigFileName().c_str());
		}

		//
		// コンフィグファイルに設定を保存します。
		//
		BOOL save(LPCWSTR path)
		{
			return TRUE;
		}

		//
		// 初期化処理です。
		//
		BOOL init()
		{
			return TRUE;
		}

		//
		// 後始末処理です。
		//
		BOOL exit()
		{
			return TRUE;
		}

		//
		// このクラスはアクティブ化コンテキストを管理します。
		//
		struct
		{
			ACTCTXW ac = { sizeof(ac) };
			HANDLE hac = INVALID_HANDLE_VALUE;
			ULONG_PTR cookie = 0;

			//
			// アクティブ化コンテキストを作成します。
			//
			BOOL init(HINSTANCE instance)
			{
				MY_TRACE(_T("actctx.init(0x%08X)\n"), instance);

				// マニフェストのファイル名を取得します。
				WCHAR fileName[MAX_PATH] = {};
				::GetModuleFileNameW(instance, fileName, std::size(fileName));
				::PathRemoveFileSpecW(fileName);
				::PathAppendW(fileName, L"cc6.manifest");
				MY_TRACE_WSTR(fileName);

				// アクティブ化コンテキストを作成します。
				ac.lpSource = fileName;
				hac = ::CreateActCtxW(&ac);
				BOOL result = ::ActivateActCtx(hac, &cookie);
				MY_TRACE_INT(result);

				return result;
			}

			//
			// アクティブ化コンテキストを削除します。
			//
			BOOL exit()
			{
				MY_TRACE(_T("actctx.exit()\n"));

				if (cookie) ::DeactivateActCtx(0, cookie), cookie = 0;
				if (hac != INVALID_HANDLE_VALUE) ::ReleaseActCtx(hac), hac = INVALID_HANDLE_VALUE;
				return TRUE;
			}
		} actctx;
	} servant;
}
