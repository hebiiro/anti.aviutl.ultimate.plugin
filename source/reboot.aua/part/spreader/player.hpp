#pragma once

namespace apn::reboot::spreader
{
	//
	// このクラスはプレイヤーウィンドウです。
	//
	inline struct Player : my::Window
	{
		// 表示する映像信号です。
		BITMAPINFOHEADER header = { sizeof(header) };
		my::gdi::unique_ptr<HBITMAP> bitmap;
		AviUtl::PixelBGR* bits = nullptr;

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
			if (header.biWidth != width || header.biHeight != height)
			{
				header.biWidth = width;
				header.biHeight = height;
				header.biPlanes = 1;
				header.biBitCount = byte_count * 8;
				header.biCompression = BI_RGB;

				bits = nullptr;
				bitmap.reset(::CreateDIBSection(dc, (BITMAPINFO*)&header,
					DIB_RGB_COLORS, (void**)&bits, nullptr, 0));
			}

			// ビットマップが作成できなかった場合は何もしません。
			if (!bitmap) return FALSE;

			// 映像信号をDIBitsに変換します。
			auto dst_stride = stride / byte_count;
			auto src_stride = fpip->max_w;

			auto* dst = bits;
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
		// 映像信号を描画します。
		//
		LRESULT paint(HDC dc)
		{
			// 描画範囲を取得します。
			auto rc = my::get_client_rect(hwnd);
			auto w = my::get_width(rc);
			auto h = my::get_height(rc);

			// ビットマップが有効の場合は
			if (bitmap)
			{
				// メモリDCを作成します。
				my::dc::unique_ptr<> mem_dc(::CreateCompatibleDC(dc));
				my::gdi::selector bitmap_selector(mem_dc.get(), bitmap.get());

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
					::DrawTextW(mem_dc.get(), text.c_str(), text.length(), &rc, DT_LEFT | DT_TOP);

				// ストレッチモードを変更します。
				// ※このコードを使用すると描画が非常に低速になるのでコメントアウトしています。
				//::SetStretchBltMode(dc, HALFTONE);

				// ビットマップを描画します。
				::StretchBlt(dc, 0, 0, w, h, mem_dc.get(),
					0, 0, header.biWidth, header.biHeight, SRCCOPY);
			}
			// ビットマップが無効の場合は
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
			my::PaintDC pdc(hwnd);

			return paint(pdc);
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

			::ShowWindow(hwnd, wParam);

			return 0;
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			if (message >= WM_KEYFIRST && message <= WM_KEYLAST)
			{
				MY_TRACE("キーボードメッセージをプレビューウィンドウにバイパスします\n");

				return ::SendMessage(agit.preview_window, message, wParam, lParam);
			}

			if (message >= WM_MOUSEFIRST && message <= WM_MOUSELAST)
			{
				MY_TRACE("マウスメッセージをプレビューウィンドウにバイパスします\n");

				return ::SendMessage(agit.preview_window, message, wParam, lParam);
			}

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
			case WM_PAINT: return on_paint(hwnd, message, wParam, lParam);
			case agit.c_message.c_post_init: return on_post_init(hwnd, message, wParam, lParam);
			case agit.c_message.c_show_window: return on_show_window(hwnd, message, wParam, lParam);
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} player;
}
