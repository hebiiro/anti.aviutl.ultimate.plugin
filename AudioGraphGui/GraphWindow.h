#pragma once
#include "Hive.h"

namespace fgo::audio_graph_gui
{
	//
	// このクラスは音声グラフを表示します。
	//
	struct GraphWindow : Tools::Window
	{
		inline static struct Listener {
			virtual BOOL onRedraw(GraphWindow* window) = 0;
			virtual BOOL onConfig(GraphWindow* window) = 0;
			virtual BOOL onInvalidate(GraphWindow* window) = 0;
		} *listener = 0;

		int graphWindowType = 0;
		HGLRC glrc = 0;
		NVGcontext* vg = 0;
		int fontDefault = 0;
		int fontDefault2 = 0;
		int image = 0;

		GraphWindow(int graphWindowType)
			: graphWindowType(graphWindowType)
		{
			MY_TRACE_FUNC("");
		}

		~GraphWindow()
		{
			MY_TRACE_FUNC("");
		}

		BOOL create(HWND parent)
		{
			MY_TRACE_FUNC("0x%08X", parent);

			const LPCTSTR className = _T("AudioGraph.GraphWindow");

			WNDCLASSEXW wc = { sizeof(wc) };
			wc.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance      = hive.instance;
			wc.hCursor        = ::LoadCursor(0, IDC_ARROW);
			wc.lpszClassName  = className;
			::RegisterClassExW(&wc);

			return __super::create(
				WS_EX_NOPARENTNOTIFY,
				className,
				className,
				WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				0, 0, 0, 0,
				parent, 0, hive.instance, 0);
		}

		static BOOL setupPixelFormat(HDC dc)
		{
			MY_TRACE_FUNC("0x%08X", dc);

			PIXELFORMATDESCRIPTOR pfd =
			{
				sizeof(pfd),
				1,
				PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
				PFD_TYPE_RGBA,
				24,
				0, 0, 0, 0, 0, 0,
				0,
				0,
				0,
				0, 0, 0, 0,
				32,
				1,
				0,
				PFD_MAIN_PLANE,
				0,
				0, 0, 0
			};

			int pixelFormat = ::ChoosePixelFormat(dc, &pfd);
			MY_TRACE_INT(pixelFormat);

			if (!pixelFormat)
			{
				MY_TRACE(_T("::ChoosePixelFormat()が失敗しました\n"));

				return FALSE;
			}

			if (!::SetPixelFormat(dc, pixelFormat, &pfd))
			{
				MY_TRACE(_T("::SetPixelFormat()が失敗しました\n"));

				return FALSE;
			}

			return TRUE;
		}

		BOOL initOpenGL()
		{
			MY_TRACE_FUNC("");

			// OpenGLを初期化します。

			ClientDC dc(*this);

			setupPixelFormat(dc);

			glrc = wglCreateContext(dc);
			MY_TRACE_HEX(glrc);

			if (!glrc)
			{
				MY_TRACE(_T("wglCreateContext()が失敗しました\n"));

				return FALSE;
			}

			Tools::MakeCurrent makeCurrent(dc, glrc);

			{
				// gladを初期化します。

				int result = gladLoaderLoadGL();
				MY_TRACE_INT(result);
			}

			{
				// NanoVGを初期化します。

				vg = NanoVG::init();
				MY_TRACE_HEX(vg);
			}

			updateDesign();

			return TRUE;
		}

		BOOL termOpenGL()
		{
			MY_TRACE_FUNC("");

			NanoVG::exit(vg), vg = 0;

			wglDeleteContext(glrc), glrc = 0;

			return TRUE;
		}

		//
		// デザインを更新し、ウィンドウを再描画します。
		//
		void updateDesign()
		{
			MY_TRACE_FUNC("");

			ClientDC dc(*this);
			Tools::MakeCurrent makeCurrent(dc, glrc);

			updateDesignInternal();

			::InvalidateRect(*this, 0, FALSE);
		}

