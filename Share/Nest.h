#pragma once

namespace Share::Nest {
	struct Exports {
		struct Shuttle {
			virtual BOOL rename(HWND target, LPCWSTR name) =  0;
			virtual BOOL is_empty_name(LPCWSTR name) = 0;
		} *shuttle = 0;
		BOOL init() {
			auto nest = fgo::fate.get_servant<fgo::Servant>(L"Nest");
			if (!nest) return FALSE;
			nest->on_get_address(L"shuttle", shuttle);
			return TRUE;
		}
	};
}
