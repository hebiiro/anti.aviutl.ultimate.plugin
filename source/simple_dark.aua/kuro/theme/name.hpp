#pragma once

namespace apn::dark::kuro::theme
{
	//
	// このクラスはテーマ名を管理します。
	//
	inline struct name_t
	{
		//
		// テーマ名のマップです。
		//
		std::unordered_map<HTHEME, std::wstring> map;

		//
		// テーマ名を追加します。
		//
		void set(HTHEME theme, const std::wstring& name) { map[theme] = name; }

		//
		// テーマ名を返します。
		//
		std::wstring get(HTHEME theme) { return map[theme]; }

		//
		// テーマ名を返します。
		//
		std::wstring get_safe(HTHEME theme)
		{
			if (auto it = map.find(theme); it != map.end())
				return my::format(L"{/hex}({/})", theme, it->second);
			else
				return my::format(L"{/hex}", theme);
		}
	} name;
}
