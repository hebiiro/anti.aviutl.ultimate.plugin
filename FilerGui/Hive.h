#pragma once

//
// このクラスは他クラスから共通して使用される変数を保持します。
//
struct Hive
{
	struct TimerID {
		inline static const UINT CheckHostWindow =		1000;
		inline static const UINT CheckConfig =			1001;
	};

	HWND hostWindow = 0;
	HWND mainWindow = 0;
	TCHAR configFileName[MAX_PATH] = {};

	Hive(HWND hostWindow)
		: hostWindow(hostWindow)
	{
		MY_TRACE(_T("Hive::Hive(0x%08X)\n"), hostWindow);

		TCHAR fileName[MAX_PATH] = {};
		::GetModuleFileName(0, fileName, std::size(fileName));
		::PathAppend(fileName, _T("../../UltimateConfig/Filer.xml"));
		::PathCanonicalize(configFileName, fileName);
		MY_TRACE_TSTR(configFileName);
	}
}; inline std::shared_ptr<Hive> hive;
