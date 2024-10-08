#pragma once

namespace my
{
	//
	// このクラスは外部クラスでメッセージをハンドルできるようにします。
	//
	struct Window2 : Window
	{
		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			LRESULT lr = 0;
			for (auto& pair : extensions)
			{
				if (pair.second->on_wnd_proc(this, hwnd, message, wParam, lParam, &lr))
					return lr;
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		//
		// このクラスは外部クラスのインターフェイスです。
		//
		struct Extension
		{
			virtual constexpr LPCWSTR get_extension_name() const = 0;
			virtual BOOL on_wnd_proc(Window2* window, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* lr) = 0;
		};

		//
		// 外部クラスを追加します。
		//
		void add_extension(const std::shared_ptr<Extension>& extension)
		{
			extensions[extension->get_extension_name()] = extension;
		}

		//
		// 外部クラスを削除します。
		//
		void erase_extension(const std::shared_ptr<Extension>& extension)
		{
			extensions.erase(extension->get_extension_name());
		}

		//
		// すべての外部クラスを消去します。
		//
		void clear_extensions()
		{
			extensions.clear();
		}

		//
		// 指定された名前の外部クラスを返します。
		//
		std::shared_ptr<Extension> get_extension(LPCWSTR extension_name) const
		{
			auto it = extensions.find(extension_name);
			if (it == extensions.end()) return 0;
			return it->second;
		}

		//
		// 指定された型の外部クラスを返します。
		// 外部クラス内でc_nameが定義されている必要があります。
		//
		template <typename T>
		auto get_extension() const
		{
			return std::static_pointer_cast<T>(get_extension(T::c_name));
		}

		//
		// 外部クラスのコレクションです。
		//
		std::map<std::wstring, std::shared_ptr<Extension>> extensions;
	};
}
