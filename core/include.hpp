#pragma once
#define MY_TRACE_ENABLED

#include <tchar.h>
#include <crtdbg.h>
#include <locale.h>
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <memory>
#include <string>
using namespace std::string_literals;
#include <sstream>
#include <iomanip>
#include <vector>
#include <list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <deque>
#include <regex>
#include <format>
#include <filesystem>
namespace fs = std::filesystem;
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
using namespace boost::property_tree;
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/locale.hpp>
#include <boost/locale/encoding.hpp>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#pragma comment(lib, "msimg32.lib")
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")
#include <commdlg.h>
#pragma comment(lib, "comdlg32.lib")
#include <imagehlp.h>
#pragma comment(lib, "imagehlp.lib")
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include <vsstyle.h>
#include <vssym32.h>
#include <uxtheme.h>
#pragma comment(lib, "uxtheme.lib")
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;
#include <shellapi.h>
#include <psapi.h>
//#include <comdef.h>
//#include <strsafe.h>

//#import <msxml3.dll>

#include "detours/detours.h"
#pragma comment(lib, "detours/detours.lib")

#undef min
#undef max

#include "aviutl.hpp"
#include "exedit.hpp"
#include "my/win_utils.hpp"
#include "my/trace.hpp"
#include "my/trace_to_file.hpp"
#include "my/tools.hpp"
#include "my/detours.hpp"
#include "my/gdi.hpp"
#if 0
#include "my/Gdiplus.h"
#endif
#include "my/window.hpp"
#include "my/window2.hpp"
#include "my/window_interseptor.hpp"
#include "my/dialog.hpp"
#include "my/dialog2.hpp"
#if 0
#include "my/Label.h"
#include "my/ShortcutKey.h"
#include "my/Profile.h"
#include "my/FileUpdateChecker.h"
#endif
#include "my/json.hpp"
#include "my/aviutl/aviutl_internal.hpp"
#include "my/aviutl/exedit_internal.hpp"
#include "my/aviutl/window_holder_extension.hpp"
#include "my/aviutl/plugin_window_extension.hpp"
#include "my/aviutl/plugin_window.hpp"
#include "core/module.hpp"
#include "core/magi.hpp"
#include "core/addin.hpp"
#include "core/addin_manager.hpp"
#include "core/std_config_io.hpp"
/*
#include "Tools/edit_predicates.h"
#include "Tools/Tools.h"
#include "Tools/Hook.h"
#include "Tools/ObjectHolder.h"
#include "Tools/FilterHolder.h"
#include "Tools/LayeredWindowInfo.h"
#include "Tools/GdiBitmap.h"
*/