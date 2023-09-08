﻿#pragma once
#include "Shuttle/AviUtlWindow.h"
#include "Shuttle/ExEditWindow.h"
#include "Shuttle/SettingDialog.h"
#include "MainWindow.h"
#include "ConfigLoader.h"
#include "ConfigSaver.h"
#include "Module.h"

namespace fgo::nest::hook
{
	//
	// このクラスは::CreateWindowExA()をフックします。
	//
	inline struct
	{
		inline static HWND WINAPI hook(DWORD exStyle, LPCSTR className, LPCSTR windowName, DWORD style,
			int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param)
		{
			if ((DWORD)className <= 0x0000FFFF)
			{
				// classNameがATOMの場合は何もしません。
				return orig(
					exStyle, className, windowName, style,
					x, y, w, h, parent, menu, instance, param);
			}

//				MY_TRACE(_T("::CreateWindowExA(%hs, %hs)\n"), className, windowName);

			if (className && windowName)
			{
				if (_stricmp(className, "Nest") == 0)
				{
					// メインウィンドウのクラス名を置き換えます。
					className = "AviUtl";
				}
				else if (_stricmp(windowName, "AviUtl") == 0)
				{
					// AviUtlウィンドウが作成される直前のタイミングです。

					// コンテナのウィンドウクラスを登録します。
					Container::registerWndClass();

					// メインウィンドウを作成します。
					if (mainWindow->create(std::make_shared<ConfigLoader>(), std::make_shared<ConfigSaver>()))
						hive.mainWindow = *mainWindow;
				}
				else if (_stricmp(className, "AviUtl") == 0 && parent && parent == hive.aviutlWindow)
				{
//						MY_TRACE_STR(windowName);

					// AviUtlのプラグインウィンドウの親をメインウィンドウに変更します。
					parent = hive.mainWindow;
				}
			}

			HWND hwnd = orig(
				exStyle, className, windowName, style,
				x, y, w, h, parent, menu, instance, param);

			if (className && windowName)
			{
				if (_stricmp(className, "AviUtl") == 0)
				{
					if (_stricmp(windowName, "AviUtl") == 0)
					{
						MY_TRACE_STR(windowName);

						// AviUtlウィンドウ用のシャトルを初期化します。
						LPCTSTR name = _T("* AviUtl");
						aviutlWindow->init(name, hwnd);
						shuttleManager.add(aviutlWindow, name);
						hive.aviutlWindow = hwnd;
					}
					else if (_stricmp(windowName, "拡張編集") == 0)
					{
						MY_TRACE_STR(windowName);

						// 拡張編集ウィンドウ用のシャトルを初期化します。
						LPCTSTR name = _T("* 拡張編集");
						exeditWindow->init(name, hwnd);
						shuttleManager.add(exeditWindow, name);
						hive.exeditWindow = hwnd;
					}
					else if (parent && parent == hive.mainWindow)
					{
						MY_TRACE_STR(windowName);

						// プラグインウィンドウ用のシャトルを初期化します。
						auto shuttle = std::make_shared<Shuttle>();
						shuttle->init(windowName, hwnd);
						shuttleManager.add(shuttle, windowName);
					}
				}
				else if (_stricmp(windowName, "ExtendedFilter") == 0)
				{
					// 設定ダイアログ用のシャトルを初期化します。
					LPCTSTR name = _T("* 設定ダイアログ");
					settingDialog->init(name, hwnd);
					shuttleManager.add(settingDialog, name);
					hive.settingDialog = hwnd;

					// モジュール別フックを初期化します。
					module.init();

					// メッセージをポストして最初のレイアウト計算を行います。
					::PostMessage(hive.mainWindow, Hive::WindowMessage::WM_POST_INIT, 0, 0);
				}
			}

			return hwnd;
		}
		inline static decltype(&hook) orig = ::CreateWindowExA;
	} CreateWindowExA;
}