		//
		// デザインを更新します。内部的に使用されます。
		//
		void updateDesignInternal()
		{
			MY_TRACE_FUNC("");

			if (wcslen(design.fontDefault) && wcslen(design.fontDefault2))
			{
				// NanoVGを使用してフォントを読み込みます。

				if (wcslen(design.fontDefault) != 0)
				{
					MY_TRACE_STR((LPCSTR)(design.fontDefault));
					fontDefault = nvgCreateFont(vg, "default", (LPCSTR)(design.fontDefault));
					MY_TRACE_INT(fontDefault);

					if (wcslen(design.fontDefault2) != 0)
					{
						MY_TRACE_STR((LPCSTR)(design.fontDefault2));
						fontDefault2 = nvgCreateFont(vg, "default2", (LPCSTR)(design.fontDefault2));
						MY_TRACE_INT(fontDefault2);

						nvgAddFallbackFontId(vg, fontDefault, fontDefault2);
					}
				}
			}

			{
				// NanoVGを使用して画像を読み込みます。

				if (image)
					nvgDeleteImage(vg, image);

				const Design::Image* image = &design.image;

				switch (graphWindowType)
				{
				case Share::AudioGraph::GraphWindowType::Left: image = &design.left.image; break;
				case Share::AudioGraph::GraphWindowType::Right: image = &design.right.image; break;
				}

				MY_TRACE_WSTR((BSTR)image->fileName);
				MY_TRACE_STR((LPCSTR)(image->fileName));

				this->image = nvgCreateImage(vg, (LPCSTR)(image->fileName), 0);
			}
		}

		void doPaint()
		{
			MY_TRACE_FUNC("");

			ClientDC dc(*this);
			RECT rc; ::GetClientRect(*this, &rc);

			doPaint(dc, rc);
		}

		void doPaint(HDC dc, const RECT& rc)
		{
			MY_TRACE_FUNC("");

			Tools::MakeCurrent makeCurrent(dc, glrc);
			int w = getWidth(rc);
			int h = getHeight(rc);

			glViewport(0, 0, w, h);
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			nvgBeginFrame(vg, (float)w, (float)h, 1.0f);
			nvgSave(vg);

			doPaint(rc);

			nvgRestore(vg);
			nvgEndFrame(vg);

			::SwapBuffers(dc);
		}

