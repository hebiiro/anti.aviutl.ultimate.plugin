#pragma once
#include "Skin/Dwm.h"
#include "Skin/Icon.h"
#include "Skin/Figure/Manager.h"
#include "Skin/Theme/Manager.h"

namespace fgo::dark::skin
{
	inline struct Manager
	{
		FileUpdateCheckers config_checker;
		UINT currentId = 0;

		BOOL init(HWND hwnd)
		{
			MY_TRACE_FUNC("0x%08X", hwnd);

			// DWMを初期化します。
			dwm.init(hwnd);

			// テーマを初期化します。
			theme::manager.init();

			// スキンの設定を読み込みます。
			reloadSettings();

			// コンフィグチェッカー用のタイマーを開始します。
			::SetTimer(hwnd, (UINT_PTR)this, 1000, onTimerProc);

			return TRUE;
		}

		static void CALLBACK onTimerProc(HWND hwnd, UINT message, UINT_PTR timerId, DWORD time)
		{
			if (manager.config_checker.isFileUpdated())
			{
				if (manager.reloadSettings())
					hive.redraw();
			}
		}

		BOOL reloadSettings()
		{
			MY_TRACE_FUNC("");

			config_checker.clear();
			theme::manager.clear();
			figure::manager.clear();

			reloadSettingsInternal();
			reloadExEditSettings();

			// スキンが更新されたことを判別できるように、IDも更新(インクリメント)します。
			currentId++;

			return TRUE;
		}

		BOOL reloadSettingsInternal()
		{
			MY_TRACE_FUNC("");

			WCHAR fileName[MAX_PATH] = {};
			config.getConfigFileName(fileName, std::size(fileName));

			return reloadSettingsInternal(fileName);
		}

		BOOL reloadSettingsInternal(LPCWSTR fileName)
		{
			MY_TRACE_FUNC("%ws", fileName);

			config_checker.add(fileName);

			try
			{
				MSXML2::IXMLDOMDocumentPtr document(__uuidof(MSXML2::DOMDocument));

				if (document->load(fileName) == VARIANT_FALSE)
				{
					MY_TRACE(_T("%ws の読み込みに失敗しました\n"), fileName);

					return FALSE;
				}

				MSXML2::IXMLDOMElementPtr element = document->documentElement;

				getPrivateProfileString(element, L"skin", config.skinFileName);
				getPrivateProfileLabel(element, L"shadowMode", config.shadowMode, Config::ShadowMode::label);
				getPrivateProfileLabel(element, L"roundMode", config.roundMode, Config::RoundMode::label);
				getPrivateProfileLabel(element, L"staticEdgeMode", config.staticEdgeMode, Config::StaticEdgeMode::label);
				getPrivateProfileBool(element, L"useLayerColor", config.useLayerColor);
				getPrivateProfileBool(element, L"useLayerColorEx", config.useLayerColorEx);
				getPrivateProfileLabel(element, L"omitFileDialog", config.omitFileDialog, Config::OmitFileDialog::label);

				_bstr_t skin;
				getPrivateProfileFileName(element, L"skin", skin);

				return reloadSkinSettings(skin);
			}
			catch (_com_error& e)
			{
				MY_TRACE(_T("%s\n"), e.ErrorMessage());

				return FALSE;
			}
		}

		BOOL reloadExEditSettings()
		{
			MY_TRACE_FUNC("");

			uintptr_t exedit = (uintptr_t)::GetModuleHandle(_T("exedit.auf"));

			if (config.staticEdgeMode == Config::StaticEdgeMode::Off)
			{
				// ボタンの拡張スタイル(WS_EX_STATICEDGE)を消します。
				writeAbsoluteAddress(exedit + 0x2ED42 + 1, (uintptr_t)0);
			}

			{
				int style = PS_SOLID;
				int width = 0;
				COLORREF color = RGB(0x00, 0xff, 0xff);

				if (config.exedit.xorPen.style != -1 &&
					config.exedit.xorPen.width != -1 &&
					config.exedit.xorPen.color != CLR_NONE)
				{
					style = config.exedit.xorPen.style;
					width = config.exedit.xorPen.width;
					color = config.exedit.xorPen.color;
				}

				HPEN newPen = ::CreatePen(style, width, color);
				HPEN oldPen = writeAbsoluteAddress(exedit + 0x1538B4, newPen);
				::DeleteObject(oldPen);
			}

			{
				HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_EXEDIT);
				auto state = skin::theme::manager.getState(theme, skin::theme::EXEDIT_SELECTION, 0);
				if (state && state->fillColor != CLR_NONE)
					writeAbsoluteAddress(exedit + 0x0003807E, &state->fillColor);
			}
			{
				HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_EXEDIT);
				auto state = skin::theme::manager.getState(theme, skin::theme::EXEDIT_SELECTIONEDGE, 0);
				if (state && state->fillColor != CLR_NONE)
					writeAbsoluteAddress(exedit + 0x00038076, &state->fillColor);
			}
			{
				HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_EXEDIT);
				auto state = skin::theme::manager.getState(theme, skin::theme::EXEDIT_SELECTIONBK, 0);
				if (state && state->fillColor != CLR_NONE)
					writeAbsoluteAddress(exedit + 0x00038087, &state->fillColor);
			}

			return TRUE;
		}

		BOOL reloadSkinSettings(LPCWSTR fileName)
		{
			MY_TRACE_FUNC("%ws", fileName);

			{
				// スキンを変更したとき、前回の値を引き継がないようにデフォルト値に戻しておきます。

				config.exedit.xorPen.style = -1;
				config.exedit.xorPen.width = -1;
				config.exedit.xorPen.color = CLR_NONE;
			}

			config_checker.add(fileName);

			try
			{
				MSXML2::IXMLDOMDocumentPtr document(__uuidof(MSXML2::DOMDocument));

				if (document->load(fileName) == VARIANT_FALSE)
				{
					MY_TRACE(_T("%ws の読み込みに失敗しました\n"), fileName);

					return FALSE;
				}

				{
					// スキンファイルを読み込みます。

					MSXML2::IXMLDOMNodeListPtr nodeList =
						document->documentElement->selectNodes(L"Skin");
					int c = nodeList->length;
					for (int i = 0; i < c; i++)
					{
						MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

						_bstr_t fileName;
						getPrivateProfileFileName(element, L"fileName", fileName);

						reloadSkin(fileName);
					}
				}

				return TRUE;
			}
			catch (_com_error& e)
			{
				MY_TRACE(_T("%s\n"), e.ErrorMessage());

				return FALSE;
			}
		}

		BOOL reloadSkin(LPCWSTR fileName)
		{
			MY_TRACE_FUNC("%ws", fileName);

			config_checker.add(fileName);

			try
			{
				MSXML2::IXMLDOMDocumentPtr document(__uuidof(MSXML2::DOMDocument));

				if (document->load(fileName) == VARIANT_FALSE)
				{
					MY_TRACE(_T("%ws の読み込みに失敗しました\n"), fileName);

					return FALSE;
				}

				loadAttributes(document->documentElement);
				loadFigures(document->documentElement);
				loadVSClasses(document->documentElement);

				return TRUE;
			}
			catch (_com_error& e)
			{
				MY_TRACE(_T("%s\n"), e.ErrorMessage());

				return FALSE;
			}
		}

		void loadAttributes(const MSXML2::IXMLDOMElementPtr& parentElement)
		{
			MY_TRACE_FUNC("");

			// <Attributes>を読み込みます。
			MSXML2::IXMLDOMNodeListPtr nodeList = parentElement->selectNodes(L"Attributes");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

				figure::DrawSingleRaisedEdge::loadStatic(element);
				figure::DrawSingleSunkenEdge::loadStatic(element);
				figure::DrawDoubleSunkenEdge::loadStatic(element);
				figure::DrawDoubleBumpEdge::loadStatic(element);
				figure::DrawDoubleEtchedEdge::loadStatic(element);

				config.load(element);
				dwm.load(element);
				icon_manager.load(element);
				color_set_manager.load(element);
			}
		}

		HRESULT loadFigures(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE_FUNC("");

			return figure::manager.loadFigures(element);
		}

		HRESULT loadVSClasses(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE_FUNC("");

			return theme::manager.loadVSClasses(element);
		}

		BOOL saveSettings()
		{
			MY_TRACE_FUNC("");

			return saveSettingsInternal();
		}

		BOOL saveSettingsInternal()
		{
			MY_TRACE_FUNC("");

			WCHAR fileName[MAX_PATH] = {};
			config.getConfigFileName(fileName, std::size(fileName));

			return saveSettingsInternal(fileName);
		}

		BOOL saveSettingsInternal(LPCWSTR fileName)
		{
			MY_TRACE_FUNC("%ws", fileName);

			try
			{
				// ドキュメントを作成します。
				MSXML2::IXMLDOMDocumentPtr document(__uuidof(MSXML2::DOMDocument));

				// ドキュメントエレメントを作成します。
				MSXML2::IXMLDOMElementPtr element = appendElement(document, document, L"Settings");

				setPrivateProfileString(element, L"skin", config.skinFileName);
				setPrivateProfileLabel(element, L"shadowMode", config.shadowMode, Config::ShadowMode::label);
				setPrivateProfileLabel(element, L"roundMode", config.roundMode, Config::RoundMode::label);
				setPrivateProfileLabel(element, L"staticEdgeMode", config.staticEdgeMode, Config::StaticEdgeMode::label);
				setPrivateProfileBool(element, L"useLayerColor", config.useLayerColor);
				setPrivateProfileBool(element, L"useLayerColorEx", config.useLayerColorEx);

				saveXMLDocument(document, fileName, L"UTF-16");

				return TRUE;
			}
			catch (_com_error& e)
			{
				MY_TRACE(_T("%s\n"), e.ErrorMessage());

				return FALSE;
			}
		}

		//
		// スキンを選択するためのメニューを表示します。
		//
		void showMenu(HWND hwnd)
		{
			MY_TRACE_FUNC("0x%08X", hwnd);

			std::wstring origin = magi.getConfigFileName(L"Skin");
			MY_TRACE_WSTR(origin);

			std::vector<std::wstring> fileNames;
			{
				WCHAR folder[MAX_PATH] = {};
				::StringCchCopyW(folder, std::size(folder), origin.c_str());
				::PathAppendW(folder, L"*.xml");
				MY_TRACE_WSTR(folder);

				WIN32_FIND_DATAW ffd = {};
				HANDLE handle = ::FindFirstFileW(folder, &ffd);

				if (handle == INVALID_HANDLE_VALUE)
					return;

				do
				{
					if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
						continue;

					fileNames.push_back(ffd.cFileName);
				}
				while (::FindNextFileW(handle, &ffd));

				::FindClose(handle);

				std::sort(fileNames.begin(), fileNames.end());
			}

			WCHAR current[MAX_PATH] = {};
			::StringCchCopyW(current, std::size(current), config.getSkinFileName());
			::PathStripPathW(current);

			const int ID_SHADOW_MODE = 20000;
			const int ID_ROUND_MODE = 20001;
			const int ID_STATIC_EDGE_MODE = 20002;
			const int ID_USE_LAYER_COLOR_EX = 20003;
			const int ID_OMIT_FILE_DIALOG = 20004;

			HMENU menu = ::CreatePopupMenu();

			for (int i = 0; i < (int)fileNames.size(); i++)
			{
				WCHAR text[MAX_PATH] = {};
				::StringCchCopyW(text, std::size(text), fileNames[i].c_str());
				::PathRemoveExtensionW(text);

				::AppendMenuW(menu, MF_STRING, i + 1, text);

				if (::lstrcmpiW(fileNames[i].c_str(), current) == 0)
					::CheckMenuItem(menu, i + 1, MF_CHECKED);
			}

			::AppendMenuW(menu, MF_SEPARATOR, -1, 0);
			::AppendMenuW(menu, MF_STRING, ID_SHADOW_MODE, L"影を付ける");
			::AppendMenuW(menu, MF_STRING, ID_ROUND_MODE, L"丸くする");
			::AppendMenuW(menu, MF_STRING, ID_STATIC_EDGE_MODE, L"ボタンにスタティックエッジを付ける");
			::AppendMenuW(menu, MF_STRING, ID_USE_LAYER_COLOR_EX, L"複数行の色分け");
			::AppendMenuW(menu, MF_STRING, ID_OMIT_FILE_DIALOG, L"ファイル選択ダイアログを除外する");

			if (config.getShadowMode() == Config::ShadowMode::On)
				::CheckMenuItem(menu, ID_SHADOW_MODE, MF_CHECKED);

			if (config.getRoundMode() == Config::RoundMode::On)
				::CheckMenuItem(menu, ID_ROUND_MODE, MF_CHECKED);

			if (config.getStaticEdgeMode() == Config::StaticEdgeMode::On)
				::CheckMenuItem(menu, ID_STATIC_EDGE_MODE, MF_CHECKED);

			if (config.getUseLayerColorEx())
				::CheckMenuItem(menu, ID_USE_LAYER_COLOR_EX, MF_CHECKED);

			if (config.getOmitFileDialog() == Config::OmitFileDialog::On)
				::CheckMenuItem(menu, ID_OMIT_FILE_DIALOG, MF_CHECKED);

			POINT pt; ::GetCursorPos(&pt);
			int id = ::TrackPopupMenu(menu, TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, 0, hwnd, 0);

			::DestroyMenu(menu);

			if (id > 0 && id <= (int)fileNames.size())
			{
				WCHAR fileName[MAX_PATH] = {};
				::StringCchCopyW(fileName, std::size(fileName), L"Skin");
				::PathAppendW(fileName, fileNames[id - 1].c_str());
				MY_TRACE_WSTR(fileName);
				config.setSkinFileName(fileName);

				saveSettings();
			}
			else if (id == ID_SHADOW_MODE)
			{
				if (config.getShadowMode() == Config::ShadowMode::On)
					config.setShadowMode(Config::ShadowMode::Off);
				else
					config.setShadowMode(Config::ShadowMode::On);

				saveSettings();
			}
			else if (id == ID_ROUND_MODE)
			{
				if (config.getRoundMode() == Config::RoundMode::On)
					config.setRoundMode(Config::RoundMode::Off);
				else
					config.setRoundMode(Config::RoundMode::On);

				saveSettings();
			}
			else if (id == ID_STATIC_EDGE_MODE)
			{
				if (config.getStaticEdgeMode() == Config::StaticEdgeMode::On)
					config.setStaticEdgeMode(Config::StaticEdgeMode::Off);
				else
					config.setStaticEdgeMode(Config::StaticEdgeMode::On);

				saveSettings();

				::MessageBoxW(hwnd, L"このオプションはAviUtlを再起動したときに反映されます", hive.Name, MB_OK);
			}
			else if (id == ID_USE_LAYER_COLOR_EX)
			{
				config.setUseLayerColorEx(!config.getUseLayerColorEx());

				saveSettings();

				::InvalidateRect(magi.auin.GetExEditWindow(), 0, TRUE);
			}
			else if (id == ID_OMIT_FILE_DIALOG)
			{
				if (config.getOmitFileDialog() == Config::OmitFileDialog::On)
					config.setOmitFileDialog(Config::OmitFileDialog::Off);
				else
					config.setOmitFileDialog(Config::OmitFileDialog::On);

				saveSettings();
			}
		}
	} manager;
}
