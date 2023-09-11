#pragma once

namespace fgo::nest::hook
{
	//
	// このクラスはWinEventをフックします。
	//
	inline static struct WinEvent
	{
		HWINEVENTHOOK hook = 0;

		//
		// 初期化処理です。
		//
		void init()
		{
			MY_TRACE(_T("WinEvent::init()\n"));

			DWORD pid = 0;
			DWORD tid = 0;

			hook = ::SetWinEventHook(
				EVENT_MIN, EVENT_MAX,
				0,
				hookProc,
				pid,
				tid,
				WINEVENT_OUTOFCONTEXT);
//				WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
			MY_TRACE_HEX(hook);
		}

		//
		// 後始末処理です。
		//
		void exit()
		{
			MY_TRACE(_T("WinEvent::exit()\n"));

			::UnhookWinEvent(hook); hook = 0;
		}

		static void CALLBACK hookProc(
			HWINEVENTHOOK hWinEventHook,
			DWORD event,
			HWND hwnd,
			LONG idObject,
			LONG idChild,
			DWORD idEventThread,
			DWORD dwmsEventTime)
		{
			if (::GetAsyncKeyState(VK_SHIFT) < 0)
				traceEventHook(hWinEventHook, event, hwnd, idObject, idChild, idEventThread, dwmsEventTime);
		}

static void CALLBACK traceEventHook(
	HWINEVENTHOOK hWinEventHook,
	DWORD event,
	HWND hwnd,
	LONG idObject,
	LONG idChild,
	DWORD idEventThread,
	DWORD dwmsEventTime)
{
	static int i = 0; i++;
#if 0
	if (event == 0x00007562) return;

	if (event == EVENT_OBJECT_REORDER) return;
	if (event == EVENT_OBJECT_VALUECHANGE) return;
	if (event == EVENT_OBJECT_NAMECHANGE && idObject == OBJID_CURSOR) return;
	if (event == EVENT_OBJECT_LOCATIONCHANGE && idObject == OBJID_CURSOR) return;
	if (hwnd != ::GetDesktopWindow()) return;
#endif
	TCHAR eventText[MAX_PATH];
	switch (event)
	{
	case EVENT_SYSTEM_FOREGROUND:
		::StringCbCopy(eventText, sizeof(eventText), _T("EVENT_SYSTEM_FOREGROUND"));
		break;
	case EVENT_SYSTEM_MENUSTART:
		::StringCbCopy(eventText, sizeof(eventText), _T("EVENT_SYSTEM_MENUSTART"));
		break;
	case EVENT_SYSTEM_MENUEND:
		::StringCbCopy(eventText, sizeof(eventText), _T("EVENT_SYSTEM_MENUEND"));
		break;
	case EVENT_SYSTEM_MENUPOPUPSTART:
		::StringCbCopy(eventText, sizeof(eventText), _T("EVENT_SYSTEM_MENUPOPUPSTART"));
		break;
	case EVENT_SYSTEM_MENUPOPUPEND:
		::StringCbCopy(eventText, sizeof(eventText), _T("EVENT_SYSTEM_MENUPOPUPEND"));
		break;
	case EVENT_SYSTEM_IME_KEY_NOTIFICATION:
		::StringCbCopy(eventText, sizeof(eventText), _T("EVENT_SYSTEM_IME_KEY_NOTIFICATION"));
		break;
	case EVENT_OBJECT_CREATE:
		::StringCbCopy(eventText, sizeof(eventText), _T("CREATE"));
		break;
	case EVENT_OBJECT_DESTROY:
		::StringCbCopy(eventText, sizeof(eventText), _T("DESTROY"));
		break;
	case EVENT_OBJECT_SHOW:
		::StringCbCopy(eventText, sizeof(eventText), _T("SHOW"));
		break;
	case EVENT_OBJECT_HIDE:
		::StringCbCopy(eventText, sizeof(eventText), _T("HIDE"));
		break;
	case EVENT_OBJECT_REORDER:
		::StringCbCopy(eventText, sizeof(eventText), _T("REORDER"));
		break;
	case EVENT_OBJECT_SELECTION:
		::StringCbCopy(eventText, sizeof(eventText), _T("SELECTION"));
		break;
	case EVENT_OBJECT_FOCUS:
		::StringCbCopy(eventText, sizeof(eventText), _T("FOCUS"));
		break;
	case EVENT_OBJECT_STATECHANGE:
		::StringCbCopy(eventText, sizeof(eventText), _T("STATECHANGE"));
		break;
	case EVENT_OBJECT_LOCATIONCHANGE:
		::StringCbCopy(eventText, sizeof(eventText), _T("LOCATIONCHANGE"));
		break;
	case EVENT_OBJECT_NAMECHANGE:
		::StringCbCopy(eventText, sizeof(eventText), _T("NAMECHANGE"));
		break;
	case EVENT_OBJECT_VALUECHANGE:
		::StringCbCopy(eventText, sizeof(eventText), _T("VALUECHANGE"));
		break;
	case EVENT_OBJECT_TEXTSELECTIONCHANGED:
		::StringCbCopy(eventText, sizeof(eventText), _T("TEXTSELECTIONCHANGED"));
		break;
	case EVENT_OBJECT_IME_CHANGE:
		::StringCbCopy(eventText, sizeof(eventText), _T("IME_CHANGE"));
		break;
	case EVENT_OBJECT_IME_SHOW:
		::StringCbCopy(eventText, sizeof(eventText), _T("IME_SHOW"));
		break;
	case EVENT_OBJECT_IME_HIDE:
		::StringCbCopy(eventText, sizeof(eventText), _T("IME_HIDE"));
		break;
	default:
		if (event >= EVENT_UIA_EVENTID_START && event <= EVENT_UIA_EVENTID_END)
		{
			::StringCbPrintf(eventText, sizeof(eventText), _T("UIA_EVENT_0x%08X"), event);
		}
		else if (event >= EVENT_UIA_PROPID_START && event <= EVENT_UIA_PROPID_END)
		{
			::StringCbPrintf(eventText, sizeof(eventText), _T("UIA_PROP_0x%08X"), event);
		}
		else
		{
			::StringCbPrintf(eventText, sizeof(eventText), _T("0x%08X"), event);
		}
		break;
	}

	TCHAR wndText[MAX_PATH];
	if (!::GetClassName(hwnd, wndText, MAX_PATH))
	{
		if (hwnd)
			::StringCbPrintf(wndText, sizeof(wndText), _T("0x%08X"), hwnd);
		else
			::StringCbCopy(wndText, sizeof(wndText), _T("0"));
	}

	TCHAR objectText[MAX_PATH];
	switch (idObject)
	{
	case OBJID_WINDOW:
		::StringCbCopy(objectText, sizeof(objectText), _T("Window"));
		break;
	case OBJID_SYSMENU:
		::StringCbCopy(objectText, sizeof(objectText), _T("SysMenu"));
		break;
	case OBJID_TITLEBAR:
		::StringCbCopy(objectText, sizeof(objectText), _T("TitleBar"));
		break;
	case OBJID_MENU:
		::StringCbCopy(objectText, sizeof(objectText), _T("Menu"));
		break;
	case OBJID_CLIENT:
		::StringCbCopy(objectText, sizeof(objectText), _T("Client"));
		break;
	case OBJID_VSCROLL:
		::StringCbCopy(objectText, sizeof(objectText), _T("VScroll"));
		break;
	case OBJID_HSCROLL:
		::StringCbCopy(objectText, sizeof(objectText), _T("HScroll"));
		break;
	case OBJID_SIZEGRIP:
		::StringCbCopy(objectText, sizeof(objectText), _T("SizeGrip"));
		break;
	case OBJID_CARET:
		::StringCbCopy(objectText, sizeof(objectText), _T("Caret"));
		break;
	case OBJID_CURSOR:
		::StringCbCopy(objectText, sizeof(objectText), _T("Cursor"));
		break;
	default:
		::StringCbPrintf(objectText, sizeof(objectText), _T("0x%08X"), idObject);
		break;
	}

	TCHAR childText[MAX_PATH];
	if (idChild)
		::StringCbPrintf(childText, sizeof(childText), _T("0x%08X"), idChild);
	else
		::StringCbCopy(childText, sizeof(childText), _T("0"));

	HWND foreWnd = ::GetForegroundWindow();
	DWORD forePID = 0;
	DWORD foreTID = ::GetWindowThreadProcessId(foreWnd, &forePID);

	TCHAR foreWndText[MAX_PATH];
	if (!::GetClassName(foreWnd, foreWndText, MAX_PATH))
	{
		if (foreWnd)
			::StringCbPrintf(foreWndText, sizeof(foreWndText), _T("0x%08X"), foreWnd);
		else
			::StringCbCopy(foreWndText, sizeof(foreWndText), _T("0"));
	}

//	if (idObject == OBJID_CURSOR) return;
	if (::StrStr(wndText, _T("HwndWrapper"))) return;
	if (::StrStr(wndText, _T("Internet Explorer_Server"))) return;

	MY_TRACE(_T("%d: e=%s, w=%s, o=%s, c=%s, t=0x%08X, ms=%d, fw=%s, ft=0x%08X\n"),
		i, eventText, wndText, objectText, childText, idEventThread, dwmsEventTime, foreWndText, foreTID);
}
	} win_event;
}
