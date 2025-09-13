#pragma once

namespace apn::optima::part
{
	//
	// このクラスはパーツです。
	//
	struct Node : NodeBase
	{
		//
		// コンストラクタです。
		//
		Node()
		{
			manager.add(this);
		}
	};
}
