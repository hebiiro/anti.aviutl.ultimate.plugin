#pragma once

namespace fgo
{
	//
	// このクラスはこのプラグインのウィンドウです。
	//
	inline struct Chaldeas : Tools::Window2
	{
		//
		// アドインの一覧を表示するためのリストボックスです。
		//
		Tools::Window listbox;

		//
		// 初期化を実行します。
		// 内部的に使用されます。
		//
		BOOL init(AviUtl::FilterPlugin* fp)
		{
			if (!subclass(fp->hwnd)) return FALSE;

			if (!listbox.create(
				WS_EX_CLIENTEDGE,
				WC_LISTBOX,
				_T("アドインの一覧"),
				WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				0, 0, 0, 0,
				*this, 0, 0, 0))
			{
				::MessageBox(*this, _T("アドイン一覧のリストボックスの作成に失敗しました"), _T("Ultimate"), MB_OK);
			}

			auto windowHolderExtension = std::make_shared<Tools::AviUtl::WindowHolderExtension>();

			// このウィンドウにウィンドウホルダー拡張を追加します。
			// これによって、クライアント領域にターゲット(他のウィンドウ)を保持できるようになります。
			addExtension(windowHolderExtension);

			// リストボックスをターゲットにします。
			windowHolderExtension->setTarget(listbox);

			return TRUE;
		}

		//
		// 後始末を実行します。
		// 内部的に使用されます。
		//
		BOOL exit()
		{
			return TRUE;
		}

		//
		// ウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			return __super::onWndProc(hwnd, message, wParam, lParam);
		}

		//
		// このプロセスで唯一のインスタンスを返します。
		// 内部的に使用されます。
		//
		static Chaldeas* WINAPI get_instance()
		{
			auto get_chaldeas = (Chaldeas* (WINAPI*)())chaldea.get_proc("get_chaldeas");
			if (!get_chaldeas) {
				static Chaldeas chaldeas;
				return &chaldeas;
			}
			return (*get_chaldeas)();
		}
	} &chaldeas = *Chaldeas::get_instance();
}
