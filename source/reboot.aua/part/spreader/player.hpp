#pragma once

namespace apn::reboot::spreader
{
	//
	// このクラスはプレイヤーウィンドウです。
	//
	inline struct Player : my::Window
	{
#ifdef _DEBUG
		struct Clocker {
			LPCTSTR label = nullptr;
			DWORD start_time = 0;
			DWORD end_time = 0;
			Clocker(LPCTSTR label) : label(label) { start_time = ::timeGetTime(); }
			~Clocker() { end_time = ::timeGetTime(); MY_TRACE("{/} = {/}ms\n", label, end_time - start_time); }
		};
#else
		struct Clocker {
			Clocker(LPCTSTR label) {}
			~Clocker() {}
		};
#endif
		//
		// この構造体はビットマップのピクセルデータです。
		//
		struct PixelBGRA { uint8_t b, g, r, a; };

		//
		// フレーム情報です。
		//
		int32_t current_frame = 0;
		int32_t start_frame = 0;
		int32_t end_frame = 0;
		AviUtl::FileInfo file_info = {};

		//
		// Direct2D関連の変数です。
		//
		ComPtr<ID2D1Factory> d2d_factory;
		ComPtr<ID2D1HwndRenderTarget> render_target;
		ComPtr<ID2D1Bitmap> d2d_bitmap;

		//
		// DirectWrite関連の変数です。
		//
		ComPtr<IDWriteFactory> dw_factory;

		//
		// 映像信号情報です。
		//
		SIZE video_size = {};

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// Direct2Dを初期化します。
			auto hr = ::D2D1CreateFactory(
				D2D1_FACTORY_TYPE_SINGLE_THREADED,
				d2d_factory.GetAddressOf());
			if (FAILED(hr))
			{
				MY_TRACE_STR(my::get_error_message(hr));

				return FALSE;
			}

			// DirectWriteを初期化します。
			hr = ::DWriteCreateFactory(
				DWRITE_FACTORY_TYPE_SHARED,
				__uuidof(IDWriteFactory),
				(IUnknown**)dw_factory.GetAddressOf());
			if (FAILED(hr))
			{
				MY_TRACE_STR(my::get_error_message(hr));

				return FALSE;
			}

			// ウィンドウクラスを登録します。
			WNDCLASSEXW wc = { sizeof(wc) };
			wc.style = 0;
			wc.lpfnWndProc = ::DefWindowProcW;
			wc.hInstance = hive.instance;
			wc.hCursor = ::LoadCursor(nullptr, IDC_CROSS);
			wc.lpszClassName = L"reboot.spreader.player";
			::RegisterClassExW(&wc);

			struct DpiSetter {
				DPI_AWARENESS_CONTEXT context;
				DpiSetter() { context = ::SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2); }
				~DpiSetter() { ::SetThreadDpiAwarenessContext(context); }
			} dpi_setter;

