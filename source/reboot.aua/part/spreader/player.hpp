#pragma once

namespace apn::reboot::spreader
{
	//
	// このクラスはプレイヤーウィンドウです。
	//
	inline struct Player : my::Window
	{
		using DIB = my::DIBDC<AviUtl::PixelBGR>;

		// 表示する映像信号です。
		std::unique_ptr<DIB> video;

		// フレーム情報です。
		int32_t current_frame = 0;
		int32_t start_frame = 0;
		int32_t end_frame = 0;
		AviUtl::FileInfo file_info = {};

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

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
				DpiSetter() { context = ::SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ); }
				~DpiSetter() { ::SetThreadDpiAwarenessContext(context); }
			} dpi_setter;

			// NC領域が存在しない非表示のオーバラップウィンドウを作成します。
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

			// DPI調整されたサイズを取得します。
			auto dpi_rc = get_dpi_rect(rc);
			auto w = my::get_width(dpi_rc);
			auto h = my::get_height(dpi_rc);

			update_back_buffer(w, h);
			remove_message(*this, agit.c_message.c_show_window);
			my::set_window_pos(*this, HWND_TOP, &rc, SWP_SHOWWINDOW);
		}

		//
		// バックバッファを更新します。
		//
		void update_back_buffer(int32_t w, int32_t h)
		{
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

			// フレーム情報を取得します。
			current_frame = fpip->frame;
			start_frame = 0;
			end_frame = fpip->frame_n;
			fp->exfunc->get_file_info(fpip->editp, &file_info);

			my::ClientDC dc(*this);

			// 映像信号情報を取得します。
			auto width = fpip->w;
			auto height = fpip->h;
			auto stride = width * 3 + width % 4;
			auto byte_count = sizeof(AviUtl::PixelBGR);

			// 映像サイズが異なる場合はビットマップを作り直します。
			if (!video || video->header.biWidth != width || video->header.biHeight != height)
				video = std::make_unique<DIB>(dc, width, height);

			// ビットマップが作成できなかった場合は何もしません。
			if (!video->bits) return FALSE;

			// 映像信号をDIBitsに変換します。
			auto dst_stride = stride / byte_count;
			auto src_stride = fpip->max_w;

			auto* dst = video->bits;
			auto* src = (AviUtl::PixelYC*)fpip->ycp_edit;

			// aviutlの色変換関数を取得します。
			auto yc_to_rgb = magi.fp->exfunc->yc2rgb;

			#pragma omp parallel for
			for (auto y = 0; y < height; y++)
			{
				auto dst_y = height - y - 1;
				auto src_y = y;

				auto* dst_line = dst + dst_y * dst_stride;
				auto* src_line = src + src_y * src_stride;

				yc_to_rgb(dst_line, src_line, width);
			}

			// 映像信号をウィンドウに出力します。
			paint(dc);

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
					::MulDiv(video->header.biWidth, agit.relative_size, 100),
					::MulDiv(video->header.biHeight, agit.relative_size, 100),
				};
			case agit.c_size_mode.c_absolute:
				return {
					agit.absolute_size.cx ? agit.absolute_size.cx : video->header.biWidth,
					agit.absolute_size.cy ? agit.absolute_size.cy : video->header.biHeight,
				};
			}

			// それ以外の場合は映像信号のサイズをそのまま返します。
			return { video->header.biWidth, video->header.biHeight };
		}

		//
		// 伸縮モードがc_noneの場合の
		// ビットブロックの転送範囲を返します。
		//
		void get_none_blt(Blt& blt)
		{
			auto display_size = get_display_size();
			auto video_size = SIZE { video->header.biWidth, video->header.biHeight };

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
			auto video_size = SIZE { video->header.biWidth, video->header.biHeight };

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
			auto video_size = SIZE { video->header.biWidth, video->header.biHeight };

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
			auto video_size = SIZE { video->header.biWidth, video->header.biHeight };

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

		int32_t need_time = 0;

		//
		// 映像信号を描画します。
		//
		LRESULT paint(HDC dc)
		{
			// 描画範囲を取得します。
			auto rc = my::get_client_rect(*this);
			rc = get_dpi_rect(rc);
			auto rc_x = rc.left;
			auto rc_y = rc.top;
			auto rc_w = my::get_width(rc);
			auto rc_h = my::get_height(rc);

			// 映像信号が有効の場合は
			if (video)
			{
				if (agit.show_thumbnail)
				{
					::SetStretchBltMode(*video, HALFTONE);

					auto w = video->header.biWidth;
					auto h = video->header.biHeight;
					auto scale = 0;

					for (auto thumbnail_scale : agit.thumbnail_scale)
					{
						if (thumbnail_scale <= 0) break;

						scale += thumbnail_scale + 100;

						auto tw = ::MulDiv(w, 100, scale);
						auto th = ::MulDiv(h, 100, scale);

						// 映像信号を縮小して重ねます。
						::StretchBlt(*video, 0, 0 + h - th, tw, th, *video, 0, 0, w, h, SRCCOPY);
					}
				}

				// 文字列を描画します。
				{
					auto current_frame = this->current_frame - this->start_frame;
					auto end_frame = this->end_frame - this->start_frame;

					std::wstring text;

					// フレームを表示する場合は
					if (agit.show_frame)
					{
						text += my::format(_T("{} / {}"), current_frame, end_frame);
					}

					// 時間を表示する場合は
					if (agit.show_time && file_info.video_rate && file_info.video_scale)
					{
						if (text.length()) text += _T("\n");

						auto ratio = file_info.video_scale / (double)file_info.video_rate;
						auto current_time = current_frame * ratio;
						auto end_time = end_frame * ratio;

						text += my::format(_T("{:02d}:{:02d}:{:06.3f} / {:02d}:{:02d}:{:06.3f}"),
							(int32_t)(current_time / 3600), (int32_t)(current_time / 60), std::fmod(current_time, 60),
							(int32_t)(end_time / 3600), (int32_t)(end_time / 60), std::fmod(end_time, 60));
					}

					// 文字列を描画します。
					if (text.length())
						::DrawTextW(*video, text.c_str(), text.length(), &rc, DT_LEFT | DT_TOP);
				}

				// 映像信号を描画します。
				{
					// ストレッチモードを変更します。
					// ※このコードを使用すると描画が非常に低速になるのでコメントアウトしています。
					//::SetStretchBltMode(dc, HALFTONE);

					Blt blt;
					blt.base.x = rc_x;
					blt.base.y = rc_y;
					blt.base.w = rc_w;
					blt.base.h = rc_h;
					get_blt(blt);

					// ビットマップを描画します。
					::StretchBlt(dc, blt.dst.x, blt.dst.y, blt.dst.w, blt.dst.h,
						*video, blt.src.x, blt.src.y, blt.src.w, blt.src.h, SRCCOPY);
				}
			}
			// 映像信号が無効の場合は
			else
			{
				// 背景を描画します。
				::FillRect(dc, &rc, (HBRUSH)(COLOR_WINDOW + 1));
			}

			return 0;
		}

		//
		// WM_PAINTを処理します。
		//
		LRESULT on_paint(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			// 描画DCを用意します。
			my::PaintDC dc(*this);

			// クライアント矩形を取得します。
			auto rc = my::get_client_rect(*this);

			// DPI調整された矩形を取得します。
			auto dpi_rc = get_dpi_rect(rc);

			// 背景を描画します。
			::FillRect(dc, &dpi_rc, (HBRUSH)(COLOR_WINDOW + 1));

			return 0;
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
			MY_TRACE_FUNC("{}", wParam);

			// プレイヤーの表示状態を切り替えます。
			::ShowWindow(hwnd, wParam);

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
					MY_TRACE_FUNC("WM_CREATE, {:#010x}, {:#010x}", wParam, lParam);

					// 最後の初期化処理を遅延させて実行します。
					::PostMessage(hwnd, agit.c_message.c_post_init, 0, 0);

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {:#010x}, {:#010x}", wParam, lParam);

					break;
				}
			case WM_LBUTTONDOWN:
				{
					MY_TRACE_FUNC("WM_LBUTTONDOWN, {:#010x}, {:#010x}", wParam, lParam);

					// 左クリックで再生停止するようにします。
					return ::SendMessage(hive.aviutl_window, WM_KEYDOWN, VK_SPACE, 0);
				}
			case WM_PAINT: return on_paint(hwnd, message, wParam, lParam);
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
