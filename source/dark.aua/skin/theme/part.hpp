#pragma once

namespace apn::dark::skin::theme
{
	//
	// このクラスはビジュアルスタイルのパーツです。
	//
	struct Part
	{
		std::unordered_map<int, State> states;
	};
}