			// NC領域が存在しない非表示のオーバーラップウィンドウを作成します。
			return __super::create(
				0,
				wc.lpszClassName,
				wc.lpszClassName,
				WS_POPUP | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				0, 0, 0, 0,
				nullptr, nullptr, hive.instance, nullptr);
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return destroy();
		}

		//
		// ウィンドウが表示されているスクリーンの矩形を返します。
		//
		inline static auto get_screen_rect(HWND hwnd)
		{
			auto monitor = ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
			MONITORINFOEX monitor_info = {};
			monitor_info.cbSize = sizeof(monitor_info);
			::GetMonitorInfo(monitor, &monitor_info);
			return monitor_info.rcMonitor;
		}

		//
		// 指定されたメッセージをメッセージキューから取り除きます。
		//
		inline static void remove_message(HWND hwnd, uint32_t message)
		{
			auto msg = MSG {};
			while (::PeekMessage(&msg, hwnd, message, message, PM_REMOVE)) {}
		}

		//
		// DPIで補正した値を返します。
		//
		inline static auto mul_dpi(auto value, auto dpi)
		{
			return ::MulDiv(value, dpi, USER_DEFAULT_SCREEN_DPI);
		}

		//
		// 指定された矩形のDPIを調整して返します。
		//
		RECT get_dpi_rect(const RECT& rc)
		{
			auto dpi = ::GetDpiForWindow(*this);

			return {
				.left = ::MulDiv(rc.left, dpi, USER_DEFAULT_SCREEN_DPI),
				.top = ::MulDiv(rc.top, dpi, USER_DEFAULT_SCREEN_DPI),
				.right = ::MulDiv(rc.right, dpi, USER_DEFAULT_SCREEN_DPI),
				.bottom = ::MulDiv(rc.bottom, dpi, USER_DEFAULT_SCREEN_DPI),
			};
		}

		//
		// プレイヤーを表示します。
		//
		void show(HWND hwnd)
		{
			MY_TRACE_FUNC("");

			// スクリーン矩形を取得します。
			auto rc = get_screen_rect(hwnd);

			remove_message(*this, agit.c_message.c_show_window);
			my::set_window_pos(*this, HWND_TOP, &rc, SWP_SHOWWINDOW);
		}

		//
		// バックバッファを更新します。
		//
		void update_back_buffer(int32_t w, int32_t h)
		{
			// ファクトリが作成されていない場合は何もしません。
			if (!d2d_factory) return;

			// バックバッファが作成されていない場合は
			if (!render_target)
			{
				// バックバッファを作成します。
				auto hr = d2d_factory->CreateHwndRenderTarget(
					D2D1::RenderTargetProperties(
						D2D1_RENDER_TARGET_TYPE_DEFAULT,
						D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
						96.0f, 96.0f),
					D2D1::HwndRenderTargetProperties(*this, D2D1::SizeU(w, h)),
					render_target.GetAddressOf());
				MY_TRACE_STR(my::get_error_message(hr));
			}

			// バックバッファが作成済みの場合は
			if (render_target)
			{
				// バックバッファをリサイズします。
				render_target->Resize(D2D1::SizeU(w, h));
			}
		}

		//
		// プレイヤーを非表示にします。
		//
		void hide()
		{
			MY_TRACE_FUNC("");

			::PostMessage(*this, agit.c_message.c_show_window, SW_HIDE, 0);
		}

		//
		// 表示する映像を更新します。
		//
		virtual BOOL update(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip, const Addin::ProcState& proc_state)
		{
			// プレイヤーが表示されていない場合は何もしません。
			if (!::IsWindowVisible(*this)) return FALSE;

			// 保存中のときは何もしません。
			if (proc_state.is_saving) return FALSE;

			// バックバッファが作成されていない場合は何もしません。
			if (!render_target) return FALSE;

			{
				// 前回の映像信号サイズを取得しておきます。
				auto old_video_size = video_size;

				// 映像信号サイズを更新します。
				video_size = { fpip->w, fpip->h };

				// 映像信号サイズが異なる場合は
				if (video_size.cx != old_video_size.cx || video_size.cy != old_video_size.cy)
				{
					Clocker clocker(_T("create bitmap"));

					// Direct2Dビットマップを作成します。
					auto bitmap_props = D2D1::BitmapProperties(
						D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE));
					auto hr = render_target->CreateBitmap(
						D2D1::SizeU(video_size.cx, video_size.cy),
						nullptr,
						0,
						&bitmap_props,
						d2d_bitmap.ReleaseAndGetAddressOf());
//					MY_TRACE_STR(my::get_error_message(hr));
				}

				// ビットマップが作成できなかった場合は何もしません。
				if (!d2d_bitmap) return FALSE;
			}

			// フレーム情報を取得します。
			current_frame = fpip->frame;
			start_frame = 0;
			end_frame = fpip->frame_n;
			fp->exfunc->get_file_info(fpip->editp, &file_info);

			{
				Clocker clocker(_T("update bitmap"));

				// ビットマップデータのバッファを確保します。
				std::vector<PixelBGRA> bits(video_size.cx * video_size.cy, {});

				auto dst = bits.data();
				auto dst_stride = video_size.cx;

				auto src = (AviUtl::PixelYC*)fpip->ycp_edit;
				auto src_stride = fpip->max_w;

				// aviutlの色変換関数を取得します。
				auto yc_to_rgb = magi.fp->exfunc->yc2rgb;

				// ピクセルデータを変換します。
				// OpenMPを使用して並列化しています。
				#pragma omp parallel for
				for (auto y = 0; y < video_size.cy; y++)
				{
					auto dst_y = y;
					auto src_y = y;

					auto dst_line = dst + dst_y * dst_stride;
					auto src_line = src + src_y * src_stride;

					#pragma omp parallel for
					for (auto x = 0; x < video_size.cx; x++)
					{
						auto dst_p = dst_line + x;
						auto src_p = src_line + x;

						yc_to_rgb((AviUtl::PixelBGR*)dst_p, src_p, 1);
					}
				}

				// ピクセルデータをビットマップにコピーします。
				auto hr = d2d_bitmap->CopyFromMemory(
					nullptr, dst, dst_stride * sizeof(PixelBGRA));
//				MY_TRACE_STR(my::get_error_message(hr));
			}

			// 映像信号をウィンドウに出力します。
			my::invalidate(*this);
			::UpdateWindow(*this);

			return TRUE;
		}

		//
		// ビットブロックの転送範囲です。
		//
		struct Blt {
			struct Area {
				int32_t x, y, w, h;
			} base, src, dst;
		};

		//
		// 映像信号の表示サイズを返します。
		//
		SIZE get_display_size()
		{
			switch (agit.size_mode)
			{
			case agit.c_size_mode.c_relative:
				return {
					::MulDiv(video_size.cx, agit.relative_size, 100),
					::MulDiv(video_size.cy, agit.relative_size, 100),
				};
			case agit.c_size_mode.c_absolute:
				return {
					agit.absolute_size.cx ? agit.absolute_size.cx : video_size.cx,
					agit.absolute_size.cy ? agit.absolute_size.cy : video_size.cy,
				};
			}

			// それ以外の場合は映像信号のサイズをそのまま返します。
			return video_size;
		}

		//
		// 伸縮モードがc_noneの場合の
		// ビットブロックの転送範囲を返します。
		//
		void get_none_blt(Blt& blt)
		{
			auto display_size = get_display_size();

			blt.src.x = 0;
			blt.src.y = 0;
			blt.src.w = video_size.cx;
			blt.src.h = video_size.cy;

			blt.dst.x = blt.base.x + (blt.base.w - display_size.cx) / 2;
			blt.dst.y = blt.base.y + (blt.base.h - display_size.cy) / 2;
			blt.dst.w = display_size.cx;
			blt.dst.h = display_size.cy;
		}

		//
		// 伸縮モードがc_fitの場合の
		// ビットブロックの転送範囲を返します。
		//
		void get_fit_blt(Blt& blt)
		{
			blt.src.x = 0;
			blt.src.y = 0;
			blt.src.w = video_size.cx;
			blt.src.h = video_size.cy;

			blt.dst.w = blt.base.w;
			blt.dst.h = blt.base.w * video_size.cy / video_size.cx;

			if (blt.dst.h > blt.base.h)
			{
				blt.dst.w = blt.base.h * video_size.cx / video_size.cy;
				blt.dst.h = blt.base.h;
			}

			blt.dst.x = blt.base.x + (blt.base.w - blt.dst.w) / 2;
			blt.dst.y = blt.base.y + (blt.base.h - blt.dst.h) / 2;
		}

		//
		// 伸縮モードがc_cropの場合の
		// ビットブロックの転送範囲を返します。
		//
		void get_crop_blt(Blt& blt)
		{
			blt.src.x = 0;
			blt.src.y = 0;
			blt.src.w = video_size.cx;
			blt.src.h = video_size.cy;

			blt.dst.w = blt.base.w;
			blt.dst.h = blt.base.w * video_size.cy / video_size.cx;

			if (blt.dst.h < blt.base.h)
			{
				blt.dst.w = blt.base.h * video_size.cx / video_size.cy;
				blt.dst.h = blt.base.h;
			}

			blt.dst.x = blt.base.x + (blt.base.w - blt.dst.w) / 2;
			blt.dst.y = blt.base.y + (blt.base.h - blt.dst.h) / 2;
		}

		//
		// 伸縮モードがc_fullの場合の
		// ビットブロックの転送範囲を返します。
		//
		void get_full_blt(Blt& blt)
		{
			blt.src.x = 0;
			blt.src.y = 0;
			blt.src.w = video_size.cx;
			blt.src.h = video_size.cy;

			blt.dst.x = blt.base.x;
			blt.dst.y = blt.base.y;
			blt.dst.w = blt.base.w;
			blt.dst.h = blt.base.h;
		}

		//
		// 現在の伸縮モードに合わせて
		// ビットブロックの転送範囲を返します。
		//
		void get_blt(Blt& blt)
		{
			switch (agit.stretch_mode)
			{
			default:
			case agit.c_stretch_mode.c_none: return get_none_blt(blt);
			case agit.c_stretch_mode.c_fit: return get_fit_blt(blt);
			case agit.c_stretch_mode.c_crop: return get_crop_blt(blt);
			case agit.c_stretch_mode.c_full: return get_full_blt(blt);
			}
		}

		//
		// テキストを描画します。
		//
		BOOL draw_text(const RECT& rc)
		{
			auto dpi = ::GetDpiForWindow(*this);

			auto rc_x = rc.left;
			auto rc_y = rc.top;
			auto rc_w = my::get_width(rc);
			auto rc_h = my::get_height(rc);

			auto current_frame = this->current_frame - this->start_frame;
			auto end_frame = this->end_frame - this->start_frame;

			std::wstring text;

			// フレームを表示する場合は
			if (agit.show_frame)
			{
				text += my::format(_T("{/} / {/}"), current_frame, end_frame);
			}

			// 時間を表示する場合は
			if (agit.show_time && file_info.video_rate && file_info.video_scale)
			{
				if (text.length()) text += _T("\n");

				auto ratio = file_info.video_scale / (double)file_info.video_rate;
				auto current_time = current_frame * ratio;
				auto end_time = end_frame * ratio;

				text += my::format(_T("{/02d}:{/02d}:{/06.3f} / {/02d}:{/02d}:{/06.3f}"),
					(int32_t)(current_time / 3600), (int32_t)(current_time / 60), std::fmod(current_time, 60),
					(int32_t)(end_time / 3600), (int32_t)(end_time / 60), std::fmod(end_time, 60));
			}

			// テキストが空の場合は何もしません。
			if (text.empty()) return FALSE;

			// 背景のブラシを作成します。
			ComPtr<ID2D1SolidColorBrush> fill_brush;
			auto hr = render_target->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::White), &fill_brush);
