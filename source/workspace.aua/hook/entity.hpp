#pragma once

namespace apn::workspace::hook
{
	//
	// フックエンティティです。
	//
	struct Entity
	{
		//
		// フックエンティティのコレクションです。
		//
		inline static std::vector<Entity*> collection;

		//
		// コンストラクタです。
		//
		Entity()
		{
			// このエンティティをコレクションに追加します。
			collection.emplace_back(this);
		}

		virtual BOOL on_dll_init() { return TRUE; }
		virtual BOOL on_dll_exit() { return TRUE; }
		virtual BOOL on_init() { return TRUE; }
		virtual BOOL on_exit() { return TRUE; }
	};
}
