#pragma once
#include "Resource.h"
#include "Hive.h"

namespace fgo::scene_select
{
	//
	// このクラスはメインウィンドウです。
	// このウィンドウはプラグインウィンドウのように振る舞います。
	//
	struct MainWindow : Tools::AviUtl::PluginWindow
	{
		struct CommandID {
			inline static constexpr UINT Config = 2023;
		};

		struct Label { LPCWSTR label; int value; };

		struct LayoutMode {
			inline static constexpr int Horz = 0;
			inline static constexpr int Vert = 1;
			inline static constexpr Label label[] = {
				{ L"Horz", Horz },
				{ L"Vert", Vert },
			};
		};

		inline static constexpr int MaxSceneCount = 50;

		std::wstring configFileName;
		HTHEME themeWindow = 0;
		HTHEME themeButton = 0;

		int hotScene = -1;
		int dragScene = -1;
		std::vector<RECT> buttonRectArray;

		int layoutMode = LayoutMode::Horz;
		int rowLimit = 10;
		int colLimit = 10;
		int sceneCount = MaxSceneCount;
		BOOL fixedSizeMode = FALSE;
		int buttonWidth = 80;
		int buttonHeight = 24;

		//
		// 初期化処理を行います。
		//
		BOOL init()
		{
			// コンフィグファイル名をセットします。
			configFileName = magi.getConfigFileName(L"SceneSelect.ini");

			// ウィンドウを作成します。
			// このウィンドウはAviUtlプラグインウィンドウのように振る舞います。
			if (!createAsPlugin(
				hive.instance,
				magi.auin.GetAviUtlWindow(),
				hive.DisplayName,
				WS_EX_NOPARENTNOTIFY,
				WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				100, 100, 300, 300))
			{
				return FALSE;
			}

			loadConfig(configFileName.c_str());

			calcLayout(*this);
			redraw();

			return TRUE;
		}

		//
		// 後始末処理を行います。
		//
		BOOL exit()
		{
			saveConfig(configFileName.c_str());

			destroy();

			return TRUE;
		}

		//
		// 設定をファイルから読み込みます。
		//
		void loadConfig(LPCWSTR configFileName)
		{
			MY_TRACE_FUNC("%ws", configFileName);

			getPrivateProfileLabel(configFileName, L"Config", L"layoutMode", layoutMode, LayoutMode::label);
			getPrivateProfileInt(configFileName, L"Config", L"rowLimit", rowLimit);
			getPrivateProfileInt(configFileName, L"Config", L"colLimit", colLimit);
			getPrivateProfileInt(configFileName, L"Config", L"sceneCount", sceneCount);
			getPrivateProfileBool(configFileName, L"Config", L"fixedSizeMode", fixedSizeMode);
			getPrivateProfileInt(configFileName, L"Config", L"buttonWidth", buttonWidth);
			getPrivateProfileInt(configFileName, L"Config", L"buttonHeight", buttonHeight);
			getPrivateProfileWindow(configFileName, L"MainWindow", *this);
		}

		//
		// 設定をファイルに保存します。
		//
		void saveConfig(LPCWSTR configFileName)
		{
			MY_TRACE_FUNC("%ws", configFileName);

			setPrivateProfileLabel(configFileName, L"Config", L"layoutMode", layoutMode, LayoutMode::label);
			setPrivateProfileInt(configFileName, L"Config", L"rowLimit", rowLimit);
			setPrivateProfileInt(configFileName, L"Config", L"colLimit", colLimit);
			setPrivateProfileInt(configFileName, L"Config", L"sceneCount", sceneCount);
			setPrivateProfileBool(configFileName, L"Config", L"fixedSizeMode", fixedSizeMode);
			setPrivateProfileInt(configFileName, L"Config", L"buttonWidth", buttonWidth);
			setPrivateProfileInt(configFileName, L"Config", L"buttonHeight", buttonHeight);
			setPrivateProfileWindow(configFileName, L"MainWindow", *this);
		}

		//
		// ウィンドウ内容を再描画します。
		//
		BOOL redraw()
		{
			return ::InvalidateRect(*this, 0, FALSE);
		}

		//
		// 指定されたシーンインデックスが有効ならTRUEを返します。
		//
		inline BOOL isSceneIndexValid(int sceneIndex)
		{
			return sceneIndex >= 0 && sceneIndex < sceneCount;
		}

		//
		// ボタンの配置位置を再計算します。
		//
		void calcLayout(HWND hwnd)
		{
			switch (layoutMode)
			{
			case LayoutMode::Vert: calcLayoutVert(hwnd); break;
			case LayoutMode::Horz: calcLayoutHorz(hwnd); break;
			}
		}

