#pragma once

namespace Tools
{
	struct MakeCurrent
	{
		HDC prevDC = 0;
		HGLRC prevGLRC = 0;

		MakeCurrent(HDC dc, HGLRC glrc)
		{
			prevDC = wglGetCurrentDC();
			prevGLRC = wglGetCurrentContext();

			wglMakeCurrent(dc, glrc);
		}

		~MakeCurrent()
		{
			wglMakeCurrent(prevDC, prevGLRC);
		}
	};

	struct XYWHRect
	{
		int x, y, w, h;

		XYWHRect()
		{
			x = y = w = h = 0;
		}

		XYWHRect(int x, int y, int w, int h)
		{
			this->x = x;
			this->y = y;
			this->w = w;
			this->h = h;
		}

		XYWHRect(const RECT& rc)
		{
			operator=(rc);
		}

		XYWHRect& operator=(const RECT& rc)
		{
			this->x = rc.left;
			this->y = rc.top;
			this->w = getWidth(rc);
			this->h = getHeight(rc);

			return *this;
		}
	};

	struct MyColor : public NVGcolor
	{
		MyColor()
		{
		}

		MyColor(DWORD color)
		{
			this->r = GetRValue(color) / 255.0f;
			this->g = GetGValue(color) / 255.0f;
			this->b = GetBValue(color) / 255.0f;
			this->a = GetAValue(color) / 255.0f;
		}

		operator DWORD()
		{
			BYTE r = (BYTE)roundf(this->r * 255.0f);
			BYTE g = (BYTE)roundf(this->g * 255.0f);
			BYTE b = (BYTE)roundf(this->b * 255.0f);
			BYTE a = (BYTE)roundf(this->a * 255.0f);

			return getDWORD(r, g, b, a);
		}

		MyColor& operator=(const NVGcolor& color)
		{
			((NVGcolor&)*this) = color;

			return *this;
		}

		static BYTE GetAValue(DWORD color)
		{
			return LOBYTE(color >> 24);
		}

		static COLORREF getCOLORREF(DWORD rgba)
		{
			return (rgba & 0x00FFFFFF);
		}

		static DWORD getDWORD(BYTE r, BYTE g, BYTE b, BYTE a)
		{
			return r | ((WORD)g << 8) | ((DWORD)b << 16) | ((DWORD)a << 24);
		}

		static DWORD getDWORD(COLORREF color, DWORD rgba)
		{
			return (color & 0x00FFFFFF) | (rgba & 0xFF000000);
		}
	};

	struct StringU8
	{
		char m_buffer[MAX_PATH] = {};

		StringU8()
		{
		}

		StringU8(LPCWSTR x)
		{
			operator=(x);
		}

		StringU8& operator=(LPCWSTR x)
		{
			::WideCharToMultiByte(CP_UTF8, 0, x, -1, m_buffer, MAX_PATH, 0, 0);
			return *this;
		}

		operator LPCSTR() const
		{
			return m_buffer;
		}
	};

	struct DesignBase
	{
		struct Label { int value; LPCWSTR label; };

		struct AlignX {
			inline static const int left = 0;
			inline static const int right = 1;
			inline static const int center = 2;
			inline static const Label labels[] = {
				{ left, L"left" },
				{ right, L"right" },
				{ center, L"center" },
			};
		};

		struct AlignY {
			inline static const int top = 0;
			inline static const int bottom = 1;
			inline static const int center = 2;
			inline static const Label labels[] = {
				{ top, L"top" },
				{ bottom, L"bottom" },
				{ center, L"center" },
			};
		};

		struct ScaleMode {
			inline static const int normal = 0;
			inline static const int fit = 1;
			inline static const int crop = 2;
			inline static const Label labels[] = {
				{ normal, L"normal" },
				{ fit, L"fit" },
				{ crop, L"crop" },
			};
		};

		struct LineStyle {
			inline static const Label labels[] = {
				{ 0, L"normal" },
				{ NVG_LINE_SOLID, L"solid" },
				{ NVG_LINE_DASHED, L"dashed" },
				{ NVG_LINE_DOTTED, L"dotted" },
				{ NVG_LINE_GLOW, L"glow" },
			};
		};

		struct Fill {
			MyColor color;
		};

		struct GradientFill {
			MyColor start;
			MyColor end;
		};

		struct Stroke {
			int style;
			int width;
			MyColor color;
		};

		struct Text {
			int height;
			float dilate;
			float blur;
			MyColor color;
			struct Shadow {
				float dilate;
				float blur;
				MyColor color;
				POINT offset;
			} shadow;
		};

		struct Image {
			_bstr_t fileName;
			POINT offset;
			POINT align;
			int scaleMode;
			float scale;
			float angle;
			float alpha;
		};

		inline static const float offset = 0.5f;
	};

	template<class Rect>
	inline NVGpaint nvgVertGradient(NVGcontext* vg, const Rect& rc, const DesignBase::GradientFill& gradientFill)
	{
		return nvgLinearGradient(vg,
			(float)rc.x, (float)rc.y, (float)rc.x, (float)(rc.y + rc.h),
			gradientFill.start, gradientFill.end);
	};

