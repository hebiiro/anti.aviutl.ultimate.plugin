#pragma once

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
	inline void read_window_pos(const n_json& node, const std::string& name, HWND hwnd, DWORD show_cmd = -1)
	{
		if (exists()) return;

		return my::json::read_window_pos(node, name, hwnd, show_cmd);
	}

	//
	// コンフィグにウィンドウ位置を保存します。
	// ただし、ワークスペース化アドインが存在する場合は
	// 正しいウィンドウ位置を取得できないので何もしません。
	//
	inline void write_window_pos(n_json& node, const std::string& name, HWND hwnd)
	{
		if (exists()) return;

		return my::json::write_window_pos(node, name, hwnd);
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
		} *shuttle = nullptr;

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

	//
	// このクラスはスリムバーです。
	//
	struct SlimBar
	{
		//
		// ウィンドウメッセージです。
		//
		inline static constexpr struct Message {
			inline static const auto c_draw = ::RegisterWindowMessageW(L"apn::workspace::slimbar::draw");
		} c_message;

		//
		// 描画コンテキストです。
		//
		struct DrawContext {
			HWND hwnd;
			HTHEME theme;
			HDC dc;
			int part_id;
			int state_id;
			LPCRECT rc;
		};
	};
}
