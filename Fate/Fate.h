#pragma once

namespace fgo
{
	//
	// このクラスはサーヴァントを管理します。
	//
	inline struct Fate
	{
		//
		// コレクションの要素です。
		//
		struct Node {
			HINSTANCE instance;
			Servant* servant;
		};

		//
		// サーヴァントのコレクションです。
		//
		struct {
			std::unordered_map<std::wstring, Node> map;
			std::vector<Servant*> vector;
		} servants;

		//
		// 指定されたサーヴァントをコレクションに追加します。
		//
		BOOL add_servant(HINSTANCE instance, Servant* servant)
		{
			if (!servant) return FALSE;
			servants.map.emplace(std::piecewise_construct,
				std::forward_as_tuple(servant->get_servant_name()),
				std::forward_as_tuple(instance, servant));
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

		//
		// サーヴァントの数を返します。
		//
		size_t get_servant_count() const
		{
			return servants.vector.size();
		}

		//
		// 指定された位置にあるサーヴァントを返します。
		//
		template<class T>
		T* get_servant(size_t index) const
		{
			if (index >= servants.vector.size()) return 0;
			return static_cast<T*>(servants.vector[index]);
		}

		//
		// 指定された名前のサーヴァントを返します。
		//
		template<class T>
		T* get_servant(LPCWSTR name) const
		{
			auto it = servants.map.find(name);
			if (it == servants.map.end()) return 0;
			return static_cast<T*>(it->second.servant);
		}

		//
		// サーヴァントにDLLの初期化を実行させます。
		// 内部的に使用されます。
		//
		BOOL fire_dll_init()
		{
			for (const auto& pair : servants.map)
				pair.second.servant->on_dll_init(pair.second.instance);

			return TRUE;
		}

		//
		// サーヴァントにDLLの後始末を実行させます。
		// 内部的に使用されます。
		//
		BOOL fire_dll_exit()
		{
			for (const auto& pair : servants.map)
				pair.second.servant->on_dll_exit(pair.second.instance);

			return TRUE;
		}

		//
		// サーヴァントに初期化を実行させます。
		// 内部的に使用されます。
		//
		BOOL fire_init()
		{
			for (const auto& pair : servants.map)
				pair.second.servant->on_init();

			return TRUE;
		}

		//
		// サーヴァントに後始末を実行させます。
		// 内部的に使用されます。
		//
		BOOL fire_exit()
		{
			for (const auto& pair : servants.map)
				pair.second.servant->on_exit();

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
				result |= pair.second.servant->on_window_init(hwnd, message, wParam, lParam, editp, fp);
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
				result |= pair.second.servant->on_window_exit(hwnd, message, wParam, lParam, editp, fp);
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
				result |= pair.second.servant->on_window_command(hwnd, message, wParam, lParam, editp, fp);
			return result;
		}

		//
		// サーヴァントにウィンドウ関数を処理させます。
		// 内部的に使用されます。
		//
		BOOL fire_window_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
		{
			BOOL result = FALSE;
			for (const auto& pair : servants.map)
				result |= pair.second.servant->on_window_proc(hwnd, message, wParam, lParam, editp, fp);
			return result;
		}

		//
		// サーヴァントに映像フィルタを処理させます。
		// 内部的に使用されます。
		//
		BOOL fire_video_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip)
		{
			Servant::ProcState proc_state = {};
			proc_state.is_editing = fp->exfunc->is_editing(fpip->editp);
			proc_state.is_saving = fp->exfunc->is_saving(fpip->editp);
			proc_state.is_playing = !!((DWORD)fpip->editp->aviutl_window_info.flag & 0x00040000);

			BOOL result = FALSE;
			for (const auto& pair : servants.map)
				result |= pair.second.servant->on_video_proc(fp, fpip, proc_state);
			return result;
		}

		//
		// サーヴァントに音声フィルタを処理させます。
		// 内部的に使用されます。
		//
		BOOL fire_audio_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip)
		{
			Servant::ProcState proc_state = {};
			proc_state.is_editing = fp->exfunc->is_editing(fpip->editp);
			proc_state.is_saving = fp->exfunc->is_saving(fpip->editp);
			proc_state.is_playing = !!((DWORD)fpip->editp->aviutl_window_info.flag & 0x00040000);

			BOOL result = FALSE;
			for (const auto& pair : servants.map)
				result |= pair.second.servant->on_audio_proc(fp, fpip, proc_state);
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
