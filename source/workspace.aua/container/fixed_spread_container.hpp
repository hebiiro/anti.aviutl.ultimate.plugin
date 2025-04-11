#pragma once

namespace apn::workspace
{
	//
	// このクラスはコンテンツをクライアント領域全体まで広げるコンテナです。
	// ただし、コンテナ自身のサイズは変更しません。
	//
	struct FixedSpreadContainer : SpreadContainer
	{
		//
		// コンストラクタです。
		//
		FixedSpreadContainer(Content* content, DWORD style, DWORD ex_style)
			: SpreadContainer(content, style, ex_style)
		{
		}

		//
		// デストラクタです。
		//
		virtual ~FixedSpreadContainer() override
		{
		}

		//
		// コンテンツの位置を修正するために呼ばれます。
		//
		virtual BOOL revise_content_position(WINDOWPOS* wp) override
		{
			MY_TRACE_FUNC("{/}, {/}, {/}, {/}, {/hex}", wp->x, wp->y, wp->cx, wp->cy, wp->flags);

			// コンテンツのHWNDを取得します。
			auto hwnd = content->get_hwnd();

			// コンテンツに最適な位置を取得して返します。
			return get_ideal_content_position(hwnd, wp);
		}

		//
		// コンテンツの位置にフィットするようにコンテナの位置を変更します。
		//
		virtual BOOL fit_container_position(const WINDOWPOS* content_wp) override
		{
			MY_TRACE_FUNC("{/}, {/}, {/}, {/}, {/hex}", content_wp->x, content_wp->y, content_wp->cx, content_wp->cy, content_wp->flags);

			return TRUE; // コンテナのサイズを変更しないようにします。
		}
	};
}