		void doPaint(const RECT& rc)
		{
			MY_TRACE_FUNC("");

			using namespace Tools;

			int w = getWidth(rc);
			int h = getHeight(rc);

			if (w <= 0 || h <= 0) return;

			{
				// 背景を描画する。

				XYWHRect background(rc.left, rc.top, w, h);

				// 縦のグラデーションで背景を塗りつぶす。
				NVGpaint paint = nvgVertGradient(vg, background, design.background.fill);
				nvgBeginPath(vg);
				nvgXYWHRect(vg, background);
				nvgFillPaint(vg, paint);
				nvgFill(vg);

				// 背景画像を描画する。
				drawImage(vg, image, background, design.image);
			}

			int textPadding = (int)(design.scale.text.dilate + design.scale.text.blur +
				design.scale.text.shadow.dilate + design.scale.text.shadow.blur);
			int textHeight = design.scale.text.height + textPadding * 2;

			int range = hive.maxRange - hive.minRange;
			if (range <= 0) return; // 音量範囲が無効の場合は描画を中止します。

			int freq = range * textHeight / h + 1;
			if (freq <= 0) return; // 目盛り描画頻度が無効の場合は描画を中止します。

			struct Rect { float x, y, w, h; };

			struct Layout {
				BOOL enable;
				float left;
				float center;
				float right;
				int align;
			} layouts[2] = {};

			switch (graphWindowType)
			{
			case Share::AudioGraph::GraphWindowType::Both:
				{
					// 左のレイアウト。
					layouts[0].enable = TRUE;
					layouts[0].left = (float)(rc.left + ::MulDiv(w, 0, 4)),
					layouts[0].center = (float)(rc.left + ::MulDiv(w, 1, 4)),
					layouts[0].right = (float)(rc.left + ::MulDiv(w, 2, 4)) - 0.5f,
					layouts[0].align = NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM;

					// 右のレイアウト。
					layouts[1].enable = TRUE;
					layouts[1].left = (float)(rc.left + ::MulDiv(w, 4, 4)),
					layouts[1].center = (float)(rc.left + ::MulDiv(w, 3, 4)),
					layouts[1].right = (float)(rc.left + ::MulDiv(w, 2, 4)) + 0.5f,
					layouts[1].align = NVG_ALIGN_RIGHT | NVG_ALIGN_BOTTOM;

					break;
				}
			case Share::AudioGraph::GraphWindowType::Left:
				{
					// 左のレイアウト。
					layouts[0].enable = TRUE;
					layouts[0].left = (float)(rc.left + ::MulDiv(w, 0, 4)),
					layouts[0].center = (float)(rc.left + ::MulDiv(w, 2, 4)),
					layouts[0].right = (float)(rc.left + ::MulDiv(w, 4, 4)),
					layouts[0].align = NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM;

					// 右のレイアウト。
					layouts[1].enable = FALSE;

					break;
				}
			case Share::AudioGraph::GraphWindowType::Right:
				{
					// 左のレイアウト。
					layouts[0].enable = FALSE;

					// 右のレイアウト。
					layouts[1].enable = TRUE;
					layouts[1].left = (float)(rc.left + ::MulDiv(w, 4, 4)),
					layouts[1].center = (float)(rc.left + ::MulDiv(w, 2, 4)),
					layouts[1].right = (float)(rc.left + ::MulDiv(w, 0, 4)),
					layouts[1].align = NVG_ALIGN_RIGHT | NVG_ALIGN_BOTTOM;

					break;
				}
			}

			{
				// 音量のレベルとピークを描画する。

				for (int j = 0; j < 2; j++)
				{
					if (!layouts[j].enable) continue;

					if (hive.level[j] > hive.minRange)
					{
						// 音量レベルを描画する。

						auto ratio = (hive.level[j] - hive.minRange) / range;
						int y = rc.bottom - (int)(h * ratio);

						Rect level;
						level.x = (float)(layouts[j].center) + DesignBase::offset;
						level.y = (float)(y) + DesignBase::offset;
						level.w = (float)(layouts[j].right - layouts[j].center);
						level.h = (float)(rc.bottom - y);

						Rect gradient;
						gradient.x = (float)(layouts[j].left);
						gradient.y = (float)(rc.top);
						gradient.w = (float)(layouts[j].left);
						gradient.h = (float)(rc.bottom);

						NVGpaint paint = nvgVertGradient(vg, gradient, design.volume.level.fill);
						nvgBeginPath(vg);
						nvgXYWHRect(vg, level);
						nvgFillPaint(vg, paint);
						nvgFill(vg);
					}

					if (hive.peak[j] > hive.minRange)
					{
						auto ratio = (hive.peak[j] - hive.minRange) / range;
						int y = rc.bottom - (int)(h * ratio);

						{
							float mx = (float)(layouts[j].center);
							float my = (float)(y);
							float lx = (float)(layouts[j].right);
							float ly = (float)(y);

							drawLine(vg, mx, my, lx, ly, design.volume.peak.stroke);
						}

						if (design.volume.zebra.step > 0)
						{
							for (int k = 0; k < (rc.bottom - y) - design.volume.zebra.step / 2; k += design.volume.zebra.step)
							{
								float mx = (float)(layouts[j].center);
								float my = (float)(rc.bottom - k);
								float lx = (float)(layouts[j].right);
								float ly = (float)(rc.bottom - k);

								drawLine(vg, mx, my, lx, ly, design.volume.zebra.stroke);
							}
						}
					}
				}
			}

			{
				// 目盛りを描画する。

				for (int i = hive.minRange; i <= hive.maxRange; i++)
				{
					int offset = i - hive.minRange; // minRange から i までのオフセット。
					if (offset % freq) continue; // 描画頻度が高すぎる場合は何もしない。

					auto ratio = (float)offset / range; // 高さに対する比率。
					int y = (int)(rc.bottom - h * ratio); // 描画位置の Y 座標。(ピクセル単位)

					char text[MAX_PATH] = {};
					::StringCbPrintfA(text, sizeof(text), "%+d", i);

					for (int j = 0; j < 2; j++)
					{
						if (!layouts[j].enable) continue;

						{
							// 目盛りの水平線を描画する。

							float mx = (float)(layouts[j].left);
							float my = (float)(y);
							float lx = (float)(layouts[j].center);
							float ly = (float)(y);

							drawLine(vg, mx, my, lx, ly, design.scale.stroke);
						}

						{
							// 目盛りの文字列を描画する。

							float tx = (float)layouts[j].left;
							float ty = (float)(y - textPadding / 2);

							if (layouts[j].align & NVG_ALIGN_RIGHT)
								tx -= (float)textPadding;
							else
								tx += (float)textPadding;

							nvgTextAlign(vg, layouts[j].align);
							drawText(vg, text, tx, ty, design.scale.text, fontDefault);
						}
					}
				}
			}
		}

