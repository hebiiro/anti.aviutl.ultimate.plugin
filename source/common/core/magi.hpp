#pragma once

namespace apn
{
	//
	// このクラスはプロセス全体から共通して使用される変数を保持します。
	//
	inline struct Magi
	{
		inline static constexpr struct CommandID {
			inline static constexpr struct Addin {
				inline static constexpr uint32_t c_command = 1;
				inline static constexpr uint32_t c_begin = 1000;
				inline static constexpr uint32_t c_end = 2000;
			} c_addin;
			inline static constexpr struct SettingDialog {
				inline static constexpr uint32_t c_create_clone = 12020;
				inline static constexpr uint32_t c_create_same_above = 12021;
				inline static constexpr uint32_t c_create_same_below = 12022;

				inline static constexpr uint32_t c_cut_filter = 12023;
				inline static constexpr uint32_t c_cut_filter_above = 12024;
				inline static constexpr uint32_t c_cut_filter_below = 12025;
				inline static constexpr uint32_t c_copy_filter = 12026;
				inline static constexpr uint32_t c_copy_filter_above = 12027;
				inline static constexpr uint32_t c_copy_filter_below = 12028;
				inline static constexpr uint32_t c_paste_filter = 12029;
			} c_setting_dialog;
		} c_command_id;

		HINSTANCE instance = nullptr; // ultimate.aufのインスタンスハンドルです。
		AviUtl::FilterPlugin* fp = nullptr; // フィルタプラグインのポインタです。
		my::AviUtlInternal auin; // AviUtlにアクセスするためのオブジェクトです。
		my::ExEditInternal exin; // 拡張編集にアクセスするためのオブジェクトです。

		//
		// カラー選択ダイアログのカスタムカラーです。
		//
		COLORREF custom_colors[16] = {};

		//
		// コンストラクタです。
		//
		Magi()
		{
			MY_TRACE_FUNC("");

			// カスタムカラーを初期化します。
			std::fill(std::begin(custom_colors), std::end(custom_colors), RGB(255, 255, 255));
		}

		//
		// カラー選択ダイアログを表示します。
		//
		COLORREF choose_color(HWND hwnd, COLORREF color)
		{
			MY_TRACE_FUNC("");

			CHOOSECOLOR cc { sizeof(cc) };
			cc.hwndOwner = hwnd;
			cc.lpCustColors = custom_colors;
			cc.rgbResult = color;
			cc.Flags = CC_RGBINIT | CC_FULLOPEN;
			if (!::ChooseColor(&cc))
				throw std::exception("dialog canceled");
			return cc.rgbResult;
		}

		//
		// AviUtlプロセス内の場合はTRUEを返します。
		//
		auto in_aviutl() const
		{
			return !!instance;
		}

		//
		// 指定されたフォルダとファイル名からフルパスを構築して返します。
		//
		std::wstring get_file_name(const std::wstring& folder, const std::wstring& name) const
		{
			auto module_file_name = my::get_module_file_name(instance);

			if (instance)
				// AviUtlプロセス内の場合は
				return module_file_name.parent_path() / module_file_name.stem() / folder / name;
			else
				// AviUtlプロセス外の場合は
				return module_file_name.parent_path().parent_path() / folder / name;
		}

		//
		// 指定されたアドインのフルパスを返します。
		//
		std::wstring get_addin_file_name(const std::wstring& name) const
		{
			return get_module_file_name(name +  L".aua");
		}

		//
		// 指定されたモジュールに対応するコンフィグのフルパスを返します。
		//
		std::wstring get_config_file_name(HINSTANCE instance)
		{
			return get_config_file_name(my::get_module_file_name(instance).filename() += L".json");
		}

		//
		// 指定されたモジュールに対応するデフォルトのコンフィグのフルパスを返します。
		//
		std::wstring get_default_config_file_name(HINSTANCE instance)
		{
			return get_assets_file_name(L"default" / my::get_module_file_name(instance).filename() += L".json");
		}

		//
		// 指定されたコンフィグのフルパスを返します。
		//
		std::wstring get_config_file_name(const std::wstring& name) const
		{
			return get_file_name(L"config", name);
		}

		//
		// 指定されたアセットのフルパスを返します。
		//
		std::wstring get_assets_file_name(const std::wstring& name) const
		{
			return get_file_name(L"assets", name);
		}

		//
		// 指定されたモジュールのフルパスを返します。
		//
		std::wstring get_module_file_name(const std::wstring& name) const
		{
			return get_file_name(L"module", name);
		}

		//
		// 指定された原点にフォルダを作成します。
		//
		BOOL create_folders(const std::filesystem::path& origin, const std::vector<std::filesystem::path>& folders)
		{
			try
			{
				if (folders.empty())
				{
					std::filesystem::create_directories(origin);
				}
				else
				{
					for (const auto& folder : folders)
						std::filesystem::create_directories(origin / folder);
				}

				return TRUE;
			}
			catch (...)
			{
			}

			return FALSE;
		}

		//
		// 指定されたコンフィグフォルダを作成します。
		//
		BOOL create_config_folders(const std::wstring& name, const std::vector<std::filesystem::path>& folders)
		{
			return create_folders(get_config_file_name(name), folders);
		}

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, const std::wstring& title,
			HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING)
		{
			if (!hwnd && magi.fp) hwnd = magi.fp->hwnd;

			return ::MessageBoxW(hwnd, text.c_str(), title.c_str(), type);
		}

		//
		// AviUtlを再描画します。
		//
		void redraw()
		{
			::PostMessage(fp->hwnd, AviUtl::FilterPlugin::WindowMessage::Command, 0, 0);
		}

		//
		// 初期化を実行します。
		// 内部的に使用されます。
		//
		BOOL init(AviUtl::FilterPlugin* fp)
		{
			this->fp = fp;

			return TRUE;
		}

		//
		// 後始末を実行します。
		// 内部的に使用されます。
		//
		BOOL exit()
		{
			return TRUE;
		}

		//
		// このプロセスで唯一のインスタンスを返します。
		// 内部的に使用されます。
		//
		static Magi* WINAPI get_instance()
		{
			auto get_magi = (Magi* (WINAPI*)())module.get_proc("core_get_magi");
			if (!get_magi) {
				static Magi magi;
				return &magi;
			}
			return (*get_magi)();
		}
	} &magi = *Magi::get_instance();
}
