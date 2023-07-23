#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")
#include <comdef.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include <strsafe.h>

#include <tchar.h>
#include <crtdbg.h>
#include <locale.h>
#include <string>
#include <unordered_map>

#include "AviUtl/aviutl_exedit_sdk/aviutl.hpp"
#include "AviUtl/aviutl_exedit_sdk/exedit.hpp"
#include "Common/Tracer.h"
#include "Common/WinUtility.h"
#include "Common/Gdi.h"
#include "Common/Profile.h"
#include "Common/Hook.h"
#include "Common/AviUtlInternal.h"
#include "Detours.4.0.1/detours.h"
#pragma comment(lib, "Detours.4.0.1/detours.lib")
