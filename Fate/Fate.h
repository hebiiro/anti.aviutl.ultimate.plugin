#pragma once

namespace fgo
{
	//
	// このクラスはサーヴァントを管理します。
	//
	inline struct Fate
	{
		//
		// サーヴァントのコレクションです。
		//
		struct {
			std::unordered_map<std::wstring, Servant*> map;
			std::vector<Servant*> vector;
		} servants;

		//
		// 指定されたサーヴァントをコレクションに追加します。
		//
		BOOL add_servant(Servant* servant)
		{
			if (!servant) return FALSE;
			servants.map[servant->get_servant_name()] = servant;
			servants.vector.emplace_back(servant);
			return TRUE;
		}

		//
		// 指定されたサーヴァントをコレクションから削除します。
		//
		BOOL erase_servant(Servant* servant)
		{
			if (!servant) return FALSE;
			servants.map.erase(servant->get_servant_name());
			std::erase(servants.vector, servant);
			return TRUE;
		}

		int get_servant_count() const
		{
			return (int)servants.vector.size();
		}

		//
		// 指定された位置にあるサーヴァントを返します。
		//
		template<class T>
		T* get_servant(int index) const
		{
			try {
				return static_cast<T*>(servants.vector.at(index));
			} catch (std::out_of_range&) {
				return 0;
			}
		}

		//
		// 指定された名前のサーヴァントを返します。
		//
		template<class T>
		T* get_servant(LPCWSTR name) const
		{
			try {
				return static_cast<T*>(servants.map.at(name));
			} catch (std::out_of_range&) {
				return 0;
			}
		}

		//
		// サーヴァントに初期化を実行させます。
		// 内部的に使用されます。
		//
		BOOL fire_init()
		{
			for (const auto& pair : servants.map)
				pair.second->on_init();

			return TRUE;
		}

		//
		// サーヴァントに後始末を実行させます。
		// 内部的に使用されます。
		//
		BOOL fire_exit()
		{
			for (const auto& pair : servants.map)
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
			for (const auto& pair : servants.map)
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
			for (const auto& pair : servants.map)
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
			for (const auto& pair : servants.map)
				result |= pair.second->on_window_command(hwnd, message, wParam, lParam, editp, fp);
			return result;
		}

		//
		// このプロセスで唯一のインスタンスを返します。
		// 内部的に使用されます。
		//
		static Fate* WINAPI get_instance()
		{
			auto get_fate = (Fate* (WINAPI*)())chaldea.get_proc("get_fate");
			if (!get_fate) {
				static Fate fate;
				return &fate;
			}
			return (*get_fate)();
		}
	} &fate = *Fate::get_instance();
}