//			MY_TRACE_STR(my::get_error_message(hr));
			if (!fill_brush) return FALSE;

			// テキストのブラシを作成します。
			ComPtr<ID2D1SolidColorBrush> text_brush;
			hr = render_target->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Black), &text_brush);
//			MY_TRACE_STR(my::get_error_message(hr));
			if (!text_brush) return FALSE;

			// フォントを取得します。
			AviUtl::SysInfo si = {};
			magi.fp->exfunc->get_sys_info(nullptr, &si);

			// フォント情報を取得します。
			LOGFONTW lf = {};
			::GetObject(si.hfont, sizeof(lf), &lf);

			// テキストフォーマットを作成します。
			ComPtr<IDWriteTextFormat> text_format;
			hr = dw_factory->CreateTextFormat(
				lf.lfFaceName,
				nullptr,
				DWRITE_FONT_WEIGHT_NORMAL,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				(float)mul_dpi(std::abs(lf.lfHeight), dpi),
				L"",
				&text_format);
//			MY_TRACE_STR(my::get_error_message(hr));
			if (!text_format) return FALSE;

			// テキストレイアウトを作成します。
			ComPtr<IDWriteTextLayout> text_layout;
			hr = dw_factory->CreateTextLayout(
				text.c_str(),
				(UINT32)text.length(),
				text_format.Get(),
				(float)rc_w,
				(float)rc_h,
				&text_layout);
