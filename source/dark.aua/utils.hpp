#pragma once

namespace apn::dark
{
	//
	// このクラスは::ExtTextOut()のフックをロックします。
	//
	struct ExtTextOutLocker
	{
		//
		// ::ExtTextOutW()をフックするかどうかのフラグです。
		//
		thread_local inline static BOOL locked = FALSE;

		//
		// コンストラクタです。
		//
		ExtTextOutLocker()
		{
			locked = TRUE;
		}

		//
		// デストラクタです。
		//
		~ExtTextOutLocker()
		{
			locked = FALSE;
		}
	};

	//
	// リターンアドレスを返します。
	//
	inline constexpr auto ret_addr(auto* arg1)
	{
		return reinterpret_cast<my::addr_t*>(arg1)[-1];
	}

	inline std::string safe_string(LPCSTR name)
	{
		if ((ULONG_PTR)name > 0x0000FFFF)
			return name;
		else
			return my::format("{/hex}", (ULONG_PTR)name);
	}

	inline std::wstring safe_string(LPCWSTR name)
	{
		if ((ULONG_PTR)name > 0x0000FFFF)
			return name;
		else
			return my::format(L"{/hex}", (ULONG_PTR)name);
	}

	inline std::wstring safe_string(LPCRECT rc)
	{
		if (rc)
			return my::format(L"{/}, {/}, {/}, {/}", rc->left, rc->top, rc->right, rc->bottom);
		else
			return L"null";
	}

	inline std::wstring safe_string(LPCWSTR str, int c)
	{
		if (c < 0)
			return str;
		else
			return std::wstring(str, c);
	}

	inline std::wstring safe_string(LPCWSTR str, int c, UINT options)
	{
		if (options & ETO_GLYPH_INDEX)
			return L"ETO_GLYPH_INDEX";
		else
			return safe_string(str, c);
	}
}
