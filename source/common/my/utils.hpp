#pragma once

namespace my
{
	//
	// メモリアドレスの型です。
	//
	using addr_t = uintptr_t;

	//
	// このクラスはスコープ終了処理を実行します。
	//
	template <typename F>
	struct scope_exit {
		F func;
		~scope_exit() { func(); }
	};

	//
	// 指定された変数をリセットします。
	//
	template <typename T>
	inline void reset(T& obj) { obj = T {}; }

	namespace win32
	{
		//
		// 指定されたwin32構造体をリセットします。
		//
		template <typename T>
		inline void reset(T& obj) { obj = T { sizeof(T) }; }
	}
}
