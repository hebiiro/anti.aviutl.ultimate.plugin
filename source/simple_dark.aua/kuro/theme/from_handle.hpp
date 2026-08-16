#pragma once

namespace apn::dark::kuro::theme
{
	//
	// このクラスはテーマレンダラーを管理します。
	// テーマレンダラーはテーマハンドルをキーにして取得できます。
	//
	inline struct from_handle_t
	{
		//
		// レンダラーのマップです。キーはテーマハンドルです。
		//
		std::unordered_map<HTHEME, renderer_t*> map;

		//
		// レンダラーを追加します。
		//
		void set(HTHEME theme, renderer_t* renderer) { map[theme] = renderer; }

		//
		// レンダラーを返します。
		//
		renderer_t* get(HTHEME theme) { return map[theme]; }
	} from_handle;
}
