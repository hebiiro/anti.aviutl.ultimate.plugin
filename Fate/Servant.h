#pragma once

namespace fgo
{
	//
	// このクラスはサーヴァントのスーパークラスです。
	//
	struct Servant
	{
		//
		// このクラスは関数の状態です。
		//
		struct ProcState {
			BOOL is_editing;
			BOOL is_saving;
			BOOL is_playing;
		};

		//
		// サーヴァントの識別名を返します。
		//
		virtual LPCWSTR get_servant_name() = 0;

		//
		// サーヴァントの表示名を返します。
		//
		virtual LPCWSTR get_servant_display_name() = 0;

		//
		// DLLの初期化のタイミングで呼び出されます。
		// DllMain(DLL_PROCESS_ATTACH)を抜けたあとに呼び出されるので安全に初期化ができます。
		//
		virtual BOOL on_dll_init(HINSTANCE instance) { return FALSE; }

		//
		// DLLの後始末のタイミングで呼び出されます。
		// DllMain(DLL_PROCESS_DETACH)に入る前に呼び出されるので安全に初期化ができます。
		// ※現在未実装。
		//
		virtual BOOL on_dll_exit(HINSTANCE instance) { return FALSE; }

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

		//
		// ウィンドウ関数のタイミングで呼び出されます。
		//
		virtual BOOL on_window_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
		{
			return FALSE;
		}

		//
		// 映像フィルタのタイミングで呼び出されます。
		//
		virtual BOOL on_video_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip, const ProcState& proc_state)
		{
			return FALSE;
		}

		//
		// 音声フィルタのタイミングで呼び出されます。
		//
		virtual BOOL on_audio_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip, const ProcState& proc_state)
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
