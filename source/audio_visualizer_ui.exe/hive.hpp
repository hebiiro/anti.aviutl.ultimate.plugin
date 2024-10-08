#pragma once

namespace apn::audio_visualizer::ui
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"audio_visualizer_ui";
		inline static constexpr auto c_display_name = L"音声視覚化(外部)";

		inline static constexpr struct Message {
			inline static constexpr uint32_t c_analyze_audio = WM_APP + 1;
			inline static constexpr uint32_t c_select_scheme = WM_APP + 2;
			inline static constexpr uint32_t c_show_visual_editor = WM_APP + 3;
			inline static constexpr uint32_t c_notify_change_preference = WM_APP + 4;
		} c_message;

		//
		// このモジュールのインスタンスハンドルです。
		//
		HINSTANCE instance = nullptr;

		//
		// コンフィグのファイル名です。
		//
		std::wstring config_file_name;

		//
		// このモジュールのメインウィンドウです。
		//
		HWND main_window = nullptr;

		//
		// ホストプロセスのウィンドウです。
		//
		HWND host_window = nullptr;

		//
		// ホストプロセスのハンドルです。
		//
		my::handle::unique_ptr<> host_process;

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
