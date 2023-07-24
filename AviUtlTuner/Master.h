#pragma once
#include "Fate/Fate.h"
#include "Fate/GrandOrder.h"
#include "EditBoxTweaker.h"
#include "ExEditTweaker.h"
#include "FilterCopy.h"

namespace fgo
{
	//
	// このクラスはサーヴァントを統率するマスターです。
	//
	inline struct Master
	{
		//
		// 初期化を実行します。
		//
		BOOL init()
		{
			std::wstring path = fate.getConfigFileName(L"AviUtlTuner.ini");

			grand_order.contract_with(&editbox_tweaker::servant, path.c_str());
			grand_order.contract_with(&exedit_tweaker::servant, path.c_str());
			grand_order.contract_with(&filter_copy::servant, path.c_str());

			return TRUE;
		}

		//
		// 後始末を実行します。
		//
		BOOL exit()
		{
			return TRUE;
		}
	} master;
}
