#include "pch.h"
#include "Servant.h"

using namespace fgo::filter_drag;

fgo::Servant* get_servant()
{
	CMyTracer::logger = 0; // デバッグトレースを有効にする場合はこの行をコメントアウトしてください。

	return &servant;
}

//
// エントリポイントです。
//
BOOL APIENTRY DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
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
