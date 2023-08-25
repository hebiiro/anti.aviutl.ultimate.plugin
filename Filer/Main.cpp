#include "pch.h"
#include "Hive.h"
#include "Servant.h"

//
// エントリポイントです。
//
BOOL APIENTRY DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		{
			fgo::filer::hive.instance = instance;

			break;
		}
	case DLL_PROCESS_DETACH:
		{
			break;
		}
	}

	return TRUE;
}

//
// エクスポート関数です。この DLL で実装したサーヴァントを返します。
//
fgo::Servant* get_servant()
{
//	CMyTracer::logger = 0; // デバッグトレースを有効にする場合はこの行をコメントアウトします。

	return &fgo::filer::servant;
}
