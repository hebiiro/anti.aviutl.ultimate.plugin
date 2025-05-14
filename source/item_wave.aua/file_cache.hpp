#pragma once

namespace apn::item_wave
{
	//
	// このクラスはファイル毎のキャッシュです。
	//
	struct FileCache {
		std::vector<share::Volume> volumes;
	};
}
