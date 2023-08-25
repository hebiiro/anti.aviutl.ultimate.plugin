#pragma once

//
// このクラスは他クラスから共通して使用される変数を保持します。
//
struct Hive
{
	struct TimerID {
		const static UINT CheckHostWindow =		1000;
		const static UINT CheckConfig =			1001;
	};

	TCHAR hostProcessFileName[MAX_PATH] = {};
	TCHAR wavPlayerFileName[MAX_PATH] = {};
	TCHAR configFileName[MAX_PATH] = {};
	HWND hostWindow = 0;
	BOOL isVoiceEnabled = TRUE;
	std::vector<CString> favorites;

	Hive(HWND hostWindow)
		: hostWindow(hostWindow)
	{
		MY_TRACE(_T("Hive::Hive(0x%08X)\n"), hostWindow);

		DWORD pid = 0;
		::GetWindowThreadProcessId(hostWindow, &pid);
		Handle process = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
		::GetModuleFileNameEx(process, 0, hostProcessFileName, std::size(hostProcessFileName));
		MY_TRACE_TSTR(hostProcessFileName);

		::StringCbCopy(wavPlayerFileName, sizeof(wavPlayerFileName), hostProcessFileName);
		::PathRemoveExtension(wavPlayerFileName);
		::StringCbCat(wavPlayerFileName, sizeof(wavPlayerFileName), L"Plugin/WavPlayer.exe");
		::PathQuoteSpaces(wavPlayerFileName);
		MY_TRACE_TSTR(wavPlayerFileName);

		::StringCbCopy(configFileName, sizeof(configFileName), hostProcessFileName);
		::PathRemoveExtension(configFileName);
		::StringCbCat(configFileName, sizeof(configFileName), L"Config/FilerGui.xml");
		MY_TRACE_TSTR(configFileName);
	}
}; inline std::shared_ptr<Hive> hive;
