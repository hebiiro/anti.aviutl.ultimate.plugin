#pragma once

namespace apn
{
	inline struct Env
	{
		BOOL init()
		{
			MY_TRACE_FUNC("");

			MY_TRACE_STR(hive.python_file_name);

			if (!hive.python_file_name.empty())
			{
				auto module_file_name = my::get_module_file_name(hive.instance);
				auto file_name = module_file_name.parent_path() / hive.python_file_name;
				MY_TRACE_STR(file_name);

				auto python32 = ::LoadLibraryEx(file_name.c_str(), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
				MY_TRACE_HEX(python32);
			}

			return TRUE;
		}

		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}
	} env;
}
