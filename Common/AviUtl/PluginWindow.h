#pragma once
#include "../Window2.h"
#include "PluginWindowExtension.h"
#include "WindowHolderExtension.h"

namespace Tools::AviUtl
{
	//
	// このクラスは AviUtl のプラグインウィンドウのように振る舞うウィンドウです。
	//
	struct PluginWindow : Window2
	{
		//
		// コンストラクタです。
		//
		PluginWindow()
		{
			// このウィンドウに AviUtl プラグイン拡張を追加します。
			// これによって、このウィンドウは AviUtl プラグインウィンドウのように振る舞います。
			addExtension(std::make_shared<PluginWindowExtension>());

			// このウィンドウにウィンドウホルダー拡張を追加します。
			// これによって、このクラス及び派生クラスでクライアント領域にターゲット(他のウィンドウ)を保持できるようになります。
			addExtension(std::make_shared<WindowHolderExtension>());
		}
	};
}
