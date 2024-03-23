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
		SpreadContainer(Content* content, DWORD style)
			: Container(content, style)
		{
		}

		//
		// デストラクタです。
		//
		virtual ~SpreadContainer() override
		{
		}

		//
		// コンテンツの位置を変更するために呼ばれます。
		//
		virtual BOOL set_content_position() override
		{
			// コンテンツをコンテナのクライアント領域全体まで広げます。

			// コンテンツのHWNDを取得します。
			auto hwnd = content->get_hwnd();

			// コンテナのクライアント矩形を取得します。
			auto rc = my::get_client_rect(*this);

			// コンテナのクライアント矩形をコンテンツのウィンドウ矩形に変換します。
			my::client_to_window(hwnd, &rc);

			// rcを元にしてコンテンツから最適な位置を取得します。
			content->revise_content_position(&rc);

			// コンテンツの位置を変更します。
			set_window_pos(hwnd, &rc);

			return TRUE;
		}

		//
		// コンテンツの位置にフィットするようにコンテナの位置を変更します。
		//
		virtual BOOL fit_container_position(const WINDOWPOS* content_wp) override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}
	};
}
