#include "common/core/include.hpp"
#include "common/share/audio_visualizer.hpp"

#include <wrl.h>
#include <wil/com.h>
#include <webview2.h>
using namespace Microsoft::WRL;

#pragma comment(linker, "\"/manifestdependency:type='win32' \
	name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
	processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
