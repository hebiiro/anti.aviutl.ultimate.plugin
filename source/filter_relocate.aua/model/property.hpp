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
	} property;
}
