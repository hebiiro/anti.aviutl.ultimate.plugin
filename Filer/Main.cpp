#include "pch.h"
#include "Hive.h"
#include "Servant.h"

using namespace fgo::filer;

//
// エクスポート関数です。このDLLで実装したサーヴァントを返します。
//
fgo::Servant* WINAPI get_servant()
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
