#pragma once
#include "Module/AviUtl.h"
#include "Module/ExEdit.h"
#include "Module/vsthost.h"
#include "Module/color_palette.h"
#include "Module/extoolbar.h"
#include "Module/ZRamPreview.h"

namespace fgo::nest::hook
{
	//
	// このクラスはモジュール別のフックします。
	//
	inline static struct Module
	{
		//
		// モジュール別フックの初期化処理です。
		//
		BOOL init()
		{
			MY_TRACE(_T("Module::init()\n"));

			aviutl::init();
			ExEdit::init();
			vsthost::init();
			color_palette::init();
			extoolbar::init();
			ZRamPreview::init();

			return TRUE;
		}

		//
		// モジュール別フックの後始末処理です。
		//
		BOOL exit()
		{
			MY_TRACE(_T("Module::exit()\n"));

			return TRUE;
		}
	} module;
}
