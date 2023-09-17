#include "pch.h"
#include "Rayshift.h"

namespace fgo
{
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

				rayshift.init(instance);

				break;
			}
		case DLL_PROCESS_DETACH:
			{
				rayshift.exit();

				break;
			}
		}

		return TRUE;
	}
}