	inline void nvgStrokeDesign(NVGcontext* vg, const DesignBase::Stroke& stroke)
	{
		nvgLineStyle(vg, stroke.style);
		nvgStrokeWidth(vg, (float)stroke.width);
		nvgStrokeColor(vg, stroke.color);
	}

	template<class Rect>
	inline void nvgXYWHRect(NVGcontext* vg, const Rect& rc)
	{
		nvgRect(vg, (float)rc.x, (float)rc.y, (float)rc.w, (float)rc.h);
	}

	template<class Rect>
	inline void drawImage(NVGcontext* vg, int image, const Rect& rc, const DesignBase::Image& design)
	{
		if (!image) return;

		int imgw = 0, imgh = 0;
		nvgImageSize(vg, image, &imgw, &imgh);

		Rect dst, src;

		dst.x = src.x = 0;
		dst.y = src.y = 0;
		dst.w = src.w = imgw;
		dst.h = src.h = imgh;

		switch (design.scaleMode)
		{
		case DesignBase::ScaleMode::fit:
			{
				dst.w = rc.w;
				dst.h = rc.w * imgh / imgw;

				if (dst.h > rc.h)
				{
					dst.w = rc.h * imgw / imgh;
					dst.h = rc.h;
				}

				break;
			}
		case DesignBase::ScaleMode::crop:
			{
				dst.w = rc.w;
				dst.h = rc.w * imgh / imgw;

				if (dst.h < rc.h)
				{
					dst.w = rc.h * imgw / imgh;
					dst.h = rc.h;
				}

				break;
			}
		}

		int align = 0;

		switch (design.align.x)
		{
		case DesignBase::AlignX::left: align |= NVG_ALIGN_LEFT; break;
		case DesignBase::AlignX::right: align |= NVG_ALIGN_RIGHT; break;
		case DesignBase::AlignX::center: align |= NVG_ALIGN_CENTER; break;
		}

		switch (design.align.y)
		{
		case DesignBase::AlignY::top: align |= NVG_ALIGN_TOP; break;
		case DesignBase::AlignY::bottom: align |= NVG_ALIGN_BOTTOM; break;
		case DesignBase::AlignY::center: align |= NVG_ALIGN_MIDDLE; break;
		}

		if (align & NVG_ALIGN_RIGHT)
		{
			dst.x = (rc.x + rc.w) - dst.w;
			dst.x -= design.offset.x;
		}
		else
		{
			if (align & NVG_ALIGN_CENTER)
				dst.x = rc.x + (rc.w - dst.w) / 2;
	
			dst.x += design.offset.x;
		}

		if (align & NVG_ALIGN_BOTTOM)
		{
			dst.y = (rc.y + rc.h) - dst.h;
			dst.y -= design.offset.y;
		}
		else
		{
			if (align & NVG_ALIGN_MIDDLE)
				dst.y = rc.y + (rc.h - dst.h) / 2;
			
			dst.y += design.offset.y;
		}

		NVGpaint imgPaint = nvgImagePattern(vg,
			(float)dst.x, (float)dst.y, (float)dst.w, (float)dst.h,
			design.angle / 360.0f * (2.0f * NVG_PI), image, design.alpha);
		nvgBeginPath(vg);
		nvgXYWHRect(vg, rc);
		nvgFillPaint(vg, imgPaint);
		nvgFill(vg);
	}

	inline void drawLine(NVGcontext* vg, float mx, float my, float lx, float ly, const DesignBase::Stroke& stroke)
	{
		const float offset = DesignBase::offset; // 線はオフセットを与えないとぼやける。

		nvgBeginPath(vg);
		nvgMoveTo(vg, mx + offset, my + offset);
		nvgLineTo(vg, lx + offset, ly + offset);
		nvgStrokeDesign(vg, stroke);
		nvgStroke(vg);
	}

	inline void drawText(NVGcontext* vg, LPCSTR text, float x, float y, const DesignBase::Text& design, int fontId)
	{
//		const float offset = DesignBase::offset; // テキストはオフセットを与えると逆にぼやける。
		const float offset = 0;

		struct Point { float x, y; };

		Point textPos;
		textPos.x = x + offset;
		textPos.y = y + offset;

		nvgFontSize(vg, (float)design.height);
		nvgFontFaceId(vg, fontId); // nvgFontSize() のあとに呼ばなければならない。

		if (design.shadow.dilate > 0.0f || design.shadow.blur > 0.0f)
		{
			Point shadowPos;
			shadowPos.x = textPos.x + design.shadow.offset.x;
			shadowPos.y = textPos.y + design.shadow.offset.y;

			nvgFontDilate(vg, design.shadow.dilate);
			nvgFontBlur(vg, design.shadow.blur);
			nvgFillColor(vg, design.shadow.color);
			nvgText(vg, shadowPos.x, shadowPos.y, text, 0);
		}

		nvgFontDilate(vg, design.dilate);
		nvgFontBlur(vg, design.blur);
		nvgFillColor(vg, design.color);
		nvgText(vg, textPos.x, textPos.y, text, 0);
	}
}
