#pragma once
#include "Servant.h"

namespace fgo
{
	//
	// このクラスはサーヴァントを管理します。
	//
	inline struct GrandOrder
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
		// 初期化を実行します。
		// 内部的に使用されます。
		//
		BOOL init()
		{
			// サーヴァントに初期化を実行させます。
			for (const auto& pair : servants)
				pair.second->on_init();

			return TRUE;
		}

		//
		// 後始末を実行します。
		// 内部的に使用されます。
		//
		BOOL exit()
		{
			// サーヴァントに後始末を実行させます。
			for (const auto& pair : servants)
				pair.second->on_exit();

			return TRUE;
		}

		//
		// インスタンスを返します。
		// 内部的に使用されます。
		//
		static GrandOrder* WINAPI get_instance()
		{
			HMODULE module = ::GetModuleHandleW(L"AviUtlTuner.auf");
			auto get_grand_order = (GrandOrder* (WINAPI*)())::GetProcAddress(module, "get_grand_order");
			return (*get_grand_order)();
		}
	} &grand_order = *GrandOrder::get_instance();
}
