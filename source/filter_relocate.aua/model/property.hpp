#pragma once

namespace apn::filter_relocate::model
{
	//
	// このクラスはモデル層のプロパティです。
	// 他クラスから共通して使用される蒸発しない変数を保持します。
	//
	inline struct property_t
	{
		inline static constexpr auto c_name = L"filter_relocate";
		inline static constexpr auto c_display_name = L"フィルタ引っ越し";

		//
		// このアドインのインスタンスハンドルです。
		//
		HINSTANCE instance = nullptr;

		//
		// コンフィグのファイル名です。
		//
		std::wstring config_file_name;

		//
		// アンドゥから除外するオブジェクトです。
		//
		std::unordered_set<int32_t> no_undos;

		//
		// TRUEの場合は描画フィルタ(標準描画または拡張描画)もグループ制御に移します。
		//
		BOOL flag_relocate_draw_filter = FALSE;

		//
		// TRUEの場合は2つ上のレイヤーに移動することを許可します。
		//
		BOOL flag_up_two_layers = FALSE;
	} property;
}
