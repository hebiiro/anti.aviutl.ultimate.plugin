#pragma once

namespace apn::filter_copy::model
{
	//
	// このクラスはモデルのステート(液体データ)です。
	//
	inline struct state_t
	{
		//
		// このクラスはオーダーです。
		//
		struct order_t {
			int32_t filter_index = -1; // フィルタのインデックスです。
			std::string file_name = {}; // エイリアスのファイル名です。
		};

		//
		// ソースオブジェクトのフラグです。
		//
		ExEdit::Object::Flag flag = {};

		//
		// オーダーのコレクションです。
		//
		std::vector<order_t> orders;
	} state;
}
