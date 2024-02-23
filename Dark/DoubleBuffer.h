#pragma once

namespace fgo::dark
{
	//
	// このクラスはダブルバッファリングを管理します。
	//
	inline struct DoubleBuffer
	{
		inline static constexpr LPCTSTR PropName = _T("fgo::dark::double_buffer");

		//
		// このクラスはバックバッファを管理します。
		//
		struct Node
		{
			HDC frontDC = 0;
			HDC backDC = 0;
			HBITMAP bitmap = 0;
			HBITMAP oldBitmap = 0;
			RECT rc = {};

			//
			// バックバッファを返します。
			//
			HDC getBackBuffer()
			{
				return backDC;
			}

			//
			// バックバッファを作成して返します。
			//
			HDC create(HWND hwnd, HDC dc)
			{
				// バックバッファが作成済みの場合は
				if (backDC)
				{
					// 作成済みのバックバッファを返します。
					return backDC;
				}

				// クライアント矩形を取得します。
				::GetClientRect(hwnd, &rc);
				int w = rc.right - rc.left;
				int h = rc.bottom - rc.top;

				// バックバッファを作成します。
				frontDC = dc;
				backDC = ::CreateCompatibleDC(frontDC);
				bitmap = ::CreateCompatibleBitmap(frontDC, w, h);
				oldBitmap = (HBITMAP)::SelectObject(backDC, bitmap);

				// バックバッファを返します。
				return backDC;
			}

			//
			// バックバッファを削除します。
			//
			BOOL destroy()
			{
				::SelectObject(backDC, oldBitmap), oldBitmap = 0;
				::DeleteObject(bitmap), bitmap = 0;
				::DeleteDC(backDC), backDC = 0;

				return TRUE;
			}

			//
			// バックバッファをフロントバッファに転送します。
			//
			BOOL swap()
			{
				int w = rc.right - rc.left;
				int h = rc.bottom - rc.top;

				return ::BitBlt(frontDC, rc.left, rc.top, w, h, backDC, 0, 0, SRCCOPY);
			}
		};

		//
		// 指定されたウィンドウに関連付けられているノードを返します。
		// 内部的に使用されます。
		//
		static Node* getNode(HWND hwnd)
		{
			return (Node*)::GetProp(hwnd, PropName);
		}

		//
		// 指定されたウィンドウにノードを関連付けます。
		// 内部的に使用されます。
		//
		static BOOL setNode(HWND hwnd, Node* node)
		{
			return ::SetProp(hwnd, PropName, node);
		}

		//
		// ダブルバッファリングに必要なノードを作成します。
		// ウィンドウ作成時に呼び出してください。
		//
		static BOOL create(HWND hwnd)
		{
			auto node = getNode(hwnd);
			if (node) return FALSE;
			node = new Node();
			return setNode(hwnd, node);
		}

		//
		// ノードを削除します。
		// ウィンドウ破壊時に呼び出してください。
		//
		static BOOL destroy(HWND hwnd)
		{
			auto node = getNode(hwnd);
			if (!node) return FALSE;
			delete node;
			return setNode(hwnd, 0);
		}

		//
		// 指定されウィンドウのバックバッファを返します。
		//
		static HDC getBackBuffer(HWND hwnd)
		{
			auto node = getNode(hwnd);
			if (!node) return 0;
			return node->getBackBuffer();
		}

		//
		// 指定されウィンドウのバックバッファを作成します。
		//
		static HDC createBackBuffer(HWND hwnd, HDC dc)
		{
			auto node = getNode(hwnd);
			if (!node) return 0;
			return node->create(hwnd, dc);
		}

		//
		// 指定されウィンドウのバックバッファを
		// フロントバッファに転送してから削除します。
		//
		static BOOL swap(HWND hwnd)
		{
			auto node = getNode(hwnd);
			if (!node) return FALSE;
			node->swap();
			return node->destroy();
		}
	} double_buffer;
}
