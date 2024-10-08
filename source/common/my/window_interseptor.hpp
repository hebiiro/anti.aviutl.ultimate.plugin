﻿#pragma once

namespace my
{
	//
	// このクラスはWin32のウィンドウです。
	// HWNDに対してインターセプターは複数存在する場合があるので、
	// HWNDからインターセプターを特定することはできません。
	//
	struct WindowInterseptor
	{
		//
		// コンストラクタです。
		//
		WindowInterseptor() : hwnd(0)
		{
		}

		//
		// デストラクタです。
		//
		virtual ~WindowInterseptor()
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

			associator.start(this);
			HWND result = ::CreateWindowEx(ex_style, class_name, window_name, style, x, y, w, h, parent, menu, instance, param);
			associator.stop();

			return !!result;
		}

		//
		// ウィンドウを破壊します。
		//
		virtual BOOL destroy()
		{
			if (!hwnd) return FALSE;
			BOOL result = ::DestroyWindow(hwnd);
			hwnd = 0;
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
			BOOL result = ::RemoveWindowSubclass(hwnd, subclass_proc, get_subclass_id());
			hwnd = 0;
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
					LRESULT lr = ::DefSubclassProc(hwnd, message, wParam, lParam);
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
//			MY_TRACE_FUNC("{:#010x}, {:#010x}, {:#010x}, {:#010x}, {:#010x}, {:#010x}", hwnd, message, wParam, lParam, id, ref_data);

			auto window = (WindowInterseptor*)ref_data;
			return window->on_wnd_proc(hwnd, message, wParam, lParam);
		}

	public:

		//
		// この構造体はHWNDとWindowInterseptor*を関連付けます。
		// HWNDを生成するタイミングで使用してください。
		//
		thread_local inline static struct Associator
		{
			WindowInterseptor* target; // HWNDを生成中のWindowInterseptorオブジェクトです。
			HHOOK hook; // CBTフックのハンドルです。

			//
			// ウィンドウハンドルが生成される関数の直前にこの関数を呼んでください。
			//
			void start(WindowInterseptor* window)
			{
				target = window;
				if (hook) ::UnhookWindowsHookEx(hook);
				hook = ::SetWindowsHookEx(WH_CBT, hook_proc, 0, ::GetCurrentThreadId());
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
			// HWNDとWindowInterseptor*を双方向に関連付けます。サブクラス化も行います。
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
//				MY_TRACE_FUNC("{}, {:#010x}, {:#010x}", code, wParam, lParam);

				if (code == HCBT_CREATEWND)
				{
					associator.subclass((HWND)wParam);
					associator.stop();
				}

				return ::CallNextHookEx(0, code, wParam, lParam);
			}
		} associator = {};

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