//			MY_TRACE_STR(my::get_error_message(hr));
			if (!text_layout) return FALSE;

			// テキストの寸法を取得します。
			DWRITE_TEXT_METRICS tm = {};
			text_layout->GetMetrics(&tm);

			float origin = 8.0f;
			float padding = 4.0f;

			float left = origin + tm.left;
			float top = origin + tm.top;
			float right = left + tm.width + padding * 2;
			float bottom = top + tm.height + padding * 2;

			auto fill_rc = D2D1::RoundedRect(D2D1::RectF(left, top, right, bottom), 4.0f, 4.0f);

			// 背景矩形を描画します。
			render_target->FillRoundedRectangle(fill_rc, fill_brush.Get());
			render_target->DrawRoundedRectangle(fill_rc, text_brush.Get());

			// テキストを描画します。
			render_target->DrawTextLayout(
				D2D1::Point2F(left + padding, top + padding),
				text_layout.Get(),
				text_brush.Get(),
				D2D1_DRAW_TEXT_OPTIONS_NONE);

			return TRUE;
		}

		//
		// WM_PAINTを処理します。
		//
		LRESULT on_paint(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			auto rc = my::get_client_rect(*this);
			auto rc_x = rc.left;
			auto rc_y = rc.top;
			auto rc_w = my::get_width(rc);
			auto rc_h = my::get_height(rc);
			my::PaintDC dc(*this);

			if (!render_target)
				return 0;

			Clocker clocker(_T("paint"));

			render_target->BeginDraw();
			render_target->Clear(D2D1::ColorF(D2D1::ColorF::Black));

			// ビットマップが有効の場合は
			if (d2d_bitmap)
			{
				Blt blt;
				blt.base.x = rc_x;
				blt.base.y = rc_y;
				blt.base.w = rc_w;
				blt.base.h = rc_h;
				get_blt(blt);

				D2D1_RECT_F bitmap_rc = D2D1::RectF(
					static_cast<float>(blt.dst.x),
					static_cast<float>(blt.dst.y),
					static_cast<float>(blt.dst.x + blt.dst.w),
					static_cast<float>(blt.dst.y + blt.dst.h));

				// ビットマップを描画します。
				render_target->DrawBitmap(d2d_bitmap.Get(), bitmap_rc);

				// サムネイルを表示する場合は
				if (agit.show_thumbnail)
				{
					auto w = video_size.cx;
					auto h = video_size.cy;
					auto scale = 0;

					for (auto thumbnail_scale : agit.thumbnail_scale)
					{
						if (thumbnail_scale <= 0) break;

						scale += thumbnail_scale + 100;

						auto tw = (float)::MulDiv(w, 100, scale);
						auto th = (float)::MulDiv(h, 100, scale);

						auto x = (float)rc_x;
						auto y = (float)rc_y + (float)rc_h - th;
						D2D1_RECT_F bitmap_rc = D2D1::RectF(x, y, x + tw, y + th);

						// ビットマップを縮小して描画します。
						render_target->DrawBitmap(d2d_bitmap.Get(), bitmap_rc);
					}
				}
			}

			// テキストを描画します。
			draw_text(rc);

			render_target->EndDraw();

			return 0;
		}

		//
		// WM_SIZEを処理します。
		//
		LRESULT on_size(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			auto w = LOWORD(lParam);
			auto h = HIWORD(lParam);

			// バックバッファをリサイズします。
			update_back_buffer(w, h);

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		//
		// agit.c_message.c_post_initを処理します。
		//
		LRESULT on_post_init(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			MY_TRACE_FUNC("");

			// aviutlウィンドウを取得します。
			agit.preview_window = hive.aviutl_window;

			// 再生ウィンドウで再生する場合は
			if (!agit.movieplaymain)
			{
				// 再生ウィンドウを取得します。
				if (auto filter_plugin = magi.auin.get_filter_plugin(magi.fp, "再生ウィンドウ"))
					agit.preview_window = filter_plugin->hwnd;
			}

			return 0;
		}

		//
		// agit.c_message.c_show_windowを処理します。
		//
		LRESULT on_show_window(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			MY_TRACE_FUNC("{/}", wParam);

			// プレイヤーの表示状態を切り替えます。
			::ShowWindow(hwnd, (int)wParam);

			return 0;
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_CREATE:
				{
					MY_TRACE_FUNC("WM_CREATE, {/hex}, {/hex}", wParam, lParam);

					// 最後の初期化処理を遅延させて実行します。
					::PostMessage(hwnd, agit.c_message.c_post_init, 0, 0);

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {/hex}, {/hex}", wParam, lParam);

					break;
				}
			case WM_LBUTTONDOWN:
				{
					MY_TRACE_FUNC("WM_LBUTTONDOWN, {/hex}, {/hex}", wParam, lParam);

					// 左クリックで再生停止するようにします。
					return ::SendMessage(hive.aviutl_window, WM_KEYDOWN, VK_SPACE, 0);
				}
			case WM_PAINT: return on_paint(hwnd, message, wParam, lParam);
			case WM_SIZE: return on_size(hwnd, message, wParam, lParam);
			case agit.c_message.c_post_init: return on_post_init(hwnd, message, wParam, lParam);
			case agit.c_message.c_show_window: return on_show_window(hwnd, message, wParam, lParam);
			}

			if (message >= WM_KEYFIRST && message <= WM_KEYLAST)
			{
				MY_TRACE("キーボードメッセージをaviutlウィンドウにバイパスします\n");

				return ::SendMessage(hive.aviutl_window, message, wParam, lParam);
			}
#if 0
			if (message >= WM_MOUSEFIRST && message <= WM_MOUSELAST)
			{
				MY_TRACE("マウスメッセージをプレビューウィンドウにバイパスします\n");

				return ::SendMessage(agit.preview_window, message, wParam, lParam);
			}
#endif
			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} player;
}
