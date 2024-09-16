#pragma once

namespace apn::editbox_tweaker
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"editbox_tweaker";
		inline static constexpr auto c_display_name = L"エディットボックス微調整";

		//
		// このアドインのインスタンスハンドルです。
		//
		HINSTANCE instance = nullptr;

		//
		// コンフィグのファイル名です。
		//
		std::wstring config_file_name;

		//
		// TRUEの場合はUNICODE文字を入力できるようにします。
		//
		bool unicode_input = true;

		//
		// TRUEの場合はCTRL+Aでテキストを全選択します。
		//
		bool ctrl_a = true;

		//
		// TRUEの場合はCTRL+マウスホイールでズームできるようにします。
		// コモンコントロールV6が場合にのみ有効になります。
		//
		bool zoomable = true;

		//
		// 複数行エディットボックスの高さに加算する値です。
		//
		int32_t delta = 0;

		//
		// このクラスは複数行エディットボックスで使用するフォントです。
		//
		struct Font {
			//
			// フォント名です。
			//
			std::wstring name = L"";

			//
			// フォントの高さです。
			//
			int32_t height = 0;

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
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
