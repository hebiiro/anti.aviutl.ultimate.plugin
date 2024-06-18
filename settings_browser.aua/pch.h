#if 0
#define ISOLATION_AWARE_ENABLED 1
#pragma comment(linker, "\"/manifestdependency:type='win32' \
	name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
	processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include "common/core/include.hpp"
using namespace my::json;

#include <wrl.h>
#include <wil/com.h>
#include <WebView2/WebView2.h>
#pragma comment(lib, "WebView2/x86/WebView2Loader.dll.lib")
using namespace Microsoft::WRL;
