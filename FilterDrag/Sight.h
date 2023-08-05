#pragma once

namespace fgo::filter_drag
{
	struct Sight : Tools::Window
	{
		struct Config {
			BOOL enable;
			int alpha;
			Color penColor;
			REAL penWidth;
			Color brushColor;
			int base;
			int width;
			_bstr_t fontName;
			REAL fontSize;
			REAL rotate;
			Point beginMove;
		} config;

		LayeredWindowInfo m_info;
		GdiBitmap m_bitmap;
		Graphics m_graphics;

		POINT m_beginPos;
		POINT m_endPos;
		POINT m_currentPos;
		int m_currentCount;

		static const UINT TIMER_ID = 1000;
		static const int MAX_COUNT = 30;

		Sight()
			: m_info(600, 400)
			, m_bitmap(m_info.GetWidth(), m_info.GetHeight())
			, m_graphics(m_bitmap.GetDC())
			, m_beginPos()
			, m_endPos()
			, m_currentPos()
			, m_currentCount()
		{
			m_graphics.SetSmoothingMode(SmoothingModeAntiAlias);
//			m_graphics.SetCompositingMode(CompositingModeSourceOver);
//			m_graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
			m_graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
//			m_graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
			m_graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
			m_graphics.TranslateTransform(-0.5f, -0.5f);
		}

		BOOL create(HINSTANCE instance)
		{
			MY_TRACE(_T("Sight::create()\n"));

			static const LPCTSTR className = _T("FilterDrag.Sight");

			WNDCLASS wc = {};
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_NOCLOSE;
			wc.hCursor = ::LoadCursor(0, IDC_ARROW);
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = instance;
			wc.lpszClassName = className;
			::RegisterClass(&wc);

			return __super::create(
				WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
				className,
				className,
				WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				0, 0, 0, 0,
				0, 0, instance, 0);
		}

		void move(const Layout& layout, FilterHolder filter, BOOL show)
		{
			if (!config.enable)
				return;

			LPCSTR name = filter.getName();
//			MY_TRACE_STR(name);

			render(name, config.alpha);

			RECT rc; layout.getFilterRect(filter, &rc);
			POINT pos;
			pos.x = (rc.left + rc.right) / 2;
			pos.y = (rc.top + rc.bottom) / 2;
			::ClientToScreen(layout.getDialog(), &pos);

			if (show)
			{
				this->show(pos.x, pos.y);
			}
			else
			{
				this->move(pos.x, pos.y);
			}
		}

