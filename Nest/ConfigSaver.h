﻿#pragma once
#include "MainWindow.h"

namespace fgo::nest
{
	//
	// このクラスはファイルに設定を保存します。
	//
	struct ConfigSaver : MainWindow::Saver
	{
		std::wstring getName(LPCWSTR x)
		{
			std::wstring name(x);
			std::wstring t = L"* "; // 検索文字列
			auto pos = name.find(t); // 検索文字列が見つかった位置
			auto len = t.length(); // 検索文字列の長さ
			if (pos != std::string::npos)
				name.replace(pos, len, L"");

			return name;
		}

		//
		// 指定されたファイルに設定を保存します。
		//
		HRESULT saveConfig(LPCWSTR fileName, BOOL _export) override
		{
			MY_TRACE_FUNC("%ws, %d", fileName, _export);

			try
			{
				// ドキュメントを作成します。
				MSXML2::IXMLDOMDocumentPtr document(__uuidof(MSXML2::DOMDocument));

				// ドキュメントエレメントを作成します。
				MSXML2::IXMLDOMElementPtr element = appendElement(document, document, L"config");

				// エクスポートのときでもレイアウト以外の変数も保存します。
				//if (!_export) // エクスポートのときはこれらの変数は保存しません。
				{
					setPrivateProfileInt(element, L"borderWidth", Pane::borderWidth);
					setPrivateProfileInt(element, L"captionHeight", Pane::captionHeight);
					setPrivateProfileInt(element, L"tabHeight", Pane::tabHeight);
					setPrivateProfileLabel(element, L"tabMode", Pane::tabMode, Pane::TabMode::label);
					setPrivateProfileInt(element, L"menuBreak", hive.menuBreak);
					setPrivateProfileColor(element, L"fillColor", hive.fillColor);
					setPrivateProfileColor(element, L"borderColor", hive.borderColor);
					setPrivateProfileColor(element, L"hotBorderColor", hive.hotBorderColor);
					setPrivateProfileColor(element, L"activeCaptionColor", hive.activeCaptionColor);
					setPrivateProfileColor(element, L"activeCaptionTextColor", hive.activeCaptionTextColor);
					setPrivateProfileColor(element, L"inactiveCaptionColor", hive.inactiveCaptionColor);
					setPrivateProfileColor(element, L"inactiveCaptionTextColor", hive.inactiveCaptionTextColor);
					setPrivateProfileBool(element, L"useTheme", hive.useTheme);
					setPrivateProfileBool(element, L"forceScroll", hive.forceScroll);
					setPrivateProfileBool(element, L"showPlayer", hive.showPlayer);
					setPrivateProfileBool(element, L"showshowPlayer", hive.showshowPlayer);
				}

				// <mainWindow>を作成します。
				saveMainWindow(element);

				// <subWindow>を作成します。
				saveSubWindow(element);

				// <floatShuttle>を作成します。
				saveFloatShuttle(element);

				return saveXMLDocument(document, fileName, L"UTF-16");
			}
			catch (_com_error& e)
			{
				MY_TRACE(_T("%s\n"), e.ErrorMessage());
				return e.Error();
			}
		}

		// <mainWindow>を作成します。
		HRESULT saveMainWindow(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE_FUNC("");

			{
				// <mainWindow>を作成します。
				MSXML2::IXMLDOMElementPtr mainWindowElement = appendElement(element, L"mainWindow");

				setPrivateProfileWindow(mainWindowElement, L"placement", hive.mainWindow);

				auto root = MainWindow::getRootPane(hive.mainWindow);

				// <pane>を作成します。
				savePane(mainWindowElement, root);
			}

			return S_OK;
		}

		// <subWindow>を作成します。
		HRESULT saveSubWindow(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE_FUNC("");

			for (auto subWindow : subWindowManager.collection)
			{
				// <subWindow>を作成します。
				MSXML2::IXMLDOMElementPtr subWindowElement = appendElement(element, L"subWindow");

				// name はウィンドウタイトルではなく Shuttle::name フィールドで保存．
				auto* subwnd = dynamic_cast<SubWindow*>(Shuttle::getPointer(subWindow));
				setPrivateProfileString(subWindowElement, L"name", subwnd->name);

				// タイトル同期先の名前も保存．
				if (auto title_src = subwnd->GetTitleSource())
					setPrivateProfileString(subWindowElement, L"title_source", title_src->name);

				auto root = SubWindow::getRootPane(subWindow);

				// <pane>を作成します。
				savePane(subWindowElement, root);
			}

			return S_OK;
		}

		// <pane>を作成します。
		HRESULT savePane(const MSXML2::IXMLDOMElementPtr& element, const std::shared_ptr<Pane>& pane)
		{
			MY_TRACE_FUNC("");

			// <pane>を作成します。
			MSXML2::IXMLDOMElementPtr paneElement = appendElement(element, L"pane");

			int current = pane->getCurrentIndex();

			setPrivateProfileLabel(paneElement, L"splitMode", pane->splitMode, Pane::SplitMode::label);
			setPrivateProfileLabel(paneElement, L"origin", pane->origin, Pane::Origin::label);
			setPrivateProfileInt(paneElement, L"border", pane->border);
			setPrivateProfileInt(paneElement, L"isBorderLocked", pane->isBorderLocked);
			setPrivateProfileInt(paneElement, L"current", current);

			int c = pane->getTabCount();
			for (int i = 0; i < c; i++)
			{
				Shuttle* shuttle = pane->getShuttle(i);

				// <dockShuttle>を作成します。
				MSXML2::IXMLDOMElementPtr dockShuttleElement = appendElement(paneElement, L"dockShuttle");

				std::wstring name = getName(shuttle->name);

				setPrivateProfileString(dockShuttleElement, L"name", name.c_str());
			}

			for (auto& child : pane->children)
			{
				if (child)
					savePane(paneElement, child);
			}

			return S_OK;
		}

		// <floatShuttle>を作成します。
		HRESULT saveFloatShuttle(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE_FUNC("");

			for (auto& [name1, shuttle] : shuttleManager.shuttles)
			{
				// <floatShuttle>を作成します。
				MSXML2::IXMLDOMElementPtr floatShuttleElement = appendElement(element, L"floatShuttle");

				std::wstring name = getName(name1);
				MY_TRACE_WSTR(name);

				setPrivateProfileString(floatShuttleElement, L"name", name.c_str());
				// show_caption は <floatShuttle> に保存しておきます．全ての Shuttle が列挙されているため都合がいいからです．
				setPrivateProfileBool(floatShuttleElement, L"show_caption", shuttle->show_caption);
				setPrivateProfileWindow(floatShuttleElement, L"placement", *shuttle->floatContainer);
			}

			return S_OK;
		}
	};
}
