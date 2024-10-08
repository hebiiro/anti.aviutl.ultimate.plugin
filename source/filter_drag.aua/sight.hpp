#pragma once

namespace apn::filter_drag
{
	inline struct Sight : my::Window
	{
		struct Config {
			BOOL available = FALSE;
			SIZE canvas_size = { 600, 400 };
			int alpha = 192;
			Color pen_color = Color(192, 0, 0, 0);
			REAL pen_width = 2.0f;
			Color brush_color = Color(255, 255, 255, 255);
			int base = 16;
			int width = 8;
			std::wstring font_name = L"Segoe UI";
			REAL font_size = 32.0f;
			REAL rotate = 7.77f;
			Point start_offset = Point(0, 100);
		} config;

		struct Context {
			LayeredWindowInfo info;
			GdiBitmap bitmap;
			Graphics graphics;
			Context(const SIZE& canvas_size)
				: info(canvas_size.cx, canvas_size.cy)
				, bitmap(info.GetWidth(), info.GetHeight())
				, graphics(bitmap.GetDC())
			{
				graphics.SetSmoothingMode(SmoothingModeAntiAlias);
//				graphics.SetCompositingMode(CompositingModeSourceOver);
//				graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
				graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
//				graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
				graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
				graphics.TranslateTransform(-0.5f, -0.5f);
			}
		}; std::unique_ptr<Context> context;

		POINT start_pos = {};
		POINT end_pos = {};
		POINT current_pos = {};
		int current_count = 0;

		struct TimerID {
			inline static constexpr UINT c_move = 1000;
		};

		inline static constexpr int c_max_count = 30;

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			constexpr LPCTSTR class_name = _T("apn::filter_drag::sight");

			WNDCLASS wc = {};
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_NOCLOSE;
			wc.hCursor = ::LoadCursor(0, IDC_ARROW);
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = hive.instance;
			wc.lpszClassName = class_name;
			::RegisterClass(&wc);

			return __super::create(
				WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
				class_name,
				class_name,
				WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				0, 0, 0, 0,
				0, 0, hive.instance, 0);
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			context = 0;

			return destroy();
		}

		//
		// サイトが使用可能な場合はTRUEを返します。
		//
		BOOL is_available() const
		{
			return config.available && ::IsWindow(*this);
		}

		//
		// コンテキストを更新します。
		//
		void update_context()
		{
			context = std::make_unique<Context>(config.canvas_size);
		}

		//
		// 指定されたフィルタの位置にサイトを移動します。
		//
		void move(const Layout& layout, const FilterHolder& filter, BOOL show)
		{
			// コンテキストがまだ作成されていない場合は何もしません。
			if (!context) return;

			LPCSTR name = filter.get_name();

			render(name, config.alpha);

			RECT rc = layout.get_filter_rect(filter);
			POINT pos;
			pos.x = (rc.left + rc.right) / 2;
			pos.y = (rc.top + rc.bottom) / 2;
			::ClientToScreen(layout.get_setting_dialog(), &pos);

			if (show)
			{
				this->show(pos.x, pos.y);
			}
			else
			{
				this->move(pos.x, pos.y);
			}
		}

		//
		// サイトを描画します。
		//
		void render(LPCSTR name, BYTE alpha)
		{
			MY_TRACE_FUNC("{}, {}", name, alpha);

			Pen pen(config.pen_color, config.pen_width);
			SolidBrush brush(config.brush_color);

			Status status = context->graphics.Clear(Color(0, 0, 0, 0));

			int w = context->info.GetWidth();
			int h = context->info.GetHeight();

			int cx = 0;
			int cy = 0;

			int i = config.base;
			int width = config.width;

			REAL ox = w / 6.0f;
			REAL oy = h / 2.0f;

			Matrix matrix;
			context->graphics.GetTransform(&matrix);
			context->graphics.TranslateTransform(ox, oy);

			context->graphics.RotateTransform(config.rotate);

			GraphicsPath corner_path;
			GraphicsPath line_path;
			GraphicsPath center_path;

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
				corner_path.AddPolygon(points, 6);

				line_path.AddRectangle(Rect(cx - i * 5, cy - width / 2, i * 3, width));

				center_path.AddEllipse(Rect(cx - i * 0 - width / 2, cy - i * 0 - width / 2, width, width));
			}

