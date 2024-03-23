#pragma once

namespace apn::workspace
{
	//
	// このクラスはHive::Appの実装です。
	//
	inline struct App : Hive::App
	{
		//
		// コンストラクタです。
		//
		App()
		{
			hive.app = this;
		}

		//
		// プリファレンスを読み込みます。
		//
		virtual BOOL read_preference() override
		{
			return preference_io.read();
		}

		//
		// プリファレンスを保存します。
		//
		virtual BOOL write_preference() override
		{
			return preference_io.write();
		}

		//
		// ユーザーが指定したファイルからレイアウトをインポートします。
		//
		virtual BOOL import_layout() override
		{
			// ファイル選択ダイアログを表示してファイル名を取得します。

			// Shiftキーが押されている場合はレイアウトだけのインポートではなく、
			// すべてのプリファレンスを完全に読み込みます。
			auto layout_only = ::GetKeyState(VK_SHIFT) >= 0;

			// ユーザーが指定したファイル名が格納されるバッファです。
			std::wstring file_name(MAX_PATH, L'\0');

			// ファイルダイアログを表示します。
			OPENFILENAMEW ofn = { sizeof(ofn) };
			ofn.hwndOwner = hive.main_window;
			ofn.Flags = OFN_FILEMUSTEXIST;
			ofn.lpstrTitle = L"レイアウトのインポート";
			ofn.lpstrFile = file_name.data();
			ofn.nMaxFile = file_name.size();
			ofn.lpstrFilter = L"レイアウトファイル (*.json)\0*.json\0" "すべてのファイル (*.*)\0*.*\0";
			ofn.lpstrDefExt = L"json";
			if (!::GetOpenFileNameW(&ofn))
				return FALSE;

			// レイアウトをインポートします。
			return PreferenceIO(TRUE).read_file(file_name, hive);
		}

		//
		// ユーザーが指定したファイルにレイアウトをエクスポートします。
		//
		virtual BOOL export_layout() override
		{
			// ファイル選択ダイアログを表示してファイル名を取得します。

			// ユーザーが指定したファイル名が格納されるバッファです。
			std::wstring file_name(MAX_PATH, L'\0');

			// ファイルダイアログを表示します。
			OPENFILENAMEW ofn = { sizeof(ofn) };
			ofn.hwndOwner = hive.main_window;
			ofn.Flags = OFN_OVERWRITEPROMPT;
			ofn.lpstrTitle = L"レイアウトのエクスポート";
			ofn.lpstrFile = file_name.data();
			ofn.nMaxFile = file_name.size();
			ofn.lpstrFilter = L"レイアウトファイル (*.json)\0*.json\0" "すべてのファイル (*.*)\0*.*\0";
			ofn.lpstrDefExt = L"json";
			if (!::GetOpenFileNameW(&ofn))
				return FALSE;

			// レイアウトをエクスポートします。
			return PreferenceIO(TRUE).write_file(file_name, hive);
		}
	} app;
}
