#pragma once

namespace apn::item_wave::file_cache
{
	//
	// このクラスはファイル毎のキャッシュです。
	//
	struct Node {
		std::vector<share::Volume> volumes;
	};
}
