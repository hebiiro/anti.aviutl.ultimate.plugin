#pragma once
#include "MainWindow.h"

namespace fgo::nest
{
	//
	// このクラスはファイルから設定を読み込みます。
	//
	struct ConfigLoader : MainWindow::Loader
	{
		//
		// XMLエレメントからシャトル名を取得します。
		//
		static HRESULT WINAPI getPrivateProfileName(const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, _bstr_t& outValue)
		{
			HRESULT hr = getPrivateProfileString(element->getAttribute(name), outValue);
			if (hr != S_OK) return hr;

			if (wcscmp(outValue, L"AviUtl") == 0 ||
				wcscmp(outValue, L"拡張編集") == 0 ||
				wcscmp(outValue, L"設定ダイアログ") == 0)
			{
				outValue = L"* " + outValue;
			}

			return hr;
		}

		//
		// 指定されたファイルから設定を読み込みます。
		//
		HRESULT loadConfig(LPCWSTR fileName, BOOL _import) override
		{
			MY_TRACE(_T("loadConfig(%ws, %d)\n"), fileName, _import);

			try
			{
				// MSXMLを使用します。
				MSXML2::IXMLDOMDocumentPtr document(__uuidof(MSXML2::DOMDocument));

				// 設定ファイルを開きます。
				if (document->load(fileName) == VARIANT_FALSE)
				{
					MY_TRACE(_T("%ws を開けませんでした\n"), fileName);

					return S_FALSE;
				}

				MSXML2::IXMLDOMElementPtr element = document->documentElement;

				if (!_import) // インポートのときはこれらの変数は取得しません。
				{
					getPrivateProfileInt(element, L"borderWidth", Pane::borderWidth);
					getPrivateProfileInt(element, L"captionHeight", Pane::captionHeight);
					getPrivateProfileInt(element, L"tabHeight", Pane::tabHeight);
					getPrivateProfileLabel(element, L"tabMode", Pane::tabMode, Pane::TabMode::label);
					getPrivateProfileInt(element, L"menuBreak", hive.menuBreak);
					getPrivateProfileColor(element, L"fillColor", hive.fillColor);
					getPrivateProfileColor(element, L"borderColor", hive.borderColor);
					getPrivateProfileColor(element, L"hotBorderColor", hive.hotBorderColor);
					getPrivateProfileColor(element, L"activeCaptionColor", hive.activeCaptionColor);
					getPrivateProfileColor(element, L"activeCaptionTextColor", hive.activeCaptionTextColor);
					getPrivateProfileColor(element, L"inactiveCaptionColor", hive.inactiveCaptionColor);
					getPrivateProfileColor(element, L"inactiveCaptionTextColor", hive.inactiveCaptionTextColor);
					getPrivateProfileBool(element, L"useTheme", hive.useTheme);
					getPrivateProfileBool(element, L"forceScroll", hive.forceScroll);
					getPrivateProfileBool(element, L"showPlayer", hive.showPlayer);
				}

				// 事前に <subWindow> を読み込みます。
				preLoadSubWindow(element);

				// <mainWindow> を読み込ます。
				loadMainWindow(element);

				// <subWindow> を読み込ます。
				loadSubWindow(element);

				// <floatShuttle> を読み込ます。
				loadFloatShuttle(element);

				MY_TRACE(_T("設定ファイルの読み込みに成功しました\n"));

				return S_OK;
			}
			catch (_com_error& e)
			{
				MY_TRACE(_T("設定ファイルの読み込みに失敗しました\n"));
				MY_TRACE(_T("%s\n"), e.ErrorMessage());
				return e.Error();
			}
		}

		// 事前に <subWindow> を読み込みます。
		// これにより、レイアウトを読み込みます前に必要なすべてのサブウィンドウが存在する状態になります。
		HRESULT preLoadSubWindow(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE(_T("preLoadSubWindow()\n"));

			// 一旦すべてのサブウィンドウを削除します。
			subWindowManager.clear();

			// <subWindow> を読み込みます。
			MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"subWindow");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr subWindowElement = nodeList->item[i];

				// シャトル名を読み込みます。
				_bstr_t name = L"";
				getPrivateProfileName(subWindowElement, L"name", name);

				// サブウィンドウを作成します。
				auto subWindow = std::make_shared<SubWindow>(name, hive.mainWindow);
				shuttleManager.add(subWindow, name);
			}

