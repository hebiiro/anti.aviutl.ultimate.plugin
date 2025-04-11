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
			MY_TRACE_FUNC("");

			hive.app = this;
		}

		//
		// プリファレンスを読み込みます。
		//
		virtual BOOL read_preference() override
		{
			MY_TRACE_FUNC("");

			return preference_io.read();
		}

		//
		// プリファレンスを保存します。
		//
		virtual BOOL write_preference() override
		{
			MY_TRACE_FUNC("");

			return preference_io.write();
		}

		//
		// ユーザーが指定したファイルからレイアウトをインポートします。
		//
		virtual BOOL import_layout() override
		{
			MY_TRACE_FUNC("");

			// ファイル選択ダイアログを表示してファイル名を取得します。

			uint32_t flags = 0;

			// Shiftキーが押されている場合は
			if (::GetKeyState(VK_SHIFT) < 0)
			{
				// レイアウトだけのインポートではなく、
				// すべてのプリファレンスを完全に読み込みます。
				flags |= hive.c_preference_flag.c_config;
			}
			else
			{
				// 流動的なレイアウトだけを読み込みます。
//				flags |= hive.c_preference_flag.c_fluid;
			}

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
			return PreferenceLoader(flags).read_file(file_name, hive);
		}

		//
		// ユーザーが指定したファイルにレイアウトをエクスポートします。
		//
		virtual BOOL export_layout() override
		{
			MY_TRACE_FUNC("");

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
			if (!::GetSaveFileNameW(&ofn))
				return FALSE;

			// レイアウトをエクスポートします。
			return PreferenceSaver().write_file(file_name, hive);
		}

		//
		// レイアウトを変更します。
		//
		virtual BOOL change_layout(const std::wstring& file_name) override
		{
			MY_TRACE_FUNC("{/}", file_name);

			return PreferenceLoader(hive.c_preference_flag.c_fluid).read_file(file_name, hive);
		}

		//
		// 再生時最大化を有効/無効化します。
		//
		virtual BOOL enable_fullscreen_preview(BOOL enable) override
		{
			MY_TRACE_FUNC("{/}", enable);

			return hook::local::aviutl.enable_hook(enable);
		}
	} app;
}
