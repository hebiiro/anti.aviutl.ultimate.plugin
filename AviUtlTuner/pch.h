#pragma once

#include <tchar.h>
#include <crtdbg.h>
#include <locale.h>
#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <list>
#include <unordered_map>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include <vsstyle.h>
#include <vssym32.h>
#include <uxtheme.h>
#pragma comment(lib, "uxtheme.lib")
namespace Gdiplus {
	template<class T> const T& min(const T& a, const T& b) { return std::min(a, b); }
	template<class T> const T& max(const T& a, const T& b) { return std::max(a, b); }
}
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;
#include <comdef.h>
#include <strsafe.h>

#include "AviUtl/aviutl_exedit_sdk/aviutl.hpp"
#include "AviUtl/aviutl_exedit_sdk/exedit.hpp"
#include "Common/Tracer.h"
#include "Common/WinUtility.h"
#include "Common/Gdi.h"
#include "Common/Profile.h"
#include "Common/FileUpdateChecker.h"
#include "Common/Hook.h"
#include "Common/AviUtlInternal.h"
#include "Detours.4.0.1/detours.h"
#pragma comment(lib, "Detours.4.0.1/detours.lib")
