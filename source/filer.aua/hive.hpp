#pragma once

namespace apn::filer
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"filer";
		inline static constexpr auto c_display_name = L"ファイラ";

		struct App {
			virtual BOOL init() = 0;
			virtual BOOL exit() = 0;
			virtual BOOL command() = 0;
			virtual BOOL refresh() = 0;
			virtual BOOL on_create_filer() = 0;
			virtual BOOL on_destroy_filer(size_t index) = 0;
			virtual BOOL on_edit_filer(size_t index) = 0;
		} *app = nullptr;

		//
		// このアドインのインスタンスハンドルです。
		//
		HINSTANCE instance = nullptr;

		//
		// コンフィグのファイル名です。
		//
		std::wstring config_file_name;

		//
		// このアドインのウィンドウハンドルです。
		//
		HWND addin_window = nullptr;

		//
		// 現在アクティブなファイラウィンドウです。
		//
		HWND active_window = nullptr;

		//
		// TRUEの場合はコモンダイアログ(ファイル選択ダイアログ)を使用するようにします。
		//
		BOOL use_common_dialog = TRUE;

		//
		// ワークスペース化アドインのエクスポート関数です。
		//
		workspace::share::Exports workspace;

		//
		// クライアントウィンドウにメッセージをポストします。
		//
		template <typename WP, typename LP>
		inline BOOL post_message(UINT message, WP wParam, LP lParam) {
			auto client_window = share::host_window::get_client_window(addin_window);
			return ::PostMessage(client_window, message, (WPARAM)wParam, (LPARAM)lParam);
		}

		//
		// クライアントウィンドウにメッセージをセンドします。
		//
		template <typename WP, typename LP>
		inline LRESULT send_message(UINT message, WP wParam, LP lParam) {
			auto client_window = share::host_window::get_client_window(addin_window);
			return ::SendMessage(client_window, message, (WPARAM)wParam, (LPARAM)lParam);
		}

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
