#pragma once

namespace apn
{
	//
	// このクラスはプロセス全体から共通して使用される変数を保持します。
	//
	inline struct Magi
	{
		struct CommandID {
			struct Addin {
				static const UINT c_command					= 1;
				static const UINT c_begin					= 1000;
				static const UINT c_end						= 2000;
			};
			struct SettingDialog {
				static const UINT c_create_clone			= 12020;
				static const UINT c_create_same_above		= 12021;
				static const UINT c_create_same_below		= 12022;

				static const UINT c_cut_filter				= 12023;
				static const UINT c_cut_filter_above		= 12024;
				static const UINT c_cut_filter_below		= 12025;
				static const UINT c_copy_filter				= 12026;
				static const UINT c_copy_filter_above		= 12027;
				static const UINT c_copy_filter_below		= 12028;
				static const UINT c_paste_filter			= 12029;
			};
		};

		HINSTANCE instance = 0; // ultimate.aufのインスタンスハンドルです。
		AviUtl::FilterPlugin* fp = 0; // フィルタプラグインのポインタです。
		my::AviUtlInternal auin; // AviUtlにアクセスするためのオブジェクトです。
		my::ExEditInternal exin; // 拡張編集にアクセスするためのオブジェクトです。

		//
		// 指定されたアドインのフルパスを返します。
		//
		std::wstring get_addin_file_name(const std::wstring& name) const
		{
			auto module_file_name = my::get_module_file_name(this->instance);

			return std::wstring(module_file_name.parent_path() / module_file_name.stem() / L"module"s / name) + L".aua"s;
		}

		//
		// 指定されたモジュールに対応するコンフィグのフルパスを返します。
		//
		std::wstring get_config_file_name(HINSTANCE instance)
		{
			auto module_file_name = my::get_module_file_name(this->instance);
			auto name = my::get_module_file_name(instance).filename();

			return std::wstring(module_file_name.parent_path() / module_file_name.stem() / L"config"s / name) + L".json"s;
		}

		//
		// 指定されたコンフィグのフルパスを返します。
		//
		std::wstring get_config_file_name(const std::wstring& name) const
		{
			auto module_file_name = my::get_module_file_name(this->instance);

			return std::wstring(module_file_name.parent_path() / module_file_name.stem() / L"config"s / name);
		}

		//
		// 指定されたアセットのフルパスを返します。
		//
		std::wstring get_assets_file_name(const std::wstring& name) const
		{
			auto module_file_name = my::get_module_file_name(this->instance);

			return std::wstring(module_file_name.parent_path() / module_file_name.stem() / L"assets"s / name);
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
