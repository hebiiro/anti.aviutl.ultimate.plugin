#pragma once

namespace apn::item_wave::file_cache
{
	//
	// このクラスはファイル毎のキャッシュです。
	//
	struct node_t {
		std::vector<share::volume_t> volumes;
	};
}
