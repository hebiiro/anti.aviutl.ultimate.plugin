#pragma once

namespace apn::scene_select
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"scene_select";
		inline static constexpr auto c_display_name = L"シーン選択";

		inline static constexpr struct CommandID {
			inline static constexpr uint32_t c_config = 2024;
			inline static constexpr uint32_t c_scene_config = 2025;
		} c_command_id;

		inline static constexpr struct SizeMode {
			inline static constexpr int32_t c_resizable = 0;
			inline static constexpr int32_t c_nonresizable = 1;
			inline static constexpr my::Label labels[] = {
				{ c_resizable, L"c_resizable" },
				{ c_nonresizable, L"c_nonresizable" },
			};
		} c_size_mode;

		inline static constexpr struct LayoutMode {
			inline static constexpr int32_t c_horz = 0;
			inline static constexpr int32_t c_vert = 1;
			inline static constexpr my::Label labels[] = {
				{ c_horz, L"c_horz" },
				{ c_vert, L"c_vert" },
			};
		} c_layout_mode;

		inline static constexpr int32_t c_max_scene_count = 50;

		//
		// このアドインのインスタンスハンドルです。
		//
		HINSTANCE instance = 0;

		//
		// コンフィグのファイル名です。
		//
		std::wstring config_file_name;

		//
		// このアドインのメインウィンドウです。
		//
		HWND main_window = 0;

		//
		// ウィンドウのサイズ変更モードです。
		//
		int32_t size_mode = c_size_mode.c_resizable;

		//
		// ボタンを並べる方向です。
		//
		int32_t layout_mode = c_layout_mode.c_horz;

		//
		// ボタンを折り返す数です。
		//
		int32_t wrap = 10;

		//
		// 表示するボタンの総数です。
		//
		int32_t button_count = c_max_scene_count;

		//
		// ボタン1個のサイズです。
		//
		SIZE button_size = { 80, 24 };

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = 0, int32_t type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
