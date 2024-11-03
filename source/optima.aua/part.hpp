#pragma once

namespace apn::optima
{
	//
	// このクラスはパーツです。
	//
	struct Part
	{
		virtual BOOL func_init(AviUtl::FilterPlugin* fp) = 0;
		virtual BOOL func_exit(AviUtl::FilterPlugin* fp) = 0;
	};

	//
	// このクラスはパーツを管理します。
	//
	inline struct PartManager
	{
		//
		// パーツのコレクションです。
		//
		std::vector<Part*> parts;

		//
		// パーツの初期化処理を実行します。
		//
		BOOL init()
		{
			for (auto part : parts)
				part->func_init(magi.fp);

			return TRUE;
		}

		//
		// パーツの後始末処理を実行します。
		//
		BOOL exit()
		{
			for (auto part : parts)
				part->func_exit(magi.fp);

			return TRUE;
		}

		//
		// パーツを追加します。
		//
		void add(Part* part)
		{
			parts.emplace_back(part);
		}
	} part_manager;
}
