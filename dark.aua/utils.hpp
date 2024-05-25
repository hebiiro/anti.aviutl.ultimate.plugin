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

	inline std::string safe_string(LPCSTR name)
	{
		if ((ULONG_PTR)name > 0x0000FFFF)
			return name;
		else
			return std::format("{:#010x}", (uint32_t)name);
	}

	inline std::wstring safe_string(LPCWSTR name)
	{
		if ((ULONG_PTR)name > 0x0000FFFF)
			return name;
		else
			return std::format(L"{:#010x}", (uint32_t)name);
	}

	inline std::wstring safe_string(LPCRECT rc)
	{
		if (rc)
			return std::format(L"{}, {}, {}, {}", rc->left, rc->top, rc->right, rc->bottom);
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
