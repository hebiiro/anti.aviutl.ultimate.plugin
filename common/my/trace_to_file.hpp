#pragma once

namespace my::tracer_to_file
{
#ifdef MY_TRACE_ENABLED
	inline HANDLE file = 0;

	inline void init(HINSTANCE instance, LPCTSTR name = 0, BOOL add_bom = TRUE)
	{
		static struct FileLogger : Tracer::Logger
		{
			void output(LPCTSTR raw, LPCTSTR text) override
			{
				DWORD cb_size = (DWORD)(::lstrlen(text) * sizeof(TCHAR));
				DWORD write_size = 0;
				::WriteFile(file, text, cb_size, &write_size, 0);
			}
		} file_logger;

		Tracer::logger = &file_logger;

		// dll\$dll@exe[name].txt

		auto dll_file_name = get_module_file_name(instance);
		auto dll_path = std::wstring(dll_file_name.parent_path());
		auto dll_file_spec = std::wstring(dll_file_name.stem());

		auto exe_file_name = get_module_file_name(0);
		auto exe_file_spec = std::wstring(exe_file_name.stem());

		auto output_file_name = dll_path + _T("\\$") + dll_file_spec + _T("@") + exe_file_spec;

		if (name)
			output_file_name += name;

		output_file_name += _T(".txt");

		file = ::CreateFileW(output_file_name.c_str(), GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);
#ifdef _UNICODE
		if (add_bom)
		{
			BYTE bom[] = { 0xFF, 0xFE };
			DWORD write_size = 0;
			::WriteFile(file, bom, sizeof(bom), &write_size, 0);
		}
#endif
	}

	inline void exit()
	{
		::CloseHandle(file), file = 0;
	}
#else
	inline void init(HINSTANCE instance, LPCTSTR name, BOOL add_bom = FALSE)
	{
	}

	inline void exit()
	{
	}
#endif
}
