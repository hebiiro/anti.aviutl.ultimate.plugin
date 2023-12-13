#include "pch.h"
#include "Hive.h"
#include "Servant.h"

namespace fgo::easing_select
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
	// エントリポイントです。
	//
	EXTERN_C BOOL APIENTRY DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
	{
		switch (reason)
		{
		case DLL_PROCESS_ATTACH:
			{
				MY_TRACE(_T("DLL_PROCESS_ATTACH\n"));

				::DisableThreadLibraryCalls(instance);

				hive.instance = instance;

				break;
			}
		case DLL_PROCESS_DETACH:
			{
				MY_TRACE(_T("DLL_PROCESS_DETACH\n"));

				break;
			}
		}

		return TRUE;
	}
}
