#pragma once

namespace Tools
{
	//
	// このクラスは複数のウィンドウを管理します。
	//
	template<class T, const LPCTSTR& PropName>
	struct WindowManager
	{
		//
		// このクラスはコレクションの要素です。
		//
		struct Node {
			T window;
			WNDPROC orig;
		};

		//
		// ウィンドウのコレクションです。
		//
		std::map<HWND, Node> collection;

		//
		// コンストラクタです。
		//
		WindowManager()
		{
		}

		//
		// デストラクタです。
		//
		virtual ~WindowManager()
		{
		}

		//
		// ウィンドウの識別名を返します。
		// 安全にダウンキャストしたいときに使用してください。
		//
		virtual LPCTSTR getWindowManagerId() { return _T(""); }

		//
		// ウィンドウを追加します。※作成はしません。
		//
		virtual BOOL attach(HWND hwnd, const T& window)
		{
			// ウィンドウプロシージャを変更します。
			auto orig = (WNDPROC)::SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG)wndProc);
			if (!orig) return FALSE;

			// プロパティを登録します。
			::SetProp(hwnd, PropName, this);

			// コレクションに追加します。
			collection.emplace(
				std::piecewise_construct,
				std::forward_as_tuple(hwnd),
				std::forward_as_tuple(window, orig));

			return TRUE;
		}

		//
		// ウィンドウを除外します。※破壊はしません。
		//
		virtual BOOL detach(HWND hwnd)
		{
			// コレクションから要素を取得します。
			auto it = collection.find(hwnd);
			if (it == collection.end()) return FALSE;

			// ウィンドウプロシージャを元に戻します。
			::SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG)it->second.orig);

			// コレクションから削除します。
			collection.erase(it);

			// プロパティを削除します。
			::RemoveProp(hwnd, PropName);

			return TRUE;
		}

		//
		// ウィンドウが破壊されたときの最終処理を行います。
		//
		virtual void postNcDestroy(HWND hwnd)
		{
			detach(hwnd);
		}

		//
		// 仮想関数版のウィンドウプロシージャです。
		// 派生クラスでこの関数をオーバーライドして処理してください。
		//
		virtual LRESULT onWndProc(const Node& node, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch (message)
			{
			case WM_NCDESTROY:
				{
					LRESULT lr = ::CallWindowProc(node.orig, hwnd, message, wParam, lParam);
					postNcDestroy(hwnd);
					return lr;
				}
			}

			return ::CallWindowProc(node.orig, hwnd, message, wParam, lParam);
		}

		//
		// HWND に関連付けられた WindowManager* を返します。
		// HWND から WindowManager* を取得する必要がある場合に使用してください。
		//
		inline static WindowManager* fromHandle(HWND hwnd)
		{
			return (WindowManager*)::GetProp(hwnd, PropName);
		}

		//
		// HWND に関連付けられた WindowManager* を返します。
		// WM 型は getWindowManagerId() が id と同じ値を返すように実装してください。
		// それにより、id で WM 型かどうか確認できるようになります。
		//
		template<class WM>
		static WM* fromHandle(HWND hwnd, LPCTSTR id)
		{
			auto windowManager = fromHandle(hwnd);
			if (!windowManager) return 0;
			if (_tcscmp(windowManager->getWindowManagerId(), id) != 0) return 0;
			return static_cast<WM*>(windowManager);
		}

		//
		// ウィンドウプロシージャです。
		// 内部的に使用されます。
		//
		inline static LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			// ウィンドウマネージャーを取得します。
			WindowManager* windowManager = fromHandle(hwnd);
			if (windowManager)
			{
				// コレクションの要素を取得します。
				auto it = windowManager->collection.find(hwnd);
				if (it != windowManager->collection.end())
				{
					// コレクションの要素を第一引数にしてウィンドウプロシージャを実行します。
					return windowManager->onWndProc(
						it->second, hwnd, message, wParam, lParam);
				}
			}

			return ::DefWindowProc(hwnd, message, wParam, lParam);
		}
	};
}
