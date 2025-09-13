#pragma once

namespace apn::ult_combobox
{
	//
	// このクラスは自作コンボボックス関連ウィンドウの基本クラスです。
	//
	struct Node : my::Window
	{
		//
		// デストラクタです。
		//
		virtual ~Node()
		{
		}

		//
		// ウィンドウをサブクラス化します。
		//
		virtual BOOL subclass(HWND hwnd) override
		{
			// サブクラス化する前にウィンドウプロシージャを
			// デフォルトウィンドウプロシージャに変更します。
			// これにより、元のコントロールの処理を省略できます。
			::SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)&::DefWindowProc);

			return __super::subclass(hwnd);
		}

		//
		// 再描画します。
		//
		virtual BOOL redraw()
		{
			return ::InvalidateRect(*this, nullptr, FALSE);
		}
	};
}
