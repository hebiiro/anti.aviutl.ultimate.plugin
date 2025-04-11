#pragma once

namespace my
{
	//
	// このクラスはWin32のウィンドウです。
	// HWNDに対してインターセプターは複数存在する場合があるので、
	// HWNDからインターセプターを特定することはできません。
	//
	struct WindowInterceptor
	{
		//
		// コンストラクタです。
		//
		WindowInterceptor() : hwnd(nullptr)
		{
		}

		//
		// デストラクタです。
		//
		virtual ~WindowInterceptor()
		{
			unsubclass();
		}

		//
		// HWNDを返します。
		//
		operator HWND() const { return hwnd; }

		//
		// ウィンドウの識別名を返します。
		// 安全にダウンキャストしたいときに使用してください。
		//
		virtual LPCTSTR get_window_id() { return _T(""); }

		//
		// ウィンドウを作成します。
		//
		virtual BOOL create(DWORD ex_style, LPCTSTR class_name, LPCTSTR window_name, DWORD style,
			int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param)
		{
			if (hwnd) return FALSE; // すでにウィンドウを作成済みの場合は失敗します。

			Hooker hooker(*this);
			return !!::CreateWindowEx(ex_style, class_name, window_name, style, x, y, w, h, parent, menu, instance, param);
		}

		//
		// ウィンドウを破壊します。
		//
		virtual BOOL destroy()
		{
			if (!hwnd) return FALSE;
			auto result = ::DestroyWindow(hwnd);
			hwnd = nullptr;
			return result;
		}

		//
		// ウィンドウをサブクラス化します。
		//
		virtual BOOL subclass(HWND hwnd)
		{
			if (this->hwnd) return FALSE;
			this->hwnd = hwnd;
			return ::SetWindowSubclass(hwnd, subclass_proc, get_subclass_id(), (DWORD_PTR)this);
		}

		//
		// ウィンドウのサブクラス化を解除します。
		//
		virtual BOOL unsubclass()
		{
			if (!hwnd) return FALSE;
			auto result = ::RemoveWindowSubclass(hwnd, subclass_proc, get_subclass_id());
			hwnd = nullptr;
			return result;
		}

		//
		// ウィンドウが破壊されたときの最終処理を行います。
		//
		virtual void post_nc_destroy()
		{
			unsubclass();
		}

		//
		// 仮想関数版のウィンドウプロシージャです。
		// 派生クラスでこの関数をオーバーライドして処理してください。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch (message)
			{
			case WM_NCDESTROY:
				{
					auto lr = ::DefSubclassProc(hwnd, message, wParam, lParam);
					post_nc_destroy();
					return lr;
				}
			}

			return ::DefSubclassProc(hwnd, message, wParam, lParam);
		}

		//
		// サブクラスプロシージャです。
		// 内部的に使用されます。
		//
		inline static LRESULT CALLBACK subclass_proc(
			HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam,
			UINT_PTR id, DWORD_PTR ref_data)
		{
//			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/hex}, {/hex}, {/hex}", hwnd, message, wParam, lParam, id, ref_data);

			auto window = (WindowInterceptor*)ref_data;
			return window->on_wnd_proc(hwnd, message, wParam, lParam);
		}

	public:

		//
		// このクラスはHWNDとWindowInterceptor*を関連付けます。
		// HWNDを生成するタイミングで使用してください。
		//
		thread_local inline static struct Associator
		{
			WindowInterceptor* target; // HWNDを生成中のWindowInterceptorオブジェクトです。
			HHOOK hook; // CBTフックのハンドルです。

			//
			// ウィンドウハンドルが生成される関数の直前にこの関数を呼んでください。
			//
			void start(WindowInterceptor* window)
			{
				target = window;
				if (hook) ::UnhookWindowsHookEx(hook);
				hook = ::SetWindowsHookEx(WH_CBT, hook_proc, nullptr, ::GetCurrentThreadId());
			}

			//
			// ウィンドウハンドルが生成される関数の直後にこの関数を呼んでください。
			//
			void stop()
			{
				if (hook) ::UnhookWindowsHookEx(hook);
				hook = nullptr;
				target = nullptr;
			}

			//
			// HWNDとWindowInterceptor*を双方向に関連付けます。サブクラス化も行います。
			// 内部的に使用されます。
			//
			BOOL subclass(HWND hwnd)
			{
				if (!target) return FALSE;

				return target->subclass(hwnd);
			}

			//
			// CBTフックプロシージャです。
			// 内部的に使用されます。
			//
			inline static LRESULT CALLBACK hook_proc(int code, WPARAM wParam, LPARAM lParam)
			{
//				MY_TRACE_FUNC("{/}, {/hex}, {/hex}", code, wParam, lParam);

				if (code == HCBT_CREATEWND)
				{
					associator.subclass((HWND)wParam);
					associator.stop();
				}

				return ::CallNextHookEx(nullptr, code, wParam, lParam);
			}
		} associator = {};

		//
		// このクラスはウィンドウプロシージャをフックします。
		//
		struct Hooker {
			Hooker(WindowInterceptor& window) { associator.start(&window); }
			~Hooker() { associator.stop(); }
		};

		//
		// サブクラスIDを返します。
		// 内部的に使用されます。
		//
		UINT_PTR get_subclass_id() { return (UINT_PTR)this; }

		//
		// ウィンドウハンドルです。
		//
		HWND hwnd;
	};
}
