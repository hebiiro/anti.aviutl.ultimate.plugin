#pragma once

namespace apn::image_export
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"image_export";
		inline static constexpr auto c_display_name = L"画像エクスポート";

		inline static constexpr struct Mode {
			inline static constexpr int32_t c_manual = 0;
			inline static constexpr int32_t c_auto = 1;
			inline static constexpr int32_t c_overwrite = 2;
			inline static constexpr my::Label labels[] = {
				{ c_manual, L"manual" },
				{ c_auto, L"auto" },
				{ c_overwrite, L"overwrite" },
			};
		} c_mode;

		struct App {
			virtual std::wstring browse() = 0;
			virtual BOOL export_image(const std::wstring& file_name, BOOL has_alpha, BOOL sel_item_only) = 0;
		} *app = 0;

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
		// JPEGの出力品質です。
		//
		int32_t quality = 90;

		//
		// 出力モードです。
		//
		int32_t mode = c_mode.c_manual;

		//
		// 出力ファイル名です。
		//
		std::wstring file_name = L"";

		//
		// 出力ファイル名で使用するインデックスです。
		//
		int32_t index = 0;

		//
		// フレーム数やインデックスの文字数です。
		//
		int32_t number_width = 6;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
