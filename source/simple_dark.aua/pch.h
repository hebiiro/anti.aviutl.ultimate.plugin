//#define MY_TRACE_ENABLED
#include "common/core/include.hpp"
#include "common/core/utils.hpp"
#include "common/my/lockable.hpp"
#include "common/share/dark_api.hpp"

#include <richedit.h>
#include <vfw.h>
#pragma comment(lib, "vfw32.lib")
#include <d2d1_1.h>
#include <d2d1_1helper.h>
#pragma comment(lib, "d2d1.lib")
#include <d2d1effects.h>
#pragma comment(lib, "dxguid.lib")
#include <dwrite.h>
#pragma comment(lib, "dwrite.lib")
#include <wincodec.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;
