#pragma once
#include "Container.h"

namespace fgo::nest
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
		~SpreadContainer() override
		{
		}

		//
		// コンテンツの位置を変更するために呼ばれます。
		//
		BOOL setContentPosition() override
		{
			// コンテンツをコンテナのクライアント領域全体まで広げます。

			// コンテンツのHWNDを取得します。
			HWND hwnd = content->getHWND();

			// コンテナのクライアント矩形を取得します。
			RECT rc; ::GetClientRect(*this, &rc);

			// コンテナのクライアント矩形をコンテンツのウィンドウ矩形に変換します。
			clientToWindow(hwnd, &rc);

			// rcを元にしてコンテンツから最適な位置を取得します。
			content->reviseContentPosition(&rc);

			// コンテンツの位置を変更します。
			setWindowPos(hwnd, &rc);

			return TRUE;
		}
	};
}
