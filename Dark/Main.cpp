#include "pch.h"
#include "Servant.h"

namespace fgo::dark
{
	//
	// エクスポート関数です。
	// このDLLで実装したサーヴァントを返します。
	//
	fgo::Servant* WINAPI get_servant()
	{
		Tools::Tracer::logger = 0; // デバッグトレースを有効にする場合はこの行をコメントアウトしてください。

		return &servant;
	}
#if 0
	//
	// エントリポイントです。
	//
	EXTERN_C BOOL APIENTRY DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
	{
		switch (reason)
		{
		case DLL_PROCESS_ATTACH:
			{
				::DisableThreadLibraryCalls(instance);

				break;
			}
		case DLL_PROCESS_DETACH:
			{
				break;
			}
		}

		return TRUE;
	}
#endif
}
