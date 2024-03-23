#pragma once

namespace my::aviutl
{
	//
	// このクラスはAviUtlのプラグインウィンドウのように振る舞うウィンドウです。
	//
	struct PluginWindow : Window2
	{
		std::shared_ptr<PluginWindowExtension> plugin_window_extension;
		std::shared_ptr<WindowHolderExtension> window_holder_extension;

		//
		// コンストラクタです。
		//
		PluginWindow()
			: plugin_window_extension(std::make_shared<PluginWindowExtension>())
			, window_holder_extension(std::make_shared<WindowHolderExtension>())
		{
			// このウィンドウにAviUtlプラグイン拡張を追加します。
			// これによって、このウィンドウはAviUtlプラグインウィンドウのように振る舞います。
			add_extension(plugin_window_extension);

			// このウィンドウにウィンドウホルダー拡張を追加します。
			// これによって、このクラス及び派生クラスでクライアント領域にターゲット(他のウィンドウ)を保持できるようになります。
			add_extension(window_holder_extension);
		}

		//
		// ウィンドウを作成します。
		//
		BOOL create_as_plugin(HINSTANCE instance, HWND parent, LPCTSTR name,
			DWORD ex_style = WS_EX_NOPARENTNOTIFY,
			DWORD style = WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			int x = CW_USEDEFAULT,
			int y = CW_USEDEFAULT,
			int w = CW_USEDEFAULT,
			int h = CW_USEDEFAULT)
		{
			return plugin_window_extension->create(
				*this, instance, parent, name, ex_style, style, x, y, w, h);
		}

		//
		// ウィンドウの表示/非表示を切り替えます。
		//
		void show() { return plugin_window_extension->show(*this); }

		//
		// ターゲットウィンドウを返します。
		//
		HWND get_target() const { return window_holder_extension->get_target(); }

		//
		// ターゲットウィンドウを設定します。
		//
		void set_target(HWND newTarget){ window_holder_extension->set_target(newTarget); }

		//
		// ターゲットウィンドウをホルダーのクライアント領域まで広げます。
		// 非同期で処理します。
		//
		void resize() { window_holder_extension->resize(*this); }
	};
}
