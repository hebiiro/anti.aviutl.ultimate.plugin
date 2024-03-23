#include "pch.h"
#include "hive.hpp"
#include "loader.hpp"
#include "hook.hpp"

namespace apn
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
				::DisableThreadLibraryCalls(hive.instance = instance);

				hook_manager.init();

				break;
			}
		case DLL_PROCESS_DETACH:
			{
				hook_manager.exit();

				break;
			}
		}

		return TRUE;
	}
}
