#pragma once

namespace fgo
{
	//
	// このクラスはサーヴァントを管理します。
	//
	inline struct Universe
	{
		//
		// サーヴァントのコレクションです。
		//
		std::unordered_map<std::wstring, Servant*> servants;

		//
		// 指定された名前のサーヴァントを返します。
		//
		template<class T>
		T* get_servant(LPCWSTR name) const
		{
			try {
				return static_cast<T*>(servants.at(name));
			} catch (std::out_of_range&) {
				return 0;
			}
		}

		//
		// 指定されたサーヴァントをコレクションに追加します。
		//
		BOOL add_servant(Servant* servant)
		{
			if (!servant) return FALSE;
			servants[servant->get_servant_name()] = servant;
			return TRUE;
		}

		//
		// 指定されたサーヴァントをコレクションから削除します。
		//
		BOOL erase_servant(Servant* servant)
		{
			if (!servant) return FALSE;
			servants.erase(servant->get_servant_name());
			return TRUE;
		}

		//
		// サーヴァントに初期化を実行させます。
		// 内部的に使用されます。
		//
		BOOL fire_init()
		{
			for (const auto& pair : servants)
				pair.second->on_init();

			return TRUE;
		}

		//
		// サーヴァントに後始末を実行させます。
		// 内部的に使用されます。
		//
		BOOL fire_exit()
		{
			for (const auto& pair : servants)
				pair.second->on_exit();

			return TRUE;
		}

		//
		// サーヴァントにウィンドウの初期化を実行させます。
		// 内部的に使用されます。
		//
		BOOL fire_window_init(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
		{
			BOOL result = FALSE;
			for (const auto& pair : servants)
				result |= pair.second->on_window_init(hwnd, message, wParam, lParam, editp, fp);
			return result;
		}

		//
		// サーヴァントにウィンドウの後始末を実行させます。
		// 内部的に使用されます。
		//
		BOOL fire_window_exit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
		{
			BOOL result = FALSE;
			for (const auto& pair : servants)
				result |= pair.second->on_window_exit(hwnd, message, wParam, lParam, editp, fp);
			return result;
		}

		//
		// サーヴァントにウィンドウのコマンドを実行させます。
		// 内部的に使用されます。
		//
		BOOL fire_window_command(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
		{
			BOOL result = FALSE;
			for (const auto& pair : servants)
				result |= pair.second->on_window_command(hwnd, message, wParam, lParam, editp, fp);
			return result;
		}

		//
		// このプロセスで唯一のインスタンスを返します。
		// 内部的に使用されます。
		//
		static Universe* WINAPI get_instance()
		{
			auto get_universe = (Universe* (WINAPI*)())chaldea.get_proc("get_universe");
			if (!get_universe) {
				static Universe universe;
				return &universe;
			}
			return (*get_universe)();
		}
	} &universe = *Universe::get_instance();
}
