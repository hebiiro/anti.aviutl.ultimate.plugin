#pragma once
#ifdef _DEBUG
inline HANDLE g_traceFile = 0;

inline void trace_init(HINSTANCE instance, LPCTSTR name, BOOL addBom = TRUE)
{
	static struct FileLogger : CMyTracer::Logger {
		void outputLog(LPCTSTR text, LPCTSTR output) override {
			DWORD cbLength = (DWORD)(::lstrlen(output) * sizeof(TCHAR));
			DWORD writeSize = 0;
			::WriteFile(g_traceFile, output, cbLength, &writeSize, 0);
		}
	} fileLogger;

	CMyTracer::logger = &fileLogger;

	// dll\$dll@exe[name].txt

	TCHAR dllPath[MAX_PATH];
	::GetModuleFileName(instance, dllPath, MAX_PATH);
	::PathRemoveFileSpec(dllPath);
	//::StringCbCopy(dllPath, sizeof(dllPath), _T("C:\\Temp"));

	TCHAR dllFileSpec[MAX_PATH];
	::GetModuleFileName(instance, dllFileSpec, MAX_PATH);
	::PathStripPath(dllFileSpec);

	TCHAR exeFileSpec[MAX_PATH];
	::GetModuleFileName(0, exeFileSpec, MAX_PATH);
	::PathStripPath(exeFileSpec);

	TCHAR reportFilePath[MAX_PATH];
	::StringCbCopy(reportFilePath, sizeof(reportFilePath), dllPath);
	::StringCbCat(reportFilePath, sizeof(reportFilePath), _T("\\$"));
	::StringCbCat(reportFilePath, sizeof(reportFilePath), dllFileSpec);
	::StringCbCat(reportFilePath, sizeof(reportFilePath), _T("@"));
	::StringCbCat(reportFilePath, sizeof(reportFilePath), exeFileSpec);

	if (name)
		::StringCbCat(reportFilePath, sizeof(reportFilePath), name);

	::StringCbCat(reportFilePath, sizeof(reportFilePath), _T(".txt"));

	g_traceFile = ::CreateFile(reportFilePath, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);
#ifdef _UNICODE
	if (addBom)
	{
		BYTE bom[] = { 0xFF, 0xFE };
		DWORD writeSize = 0;
		::WriteFile(g_traceFile, bom, sizeof(bom), &writeSize, 0);
	}
#endif
}

inline void trace_term()
{
	::CloseHandle(g_traceFile), g_traceFile = 0;
}
#else
inline void trace_init(HINSTANCE instance, LPCTSTR name, BOOL addBom = FALSE)
{
}

inline void trace_term()
{
}
#endif
