#pragma once

namespace apn::dark
{
	//
	// このクラスはダブルバッファリングを管理します。
	//
	inline struct DoubleBuffer
	{
		inline static constexpr LPCTSTR c_prop_name = _T("apn::dark::double_buffer");

		//
		// このクラスはバックバッファを管理します。
		//
		struct Node
		{
			HDC front_dc = 0;
			HDC back_dc = 0;
			HBITMAP bitmap = 0;
			HBITMAP old_bitmap = 0;
			RECT rc = {};

			//
			// バックバッファを返します。
			//
			HDC get_back_buffer()
			{
				return back_dc;
			}

			//
			// バックバッファを作成して返します。
			//
			HDC create(HWND hwnd, HDC dc)
			{
				// バックバッファが作成済みの場合は
				if (back_dc)
				{
					// 作成済みのバックバッファを返します。
					return back_dc;
				}

				// クライアント矩形を取得します。
				rc = my::get_client_rect(hwnd);
				auto w = my::get_width(rc);
				auto h = my::get_height(rc);

				// バックバッファを作成します。
				front_dc = dc;
				back_dc = ::CreateCompatibleDC(front_dc);
				bitmap = ::CreateCompatibleBitmap(front_dc, w, h);
				old_bitmap = (HBITMAP)::SelectObject(back_dc, bitmap);

				// バックバッファを返します。
				return back_dc;
			}

			//
			// バックバッファを削除します。
			//
			BOOL destroy()
			{
				::SelectObject(back_dc, old_bitmap), old_bitmap = 0;
				::DeleteObject(bitmap), bitmap = 0;
				::DeleteDC(back_dc), back_dc = 0;

				return TRUE;
			}

			//
			// バックバッファをフロントバッファに転送します。
			//
			BOOL swap()
			{
				auto w = my::get_width(rc);
				auto h = my::get_height(rc);

				return ::BitBlt(front_dc, rc.left, rc.top, w, h, back_dc, 0, 0, SRCCOPY);
			}
		};

		//
		// 指定されたウィンドウに関連付けられているノードを返します。
		// 内部的に使用されます。
		//
		inline static Node* get_node(HWND hwnd)
		{
			return (Node*)::GetProp(hwnd, c_prop_name);
		}

		//
		// 指定されたウィンドウにノードを関連付けます。
		// 内部的に使用されます。
		//
		inline static BOOL set_node(HWND hwnd, Node* node)
		{
			return ::SetProp(hwnd, c_prop_name, node);
		}

		//
		// ダブルバッファリングに必要なノードを作成します。
		// ウィンドウ作成時に呼び出してください。
		//
		inline static BOOL create(HWND hwnd)
		{
			auto node = get_node(hwnd);
			if (node) return FALSE;
			node = new Node();
			return set_node(hwnd, node);
		}

		//
		// ノードを削除します。
		// ウィンドウ破壊時に呼び出してください。
		//
		inline static BOOL destroy(HWND hwnd)
		{
			auto node = get_node(hwnd);
			if (!node) return FALSE;
			delete node;
			return set_node(hwnd, 0);
		}

		//
		// 指定されウィンドウのバックバッファを返します。
		//
		inline static HDC get_back_buffer(HWND hwnd)
		{
			auto node = get_node(hwnd);
			if (!node) return 0;
			return node->get_back_buffer();
		}

		//
		// 指定されウィンドウのバックバッファを作成します。
		//
		inline static HDC create_back_buffer(HWND hwnd, HDC dc)
		{
			auto node = get_node(hwnd);
			if (!node) return 0;
			return node->create(hwnd, dc);
		}

		//
		// 指定されウィンドウのバックバッファを
		// フロントバッファに転送してから削除します。
		//
		inline static BOOL swap(HWND hwnd)
		{
			auto node = get_node(hwnd);
			if (!node) return FALSE;
			node->swap();
			return node->destroy();
		}
	} double_buffer;
}
