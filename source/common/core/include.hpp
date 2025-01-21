#pragma once
//#define MY_TRACE_ENABLED
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
#include <fstream>
#include <iostream>
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

#include "nlohmann/json.hpp"
#include "detours/detours.h"
#pragma comment(lib, "detours/detours.lib")

#undef min
#undef max

#include "aviutl.hpp"
#include "exedit.hpp"
#include "common/my/utils.hpp"
#include "common/my/str_utils.hpp"
#include "common/my/win_utils.hpp"
#include "common/my/win_wrap.hpp"
#include "common/my/trace.hpp"
#include "common/my/trace_to_file.hpp"
#include "common/my/tools.hpp"
#include "common/my/detours.hpp"
#include "common/my/gdi.hpp"
#include "common/my/gdiplus.hpp"
#include "common/my/mfc.hpp"
#include "common/my/window.hpp"
#include "common/my/window2.hpp"
#include "common/my/window_interceptor.hpp"
#include "common/my/dialog.hpp"
#include "common/my/dialog2.hpp"
#include "common/my/layout.hpp"
#include "common/my/label.hpp"
#include "common/my/shortcut_key.hpp"
#include "common/my/json.hpp"
using namespace my::json;
#include "common/my/file_update_checker.hpp"
#include "common/my/aviutl/aviutl_internal.hpp"
#include "common/my/aviutl/exedit_internal.hpp"
#include "common/my/aviutl/window_holder_extension.hpp"
#include "common/my/aviutl/plugin_window_extension.hpp"
#include "common/my/aviutl/plugin_window.hpp"
#include "common/my/aviutl/slider_panel.hpp"
#include "common/core/module.hpp"
#include "common/core/magi.hpp"
#include "common/core/addin.hpp"
#include "common/core/addin_manager.hpp"
#include "common/core/std_config_io.hpp"
#include "common/core/std_config_io_use_hive.hpp"
#include "common/core/std_addin_dialog.hpp"
#include "common/core/std_addin_window.hpp"
#include "common/core/object_holder.hpp"
#include "common/core/filter_holder.hpp"
#include "common/core/layered_window_info.hpp"
#include "common/core/gdi_bitmap.hpp"
