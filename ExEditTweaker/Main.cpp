#include "pch.h"
#include "Servant.h"

using namespace fgo::exedit_tweaker;

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
