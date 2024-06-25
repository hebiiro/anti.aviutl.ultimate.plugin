#pragma once

namespace apn
{
	inline struct Env
	{
		BOOL init()
		{
			MY_TRACE_FUNC("");

			MY_TRACE_STR(hive.python_file_name);

			std::filesystem::path file_name = hive.python_file_name;
			if (file_name.is_relative())
			{
				auto module_file_name = my::get_module_file_name(hive.instance);
				file_name = module_file_name.parent_path() / hive.python_file_name;
			}
			MY_TRACE_STR(file_name);

			auto python32 = ::LoadLibraryEx(file_name.c_str(), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
			MY_TRACE_HEX(python32);

			return TRUE;
		}

		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}
	} env;
}
