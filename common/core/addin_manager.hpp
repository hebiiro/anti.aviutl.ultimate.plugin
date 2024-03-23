#pragma once

namespace apn
{
	//
	// このクラスはアドインを管理します。
	//
	inline struct AddinManager
	{
		//
		// コレクションの要素です。
		//
		struct Node {
			HINSTANCE instance;
			Addin* addin;
		};

		//
		// アドインのコレクションです。
		//
		struct {
			std::unordered_map<std::wstring, Node> map;
			std::vector<Addin*> vector;
		} addins;

		//
		// 指定されたアドインをコレクションに追加します。
		//
		BOOL add_addin(HINSTANCE instance, Addin* addin)
		{
			if (!addin) return FALSE;
			addins.map.emplace(std::piecewise_construct,
				std::forward_as_tuple(addin->get_addin_name()),
				std::forward_as_tuple(instance, addin));
			addins.vector.emplace_back(addin);
			return TRUE;
		}

		//
		// 指定されたアドインをコレクションから削除します。
		//
		BOOL erase_addin(Addin* addin)
		{
			if (!addin) return FALSE;
			addins.map.erase(addin->get_addin_name());
			std::erase(addins.vector, addin);
			return TRUE;
		}

		//
		// アドインの数を返します。
		//
		size_t get_addin_count() const
		{
			return addins.vector.size();
		}

		//
		// 指定された位置にあるアドインを返します。
		//
		template <typename T = Addin>
		T* get_addin(size_t index) const
		{
			if (index >= addins.vector.size()) return 0;
			return static_cast<T*>(addins.vector[index]);
		}

		//
		// 指定された名前のアドインを返します。
		//
		template <typename T = Addin>
		T* get_addin(LPCWSTR name) const
		{
			auto it = addins.map.find(name);
			if (it == addins.map.end()) return 0;
			return static_cast<T*>(it->second.addin);
		}

		//
		// アドインにDLLの初期化を実行させます。
		// 内部的に使用されます。
		//
		BOOL fire_dll_init()
		{
			for (const auto& pair : addins.map)
				pair.second.addin->on_dll_init(pair.second.instance);

			return TRUE;
		}

		//
		// アドインにDLLの後始末を実行させます。
		// 内部的に使用されます。
		//
		BOOL fire_dll_exit()
		{
			for (const auto& pair : addins.map)
				pair.second.addin->on_dll_exit(pair.second.instance);

			return TRUE;
		}

		//
		// アドインに初期化を実行させます。
		// 内部的に使用されます。
		//
		BOOL fire_init()
		{
			for (const auto& pair : addins.map)
				pair.second.addin->on_init();

			return TRUE;
		}

		//
		// アドインに後始末を実行させます。
		// 内部的に使用されます。
		//
		BOOL fire_exit()
		{
			for (const auto& pair : addins.map)
				pair.second.addin->on_exit();

			return TRUE;
		}

		//
		// アドインにウィンドウの初期化を実行させます。
		// 内部的に使用されます。
		//
		BOOL fire_window_init(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
		{
			BOOL result = FALSE;
			for (const auto& pair : addins.map)
				result |= pair.second.addin->on_window_init(hwnd, message, wParam, lParam, editp, fp);
			return result;
		}

		//
		// アドインにウィンドウの後始末を実行させます。
		// 内部的に使用されます。
		//
		BOOL fire_window_exit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
		{
			BOOL result = FALSE;
			for (const auto& pair : addins.map)
				result |= pair.second.addin->on_window_exit(hwnd, message, wParam, lParam, editp, fp);
			return result;
		}

		//
		// アドインにウィンドウのコマンドを実行させます。
		// 内部的に使用されます。
		//
		BOOL fire_window_command(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
		{
			BOOL result = FALSE;
			for (const auto& pair : addins.map)
				result |= pair.second.addin->on_window_command(hwnd, message, wParam, lParam, editp, fp);
			return result;
		}

		//
		// アドインにウィンドウ関数を処理させます。
		// 内部的に使用されます。
		//
		BOOL fire_window_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
		{
			BOOL result = FALSE;
			for (const auto& pair : addins.map)
				result |= pair.second.addin->on_window_proc(hwnd, message, wParam, lParam, editp, fp);
			return result;
		}

		//
		// アドインに映像フィルタを処理させます。
		// 内部的に使用されます。
		//
		BOOL fire_video_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip)
		{
			Addin::ProcState proc_state = {};
			proc_state.is_editing = fp->exfunc->is_editing(fpip->editp);
			proc_state.is_saving = fp->exfunc->is_saving(fpip->editp);
			proc_state.is_playing = !!((DWORD)fpip->editp->aviutl_window_info.flag & 0x00040000);

			BOOL result = FALSE;
			for (const auto& pair : addins.map)
				result |= pair.second.addin->on_video_proc(fp, fpip, proc_state);
			return result;
		}

		//
		// アドインに音声フィルタを処理させます。
		// 内部的に使用されます。
		//
		BOOL fire_audio_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip)
		{
			Addin::ProcState proc_state = {};
			proc_state.is_editing = fp->exfunc->is_editing(fpip->editp);
			proc_state.is_saving = fp->exfunc->is_saving(fpip->editp);
			proc_state.is_playing = !!((DWORD)fpip->editp->aviutl_window_info.flag & 0x00040000);

			BOOL result = FALSE;
			for (const auto& pair : addins.map)
				result |= pair.second.addin->on_audio_proc(fp, fpip, proc_state);
			return result;
		}

		//
		// アドインにフィルタの設定が更新されたことを通知します。
		// 内部的に使用されます。
		//
		BOOL fire_update(AviUtl::FilterPlugin* fp, AviUtl::FilterPlugin::UpdateStatus status)
		{
			BOOL result = FALSE;
			for (const auto& pair : addins.map)
				result |= pair.second.addin->on_update(fp, status);
			return result;
		}

		//
		// アドインに出力の開始を通知します。
		// 内部的に使用されます。
		//
		BOOL fire_save_start(AviUtl::FilterPlugin* fp, int32_t s, int32_t e, AviUtl::EditHandle* editp)
		{
			BOOL result = FALSE;
			for (const auto& pair : addins.map)
				result |= pair.second.addin->on_save_start(fp, s, e, editp);
			return result;
		}

		//
		// アドインに出力の終了を通知します。
		// 内部的に使用されます。
		//
		BOOL fire_save_end(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp)
		{
			BOOL result = FALSE;
			for (const auto& pair : addins.map)
				result |= pair.second.addin->on_save_end(fp, editp);
			return result;
		}

		//
		// 不明。
		//
		BOOL fire_is_saveframe(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, int32_t saveno, int32_t frame, int32_t fps, int32_t edit_flag, int32_t inter)
		{
			BOOL result = FALSE;
			for (const auto& pair : addins.map)
				result |= pair.second.addin->on_is_saveframe(fp, editp, saveno, frame, fps, edit_flag, inter);
			return result;
		}

		//
		// アドインにプロジェクトファイルの読み込みを通知します。
		// 内部的に使用されます。
		//
		BOOL fire_project_load(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void* data, int32_t size)
		{
			BOOL result = FALSE;
			for (const auto& pair : addins.map)
				result |= pair.second.addin->on_project_load(fp, editp, data, size);
			return result;
		}

		//
		// アドインにプロジェクトファイルの保存を通知します。
		// 内部的に使用されます。
		//
		BOOL fire_project_save(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void* data, int32_t* size)
		{
			BOOL result = FALSE;
			for (const auto& pair : addins.map)
				result |= pair.second.addin->on_project_save(fp, editp, data, size);
			return result;
		}

		//
		// アドインにタイトルを変更させます。
		// 内部的に使用されます。
		//
		BOOL fire_modify_title(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, int32_t frame, LPSTR title, int32_t max_title)
		{
			BOOL result = FALSE;
			for (const auto& pair : addins.map)
				result |= pair.second.addin->on_modify_title(fp, editp, frame, title, max_title);
			return result;
		}

		//
		// このプロセスで唯一のインスタンスを返します。
		// 内部的に使用されます。
		//
		static AddinManager* WINAPI get_instance()
		{
			auto get_addin_manager = (AddinManager* (WINAPI*)())module.get_proc("core_get_addin_manager");
			if (!get_addin_manager) {
				static AddinManager addin_manager;
				return &addin_manager;
			}
			return (*get_addin_manager)();
		}
	} &addin_manager = *AddinManager::get_instance();
}
