#pragma once

namespace apn::item_wave::item_cache
{
	//
	// このクラスはアイテムキャッシュのコアです。
	//
	inline struct Core
	{
		//
		// この仮想関数は、指定されたアイテムに対応するアイテムキャシュを取得するために呼び出されます。
		//
		virtual std::shared_ptr<Node> get(int32_t object_index) = 0;

		//
		// この仮想関数は、指定されたアイテムに対応するアイテムキャシュを作成するために呼び出されます。
		//
		virtual std::shared_ptr<Node> create(int32_t object_index) = 0;

		//
		// この仮想関数は、指定されたアイテムに対応するアイテムキャシュを削除するために呼び出されます。
		//
		virtual BOOL erase(int32_t object_index) = 0;

		//
		// この仮想関数は、対象シーンのアイテムキャッシュを更新するために呼び出されます。
		//
		virtual BOOL refresh_scene(Context& ctx, int32_t target_scene_index) = 0;

		//
		// この仮想関数は、アイテムキャッシュを更新するために呼び出されます。
		//
		virtual BOOL refresh() = 0;

		//
		// この仮想関数は、すべてのアイテムキャッシュをリセットするために呼び出されます。
		//
		virtual BOOL reset() = 0;

		//
		// この仮想関数は、すべてのキャッシュを消去するために呼び出されます。
		//
		virtual BOOL clear() = 0;
	}
	*core = {};
}
