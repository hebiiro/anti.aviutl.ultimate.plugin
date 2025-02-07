#pragma once

namespace apn::workspace
{
	//
	// このクラスはコンテンツをクライアント領域全体まで広げるコンテナです。
	//
	struct SpreadContainer : Container
	{
		//
		// コンストラクタです。
		//
		SpreadContainer(Content* content, DWORD style, DWORD ex_style)
			: Container(content, style, ex_style)
		{
		}

		//
		// デストラクタです。
		//
		virtual ~SpreadContainer() override
		{
		}

		//
		// 理想のコンテンツ位置を返します。
		// hwndはコンテンツのHWNDです。
		//
		BOOL get_ideal_content_position(HWND hwnd, WINDOWPOS* wp)
		{
			// コンテナのクライアント矩形を取得します。
			auto rc = my::get_client_rect(*this);

			// コンテナのクライアント矩形をコンテンツのウィンドウ矩形に変換します。
			my::client_to_window(hwnd, &rc);

			// rcを元にしてコンテンツから最適な位置を取得します。
			content->revise_content_position(&rc);

			// 算出したコンテンツ用の矩形を返します。
			*wp = rc_to_wp(&rc, wp->flags);

			return TRUE;
		}

		//
		// コンテンツの位置を変更するために呼ばれます。
		//
		virtual BOOL set_content_position() override
		{
			MY_TRACE_FUNC("");

			// コンテンツのHWNDを取得します。
			auto hwnd = content->get_hwnd();

			// コンテンツに最適な位置を取得します。
			auto wp = WINDOWPOS {};
			wp.flags = SWP_NOZORDER | SWP_NOACTIVATE;
			get_ideal_content_position(hwnd, &wp);

			// コンテンツの位置を変更します。
			set_window_pos(hwnd, &wp);

			return TRUE;
		}
	};
}
