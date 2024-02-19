#include "pch.h"
#include "Servant.h"
#include "ActCtx.h"
#include "Hook/Api.h"

namespace fgo::dark
{
	//
	// エクスポート関数です。
	// このDLLで実装したサーヴァントを返します。
	//
	fgo::Servant* WINAPI get_servant()
	{
//		Tools::Tracer::logger = 0; // デバッグトレースを有効にする場合はこの行をコメントアウトしてください。

		return &servant;
	}

	//
	// エクスポート関数です。
	// 外部プロセスで使用する場合はこの関数をインポートして呼び出します。
	//
	BOOL WINAPI dark_init(HWND hwnd)
	{
		MY_TRACE_FUNC("0x%08X", hwnd);

		return servant.init(hwnd);
	}

	//
	// エクスポート関数です。
	// 現在のスキンのIDを返します。
	//
	UINT WINAPI dark_getCurrentId()
	{
		return skin::manager.currentId;
	}

	//
	// エクスポート関数です。
	// 指定された名前のカラーセットを返します。
	// 指定された名前のカラーセットが存在しない場合は0を返します。
	//
	skin::ColorSet* WINAPI dark_getNamedColor(LPCWSTR name)
	{
		MY_TRACE_FUNC("%s", name);

		return skin::color_set_manager.get(name);
	}

	//
	// エントリポイントです。
	//
	EXTERN_C BOOL APIENTRY DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
	{
		switch (reason)
		{
		case DLL_PROCESS_ATTACH:
			{
				MY_TRACE_FUNC("DLL_PROCESS_ATTACH");

				::DisableThreadLibraryCalls(instance);

				hive.instance = instance;

				actctx.init(instance);
				hook::api.init();

				break;
			}
		case DLL_PROCESS_DETACH:
			{
				MY_TRACE_FUNC("DLL_PROCESS_DETACH");

				hook::api.exit();
				actctx.exit();

				break;
			}
		}

		return TRUE;
	}
}
