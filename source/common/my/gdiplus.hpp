#pragma once

namespace Gdiplus
{
	typedef std::shared_ptr<SolidBrush> SolidBrushPtr;
	typedef std::shared_ptr<Pen> PenPtr;
	typedef std::shared_ptr<Font> FontPtr;
	typedef std::shared_ptr<FontFamily> FontFamilyPtr;

	class MyPoint : public Point
	{
	public:

		MyPoint(const POINT& point) : Point(point.x, point.y) {}

		void SetPoint(int x, int y)
		{
			this->X = x;
			this->Y = y;
		}
	};

	class MyPointF : public PointF
	{
	public:

		MyPointF()
		{
		}

		MyPointF(const POINT& point)
		{
			this->X = (REAL)point.x;
			this->Y = (REAL)point.y;
		}

		void SetPoint(REAL x, REAL y)
		{
			this->X = x;
			this->Y = y;
		}
	};

	class MyRect : public Rect
	{
	public:

		MyRect()
		{
		}

		MyRect(const RECT& rc)
		{
			SetRect(rc);
		}

		MyRect(const RectF& rc)
		{
			SetRect(rc);
		}

		void SetRect(const RECT& rc)
		{
			this->X = rc.left;
			this->Y = rc.top;
			this->Width = rc.right - rc.left;
			this->Height = rc.bottom - rc.top;
		}

		void SetRect(const RectF& rc)
		{
			this->X = (int)rc.X;
			this->Y = (int)rc.Y;
			this->Width = (int)rc.Width;
			this->Height = (int)rc.Height;
		}
	};

	class MyRectF : public RectF
	{
	public:

		MyRectF()
		{
		}

		MyRectF(const RECT& rc)
		{
			SetRect(rc);
		}

		MyRectF(const Rect& rc)
		{
			SetRect(rc);
		}

		void SetRect(const RECT& rc)
		{
			this->X = (REAL)rc.left;
			this->Y = (REAL)rc.top;
			this->Width = (REAL)(rc.right - rc.left);
			this->Height = (REAL)(rc.bottom - rc.top);
		}

		void SetRect(const Rect& rc)
		{
			this->X = (REAL)rc.X;
			this->Y = (REAL)rc.Y;
			this->Width = (REAL)rc.Width;
			this->Height = (REAL)rc.Height;
		}
	};

	class MyFillRectF : public RectF
	{
	public:

		MyFillRectF()
		{
		}

		MyFillRectF(const RECT& rc)
		{
			SetRect(rc);
		}

		MyFillRectF(const Rect& rc)
		{
			SetRect(rc);
		}

		void SetRect(const RECT& rc)
		{
			this->X = (REAL)rc.left  - 0.5f;
			this->Y = (REAL)rc.top  - 0.5f;
			this->Width = (REAL)(rc.right - rc.left);
			this->Height = (REAL)(rc.bottom - rc.top);
		}

		void SetRect(const Rect& rc)
		{
			this->X = (REAL)rc.X  - 0.5f;
			this->Y = (REAL)rc.Y  - 0.5f;
			this->Width = (REAL)rc.Width;
			this->Height = (REAL)rc.Height;
		}
	};

	class MyFontRectF : public RectF
	{
	public:

		MyFontRectF()
		{
		}

		MyFontRectF(const RECT& rc, int font_height)
		{
			SetRect(rc, font_height);
		}

		MyFontRectF(const Rect& rc, int font_height)
		{
			SetRect(rc, font_height);
		}

		MyFontRectF(const RectF& rc, int font_height)
		{
			SetRect(rc, font_height);
		}

		void SetRect(const RECT& rc, int font_height)
		{
			REAL fix = (REAL)font_height / 6.0f;

			this->X = (REAL)rc.left  - fix;
			this->Y = (REAL)rc.top  - fix;
			this->Width = (REAL)(rc.right - rc.left) + fix * 2;
			this->Height = (REAL)(rc.bottom - rc.top) + fix * 2;
		}

