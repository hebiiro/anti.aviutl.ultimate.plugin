#pragma once

namespace apn
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"ultimate.aul";

		//
		// このプラグインのインスタンスハンドルです。
		//
		HMODULE instance = 0;

		//
		// ロード対象のモジュール(ultimate.auf)です。
		//
		HMODULE target = 0;

		//
		// メッセージボックスを表示します。
		//
		int message_box(const std::wstring& text, HWND hwnd = 0, int type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
