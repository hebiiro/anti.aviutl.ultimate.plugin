#pragma once

namespace apn
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"ultimate.auf";
		inline static constexpr auto c_display_name = L"アルティメットプラグイン";

		//
		// このクラスはアドイン情報です。
		// アドイン情報はアドインそのものではありません。
		//
		struct Addin
		{
			std::wstring size;
			std::wstring name;
			std::wstring display_name;
			BOOL active = FALSE;
			std::wstring args;
			std::vector<std::wstring> conflicts;

			Addin(
				const std::wstring& size,
				const std::wstring& name,
				const std::wstring& display_name,
				BOOL active,
				const std::wstring& args,
				const std::vector<std::wstring>& conflicts)
				: size(size)
				, name(name)
				, display_name(display_name)
				, active(active)
				, args(args)
				, conflicts(conflicts)
			{
			}
		};

		//
		// このプラグインのインスタンスハンドルです。
		//
		HINSTANCE instance = 0;

		//
		// コンフィグのファイル名です。
		//
		std::wstring config_file_name;

		//
		// Pythonのファイル名です。
		//
		std::wstring python_file_name = L"ultimate/assets/python32/python312.dll";

		//
		// アドイン情報のコレクションです。
		//
		std::vector<std::unique_ptr<Addin>> addins;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = 0, int32_t type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
