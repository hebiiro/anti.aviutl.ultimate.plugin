#pragma once

namespace apn::dialog_size
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"dialog_size";
		inline static constexpr auto c_display_name = L"ダイアログサイズ調整";

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
		// カスタムテンプレートの配列です。
		//
		struct CustomTemplate {
			BOOL active;
			std::string target_template_name;
			std::wstring target_display_name;
		} custom_templates[9] = {
			{ TRUE, "SYSTEM", L"システムの設定" },
			{ TRUE, "ENV_CONFIG", L"拡張編集の環境設定" },
			{ TRUE, "INPUTLIST", L"入力プラグインの優先順位" },
			{ TRUE, "PRIORITYLIST", L"フィルタプラグインの優先順位" },
			{ TRUE, "NEW_FILE", L"新規プロジェクトの作成" },
			{ TRUE, "SAVE_OBJECT", L"エイリアスの作成" },
			{ TRUE, "GRID_CONFIG", L"グリッドの設定" },
			{ TRUE, "SCENE_CONFIG", L"シーンの設定" },
			{ TRUE, "SCRIPT_PARAM", L"スクリプトの設定" },
		};

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
