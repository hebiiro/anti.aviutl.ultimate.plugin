#include "glad/gl.h"
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#include "NanoVG/nanovg.h"
#pragma comment(lib, "NanoVG/NanoVGd32.lib")
#include "Fate/Grand Order"
#include "Share/AudioGraph.h"
#include "Common/DesignBase.h"

namespace fgo::audio_graph_gui::NanoVG
{
	NVGcontext* init();
	void exit(NVGcontext* vg);
}

#pragma comment(linker, "\"/manifestdependency:type='win32' \
	name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
	processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
