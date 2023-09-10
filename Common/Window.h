#pragma once

namespace Tools
{
	//
	// このクラスは WinAPI のウィンドウを管理します。
	//
	struct Window
	{
		//
		// コンストラクタです。
		//
		Window()
		{
			hwnd = 0;
			origWndProc = 0;
		}

		//
		// デストラクタです。
		//
		virtual ~Window()
		{
			unsubclass();
		}

		//
		// HWND を返します。
		//
		operator HWND() const { return hwnd; }

		//
		// ウィンドウの識別名を返します。
		// 安全にダウンキャストしたいときに使用してください。
		//
		virtual LPCTSTR getWindowId() { return _T(""); }

		//
		// ウィンドウを作成します。
		//
		virtual BOOL create(DWORD exStyle, LPCTSTR className, LPCTSTR windowName, DWORD style,
			int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param)
		{
			if (hwnd) return FALSE; // すでにウィンドウを作成済みの場合は失敗します。

			associator.start(this);
			HWND result = ::CreateWindowEx(exStyle, className, windowName, style, x, y, w, h, parent, menu, instance, param);
			associator.stop();

			return !!result;
		}

		//
		// ウィンドウを破壊します。
		//
		virtual BOOL destroy()
		{
			if (!hwnd) return FALSE;
			return ::DestroyWindow(hwnd);
		}

		//
		// ウィンドウをサブクラス化します。
		//
		virtual BOOL subclass(HWND hwnd)
		{
			if (!associate(hwnd)) return FALSE;
			origWndProc = (WNDPROC)::SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG)wndProc);
			return TRUE;
		}

		//
		// ウィンドウのサブクラス化を解除します。
		//
		virtual BOOL unsubclass()
		{
			if (!hwnd) return FALSE;
			::SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG)origWndProc);
			return dissociate();
		}

		//
		// HWND と Window* の関連付けを作成します。
		//
		BOOL associate(HWND hwnd)
		{
			if (fromHandle(hwnd)) return FALSE;
			mapKeeper = map;
			(*map)[hwnd] = this, this->hwnd = hwnd;
			return TRUE;
		}

		//
		// HWND と Window* の関連付けを解除します。
		//
		BOOL dissociate()
		{
			if (!hwnd) return FALSE;
			map->erase(hwnd), hwnd = 0;
			return TRUE;
		}

		//
		// ウィンドウが破壊されたときの最終処理を行います。
		//
		virtual void postNcDestroy()
		{
			dissociate();
		}

		//
		// 仮想関数版のウィンドウプロシージャです。
		// サブクラスでこの関数をオーバーライドして処理してください。
		//
		virtual LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch (message)
			{
			case WM_NCDESTROY:
				{
					LRESULT lr = ::CallWindowProc(origWndProc, hwnd, message, wParam, lParam);
					postNcDestroy();
					return lr;
				}
			}

			return ::CallWindowProc(origWndProc, hwnd, message, wParam, lParam);
		}

		//
		// HWND に関連付けられた Window* を返します。
		// HWND から Window* を取得する必要がある場合に使用してください。
		//
		inline static Window* fromHandle(HWND hwnd)
		{
			auto it = map->find(hwnd);
			if (it == map->end()) return 0;
			return it->second;
		}

		//
		// HWND に関連付けられた Window* を返します。
		// T 型は getWindowId() が id と同じ値を返すように実装してください。
		// それにより、id で T 型かどうか確認できるようになります。
		//
		template<class T>
		static T* fromHandle(HWND hwnd, LPCTSTR id)
		{
			Window* window = fromHandle(hwnd);
			if (!window) return 0;
			if (_tcscmp(id, window->getWindowId()) != 0) return 0;
			return static_cast<T*>(window);
		}

		//
		// ウィンドウプロシージャです。
		// 内部的に使用されます。
		//
		inline static LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			Window* window = fromHandle(hwnd);
			if (window)
				return window->onWndProc(hwnd, message, wParam, lParam);
			else
				return ::DefWindowProc(hwnd, message, wParam, lParam);
		}

	public:

		//
		// この構造体は HWND と Window* を関連付けます。
		// HWND を生成するタイミングで使用してください。
		//
		thread_local inline static struct Associator
		{
			Window* target; // HWND を生成中の Window オブジェクトです。
			HHOOK hook; // CBT フックのハンドルです。

			//
			// ウィンドウハンドルが生成される関数の直前にこの関数を呼んでください。
			//
			void start(Window* window)
			{
				target = window;
				if (hook) ::UnhookWindowsHookEx(hook);
				hook = ::SetWindowsHookEx(WH_CBT, hookProc, 0, ::GetCurrentThreadId());
			}

			//
			// ウィンドウハンドルが生成される関数の直後にこの関数を呼んでください。
			//
			void stop()
			{
				if (hook) ::UnhookWindowsHookEx(hook);
				hook = 0;
				target = 0;
			}

			//
			// HWND と Window* を双方向に関連付けます。サブクラス化も行います。
			// 内部的に使用されます。
			//
			BOOL subclass(HWND hwnd)
			{
				if (!target) return FALSE;

				return target->subclass(hwnd);
			}

			//
			// CBT フックプロシージャです。
			// 内部的に使用されます。
			//
			inline static LRESULT CALLBACK hookProc(int code, WPARAM wParam, LPARAM lParam)
			{
//				MY_TRACE(_T("hookProc(%d, 0x%08X, 0x%08X)\n"), code, wParam, lParam);

				if (code == HCBT_CREATEWND)
				{
					associator.subclass((HWND)wParam);
					associator.stop();
				}

				return ::CallNextHookEx(0, code, wParam, lParam);
			}
		} associator = {};

		//
		// HWND から Window* を取得するためのマップです。
		//
		using Map = std::map<HWND, Window*>;
		thread_local inline static auto map = std::make_shared<Map>();

		//
		// map が先に消滅しないように参照を保持しておきます。
		//
		std::shared_ptr<Map> mapKeeper;

		//
		// ウィンドウハンドルです。
		//
		HWND hwnd;

		//
		// サブクラス化する前のオリジナルのウィンドウプロシージャです。
		//
		WNDPROC origWndProc;
	};
}