		void SetRect(const Rect& rc, int font_height)
		{
			REAL fix = (REAL)font_height / 6.0f;

			this->X = (REAL)rc.X  - fix;
			this->Y = (REAL)rc.Y  - fix;
			this->Width = (REAL)rc.Width + fix * 2;
			this->Height = (REAL)rc.Height + fix * 2;
		}

		void SetRect(const RectF& rc, int font_height)
		{
			REAL fix = (REAL)font_height / 6.0f;

			this->X = (REAL)rc.X  - fix;
			this->Y = (REAL)rc.Y  - fix;
			this->Width = (REAL)rc.Width + fix * 2;
			this->Height = (REAL)rc.Height + fix * 2;
		}
	};

	class MyColor : public Color
	{
	public:

		MyColor(COLORREF color)
			: Color(
				GetRValue(color),
				GetGValue(color),
				GetBValue(color))
		{
		}

		MyColor(BYTE alpha, COLORREF color)
			: Color(
				alpha,
				GetRValue(color),
				GetGValue(color),
				GetBValue(color))
		{
		}

		MyColor(BYTE alpha, const Color& color)
			: Color(alpha * color.GetA() / 255, color.GetR(), color.GetG(), color.GetB())
		{
		}
	};

	inline Gdiplus::SolidBrushPtr create_brush(const Gdiplus::Color& color)
	{
		return Gdiplus::SolidBrushPtr(new Gdiplus::SolidBrush(color));
	}

	inline Gdiplus::PenPtr create_pen(const Gdiplus::Color& color, REAL width)
	{
		if (width)
			return Gdiplus::PenPtr(new Gdiplus::Pen(color, width));
		else
			return {};
	}

	inline void set_line_join(Gdiplus::PenPtr& pen)
	{
		if (pen)
			pen->SetLineJoin(Gdiplus::LineJoinRound);
	}

	inline REAL get_width(Pen* pen)
	{
		if (pen)
			return pen->GetWidth();

		return 0.0f;
	}

	inline SizeF calc_text_size(Graphics& g, LPCWSTR text, Gdiplus::Font* font)
	{
		RectF rect;
		g.MeasureString(text, -1, font, PointF(0, 0), &rect);
		return SizeF(rect.Width, rect.Height);
	}

	inline void draw_round_rect(Graphics& g, Pen* pen, Brush* brush, const RectF& _rect, REAL diameter)
	{
		REAL pen_width = get_width(pen);

		RectF rect(_rect);
		rect.Inflate(-pen_width / 2.0f, -pen_width / 2.0f);

		REAL left = rect.X;
		REAL right = rect.X + rect.Width;
		REAL top = rect.Y;
		REAL bottom = rect.Y + rect.Height;

		GraphicsPath path;

		path.StartFigure();
		path.AddArc(left, top, diameter, diameter, 180, 90);
		path.AddArc(right - diameter, top, diameter, diameter, 270, 90);
		path.AddArc(right - diameter, bottom - diameter, diameter, diameter, 0, 90);
		path.AddArc(left, bottom - diameter, diameter, diameter, 90, 90);
		path.CloseFigure();

		if (brush) g.FillPath(brush, &path);
		if (pen) g.DrawPath(pen, &path);
	}

	inline void draw_round_rect(Graphics& g, Pen* pen, Brush* brush, const RectF& _rect)
	{
		REAL pen_width = get_width(pen);

		RectF rect(_rect);
		rect.Inflate(-pen_width / 2.0f, -pen_width / 2.0f);

		REAL left = rect.X;
		REAL right = rect.X + rect.Width;
		REAL top = rect.Y;
		REAL bottom = rect.Y + rect.Height;
		REAL diameter = rect.Height;

		GraphicsPath path;

		path.StartFigure();
		path.AddArc(left, top, diameter, diameter, 90.0f, 180.0f);
		path.AddArc(right - diameter, top, diameter, diameter, 270.0f, 180.0f);
		path.CloseFigure();

		if (brush) g.FillPath(brush, &path);
		if (pen) g.DrawPath(pen, &path);
	}

