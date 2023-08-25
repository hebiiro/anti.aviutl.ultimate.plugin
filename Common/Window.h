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

			::DestroyWindow(hwnd), hwnd = 0;

			return TRUE;
		}

		//
		// ウィンドウをサブクラス化します。
		//
		virtual BOOL subclass(HWND hwnd)
		{
			this->hwnd = hwnd;
			origWndProc = (WNDPROC)::SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG)wndProc);
			return setPointer(hwnd, this);
		}

		//
		// ウィンドウのサブクラス化を解除します。
		//
		virtual BOOL unsubclass()
		{
			HWND hwnd = this->hwnd;
			if (!hwnd) return FALSE;
			this->hwnd = 0;
			::SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG)origWndProc);
			return setPointer(hwnd, 0);
		}

		//
		// 仮想関数版のウィンドウプロシージャです。
		// サブクラスでこの関数をオーバーライドして処理してください。
		//
		virtual LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			return ::CallWindowProc(origWndProc, hwnd, message, wParam, lParam);
		}

		//
		// HWND に関連付けられた Window* を返します。
		// HWND から Window* を取得する必要がある場合に使用してください。
		//
		inline static Window* getPointer(HWND hwnd)
		{
			return (Window*)::GetProp(hwnd, PropName);
		}

		//
		// HWND と Window* を関連付けます。
		// 内部的に使用されます。
		//
		inline static BOOL setPointer(HWND hwnd, Window* window)
		{
			return ::SetProp(hwnd, PropName, window);
		}

		//
		// ウィンドウプロシージャです。
		// 内部的に使用されます。
		//
		inline static LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			Window* window = getPointer(hwnd);
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
				MY_TRACE(_T("hookProc(%d, 0x%08X, 0x%08X)\n"), code, wParam, lParam);

				if (code == HCBT_CREATEWND)
				{
					associator.subclass((HWND)wParam);
					associator.stop();
				}

				return ::CallNextHookEx(0, code, wParam, lParam);
			}
		} associator = {};

		//
		// ウィンドウプロパティの名前です。
		// ウィンドウハンドルとポインタを結びつけるために使用されます。
		//
		inline static const LPCTSTR PropName = _T("Window");

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
