#pragma once

namespace apn::optima
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"optima";
		inline static constexpr auto c_display_name = L"最適化";

		struct Locker {
			Locker() { hive.lock_count++; }
			~Locker() { hive.lock_count--; }
		};

		int32_t lock_count = 0;

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
		// TRUEの場合は設定ダイアログのコンボボックスを再利用します。
		//
		BOOL reuse_combobox = FALSE;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