			context->graphics.DrawPath(&pen, &center_path);
			context->graphics.FillPath(&brush, &center_path);

			for (int i = 0; i < 4; i++)
			{
//				if (i != 0)
				{
					context->graphics.DrawPath(&pen, &corner_path);
					context->graphics.FillPath(&brush, &corner_path);
				}

//				if (i != 1)
				{
					context->graphics.DrawPath(&pen, &line_path);
					context->graphics.FillPath(&brush, &line_path);
				}

				context->graphics.RotateTransform(90.0f);
			}

//			if (0)
			{
				FontFamily font_family(config.font_name.c_str());

				StringFormat string_format;
				string_format.SetAlignment(StringAlignmentNear);
				string_format.SetLineAlignment(StringAlignmentFar);

				GraphicsPath name_path;
				name_path.AddString(my::ws(name).c_str(), -1, &font_family,
					FontStyleBold, config.font_size,
					Point(cx + i * 3 + width, cy - width), &string_format);
				context->graphics.DrawPath(&pen, &name_path);
				context->graphics.FillPath(&brush, &name_path);
			}

			context->graphics.SetTransform(&matrix);

			context->info.SetWindowPosition(current_pos.x, current_pos.y);
			context->info.Update(*this, context->bitmap.GetDC(), alpha);
		}

		//
		// 指定された位置にサイトを表示します。
		//
		void show(int x, int y)
		{
			MY_TRACE_FUNC("{}, {}", x, y);

			start_pos.x = x - context->info.GetWidth() / 2 + config.start_offset.X;
			start_pos.y = y - context->info.GetHeight() / 2 + config.start_offset.Y;
			end_pos.x = start_pos.x;
			end_pos.y = start_pos.y;
			current_pos.x = start_pos.x;
			current_pos.y = start_pos.y;
			current_count = 0;

			::SetWindowPos(*this, HWND_TOPMOST,
				start_pos.x, start_pos.y,
				context->info.GetWidth(), context->info.GetHeight(),
				SWP_NOACTIVATE | SWP_SHOWWINDOW);

			move(x, y);
		}

		//
		// サイトを非表示にします。
		//
		void hide()
		{
			MY_TRACE_FUNC("");

			::ShowWindow(*this, SW_HIDE);
//			::KillTimer(*this, TimerID::c_move);
		}

		//
		// 指定された位置にサイトを移動します。
		//
		void move(int x, int y)
		{
			start_pos.x = current_pos.x;
			start_pos.y = current_pos.y;
			end_pos.x = x - context->info.GetWidth() / 2;
			end_pos.y = y - context->info.GetHeight() / 2;
			current_count = c_max_count;

			move_internal();
		}

		//
		// 移動処理です。
		// 内部的に使用されます。
		//
		void move_internal()
		{
			if (current_count == c_max_count)
				::SetTimer(*this, TimerID::c_move, 10, 0);

			double a = (double)current_count / c_max_count;
			a = a * a;
			double b = 1.0 - a;
			double tolerance = 0.0001;
			current_pos.x = (int)(start_pos.x * a + end_pos.x * b + tolerance);
			current_pos.y = (int)(start_pos.y * a + end_pos.y * b + tolerance);

			::SetWindowPos(*this, HWND_TOPMOST,
				current_pos.x, current_pos.y, 0, 0,
				SWP_NOSIZE | SWP_NOACTIVATE);

			if (current_count <= 0)
				::KillTimer(*this, TimerID::c_move);

			current_count--;
		}

		//
		// サイトのウィンドウプロシージャです。
		//
		LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_TIMER:
				{
					if (wParam == TimerID::c_move)
						move_internal();

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} sight;
}
