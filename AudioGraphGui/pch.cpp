#include "pch.h"

#pragma warning(disable:4551)
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#define NANOVG_GL3_IMPLEMENTATION
#include "NanoVG/nanovg_gl.h"

namespace fgo::audio_graph_gui::NanoVG
{
	NVGcontext* init() {
		return nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
	}

	void exit(NVGcontext* vg) {
		nvgDeleteGL3(vg);
	}
}
