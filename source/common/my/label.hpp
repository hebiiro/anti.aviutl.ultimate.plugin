#pragma once

namespace my
{
	//
	// このクラスはラベルの値とテキストを保持します。
	//
	template <typename V = int, typename T = LPCWSTR>
	struct LabelT { V value; T text; };

	//
	// このクラスはラベルの実体です。
	//
	struct Label : LabelT<> {};
}
