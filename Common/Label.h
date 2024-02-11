#pragma once

namespace Tools
{
	//
	// このクラスはラベルの値とテキストを保持します。
	//
	template <class V = int, class T = LPCWSTR>
	struct LabelT
	{ 
		V value;
		T text;
	};

	//
	// このクラスはラベルの実体です。
	//
	struct Label : LabelT<int> {};
}
