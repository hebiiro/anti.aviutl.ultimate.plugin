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

		//
		// ウィンドウを作成します。
		//
		BOOL createAsPlugin(HINSTANCE instance, HWND parent, LPCTSTR name,
			DWORD exStyle = WS_EX_NOPARENTNOTIFY,
			DWORD style = WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			int x = CW_USEDEFAULT,
			int y = CW_USEDEFAULT,
			int w = CW_USEDEFAULT,
			int h = CW_USEDEFAULT)
		{
			return pluginWindowExtension->create(
				*this, instance, parent, name, exStyle, style, x, y, w, h);
		}

		//
		// ウィンドウの表示/非表示を切り替えます。
		//
		void show() { return pluginWindowExtension->show(*this); }

		//
		// ターゲットウィンドウを返します。
		//
		HWND getTarget() const { return windowHolderExtension->getTarget(); }

		//
		// ターゲットウィンドウを設定します。
		//
		void setTarget(HWND newTarget){ windowHolderExtension->setTarget(newTarget); }

		//
		// ターゲットウィンドウをホルダーのクライアント領域まで広げます。
		// 非同期で処理します。
		//
		void resize() { windowHolderExtension->resize(*this); }
	};
}
