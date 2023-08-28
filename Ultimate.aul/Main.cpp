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
			// 参照カウンタを増やしてこの DLL がアンロードされないようにします。
			WCHAR moduleFileName[MAX_PATH] = {};
			::GetModuleFileNameW(instance, moduleFileName, MAX_PATH);
			::LoadLibraryW(moduleFileName);

			// Ultimate.auf をロードします。
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
