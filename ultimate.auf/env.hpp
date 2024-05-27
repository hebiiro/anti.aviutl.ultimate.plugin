#pragma once

namespace apn
{
	inline struct Env
	{
		BOOL init()
		{
			MY_TRACE_FUNC("");

			MY_TRACE_STR(hive.python_file_name);

			::LoadLibraryEx(hive.python_file_name.c_str(), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);

			return TRUE;
		}

		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}
	} env;
}
