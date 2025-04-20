#define ISOLATION_AWARE_ENABLED 1
#pragma comment(linker, "\"/manifestdependency:type='win32' \
	name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
	processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "common/core/include.hpp"

#include <wil/com.h>
#include <shlobj.h>
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")
#include <wininet.h>
#pragma comment(lib, "wininet.lib")
#import <shell32.dll> rename("ShellExecute", "ShellExecuteW")
