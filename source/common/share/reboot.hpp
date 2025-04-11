#pragma once

namespace apn::reboot::share
{
	inline constexpr struct Message {
		inline static constexpr uint32_t c_reboot = WM_APP + 100;
	} c_message;

	struct RebootSignal {
		WCHAR path[MAX_PATH * 4];
		WCHAR args[MAX_PATH * 4];
	};

	//
	// 指定されたファイルを実行します。
	//
	inline BOOL shell_execute(LPCTSTR verb, LPCTSTR file, LPCTSTR parameters, LPCTSTR directory = nullptr)
	{
		MY_TRACE_FUNC("{/}, {/}, {/}, {/}", verb, file, parameters, directory);

		SHELLEXECUTEINFO sei = { sizeof(sei) };
		sei.lpVerb = verb;
		sei.nShow = SW_SHOWNORMAL;
		sei.lpFile = file;
		sei.lpParameters = parameters;
		sei.lpDirectory = directory;
		return ::ShellExecuteEx(&sei);
	}
}
