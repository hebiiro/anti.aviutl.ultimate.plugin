#pragma once

namespace apn::filer::share
{
	namespace message
	{
		inline static constexpr UINT c_init = WM_APP + 100;
		inline static constexpr UINT c_exit = WM_APP + 101;
		inline static constexpr UINT c_init_filer_window = WM_APP + 200;
		inline static constexpr UINT c_exit_filer_window = WM_APP + 201;
		inline static constexpr UINT c_get_file_name = WM_APP + 400;
	}

	namespace event
	{
		inline static constexpr auto c_post_init_client = _T("apn::filer::share::event::post_init_client");
	}

	namespace host_window
	{
		namespace prop_name
		{
			inline static constexpr auto client_window = _T("apn::filer::share::host_window::client_window");
			inline static constexpr auto dark = _T("apn::filer::share::HostWindow::dark");
		}

		inline HWND get_client_window(HWND host_window)
		{
			return (HWND)::GetProp(host_window, prop_name::client_window);
		}

		inline BOOL set_client_window(HWND host_window, HWND client_window)
		{
			return ::SetProp(host_window, prop_name::client_window, client_window);
		}

		inline BOOL get_dark(HWND host_window)
		{
			return !!::GetProp(host_window, prop_name::dark);
		}

		inline BOOL set_dark(HWND host_window, BOOL dark)
		{
			return ::SetProp(host_window, prop_name::dark, (HANDLE)(intptr_t)dark);
		}
	}

	namespace filer_window
	{
		namespace prop_name
		{
			inline static constexpr auto c_browser = _T("apn::filer::share::filer_window::browser");
		}

		inline HWND get_browser(HWND filer_window)
		{
			return (HWND)::GetProp(filer_window, prop_name::c_browser);
		}

		inline BOOL set_browser(HWND filer_window, HWND browser)
		{
			return ::SetProp(filer_window, prop_name::c_browser, browser);
		}
	}
}