	inline void draw_round_rect(Graphics& g, Pen* pen, const Rect& rect, int round)
	{
		int left = rect.X;
		int right = rect.X + rect.Width - 1;
		int top = rect.Y;
		int bottom = rect.Y + rect.Height - 1;

		int roundSize = round * 2;

		g.DrawArc(pen, left, top, roundSize, roundSize, 180, 90);
		g.DrawArc(pen, right - roundSize, top, roundSize, roundSize, 270, 90);
		g.DrawArc(pen, left, bottom - roundSize, roundSize, roundSize, 90, 90);
		g.DrawArc(pen, right - roundSize, bottom - roundSize, roundSize, roundSize, 0, 90);

		g.DrawLine(pen, left + round, top, right - round, top);
		g.DrawLine(pen, left + round, bottom, right - round, bottom);

		g.DrawLine(pen, left, top + round, left, bottom - round);
		g.DrawLine(pen, right, top + round, right, bottom - round);
	}

	inline void draw_round_rect(Graphics& g, Pen* pen, Brush* brush, const Rect& _rect, int radius)
	{
		MyRectF rect(_rect);
		rect.Width -= 1.5f;
		rect.Height -= 1.5f;

		REAL left = rect.X;
		REAL right = rect.X + rect.Width;
		REAL top = rect.Y;
		REAL bottom = rect.Y + rect.Height;
		REAL diameter = (REAL)radius * 2.0f;

		GraphicsPath path;

		path.StartFigure();
		path.AddArc(left, top, diameter, diameter, 180, 90);
		path.AddArc(right - diameter, top, diameter, diameter, 270, 90);
		path.AddArc(right - diameter, bottom - diameter, diameter, diameter, 0, 90);
		path.AddArc(left, bottom - diameter, diameter, diameter, 90, 90);
		path.CloseFigure();

		if (brush) g.FillPath(brush, &path);
		if (pen) g.DrawPath(pen, &path);
	}

	inline void draw_round_rect(Graphics& g, Pen* pen, Brush* brush, const Rect& _rect)
	{
		MyRectF rect(_rect);
		rect.Width -= 1.5f;
		rect.Height -= 1.5f;

		REAL left = rect.X;
		REAL right = rect.X + rect.Width;
		REAL top = rect.Y;
		REAL bottom = rect.Y + rect.Height;
		REAL diameter = rect.Height;

		GraphicsPath path;

		path.StartFigure();
		path.AddArc(left, top, diameter, diameter, 90.0f, 180.0f);
		path.AddArc(right - diameter, top, diameter, diameter, 270.0f, 180.0f);
		path.CloseFigure();

		if (brush) g.FillPath(brush, &path);
		if (pen) g.DrawPath(pen, &path);
	}
}
#ifdef _MFC_VER
class CMyRect : public CRect
{
public:

	CMyRect()
	{
	}

	CMyRect(const Gdiplus::Rect& rc)
	{
		SetRect(rc);
	}

	CMyRect(const Gdiplus::RectF& rc)
	{
		SetRect(rc);
	}

	void SetRect(const Gdiplus::Rect& rc)
	{
		this->left = (int)rc.X;
		this->top = (int)rc.Y;
		this->right = (int)(rc.X + rc.Width);
		this->bottom = (int)(rc.Y + rc.Height);
	}

	void SetRect(const Gdiplus::RectF& rc)
	{
		this->left = (int)rc.X;
		this->top = (int)rc.Y;
		this->right = (int)(rc.X + rc.Width);
		this->bottom = (int)(rc.Y + rc.Height);
	}
};
#endif
