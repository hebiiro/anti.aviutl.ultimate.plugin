#pragma once

namespace fgo
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Magi
	{
		struct CommandID {
			struct Plugin {
				static const UINT ID_ADDIN					= 1;
				static const UINT ID_ADDIN_BEGIN			= 1000;
				static const UINT ID_ADDIN_END				= 2000;
			};
			struct SettingDialog {
				static const UINT ID_CREATE_CLONE			= 12020;
				static const UINT ID_CREATE_SAME_ABOVE		= 12021;
				static const UINT ID_CREATE_SAME_BELOW		= 12022;

				static const UINT ID_CUT_FILTER				= 12023;
				static const UINT ID_CUT_FILTER_ABOVE		= 12024;
				static const UINT ID_CUT_FILTER_BELOW		= 12025;
				static const UINT ID_COPY_FILTER			= 12026;
				static const UINT ID_COPY_FILTER_ABOVE		= 12027;
				static const UINT ID_COPY_FILTER_BELOW		= 12028;
				static const UINT ID_PASTE_FILTER			= 12029;
			};
		};

		HINSTANCE instance = 0; // Ultimate.aufのインスタンスハンドルです。
		AviUtl::FilterPlugin* fp = 0; // フィルタプラグインのポインタです。
		AviUtlInternal auin; // AviUtlや拡張編集の機能にアクセスするためのオブジェクトです。

		//
		// 指定されたコンフィグファイル名をフルパスにして返します。
		//
		std::wstring getConfigFileName(LPCWSTR fileName) const
		{
			WCHAR path[MAX_PATH] = {};
			::GetModuleFileNameW(instance, path, std::size(path));
			::PathRemoveExtensionW(path);
			::StringCchCatW(path, std::size(path), L"Config");
			::PathAppendW(path, fileName);
			return path;
		}

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
			auto get_magi = (Magi* (WINAPI*)())chaldea.get_proc("get_magi");
			if (!get_magi) {
				static Magi magi;
				return &magi;
			}
			return (*get_magi)();
		}
	} &magi = *Magi::get_instance();
}
