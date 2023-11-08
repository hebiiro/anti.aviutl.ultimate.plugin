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
	void WINAPI DarkenWindow_init(HWND hwnd)
	{
		MY_TRACE(_T("DarkenWindow_init(0x%08X)\n"), hwnd);

		servant.init(hwnd);
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
