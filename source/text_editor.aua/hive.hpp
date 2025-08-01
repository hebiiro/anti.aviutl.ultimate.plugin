#pragma once

namespace apn::text_editor
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"text_editor";
		inline static constexpr auto c_display_name = L"テキスト編集";

		//
		// このアドインのインスタンスハンドルです。
		//
		HINSTANCE instance = nullptr;

		//
		// コンフィグのファイル名です。
		//
		std::wstring config_file_name;

		//
		// このアドインのメインウィンドウです。
		//
		HWND main_window = nullptr;

		//
		// aviutlウィンドウです。
		//
		HWND aviutl_window = nullptr;

		//
		// エディットボックスに反映するときの遅延時間です。
		//
		int32_t delay = 100;

		//
		// TRUEの場合はeditsをCTRL+マウスホイールでズームできるようにします。
		//
		bool zoomable = true;

		//
		// このクラスはeditsで使用するフォントです。
		//
		struct Font {
			//
			// フォント名です。
			//
			std::wstring name = L"BIZ UDゴシック";

			//
			// フォントの高さです。
			//
			int32_t height = 16;

			//
			// フォントのピッチ(可変か固定か)です。
			//
			int32_t pitch = 0;

			//
			// フォントのハンドルです。
			//
			my::gdi::unique_ptr<HFONT> handle;
		} font;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_display_name, hwnd, type);
		}
	} hive;
}
