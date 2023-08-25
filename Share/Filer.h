#pragma once

namespace Share::Filer {
	namespace Message {
		inline const UINT FilerWindowCreated =	WM_APP + 1000;
		inline const UINT FilerDialogCreated =	WM_APP + 1001;
		inline const UINT GetUrl =				WM_APP + 1002;
		inline const UINT LoadConfig =			WM_APP + 1003;
		inline const UINT SaveConfig =			WM_APP + 1004;
	}
	namespace Event {
		inline const LPCTSTR ClientInited = _T("Share::Filer::Event::ClientInited");
	}
	namespace HostWindow {
		namespace PropName {
			inline const LPCTSTR ClientWindow = _T("Share::Filer::HostWindow::ClientWindow");
		}
		inline HWND getClientWindow(HWND hostWindow) {
			return (HWND)::GetProp(hostWindow, PropName::ClientWindow);
		}
		inline HWND setClientWindow(HWND hostWindow, HWND clientWindow) {
			return (HWND)::SetProp(hostWindow, PropName::ClientWindow, clientWindow);
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
