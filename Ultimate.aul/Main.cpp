#include "pch.h"

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

			// Ultimate.auf をロードします。
			WCHAR moduleFileName[MAX_PATH] = {};
			::GetModuleFileNameW(instance, moduleFileName, MAX_PATH);
			::PathRenameExtensionW(moduleFileName, L".auf");
			::LoadLibraryW(moduleFileName);

			break;
		}
	case DLL_PROCESS_DETACH:
		{
			break;
		}
	}

	return TRUE;
}
