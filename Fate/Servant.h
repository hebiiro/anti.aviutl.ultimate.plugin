#pragma once

namespace fgo
{
	//
	// このクラスはサーヴァントのスーパークラスです。
	//
	struct Servant
	{
		//
		// サーヴァントの識別名を返します。
		//
		virtual LPCWSTR get_servant_name() = 0;

		//
		// サーヴァントの表示名を返します。
		//
		virtual LPCWSTR get_servant_display_name() = 0;

		//
		// 初期化のタイミングで呼び出されます。
		//
		virtual BOOL on_init() = 0;

		//
		// 後始末のタイミングで呼び出されます。
		//
		virtual BOOL on_exit() = 0;

		//
		// ウィンドウの初期化のタイミングで呼び出されます。
		//
		virtual BOOL on_window_init(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
		{
			return FALSE;
		}

		//
		// ウィンドウの後始末のタイミングで呼び出されます。
		//
		virtual BOOL on_window_exit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
		{
			return FALSE;
		}

		//
		// ウィンドウのコマンドのタイミングで呼び出されます。
		//
		virtual BOOL on_window_command(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
		{
			return FALSE;
		}

		using addr_t = ULONG_PTR;

		//
		// 指定された名前に関連付けられた何らかのアドレスを要求されたときに呼び出されます。
		//
		virtual addr_t on_get_address(LPCWSTR name)
		{
			return 0;
		}

		//
		// 型変換付きのon_get_address()です。
		//
		template<class T>
		inline void on_get_address(LPCWSTR name, T& t)
		{
			t = (T)on_get_address(name);
		}
	};
}
