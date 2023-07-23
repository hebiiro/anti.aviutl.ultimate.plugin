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

		template<class T>
		BOOL contract_with(T* servant)
		{
			servants[T::getServantNameStatic()] = servant;

			return TRUE;
		}

		template<class T>
		T* summon() const
		{
			return summon<T>(T::getServantNameStatic());
		}

		template<class T>
		T* summon(LPCWSTR name) const
		{
			try {
				return static_cast<T*>(servants.at(name));
			} catch (std::out_of_range&) {
				return 0;
			}
		}

		BOOL init()
		{
			contract_with(&editbox_tweaker);
			contract_with(&exedit_tweaker);

			// サーヴァントに初期化を実行させます。
			for (const auto& pair : servants)
				pair.second->on_init();

			return TRUE;
		}

		BOOL exit()
		{
			// サーヴァントに後始末を実行させます。(逆順で)
			for (const auto& pair : servants)
				pair.second->on_exit();

			return TRUE;
		}
	} master;
}
