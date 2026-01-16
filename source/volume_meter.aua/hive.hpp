#pragma once

namespace apn::volume_meter
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct hive_t
	{
		inline static constexpr auto c_name = L"volume_meter";
		inline static constexpr auto c_display_name = L"音量メーター";

		inline static constexpr struct message_t {
			inline static constexpr uint32_t c_post_init = WM_APP + 1;
			inline static constexpr uint32_t c_set_paint_option = WM_APP + 2;
			inline static constexpr uint32_t c_set_raw_audio_data = WM_APP + 3;
		} c_message;

		inline static constexpr struct mode_t {
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
		// コンフィグのファイル名です。
		//
		std::wstring config_file_name;

		//
		// 音声処理のモードです。
		//
		int32_t mode = c_mode.c_on;

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
