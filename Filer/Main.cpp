#include "pch.h"
#include "Hive.h"
#include "Servant.h"

using namespace fgo::filer;

//
// エクスポート関数です。このDLLで実装したサーヴァントを返します。
//
fgo::Servant* get_servant()
{
//	CMyTracer::logger = 0; // デバッグトレースを有効にする場合はこの行をコメントアウトします。

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

			hive.instance = instance;

			break;
		}
	case DLL_PROCESS_DETACH:
		{
			break;
		}
	}

	return TRUE;
}
