#pragma once
#include "Window.h"

namespace Tools
{
	//
	// このクラスは WinAPI のウィンドウを管理します。
	//
	struct Window2 : Window
	{
		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			LRESULT lr = 0;
			for (auto& pair : extensions)
			{
				if (pair.second->onWndProc(this, hwnd, message, wParam, lParam, &lr))
					return lr;
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}

		//
		// このクラスはウィンドウ拡張です。
		// 外部クラスでメッセージをハンドルしないといけないときに使用してください。
		//
		struct Extension
		{
			virtual constexpr LPCWSTR getExtensionName() const = 0;
			virtual BOOL onWndProc(Window2* window, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* lr) = 0;
		};

		//
		// ウィンドウ拡張を追加します。
		//
		void addExtension(const std::shared_ptr<Extension>& extension)
		{
			extensions[extension->getExtensionName()] = extension;
		}

		//
		// ウィンドウ拡張を削除します。
		//
		void eraseExtension(const std::shared_ptr<Extension>& extension)
		{
			extensions.erase(extension->getExtensionName());
		}

		//
		// すべてのウィンドウ拡張を消去します。
		//
		void clearExtensions()
		{
			extensions.clear();
		}

		//
		// 指定された名前のウィンドウ拡張を返します。
		//
		std::shared_ptr<Extension> getExtension(LPCWSTR extensionName) const
		{
			auto it = extensions.find(extensionName);
			if (it == extensions.end()) return 0;
			return it->second;
		}

		//
		// 指定された型のウィンドウ拡張を返します。
		// ウィンドウ拡張内で inline static const LPCWSTR Name = ...; が定義されている必要があります。
		//
		template<class T>
		auto getExtension() const
		{
			return std::static_pointer_cast<T>(getExtension(T::Name));
		}

		//
		// ウィンドウ拡張のコレクションです。
		//
		std::map<std::wstring, std::shared_ptr<Extension>> extensions;
	};
}
