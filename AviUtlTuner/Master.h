#pragma once
#include "Hive.h"
#include "Servant.h"
#include "EditBoxTweaker.h"
#include "ExEditTweaker.h"

namespace aut
{
	inline struct Master
	{
		std::unordered_map<std::wstring, Servant*> servants;

		//
		// 指定されたサーヴァントと契約します。
		//
		template<class T>
		BOOL contract_with(T* servant)
		{
			servants[T::getServantNameStatic()] = servant;
			return TRUE;
		}

		//
		// 指定されたサーヴァントと契約します。
		// ただし、コンフィグファイルで Enable になっていることが条件です。
		//
		template<class T>
		BOOL contract_with(T* servant, LPCWSTR path)
		{
			BOOL enable = TRUE;
			getPrivateProfileInt(path, L"Enable", T::getServantNameStatic(), enable);
			if (!enable) return FALSE;
			servants[T::getServantNameStatic()] = servant;
			return TRUE;
		}

		//
		// 指定された型のサーヴァントを召喚します。
		//
		template<class T>
		T* summon() const
		{
			return summon<T>(T::getServantNameStatic());
		}

		//
		// 指定された名前のサーヴァントを召喚します。
		//
		template<class T>
		T* summon(LPCWSTR name) const
		{
			try {
				return static_cast<T*>(servants.at(name));
			} catch (std::out_of_range&) {
				return 0;
			}
		}

		//
		// サーヴァントとの契約を開始します。
		// 内部的に使用されます。
		//
		BOOL contract()
		{
			std::wstring path = hive.getConfigFileName(L"AviUtlTuner.ini");

			contract_with(&editbox_tweaker, path.c_str());
			contract_with(&exedit_tweaker, path.c_str());

			return TRUE;
		}

		//
		// マスターの初期化を実行します。
		//
		BOOL init()
		{
			contract();

			// サーヴァントに初期化を実行させます。
			for (const auto& pair : servants)
				pair.second->on_init();

			return TRUE;
		}

		//
		// マスターの後始末を実行します。
		//
		BOOL exit()
		{
			// サーヴァントに後始末を実行させます。
			for (const auto& pair : servants)
				pair.second->on_exit();

			return TRUE;
		}
	} master;
}
