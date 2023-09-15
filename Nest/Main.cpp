#include "pch.h"
#include "Servant.h"

using namespace fgo::nest;

//
// エクスポート関数です。
// このDLLで実装したサーヴァントを返します。
//
fgo::Servant* WINAPI get_servant()
{
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
