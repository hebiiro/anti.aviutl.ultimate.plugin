#pragma once

namespace my
{
	//
	// このクラスはWin32のウィンドウです。
	//
	template <typename T>
	struct WindowT
	{
		//
		// コンストラクタです。
		//
		WindowT() : hwnd(nullptr)
		{
		}

		//
		// デストラクタです。
		//
		virtual ~WindowT()
		{
			unsubclass();
		}

		//
		// ウィンドウハンドルを返します。
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

			associator.start(this);
			auto result = ::CreateWindowEx(ex_style, class_name, window_name, style, x, y, w, h, parent, menu, instance, param);
			associator.stop();

			return !!result;
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
//					MY_TRACE_FUNC("{/}, {/}", my::hwnd_to_string(hwnd), my::message_to_string(message));

					post_nc_destroy();

					break;
				}
			}

			return ::DefSubclassProc(hwnd, message, wParam, lParam);
		}

		//
		// HWNDに関連付けられたWindowT*を返します。
		// HWNDからWindowT*を取得する必要がある場合に使用してください。
		//
		inline static WindowT* from_handle(HWND hwnd)
		{
			DWORD_PTR ref_data = 0;
			::GetWindowSubclass(hwnd, subclass_proc, get_subclass_id(), &ref_data);
			return (WindowT*)ref_data;
		}

		//
		// HWNDに関連付けられたWindowT*を返します。
		// T型はget_window_id()がidと同じ値を返すように実装してください。
		// それにより、idでT型かどうか確認できるようになります。
		//
		template <typename T>
		static T* from_handle(HWND hwnd, LPCTSTR id)
		{
			auto window = from_handle(hwnd);
			if (!window) return nullptr;
			if (::lstrcmp(id, window->get_window_id()) != 0) return nullptr;
			return static_cast<T*>(window);
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

			auto window = (WindowT*)ref_data;
			return window->on_wnd_proc(hwnd, message, wParam, lParam);
		}

	public:

		//
		// このクラスはHWNDとWindowT*を関連付けます。
		// HWNDを生成するタイミングで使用してください。
		//
		thread_local inline static struct Associator
		{
			WindowT* target; // HWNDを生成中のWindowTオブジェクトです。
			HHOOK hook; // CBTフックのハンドルです。

			//
			// ウィンドウハンドルが生成される関数の直前にこの関数を呼んでください。
			//
			void start(WindowT* window)
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
			// HWNDとWindowT*を双方向に関連付けます。サブクラス化も行います。
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
			Hooker(WindowT& window) { associator.start(&window); }
			~Hooker() { associator.stop(); }
		};

		//
		// サブクラスID用の変数です。
		// 内部的に使用されます。
		//
		inline static UINT c_subclass_id_placement = 0;

		//
		// サブクラスIDを返します。
		// 内部的に使用されます。
		//
		inline static UINT_PTR get_subclass_id() { return (UINT_PTR)&c_subclass_id_placement; }

		//
		// ウィンドウハンドルです。
		//
		HWND hwnd;
	};

	//
	// このクラスはWindowTの実体です。
	// WindowT<Window>::c_subclass_id_placementへのポインタがサブクラスIDになります。
	// 重複してサブクラス化する場合はstruct Hoge : WindowT<Hoge>のように別のクラスとして実体化してください。
	//
	struct Window : WindowT<Window>
	{
	};
}