		int hittest(const POINT& point) const
		{
			RECT rc; ::GetClientRect(*this, &rc);
			int cy = (rc.top + rc.bottom) / 2;
			return (point.y > cy) ? Hive::HitTest::MinRange : Hive::HitTest::MaxRange;
		}

		LRESULT onCreate(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			MY_TRACE_FUNC("");

			if (!initOpenGL()) return -1;

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}

		LRESULT onDestroy(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			MY_TRACE_FUNC("");

			termOpenGL();

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}

		LRESULT onPaint(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			PaintDC dc(hwnd);
			RECT rc = dc.m_ps.rcPaint;
			doPaint(dc, rc);

			return 0;
		}

		LRESULT onLButtonDown(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			MY_TRACE_FUNC("0x%08X, 0x%08X", wParam, lParam);

			::SetCapture(hwnd);

			hive.drag.origPoint = LP2PT(lParam);
			hive.drag.ht = hittest(hive.drag.origPoint);

			switch (hive.drag.ht)
			{
			case Hive::HitTest::MinRange: hive.drag.origValue = hive.minRange; break;
			case Hive::HitTest::MaxRange: hive.drag.origValue = hive.maxRange; break;
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}

		LRESULT onMouseMove(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
//			MY_TRACE_FUNC("0x%08X, 0x%08X", wParam, lParam);

			if (::GetCapture() == hwnd)
			{
				POINT point = LP2PT(lParam);
				int offset = point.y - hive.drag.origPoint.y;

				switch (hive.drag.ht)
				{
				case Hive::HitTest::MinRange: hive.minRange = std::min(hive.drag.origValue + offset, hive.maxRange - 1); break;
				case Hive::HitTest::MaxRange: hive.maxRange = std::max(hive.drag.origValue + offset, hive.minRange + 1); break;
				}

				listener->onInvalidate(this);
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}

		LRESULT onLButtonUp(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			MY_TRACE_FUNC("0x%08X, 0x%08X", wParam, lParam);

			::ReleaseCapture();

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}

		LRESULT onRedraw(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			MY_TRACE_FUNC("");

			return listener->onRedraw(this);
		}

		LRESULT onConfig(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			MY_TRACE_FUNC("");

			return listener->onConfig(this);
		}

		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_CREATE: return onCreate(hwnd, message, wParam, lParam);
			case WM_DESTROY: return onDestroy(hwnd, message, wParam, lParam);
			case WM_PAINT: return onPaint(hwnd, message, wParam, lParam);
			case WM_LBUTTONDOWN: return onLButtonDown(hwnd, message, wParam, lParam);
			case WM_MOUSEMOVE: return onMouseMove(hwnd, message, wParam, lParam);
			case WM_LBUTTONUP: return onLButtonUp(hwnd, message, wParam, lParam);
			}

			if (message == Share::AudioGraph::Message::Redraw) return onRedraw(hwnd, message, wParam, lParam);
			else if (message == Share::AudioGraph::Message::Config) return onConfig(hwnd, message, wParam, lParam);

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}
	};
}
