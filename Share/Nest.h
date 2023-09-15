#pragma once

namespace Share::Nest {
	//
	// Nestアドインが読み込まれている場合はTRUEを返します。
	//
	inline BOOL exists() {
		return !!fgo::fate.get_servant<fgo::Servant>(L"Nest");
	}

	//
	// XMLからウィンドウ位置を読み込みます。
	// ただし、Nestが存在する場合はNestに任せます。
	//
	inline HRESULT getPrivateProfileWindow(const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, HWND hwnd, DWORD cmdShow = -1)
	{
		if (exists()) return S_FALSE;
		return ::getPrivateProfileWindow(element, name, hwnd, cmdShow);
	}

	//
	// XMLにウィンドウ位置を保存します。
	// ただし、Nestが存在する場合はNestに任せます。
	//
	inline HRESULT setPrivateProfileWindow(const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, HWND hwnd, DWORD cmdShow = -1)
	{
		if (exists()) return S_FALSE;
		return ::setPrivateProfileWindow(element, name, hwnd, cmdShow);
	}

	//
	// このクラスはNestのエクスポート関数群です。
	//
	struct Exports {
		//
		// このクラスはシャトル関連のエクスポート関数です。
		//
		struct Shuttle {
			virtual BOOL rename(HWND target, LPCWSTR name) =  0;
			virtual BOOL is_empty_name(LPCWSTR name) = 0;
		} *shuttle = 0;

		//
		// Nestのエクスポート関数群を初期化します。
		//
		BOOL init() {
			auto nest = fgo::fate.get_servant<fgo::Servant>(L"Nest");
			if (!nest) return FALSE;
			nest->on_get_address(L"shuttle", shuttle);
			return TRUE;
		}
	};
}
