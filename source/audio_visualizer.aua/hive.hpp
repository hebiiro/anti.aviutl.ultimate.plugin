#pragma once

namespace apn::audio_visualizer
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"audio_visualizer";
		inline static constexpr auto c_display_name = L"音声視覚化";

		inline static constexpr struct Message {
			inline static constexpr uint32_t c_receive_option = WM_APP + 1;
			inline static constexpr uint32_t c_receive_audio_sample = WM_APP + 2;
			inline static constexpr uint32_t c_select_scheme = WM_APP + 3;
			inline static constexpr uint32_t c_show_co_window_editor = WM_APP + 4;
			inline static constexpr uint32_t c_notify_change_preference = WM_APP + 5;
		} c_message;

		inline static constexpr struct Mode {
			inline static constexpr int32_t c_off = 0;
			inline static constexpr int32_t c_on = 1;
			inline static constexpr int32_t c_on_without_playing = 2;
			inline static constexpr my::Label labels[] = {
				{ c_off, L"c_off" },
				{ c_on, L"c_on" },
				{ c_on_without_playing, L"c_on_without_playing" },
			};
		} c_mode;

		//
		// このモジュールのインスタンスハンドルです。
		//
		HINSTANCE instance = nullptr;

		//
		// このモジュールのメインウィンドウです。
		//
		HWND main_window = nullptr;

		//
		// 外部プロセスのウィンドウです。
		//
		HWND ui_window = nullptr;

		//
		// コンフィグのファイル名です。
		//
		std::wstring config_file_name;

		//
		// 音声処理のモードです。
		//
		int32_t mode = c_mode.c_on;

		//
		// 音声を視覚化するときの描画オプションです。
		//
		share::Option option;

		//
		// ワークスペース化アドインのエクスポート関数です。
		//
		workspace::share::Exports workspace;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text,
			HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING)
		{
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
