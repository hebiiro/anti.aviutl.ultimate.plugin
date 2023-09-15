#pragma once

namespace Share::Filer {
	namespace Message {
		inline const UINT PreInit =				WM_APP + 100;
		inline const UINT PostInit =			WM_APP + 101;
		inline const UINT PreExit =				WM_APP + 102;
		inline const UINT PostExit =			WM_APP + 103;
		inline const UINT PostInitFilerWindow =	WM_APP + 200;
		inline const UINT PostExitFilerWindow =	WM_APP + 201;
		inline const UINT PostInitFilerDialog =	WM_APP + 300;
		inline const UINT GetFileName =			WM_APP + 400;
	}
	namespace Event {
		inline const LPCTSTR PostInitClient = _T("Share::Filer::Event::PostInitClient");
	}
	namespace HostWindow {
		namespace PropName {
			inline const LPCTSTR ClientWindow = _T("Share::Filer::HostWindow::ClientWindow");
			inline const LPCTSTR Dark = _T("Share::Filer::HostWindow::Dark");
		}
		inline HWND getClientWindow(HWND hostWindow) {
			return (HWND)::GetProp(hostWindow, PropName::ClientWindow);
		}
		inline HWND setClientWindow(HWND hostWindow, HWND clientWindow) {
			return (HWND)::SetProp(hostWindow, PropName::ClientWindow, (HANDLE)clientWindow);
		}
		inline BOOL getDark(HWND hostWindow) {
			return (BOOL)::GetProp(hostWindow, PropName::Dark);
		}
		inline BOOL setDark(HWND hostWindow, BOOL dark) {
			return (BOOL)::SetProp(hostWindow, PropName::Dark, (HANDLE)dark);
		}
	}
	namespace FilerWindow {
		namespace PropName {
			inline const LPCTSTR Browser = _T("Share::Filer::FilerWindow::Browser");
		}
		inline HWND getBrowser(HWND filerWindow) {
			return (HWND)::GetProp(filerWindow, PropName::Browser);
		}
		inline HWND setBrowser(HWND filerWindow, HWND browser) {
			return (HWND)::SetProp(filerWindow, PropName::Browser, browser);
		}
	}
}
