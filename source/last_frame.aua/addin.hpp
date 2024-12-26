#pragma once

namespace apn::last_frame
{
	//
	// このクラスは最終フレームを自動的に調整します。
	//
	inline struct LastFrame : Addin
	{
		//
		// この仮想関数は、このアドインの識別名が必要なときに呼ばれます。
		//
		virtual LPCWSTR get_addin_name() override
		{
			return hive.c_name;
		}

		//
		// この仮想関数は、このアドインの表示名が必要なときに呼ばれます。
		//
		virtual LPCWSTR get_addin_display_name() override
		{
			return hive.c_display_name;
		}

		//
		// この仮想関数は、ウィンドウの初期化を実行するときに呼ばれます。
		//
		virtual BOOL on_window_init(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			if (!config_io.init()) return FALSE;
			if (!addin_window.init()) return FALSE;

			if (!config_io.read()) MY_TRACE("コンフィグの読み込みに失敗しました\n");

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウの後始末を実行するときに呼ばれます。
		//
		virtual BOOL on_window_exit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			config_io.write();

			addin_window.exit();
			config_io.exit();

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウコマンドを実行するときに呼ばれます。
		//
		virtual BOOL on_window_command(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			switch (wParam)
			{
			case magi.c_command_id.c_addin.c_command:
				{
					MY_TRACE_FUNC("magi.c_command_id.c_addin.c_command");

					// アドインウィンドウを表示します。
					if (::IsWindow(addin_window)) addin_window.show();

					break;
				}
			}

			return FALSE;
		}

		//
		// この仮想関数は、映像フィルタを実行するときに呼び出されます。
		//
		virtual BOOL on_video_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip, const ProcState& proc_state) override
		{
			// 現在の呼び出し状態をチェックします。
			if (proc_state.is_saving || proc_state.is_playing)
				return FALSE; // 保存や再生中のときは何もしません。

			// 最終フレームを調整します。
			adjust_last_frame(fp, fpip);

			return FALSE;
		}

		//
		// 最終フレームを調整します。
		//
		BOOL adjust_last_frame(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip)
		{
			if (!hive.enabled)
				return FALSE; // `最終フレームを自動調整する`にチェックが入っていない場合は何もしません。

			if (magi.exin.get_exedit_frame_number() == 0)
				return FALSE; // 拡張編集の最終フレーム番号が無効の場合は何もしません。

			// 現在編集中のシーンのインデックスを取得します。
			auto scene = magi.exin.get_current_scene_index();

			// 現在編集中のシーンの中で最も後ろにあるオブジェクト位置を取得します。
			auto frame_end_number = -1000;
			{
				// オブジェクトの個数を取得します。
				auto c = magi.exin.get_sorted_object_count();

				for (decltype(c) i = 0; i < c; i++)
				{
					// オブジェクトを取得します。
					auto object = magi.exin.get_sorted_object(i);

					if (scene != object->scene_set)
						continue; // 現在のシーン内のオブジェクトではない場合は無視します。

					frame_end_number = std::max<int32_t>(frame_end_number, object->frame_end);
				}
			}

			// 最終フレーム位置を取得します。
			auto frame_max_number = fp->exfunc->get_frame_n(fpip->editp);

			// 最終オブジェクト位置が小さすぎる場合は何もしません。
			if (frame_end_number <= 0)
				return FALSE;

			// 最終オブジェクト位置が最終フレーム位置より大きい場合は何もしません。
			if (frame_end_number + 1 >= frame_max_number)
				return FALSE;

			// 拡張編集ウィンドウを取得します。
			auto exedit_window = magi.exin.get_exedit_window();

			if (!exedit_window)
				return FALSE;

			// 「最後のオブジェクト位置を最終フレーム」コマンドを送信します。
			::PostMessage(exedit_window, WM_COMMAND, 1097, 0);

			return TRUE;
		}
	} addin;
}
