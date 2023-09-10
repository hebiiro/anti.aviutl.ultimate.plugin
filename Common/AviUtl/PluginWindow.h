#pragma once
#include "../Window2.h"
#include "PluginWindowExtension.h"
#include "WindowHolderExtension.h"

namespace Tools::AviUtl
{
	//
	// このクラスはAviUtlのプラグインウィンドウのように振る舞うウィンドウです。
	//
	struct PluginWindow : Window2
	{
		std::shared_ptr<PluginWindowExtension> pluginWindowExtension;
		std::shared_ptr<WindowHolderExtension> windowHolderExtension;

		//
		// コンストラクタです。
		//
		PluginWindow()
			: pluginWindowExtension(std::make_shared<PluginWindowExtension>())
			, windowHolderExtension(std::make_shared<WindowHolderExtension>())
		{
			// このウィンドウにAviUtlプラグイン拡張を追加します。
			// これによって、このウィンドウはAviUtlプラグインウィンドウのように振る舞います。
			addExtension(pluginWindowExtension);

			// このウィンドウにウィンドウホルダー拡張を追加します。
			// これによって、このクラス及び派生クラスでクライアント領域にターゲット(他のウィンドウ)を保持できるようになります。
			addExtension(windowHolderExtension);
		}
	};
}
