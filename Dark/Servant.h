#pragma once
#include "Skin.h"
#include "Gdi/Client.h"
#include "Theme/Client.h"
#include "Hook/ExEdit.h"
#include "Hook/loudness.h"

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
		// この仮想関数は、初期化のタイミングで呼ばれます。
		//
		BOOL on_init() override
		{
			MY_TRACE_FUNC("");

			if (::GetModuleHandleW(L"DarkenWindow.aul"))
			{
				::MessageBoxW(magi.fp->hwnd,
					L"DarkとDarkenWindow.aulが競合しています"
					L"\nどちらかを除外してからAviUtlを再起動してください",
					get_servant_display_name(), MB_OK | MB_ICONWARNING);
			}

			init(magi.auin.GetAviUtlWindow());

			// 拡張編集をフックします。
			hook::exedit.init();

			// ラウドネスメーターをフックします。
			hook::loudness.init();

			// スキンの拡張編集用の設定を読み込みます。
			skin::manager.reloadExEditSettings();

			return TRUE;
		}

		//
		// この仮想関数は、後始末のタイミングで呼ばれます。
		//
		BOOL on_exit() override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// 初期化処理を実行します。
		//
		BOOL init(HWND hwnd)
		{
			static BOOL isInitialized = FALSE;
			if (isInitialized) return FALSE;
			isInitialized = TRUE;

			// hwndがAviUtlウィンドウだとみなします。
			hive.aviutlWindow = hwnd;

			// テーマレンダラの利用を開始します。
			theme::client.init(hwnd);

			// スキンを初期化します。
			skin::manager.init(hwnd);

			return TRUE;
		}
	} servant;
}
