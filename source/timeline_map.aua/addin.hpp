#pragma once

namespace apn::timeline_map
{
	//
	// このクラスはaviutlにタイムラインの全体図を追加します。
	//
	inline struct timeline_map_t : Addin
	{
		//
		// この仮想関数は、このアドインの識別名が必要なときに呼ばれます。
		//
		virtual LPCWSTR get_addin_name() override
		{
			return model::property.c_name;
		}

		//
		// この仮想関数は、このアドインの表示名が必要なときに呼ばれます。
		//
		virtual LPCWSTR get_addin_display_name() override
		{
			return model::property.c_display_name;
		}

		//
		// この仮想関数は、ウィンドウの初期化を実行するときに呼ばれます。
		//
		virtual BOOL on_window_init(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			// アプリケーションの初期化を実行します。
			return controller::app.init();
		}

		//
		// この仮想関数は、ウィンドウの後始末を実行するときに呼ばれます。
		//
		virtual BOOL on_window_exit(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			// アプリケーションの後始末を実行します。
			return controller::app.exit();
		}

		//
		// この仮想関数は、ウィンドウコマンドを実行するときに呼ばれます。
		//
		virtual BOOL on_window_command(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			switch (w_param)
			{
			case magi.c_command_id.c_addin.c_command:
				{
					MY_TRACE_FUNC("magi.c_command_id.c_addin.c_command");

					// 全体図を表示します。
					view::overview.show();

					break;
				}
			}

			return FALSE;
		}

		virtual BOOL on_video_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip, const ProcState& proc_state) override
		{
			MY_TRACE_FUNC("frame = {/}", fpip->frame);

			if (magi.auin.is_playing())
				return FALSE; // 再生中の場合は何もしません。

			if (proc_state.is_saving)
				return FALSE; // 保存中の場合は何もしません。

			return view::overview.redraw();
		}
	} addin;
}
