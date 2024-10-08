﻿#pragma once

namespace apn::workspace::share
{
	//
	// ワークスペース化アドインが読み込まれている場合はTRUEを返します。
	//
	inline BOOL exists()
	{
		return !!apn::addin_manager.get_addin<>(L"workspace");
	}

	//
	// コンフィグからウィンドウ位置を読み込みます。
	// ただし、ワークスペース化アドインが存在する場合は
	// ワークスペース化アドインがウィンドウ位置を設定するので何もしません。
	//
	inline void get_window(const n_json& node, const std::string& name, HWND hwnd, DWORD show_cmd = -1)
	{
		if (exists()) return;

		return my::json::get_window(node, name, hwnd, show_cmd);
	}

	//
	// コンフィグにウィンドウ位置を保存します。
	// ただし、ワークスペース化アドインが存在する場合は
	// 正しいウィンドウ位置を取得できないので何もしません。
	//
	inline void set_window(n_json& node, const std::string& name, HWND hwnd)
	{
		if (exists()) return;

		return my::json::set_window(node, name, hwnd);
	}

	//
	// このクラスはワークスペース化アドインのエクスポート関数群です。
	//
	struct Exports
	{
		//
		// このクラスはシャトル関連のエクスポート関数です。
		//
		struct Shuttle
		{
			virtual BOOL rename(HWND target, LPCWSTR name) =  0;
			virtual BOOL is_available_name(LPCWSTR name) = 0;
		} *shuttle = 0;

		//
		// ワークスペース化アドインのエクスポート関数群を初期化します。
		//
		BOOL init()
		{
			auto addin = apn::addin_manager.get_addin<>(L"workspace");
			if (!addin) return FALSE;
			addin->on_get_address(L"shuttle", shuttle);
			return TRUE;
		}
	};
}