			return S_OK;
		}

		// <mainWindow> を読み込みます。
		HRESULT loadMainWindow(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE(_T("loadMainWindow()\n"));

			// <mainWindow> を読み込みます。
			MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"mainWindow");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr mainWindowElement = nodeList->item[i];

				std::shared_ptr<Pane> root = MainWindow::getRootPane(hive.mainWindow);

				// 一旦すべてのペインをリセットします。
				root->resetPane();

				// ウィンドウ位置を読み込みます。
				getPrivateProfileWindow(mainWindowElement, L"placement", hive.mainWindow);

				// <pane> を読み込みます。
				MSXML2::IXMLDOMNodeListPtr nodeList = mainWindowElement->selectNodes(L"pane");
				int c = std::min<int>(1, nodeList->length);
				for (int i = 0; i < c; i++)
				{
					MSXML2::IXMLDOMElementPtr paneElement = nodeList->item[i];

					loadPane(paneElement, root);
				}
			}

			return S_OK;
		}

		// <subWindow> を読み込みます。
		HRESULT loadSubWindow(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE(_T("loadSubWindow()\n"));

			// <subWindow> を読み込みます。
			MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"subWindow");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr subWindowElement = nodeList->item[i];

				// シャトル名を読み込みます。
				_bstr_t name = L"";
				getPrivateProfileName(subWindowElement, L"name", name);

				// サブウィンドウを取得します。
				std::shared_ptr<Shuttle> shuttle = shuttleManager.get(name);
				if (!shuttle) continue;

				// ルートペインを取得します。
				std::shared_ptr<Pane> root = SubWindow::getRootPane(*shuttle);
				if (!root) continue;

				// <pane> を読み込みます。
				MSXML2::IXMLDOMNodeListPtr nodeList = subWindowElement->selectNodes(L"pane");
				int c = std::min<int>(1, nodeList->length);
				for (int i = 0; i < c; i++)
				{
					MSXML2::IXMLDOMElementPtr paneElement = nodeList->item[i];

					loadPane(paneElement, root);
				}
			}

			return S_OK;
		}

		// <pane> を読み込みます。
		HRESULT loadPane(const MSXML2::IXMLDOMElementPtr& paneElement, const std::shared_ptr<Pane>& pane)
		{
			MY_TRACE(_T("loadPane()\n"));

			// <pane> のアトリビュートを読み込みます。

			int current = -1;

			getPrivateProfileLabel(paneElement, L"splitMode", pane->splitMode, Pane::SplitMode::label);
			getPrivateProfileLabel(paneElement, L"origin", pane->origin, Pane::Origin::label);
			getPrivateProfileInt(paneElement, L"isBorderLocked", pane->isBorderLocked);
			getPrivateProfileInt(paneElement, L"border", pane->border);
			getPrivateProfileInt(paneElement, L"current", current);

			MY_TRACE_INT(pane->splitMode);
			MY_TRACE_INT(pane->origin);
			MY_TRACE_INT(pane->border);
			MY_TRACE_INT(current);

			{
				// <dockShuttle> を読み込みます。
				MSXML2::IXMLDOMNodeListPtr nodeList = paneElement->selectNodes(L"dockShuttle");
				int c = nodeList->length;
				for (int i = 0; i < c; i++)
				{
					MSXML2::IXMLDOMElementPtr dockShuttleElement = nodeList->item[i];

					_bstr_t name = L"";
					getPrivateProfileName(dockShuttleElement, L"name", name);
					MY_TRACE_WSTR((LPCWSTR)name);

					std::shared_ptr<Shuttle> shuttle = shuttleManager.get(name);
					if (shuttle)
						pane->addShuttle(shuttle.get());
				}
			}

			pane->setCurrentIndex(current);
			pane->refreshCurrent();

			// <pane> を読み込みます。
			MSXML2::IXMLDOMNodeListPtr nodeList = paneElement->selectNodes(L"pane");
			int c = std::min<int>(2, nodeList->length);
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr paneElement = nodeList->item[i];

				pane->children[i] = std::make_shared<Pane>(pane->owner);
				loadPane(paneElement, pane->children[i]);
			}

			return S_OK;
		}

		// <floatShuttle> を読み込みます。
		HRESULT loadFloatShuttle(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE(_T("loadFloatShuttle()\n"));

			// <floatShuttle> を読み込みます。
			MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"floatShuttle");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr floatShuttleElement = nodeList->item[i];

				_bstr_t name = L"";
				getPrivateProfileName(floatShuttleElement, L"name", name);

				std::shared_ptr<Shuttle> shuttle = shuttleManager.get(name);
				if (shuttle)
				{
					getPrivateProfileWindow(floatShuttleElement, L"placement", *shuttle->floatContainer);

					// フローティングコンテナが表示状態なら
					if (::IsWindowVisible(*shuttle->floatContainer))
					{
						// ドッキング状態なら
						Shuttle::Listener* pane = shuttle->getListener();
						if (pane)
						{
							// ドッキングを解除します。
							pane->releaseShuttle(shuttle.get());
						}

						// ターゲットウィンドウを表示状態にします。
						shuttle->showTargetWindow();
					}
				}
			}

			return S_OK;
		}
	};
}
