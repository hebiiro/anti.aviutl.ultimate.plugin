#pragma once

namespace apn
{
	//
	// このクラスはアドインのインターフェイスです。
	//
	struct Addin
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
		// アドインの識別名を返します。
		//
		virtual LPCWSTR get_addin_name() = 0;

		//
		// アドインの表示名を返します。
		//
		virtual LPCWSTR get_addin_display_name() = 0;

		//
		// DLLの初期化のタイミングで呼び出されます。
		// DllMain(DLL_PROCESS_ATTACH)を抜けたあとに呼び出されるので安全に初期化ができます。
		//
		virtual BOOL on_dll_init(HINSTANCE instance) { return TRUE; }

		//
		// DLLの後始末のタイミングで呼び出されます。
		// DllMain(DLL_PROCESS_DETACH)に入る前に呼び出されるので安全に初期化ができます。
		// ※現在未実装。
		//
		virtual BOOL on_dll_exit(HINSTANCE instance) { return TRUE; }

		//
		// 初期化のタイミングで呼び出されます。
		//
		virtual BOOL on_init() { return TRUE; };

		//
		// 後始末のタイミングで呼び出されます。
		//
		virtual BOOL on_exit() { return TRUE; };

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

		//
		// フィルタの設定が更新されたときに呼び出されます。
		//
		virtual BOOL on_update(AviUtl::FilterPlugin* fp, AviUtl::FilterPlugin::UpdateStatus status)
		{
			return FALSE;
		}

		//
		// 出力を開始するときに呼び出されます。
		//
		virtual BOOL on_save_start(AviUtl::FilterPlugin* fp, int32_t s, int32_t e, AviUtl::EditHandle* editp)
		{
			return FALSE;
		}

		//
		// 出力を終了するときに呼び出されます。
		//
		virtual BOOL on_save_end(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp)
		{
			return FALSE;
		}

		//
		// 不明。
		//
		virtual BOOL on_is_saveframe(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, int32_t saveno, int32_t frame, int32_t fps, int32_t edit_flag, int32_t inter)
		{
			return FALSE;
		}

		//
		// プロジェクトファイルを読み込むときに呼び出されます。
		//
		virtual BOOL on_project_load(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void* data, int32_t size)
		{
			return FALSE;
		}

		//
		// プロジェクトファイルを保存するときに呼び出されます。
		//
		virtual BOOL on_project_save(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void* data, int32_t* size)
		{
			return FALSE;
		}

		//
		// タイトルを変更する必要があるときに呼び出されます。
		//
		virtual BOOL on_modify_title(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, int32_t frame, LPSTR title, int32_t max_title)
		{
			return FALSE;
		}

		//
		// 指定された名前に関連付けられた何らかのアドレスを要求されたときに呼び出されます。
		//
		virtual my::addr_t on_get_address(LPCWSTR name)
		{
			return 0;
		}

		//
		// 型変換付きのon_get_address()です。
		//
		template <typename T>
		inline void on_get_address(LPCWSTR name, T& t)
		{
			t = (T)on_get_address(name);
		}
	};
}