		//
		// レイアウトモードが垂直の場合のボタンの配置位置を再計算します。
		//
		void calcLayoutVert(HWND hwnd)
		{
			buttonRectArray.resize(sceneCount);

			RECT clientRect; ::GetClientRect(hwnd, &clientRect);
			int clientX = clientRect.left;
			int clientY = clientRect.top;
			int clientW = clientRect.right - clientRect.left;
			int clientH = clientRect.bottom - clientRect.top;

			int rowCount = (rowLimit > 0) ? rowLimit : 5;
			int colCount = (sceneCount - 1) / rowCount + 1;

			int sceneIndex = 0;

			for (int col = 0; col < colCount; col++)
			{
				for (int row = 0; row < rowCount; row++, sceneIndex++)
				{
					if (sceneIndex >= sceneCount) break;

					RECT rc = {};

					if (fixedSizeMode)
					{
						rc.left = clientX + buttonWidth * col;
						rc.top = clientY + buttonHeight * row;
						rc.right = rc.left + buttonWidth;
						rc.bottom = rc.top + buttonHeight;
					}
					else
					{
						rc.left = clientX + ::MulDiv(clientW, col + 0, colCount);
						rc.right = clientX + ::MulDiv(clientW, col + 1, colCount);
						rc.top = clientY + ::MulDiv(clientH, row + 0, rowCount);
						rc.bottom = clientY + ::MulDiv(clientH, row + 1, rowCount);
					}

					buttonRectArray[sceneIndex] = rc;
				}
			}

			if (fixedSizeMode)
			{
				RECT rc = clientRect;
				rc.right = rc.left + buttonWidth * colCount;
				rc.bottom = rc.top + buttonHeight * rowCount;
				clientToWindow(hwnd, &rc);

				::SetWindowPos(hwnd, 0, 0, 0, getWidth(rc), getHeight(rc), SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
			}
		}

		//
		// レイアウトモードが水平の場合のボタンの配置位置を再計算します。
		//
		void calcLayoutHorz(HWND hwnd)
		{
			buttonRectArray.resize(sceneCount);

			RECT clientRect; ::GetClientRect(hwnd, &clientRect);
			int clientX = clientRect.left;
			int clientY = clientRect.top;
			int clientW = clientRect.right - clientRect.left;
			int clientH = clientRect.bottom - clientRect.top;

			int colCount = (colLimit > 0) ? colLimit : 5;
			int rowCount = (sceneCount - 1) / colCount + 1;

			int sceneIndex = 0;

			for (int row = 0; row < rowCount; row++)
			{
				for (int col = 0; col < colCount; col++, sceneIndex++)
				{
					if (sceneIndex >= sceneCount) break;

					RECT rc = {};

					if (fixedSizeMode)
					{
						rc.left = clientX + buttonWidth * col;
						rc.top = clientY + buttonHeight * row;
						rc.right = rc.left + buttonWidth;
						rc.bottom = rc.top + buttonHeight;
					}
					else
					{
						rc.left = clientX + ::MulDiv(clientW, col + 0, colCount);
						rc.right = clientX + ::MulDiv(clientW, col + 1, colCount);
						rc.top = clientY + ::MulDiv(clientH, row + 0, rowCount);
						rc.bottom = clientY + ::MulDiv(clientH, row + 1, rowCount);
					}

					buttonRectArray[sceneIndex] = rc;
				}
			}

			if (fixedSizeMode)
			{
				RECT rc = clientRect;
				rc.right = rc.left + buttonWidth * colCount;
				rc.bottom = rc.top + buttonHeight * rowCount;
				::MapWindowPoints(hwnd, 0, (LPPOINT)&rc, 2);
				clientToWindow(hwnd, &rc);

				::SetWindowPos(hwnd, 0, 0, 0, getWidth(rc), getHeight(rc), SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
			}
		}

		//
		// 指定された座標にあるボタンのインデックスを返します。
		//
		int hitTest(HWND hwnd, POINT point)
		{
			for (int i = 0; i < (int)buttonRectArray.size(); i++)
			{
				if (::PtInRect(&buttonRectArray[i], point))
					return i;
			}

			return -1;
		}

		//
		// ボタンを描画します。
		//
		void onPaint(HWND hwnd)
		{
			if (!magi.auin.GetExEdit())
				return;

			PaintDC _dc(hwnd);
			RECT clientRect; ::GetClientRect(hwnd, &clientRect);
			UxDC dc(_dc, &clientRect);

			if (!dc.isValid())
				return;

			AviUtl::SysInfo si = {};
			magi.fp->exfunc->get_sys_info(0, &si);

			// フォントをセットします。
			GdiObjSelector fontSelector(dc, si.hfont);

			{
				// ブラシを取得します。
				HBRUSH brush = (HBRUSH)::SendMessage(hwnd, WM_CTLCOLORDLG, (WPARAM)(HDC)dc, (LPARAM)hwnd);

				// 背景を塗りつぶします。
				::FillRect(dc, &clientRect, brush);
			}

			// ボタンを描画します。
			for (int i = 0; i < (int)buttonRectArray.size(); i++)
			{
				int sceneIndex = i;
				const RECT& rc = buttonRectArray[i];

				// シーン名を取得します。
				WCHAR text[MAX_PATH] = {};
				{
					ExEdit::SceneSetting* scene = magi.auin.GetSceneSetting(sceneIndex);

					if (scene->name)
					{
						::StringCchPrintfW(text, std::size(text), L"%hs", scene->name);
					}
					else
					{
						if (sceneIndex == 0)
							::StringCchCopyW(text, std::size(text), L"Root");
						else
							::StringCchPrintfW(text, std::size(text), L"%d", sceneIndex);
					}
				}

				// ボタンのパートとステートを取得します。
				int partId = BP_PUSHBUTTON;
				int stateId = PBS_NORMAL;
				{
					if (sceneIndex == magi.auin.GetCurrentSceneIndex())
					{
						stateId = PBS_PRESSED;
					}
					else if (sceneIndex == dragScene)
					{
						if (sceneIndex == hotScene)
						{
							stateId = PBS_PRESSED;
						}
						else
						{
							stateId = PBS_HOT;
						}
					}
					else if (sceneIndex == hotScene)
					{
						stateId = PBS_HOT;
					}
				}

				// テーマを使用してボタンを描画します。
				::DrawThemeBackground(themeButton, dc, partId, stateId, &rc, 0);
				::DrawThemeText(themeButton, dc, partId, stateId,
					text, ::lstrlenW(text), DT_CENTER | DT_VCENTER | DT_SINGLELINE, 0, &rc);
			}
		}

		//
		// コンテキストメニューを表示します。
		//
		void onContextMenu(HWND hwnd)
		{
			MY_TRACE_FUNC("");

			POINT cursorPos; ::GetCursorPos(&cursorPos);

			HMENU menu = ::CreatePopupMenu();

			::AppendMenu(menu, MF_STRING, CommandID::Config, _T("設定"));

			int id = ::TrackPopupMenu(menu, TPM_NONOTIFY | TPM_RETURNCMD, cursorPos.x, cursorPos.y, 0, hwnd, 0);

			switch (id)
			{
			case CommandID::Config:
				{
					onConfigDialog(hwnd);

					break;
				}
			}

			::DestroyMenu(menu);
		}

		//
		// コンフィグダイアログを表示します。
		//
		void onConfigDialog(HWND hwnd)
		{
			MY_TRACE_FUNC("");

			ConfigDialog dialog;

			if (IDOK != dialog.doModal(this, *this))
				return;

			calcLayout(hwnd);
			redraw();
		}

		//
		// ウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_CREATE:
				{
					MY_TRACE_FUNC("WM_CREATE, 0x%08X, 0x%08X", wParam, lParam);

					themeWindow = ::OpenThemeData(hwnd, VSCLASS_WINDOW);
					MY_TRACE_HEX(themeWindow);

					themeButton = ::OpenThemeData(hwnd, VSCLASS_BUTTON);
					MY_TRACE_HEX(themeButton);

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, 0x%08X, 0x%08X", wParam, lParam);

					::CloseThemeData(themeWindow), themeWindow = 0;
					::CloseThemeData(themeButton), themeButton = 0;

					break;
				}
			case WM_SIZE:
				{
					MY_TRACE_FUNC("WM_SIZE, 0x%08X, 0x%08X", wParam, lParam);

					// 固定サイズモードの場合はWM_SIZEでは何もしません。
					if (fixedSizeMode) break;

					calcLayout(hwnd);

					break;
				}
			case WM_PAINT:
				{
					MY_TRACE_FUNC("WM_PAINT, 0x%08X, 0x%08X", wParam, lParam);

					onPaint(hwnd);

					break;
				}
			case WM_LBUTTONDOWN:
				{
					MY_TRACE_FUNC("WM_LBUTTONDOWN, 0x%08X, 0x%08X", wParam, lParam);

					// マウスカーソルの座標を取得します。
					POINT point = LP2PT(lParam);

					// ドラッグを開始するシーンを取得します。
					dragScene = hitTest(hwnd, point);
					MY_TRACE_INT(dragScene);

					// ドラッグシーンが無効なら
					if (!isSceneIndexValid(dragScene))
						break; // 何もしません。

					// マウスキャプチャを開始します。
					::SetCapture(hwnd);

					// 再描画します。
					redraw();

					break;
				}
			case WM_LBUTTONUP:
				{
					MY_TRACE_FUNC("WM_LBUTTONUP, 0x%08X, 0x%08X", wParam, lParam);

					// マウスカーソルの座標を取得します。
					POINT point = LP2PT(lParam);

					// マウスキャプチャ中なら
					if (::GetCapture() == hwnd)
					{
						// マウスキャプチャを終了します。
						::ReleaseCapture();

						// ホットシーンを取得します。
						hotScene = hitTest(hwnd, point);
						MY_TRACE_INT(hotScene);

						// ドラッグシーンとホットシーンが同じなら
						if (dragScene == hotScene)
						{
							// ドラッグシーンが有効かつ現在のシーンと違うなら
							if (isSceneIndexValid(dragScene) && dragScene != magi.auin.GetCurrentSceneIndex())
							{
								// ボタンが押されたのでシーンを変更します。
								magi.auin.SetScene(dragScene, magi.auin.GetFilter(magi.fp, "拡張編集"), magi.auin.GetEditp());

								// AviUtlのプレビューウィンドウを再描画します。
								magi.redraw();
							}
						}

						// ドラッグシーンを初期値に戻します。
						dragScene = -1;

						// 再描画します。
						redraw();
					}

					break;
				}
			case WM_MOUSEMOVE:
				{
//					MY_TRACE_FUNC("WM_MOUSEMOVE, 0x%08X, 0x%08X", wParam, lParam);

					// マウスカーソルの座標を取得します。
					POINT point = LP2PT(lParam);

					if (::GetCapture() == hwnd)
					{
						// マウス座標にあるシーンを取得します。
						int scene = hitTest(hwnd, point);

						// ドラッグシーンとマウス座標にあるシーンが異なるなら
						if (dragScene != scene)
							scene = -1; // マウス座標にあるシーンを無効にします。

						// ホットシーンとマウス座標にあるシーンが異なるなら
						if (hotScene != scene)
						{
							// ホットシーンを更新します。
							hotScene = scene;

							// 再描画します。
							redraw();
						}
					}
					else
					{
						// マウス座標にあるシーンを取得します。
						int scene = hitTest(hwnd, point);

						// ホットシーンとマウス座標にあるシーンが異なるなら
						if (hotScene != scene)
						{
							// ホットシーンを更新します。
							hotScene = scene;

							// ホットシーンが有効かつマウスキャプチャ中でないなら
							if (hotScene >= 0)
							{
								// WM_MOUSELEAVEが発行されるようにします。
								TRACKMOUSEEVENT tme = { sizeof(tme) };
								tme.dwFlags = TME_LEAVE;
								tme.hwndTrack = hwnd;
								::TrackMouseEvent(&tme);
							}

							// 再描画します。
							redraw();
						}
					}

					break;
				}
			case WM_MOUSELEAVE:
				{
					MY_TRACE_FUNC("WM_MOUSELEAVE, 0x%08X, 0x%08X", wParam, lParam);

					// ホットシーンが有効なら
					if (hotScene >= 0)
					{
						// ホットシーンを初期値に戻します。
						hotScene = -1;

						// 再描画します。
						redraw();
					}

					break;
				}
			case WM_RBUTTONUP:
				{
					MY_TRACE_FUNC("WM_RBUTTONUP, 0x%08X, 0x%08X", wParam, lParam);

					// コンテキストメニューを表示します。
					onContextMenu(hwnd);

					break;
				}
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}

		//
		// このクラスは設定を変更するためのダイアログです。
		//
		struct ConfigDialog : Tools::Dialog2
		{
			int doModal(MainWindow* mainWindow, HWND parent)
			{
				create(hive.instance, MAKEINTRESOURCE(IDD_CONFIG), parent);

				::SendDlgItemMessage(*this, IDC_ROW_COUNT_SPIN, UDM_SETRANGE32, 1, 50);
				::SendDlgItemMessage(*this, IDC_COL_COUNT_SPIN, UDM_SETRANGE32, 1, 50);
				::SendDlgItemMessage(*this, IDC_SCENE_COUNT_SPIN, UDM_SETRANGE32, 1, 50);
				::SendDlgItemMessage(*this, IDC_VOICE_SPIN, UDM_SETRANGE32, 0, 10);

				setComboBox(IDC_LAYOUT_MODE, mainWindow->layoutMode, _T("水平方向"), _T("垂直方向"));
				setInt(IDC_ROW_COUNT, mainWindow->rowLimit);
				setInt(IDC_COL_COUNT, mainWindow->colLimit);
				setInt(IDC_SCENE_COUNT, mainWindow->sceneCount);
				setCheck(IDC_FIXED_SIZE, mainWindow->fixedSizeMode);
				setInt(IDC_BUTTON_WIDTH, mainWindow->buttonWidth);
				setInt(IDC_BUTTON_HEIGHT, mainWindow->buttonHeight);

				return doModal2(parent);
			}

			INT_PTR onDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
			{
				switch (message)
				{
				case WM_NOTIFY:
					{
						break;
					}
				}

				return __super::onDlgProc(hwnd, message, wParam, lParam);
			}
		};
	};
}