		void render(LPCSTR name, BYTE alpha)
		{
			MY_TRACE(_T("Sight::render(%hs, %d)\n"), name, alpha);

			Pen pen(config.penColor, config.penWidth);
			SolidBrush brush(config.brushColor);

			Status status = m_graphics.Clear(Color(0, 0, 0, 0));

			int w = m_info.GetWidth();
			int h = m_info.GetHeight();

			int cx = 0;
			int cy = 0;

			int i = config.base;
			int width = config.width;

			REAL ox = w / 6.0f;
			REAL oy = h / 2.0f;

			Matrix matrix;
			m_graphics.GetTransform(&matrix);
			m_graphics.TranslateTransform(ox, oy);

			m_graphics.RotateTransform(config.rotate);

			GraphicsPath cornerPath;
			GraphicsPath linePath;
			GraphicsPath centerPath;

			{
				Point points[6];
				points[0].X = cx - i * 3;
				points[0].Y = cy - i * 3;
				points[1].X = cx - i * 1;
				points[1].Y = cy - i * 3;
				points[2].X = points[1].X;
				points[2].Y = points[1].Y + width;
				points[3].X = points[0].X + width;
				points[3].Y = points[0].Y + width;
				points[5].X = cx - i * 3;
				points[5].Y = cy - i * 1;
				points[4].X = points[5].X + width;
				points[4].Y = points[5].Y;
				cornerPath.AddPolygon(points, 6);

				linePath.AddRectangle(Rect(cx - i * 5, cy - width / 2, i * 3, width));

				centerPath.AddEllipse(Rect(cx - i * 0 - width / 2, cy - i * 0 - width / 2, width, width));
			}

			m_graphics.DrawPath(&pen, &centerPath);
			m_graphics.FillPath(&brush, &centerPath);

			for (int i = 0; i < 4; i++)
			{
//				if (i != 0)
				{
					m_graphics.DrawPath(&pen, &cornerPath);
					m_graphics.FillPath(&brush, &cornerPath);
				}

//				if (i != 1)
				{
					m_graphics.DrawPath(&pen, &linePath);
					m_graphics.FillPath(&brush, &linePath);
				}

				m_graphics.RotateTransform(90.0f);
			}

//			if (0)
			{
				FontFamily fontFamily(config.fontName);
				StringFormat stringFormat;
				stringFormat.SetAlignment(StringAlignmentNear);
				stringFormat.SetLineAlignment(StringAlignmentFar);
				GraphicsPath namePath;
				namePath.AddString((_bstr_t)name, -1, &fontFamily,
					FontStyleBold, config.fontSize,
					Point(cx + i * 3 + width, cy - width), &stringFormat);
				m_graphics.DrawPath(&pen, &namePath);
				m_graphics.FillPath(&brush, &namePath);
			}

			m_graphics.SetTransform(&matrix);

			m_info.SetWindowPosition(m_currentPos.x, m_currentPos.y);
			m_info.Update(*this, m_bitmap.GetDC(), alpha);
		}

		void show(int x, int y)
		{
			MY_TRACE(_T("Sight::show(%d, %d)\n"), x, y);

			m_beginPos.x = x - m_info.GetWidth() / 2 + config.beginMove.X;
			m_beginPos.y = y - m_info.GetHeight() / 2 + config.beginMove.Y;
			m_endPos.x = m_beginPos.x;
			m_endPos.y = m_beginPos.y;
			m_currentPos.x = m_beginPos.x;
			m_currentPos.y = m_beginPos.y;
			m_currentCount = 0;

			::SetWindowPos(*this, HWND_TOPMOST,
				m_beginPos.x, m_beginPos.y,
				m_info.GetWidth(), m_info.GetHeight(),
				SWP_NOACTIVATE | SWP_SHOWWINDOW);

			move(x, y);
		}

		void hide()
		{
			MY_TRACE(_T("Sight::hide()\n"));

			::ShowWindow(*this, SW_HIDE);
//			::KillTimer(*this, TIMER_ID);
		}

		void move(int x, int y)
		{
			m_beginPos.x = m_currentPos.x;
			m_beginPos.y = m_currentPos.y;
			m_endPos.x = x - m_info.GetWidth() / 2;
			m_endPos.y = y - m_info.GetHeight() / 2;
			m_currentCount = MAX_COUNT;

			moveInternal();
		}

		void moveInternal()
		{
			if (m_currentCount == MAX_COUNT)
				::SetTimer(*this, TIMER_ID, 10, 0);

			double a = (double)m_currentCount / MAX_COUNT;
			a = a * a;
			double b = 1.0 - a;
			double tolerance = 0.0001;
			m_currentPos.x = (int)(m_beginPos.x * a + m_endPos.x * b + tolerance);
			m_currentPos.y = (int)(m_beginPos.y * a + m_endPos.y * b + tolerance);

			::SetWindowPos(*this, HWND_TOPMOST,
				m_currentPos.x, m_currentPos.y, 0, 0,
				SWP_NOSIZE | SWP_NOACTIVATE);

			if (m_currentCount <= 0)
				::KillTimer(*this, TIMER_ID);

			m_currentCount--;
		}

		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_TIMER:
				{
					if (wParam == TIMER_ID)
						moveInternal();

					break;
				}
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}
	};
}
