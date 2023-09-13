#pragma once

namespace Tools::Trace::File {
#ifdef _DEBUG
	inline HANDLE file = 0;

	inline void init(HINSTANCE instance, LPCTSTR name, BOOL addBom = TRUE)
	{
		static struct FileLogger : Tracer::Logger {
			void output(LPCTSTR raw, LPCTSTR text) override {
				DWORD cbLength = (DWORD)(::lstrlen(text) * sizeof(TCHAR));
				DWORD writeSize = 0;
				::WriteFile(file, text, cbLength, &writeSize, 0);
			}
		} file_logger;

		Tracer::logger = &file_logger;

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

		file = ::CreateFile(reportFilePath, GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);
#ifdef _UNICODE
		if (addBom)
		{
			BYTE bom[] = { 0xFF, 0xFE };
			DWORD writeSize = 0;
			::WriteFile(file, bom, sizeof(bom), &writeSize, 0);
		}
#endif
	}

	inline void exit()
	{
		::CloseHandle(file), file = 0;
	}
#else
	inline void init(HINSTANCE instance, LPCTSTR name, BOOL addBom = FALSE)
	{
	}

	inline void exit()
	{
	}
#endif
}
