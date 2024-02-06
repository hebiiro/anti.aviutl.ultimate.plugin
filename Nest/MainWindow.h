#pragma once
#include "Resource.h"
#include "Shuttle/AviUtlWindow.h"
#include "Shuttle/ExEditWindow.h"
#include "Shuttle/SettingDialog.h"
#include "SubWindow.h"
#include "SubProcess.h"
#include "SubProcess/PSDToolKit.h"
#include "SubProcess/Bouyomisan.h"
#include "SubProcess/Console.h"

namespace fgo::nest
{
	//
	// このクラスはメインウィンドウです。
	// このウィンドウはシャトルをドッキングさせることができます。
	//
	struct MainWindow : DockSite<Tools::Window>
	{
		struct CommandID
		{
			static const UINT SHUTTLE_BEGIN = 100;
			static const UINT SHUTTLE_END = 300;
			static const UINT SHOW_CONFIG_DIALOG = 1000;
			static const UINT IMPORT_LAYOUT = 1001;
			static const UINT EXPORT_LAYOUT = 1002;
			static const UINT CREATE_SUB_WINDOW = 1003;

			static const UINT MAXIMIZE_PLAY = 30000;
		};

		struct Loader {
			virtual HRESULT loadConfig(LPCWSTR fileName, BOOL _import) = 0;
		}; std::shared_ptr<Loader> loader;

		struct Saver {
			virtual HRESULT saveConfig(LPCWSTR fileName, BOOL _export) = 0;
		}; std::shared_ptr<Saver> saver;

		HMENU subMenu[Category::MaxSize] = {};

		//
		// コンストラクタです。
		//
		MainWindow()
		{
		}

		//
		// デストラクタです。
		//
		virtual ~MainWindow()
		{
			MY_TRACE_FUNC("0x%08X", (HWND)*this);
		}

		//
		// メインウィンドウを作成します。
		//
		BOOL create(const std::shared_ptr<Loader>& loader, const std::shared_ptr<Saver>& saver)
		{
			MY_TRACE_FUNC("");

			this->loader = loader;
			this->saver = saver;

			//「AoiSupport」用の処理です。
			// クラス名を"AviUtl"にして、AviUtlウィンドウに偽装します。
			static const LPCTSTR ClassName = _T("AviUtl");

			WNDCLASS wc = {};
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
			wc.hCursor = ::LoadCursor(0, IDC_ARROW);
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = hive.instance;
			wc.lpszClassName = ClassName;
			::RegisterClass(&wc);

			return Tools::Window::create(
				0,
				ClassName,
				_T("Nest"),
				WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				0, 0, hive.instance, 0);
		}

		//
		// シャトルを初期化します。
		//
		BOOL initShuttle(HWND hwnd, LPTSTR windowName)
		{
			// クラス名を取得します。取得できなかった場合は何もしません。
			TCHAR className[MAX_PATH] = {};
			::GetClassName(hwnd, className, std::size(className));
			if (_tcslen(className) == 0) return FALSE;

			// ウィンドウ名を取得します。取得できなかった場合は何もしません。
			if (!windowName || _tcslen(windowName) == 0) return FALSE;

			MY_TRACE_FUNC("0x%08X, %s, %s", hwnd, className, windowName);

			if (hwnd == hive.aviutlWindow)
			{
				if (*aviutlWindow) return FALSE;

				MY_TRACE(_T("AviUtlウィンドウ用のシャトルを初期化します\n"));

				LPCTSTR name = _T("* AviUtl");
				aviutlWindow->init(name, hwnd);

				return TRUE;
			}

			if (hwnd == hive.exeditWindow)
			{
				if (*exeditWindow) return FALSE;

				MY_TRACE(_T("拡張編集ウィンドウ用のシャトルを初期化します\n"));

				LPCTSTR name = _T("* 拡張編集");
				exeditWindow->init(name, hwnd);

				return TRUE;
			}

			if (hwnd == hive.settingDialog)
			{
				if (*settingDialog) return FALSE;

				MY_TRACE(_T("設定ダイアログ用のシャトルを初期化します\n"));

				LPCTSTR name = _T("* 設定ダイアログ");
				settingDialog->init(name, hwnd);

				// 最初のレイアウト計算を行う準備が整ったので、
				// メインウィンドウにメッセージをポストして通知します。
				::PostMessage(hive.mainWindow, Hive::WindowMessage::WM_POST_INIT, 0, 0);

				return TRUE;
			}

			if (_tcsicmp(className, _T("AviUtl")) == 0)
			{
				// 拡張編集ウィンドウのダミーかもしれないのでチェックしておきます。
				if (hwnd == exeditWindow->dummy) return FALSE;

				MY_TRACE(_T("「%s」用のシャトルを初期化します\n"), windowName);

				auto shuttle = std::make_shared<Shuttle>();
				shuttle->init(windowName, hwnd);

				return TRUE;
			}

			return FALSE;
		}

		//
		// システムメニューを初期化します。
		//
		void initSystemMenu()
		{
			MY_TRACE_FUNC("");

			HMENU menu = ::GetSystemMenu(*this, FALSE);
			int index = 0;

			// サブメニューを作成し、システムメニューに追加します。
			for (size_t i = 0; i < std::size(subMenu); i++)
			{
				subMenu[i] = ::CreatePopupMenu();

				::InsertMenu(menu, index++, MF_BYPOSITION | MF_POPUP, (UINT)subMenu[i], Category::labels[i]);
			}

			::InsertMenu(menu, index++, MF_BYPOSITION | MF_SEPARATOR, 0, 0);
			::InsertMenu(menu, index++, MF_BYPOSITION | MF_STRING, CommandID::CREATE_SUB_WINDOW, _T("サブウィンドウを新規作成"));
			::InsertMenu(menu, index++, MF_BYPOSITION | MF_STRING, CommandID::IMPORT_LAYOUT, _T("レイアウトのインポート"));
			::InsertMenu(menu, index++, MF_BYPOSITION | MF_STRING, CommandID::EXPORT_LAYOUT, _T("レイアウトのエクスポート"));
			::InsertMenu(menu, index++, MF_BYPOSITION | MF_STRING, CommandID::SHOW_CONFIG_DIALOG, _T("ネストの設定"));
			::InsertMenu(menu, index++, MF_BYPOSITION | MF_SEPARATOR, 0, 0);
		}

		//
		// システムメニューのサブメニューを初期化します。
		//
		void initSystemMenuPopup()
		{
			MY_TRACE_FUNC("");

			// 一旦すべての項目を削除します。
			for (auto& menu : subMenu)
				while (::DeleteMenu(menu, 0, MF_BYPOSITION)){}

			// フローティング状態のシャトルをサブメニューに追加します。
			UINT id = CommandID::SHUTTLE_BEGIN;
			for (auto& pair : shuttleManager.shuttles)
			{
				auto& shuttle = pair.second;

				// ドッキング状態のシャトルは除外します。
				if (shuttle->isDocking()) continue;

				// シャトルのカテゴリを取得します。
				int category = getCategory(shuttle.get());
				if (category == Category::None) continue;

				// 追加先メニューを取得します。
				HMENU menu = subMenu[category];

				// メニューアイテムを追加します。
				::AppendMenu(menu, MF_STRING, id, shuttle->name);

				// シャトルが表示されているなら
				if (::IsWindowVisible(*shuttle))
				{
					// メニューアイテムにチェックを入れます。
					::CheckMenuItem(menu, id, MF_CHECKED);
				}

				// IDをインクリメントします。
				id++;
			}
		}

		//
		// メインメニューを更新します。
		//
		void updateMainMenu()
		{
			MY_TRACE_FUNC("");

			HMENU menu = ::GetMenu(*this);

			LPCTSTR text = _T("再生時最大化 OFF");
			if (hive.showPlayer) text = _T("再生時最大化 ON");

			MENUITEMINFO mii = { sizeof(mii) };
			mii.fMask = MIIM_STRING;
			mii.dwTypeData = (LPTSTR)text;
			::SetMenuItemInfo(menu, CommandID::MAXIMIZE_PLAY, FALSE, &mii);

			::DrawMenuBar(*this);
		}

		//
		// メインウィンドウのタイトル名を更新します。
		//
		void refreshTitle()
		{
			MY_TRACE_FUNC("");

			TCHAR orig[MAX_PATH] = {};
			::GetWindowText(hive.aviutlWindow, orig, std::size(orig));
			MY_TRACE_TSTR(orig);

			AviUtl::EditHandle* editp = magi.auin.GetEditp();

			TCHAR title[MAX_PATH] = {};
			if (editp && editp->frame_n)
			{
				::StringCchPrintf(title, std::size(title), _T("%hs"), editp->project_filename);
				::PathStripPath(title);

				if (_tcslen(title) == 0)
					::StringCchCopy(title, std::size(title), _T("無題"));

				::StringCchCat(title, std::size(title), _T(" - "));
				::StringCchCat(title, std::size(title), orig);
			}
			else
			{
				::StringCchCopy(title, std::size(title), orig);
			}

			::SetWindowText(*this, title);
		}

		//
		// すべてのドックサイトのレイアウトを再計算します。
		//
		void calcAllLayout()
		{
			MY_TRACE_FUNC("");

			calcLayout(*this);

			for (auto subWindow : subWindowManager.collection)
				calcLayout(subWindow);
		}

		//
		// ファイルから設定を読み込みます。
		// 設定ファイルのファイル名は内部的に決定されます。
		//
		BOOL loadConfig()
		{
			MY_TRACE_FUNC("");

			return loadConfig(magi.getConfigFileName(L"Nest.xml").c_str(), FALSE);
		}

		//
		// ファイルから設定を読み込みます。
		// _importがTRUEの場合は、現在の設定を残しつつレイアウトだけをファイルから読み込みます。
		//
		BOOL loadConfig(LPCWSTR fileName, BOOL _import)
		{
			MY_TRACE_FUNC("%ws, %d", fileName, _import);

			HRESULT hr = loader->loadConfig(fileName, _import);

			// メインメニューを更新します。
			updateMainMenu();

			// レイアウトを再計算します。
			calcAllLayout();

			// メインウィンドウが非表示なら表示します。
			if (!::IsWindowVisible(hwnd))
				::ShowWindow(hwnd, SW_SHOW);

			// メインウィンドウをフォアグラウンドにします。
			::SetForegroundWindow(hwnd);
			::SetActiveWindow(hwnd);

			return hr == S_OK;
		}

		//
		// ユーザーが指定したファイルからレイアウトをインポートします。
		//
		BOOL importLayout(HWND hwnd)
		{
			// ファイル選択ダイアログを表示してファイル名を取得します。

			// Shiftキーが押されている場合はレイアウトだけのインポートではなく、
			// すべての設定を完全に読み込みます。
			BOOL import = ::GetKeyState(VK_SHIFT) >= 0;

			// ユーザーが指定したファイル名が格納されるバッファです。
			WCHAR fileName[MAX_PATH] = {};

			// ファイルダイアログを表示します。
			OPENFILENAMEW ofn = { sizeof(ofn) };
			ofn.hwndOwner = hwnd;
			ofn.Flags = OFN_FILEMUSTEXIST;
			ofn.lpstrTitle = L"レイアウトのインポート";
			ofn.lpstrFile = fileName;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFilter = L"レイアウトファイル (*.xml)\0*.xml\0" "すべてのファイル (*.*)\0*.*\0";
			ofn.lpstrDefExt = L"xml";
			if (!::GetOpenFileNameW(&ofn))
				return FALSE;

			// レイアウトファイルをインポートします。
			return loadConfig(fileName, import);
		}

		//
		// ファイルに設定を保存します。
		// 設定ファイルのファイル名は内部的に決定されます。
		//
		BOOL saveConfig()
		{
			MY_TRACE_FUNC("");

			return saveConfig(magi.getConfigFileName(L"Nest.xml").c_str(), FALSE);
		}

		//
		// ファイルに設定を保存します。
		// _exportがTRUEの場合は、レイアウトだけをファイルに保存します。
		//
		BOOL saveConfig(LPCWSTR fileName, BOOL _export)
		{
			MY_TRACE_FUNC("%ws, %d", fileName, _export);

			return saver->saveConfig(fileName, _export) == S_OK;
		}

		//
		// ユーザーが指定したファイルにレイアウトをエクスポートします。
		//
		BOOL exportLayout(HWND hwnd)
		{
			// ファイル選択ダイアログを表示してファイル名を取得します。

			// ユーザーが指定したファイル名が格納されるバッファです。
			WCHAR fileName[MAX_PATH] = {};

			// ファイルダイアログを表示します。
			OPENFILENAMEW ofn = { sizeof(ofn) };
			ofn.hwndOwner = hwnd;
			ofn.Flags = OFN_OVERWRITEPROMPT;
			ofn.lpstrTitle = L"レイアウトのエクスポート";
			ofn.lpstrFile = fileName;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFilter = L"レイアウトファイル (*.xml)\0*.xml\0" "すべてのファイル (*.*)\0*.*\0";
			ofn.lpstrDefExt = L"xml";
			if (!::GetSaveFileNameW(&ofn))
				return FALSE;

			// レイアウトファイルをエクスポートします。
			return saveConfig(fileName, TRUE);
		}

		//
		// コンフィグダイアログを表示します。
		//
		int showConfigDialog(HWND parent)
		{
			ConfigDialog dialog;

			return dialog.doModal(parent);
		}

		//
		// サブウィンドウを作成します。
		//
		BOOL createSubWindow(HWND parent)
		{
			TCHAR name[MAX_PATH] = {};
			SubWindow::getEmptyName(name, std::size(name));
			MY_TRACE_TSTR(name);

			auto subWindow = std::make_shared<SubWindow>();
			if (subWindow->create(name, parent))
			{
				subWindow->init(name, *subWindow);
				::ShowWindowAsync(*subWindow, SW_SHOW);
			}

			return TRUE;
		}

		//
		// サブプロセス用のウィンドウを作成します。
		//
		BOOL createSubProcesses(HWND parent)
		{
			if (hive.psdtoolkit.dock) {
				const LPCTSTR name = Hive::PSDToolKit::Name;
				auto shuttle = psdtoolkit = std::make_shared<PSDToolKit>();
				shuttle->create(name, *this);
				shuttle->init(name, *shuttle);
			}

			if (hive.bouyomisan.dock) {
				const LPCTSTR name = Hive::Bouyomisan::Name;
				auto shuttle = bouyomisan = std::make_shared<Bouyomisan>();
				shuttle->create(name, *this);
				shuttle->init(name, *shuttle);
			}

			if (hive.console.dock) {
				const LPCTSTR name = Hive::Console::Name;
				auto shuttle = console = std::make_shared<Console>();
				shuttle->create(name, *this);
				shuttle->init(name, *shuttle);
			}

			return TRUE;
		}

		//
		// ウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_KEYDOWN:
			case WM_KEYUP:
			case WM_CHAR:
			case WM_DEADCHAR:
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			case WM_SYSCHAR:
			case WM_SYSDEADCHAR:
				{
					MY_TRACE_FUNC("WM_***KEY***, 0x%08X, 0x%08X, 0x%08X", message, wParam, lParam);

					return ::SendMessage(hive.aviutlWindow, message, wParam, lParam);
				}
			case WM_NCACTIVATE:
				{
					MY_TRACE_FUNC("WM_NCACTIVATE, 0x%08X, 0x%08X", wParam, lParam);

					break;
				}
			case WM_ACTIVATE: // 「patch.aul」用の処理です。
				{
					MY_TRACE_FUNC("WM_ACTIVATE, 0x%08X, 0x%08X", wParam, lParam);

					if (LOWORD(wParam) == WA_INACTIVE)
						::SendMessage(hive.aviutlWindow, message, wParam, lParam);

					break;
				}
			case WM_SETFOCUS:
				{
					MY_TRACE_FUNC("WM_SETFOCUS, 0x%08X, 0x%08X", wParam, lParam);

					break;
				}
			case WM_KILLFOCUS:
				{
					MY_TRACE_FUNC("WM_KILLFOCUS, 0x%08X, 0x%08X", wParam, lParam);

					break;
				}
			case WM_MENUSELECT: // 「patch.aul」用の処理です。
				{
					MY_TRACE_FUNC("WM_MENUSELECT, 0x%08X, 0x%08X", wParam, lParam);

					::SendMessage(hive.aviutlWindow, message, wParam, lParam);

					break;
				}
			case WM_CLOSE:
				{
					MY_TRACE_FUNC("WM_CLOSE, 0x%08X, 0x%08X", wParam, lParam);

					return ::SendMessage(hive.aviutlWindow, message, wParam, lParam);
				}
			case WM_COMMAND:
				{
					MY_TRACE_FUNC("WM_COMMAND, 0x%08X, 0x%08X", wParam, lParam);

					UINT id = LOWORD(wParam);

					switch (id)
					{
					case CommandID::MAXIMIZE_PLAY:
						{
							MY_TRACE(_T("CommandID::MAXIMIZE_PLAY\n"));

							hive.showPlayer = !hive.showPlayer;

							updateMainMenu();

							return 0;
						}
					}

					// 「PSDToolKit」用の処理です。
					// WM_COMMAND終了時にマウスメッセージがPSDToolKitに飛ぶとフリーズします。
					if (psdtoolkit && ::IsWindowVisible(*psdtoolkit))
					{
						// PSDToolKitを非表示にしてマウスメッセージが飛ばないようにします。
						::ShowWindow(*psdtoolkit, SW_HIDE);

						// AviUtlにWM_COMMANDを処理させます。
						LRESULT lr = ::SendMessage(hive.aviutlWindow, message, wParam, lParam);

						// 表示コマンドをポストします。
						// これにより、若干遅れてPSDToolKitが表示状態になります。
						::ShowWindowAsync(*psdtoolkit, SW_SHOW);

						return lr;
					}

					return ::SendMessage(hive.aviutlWindow, message, wParam, lParam);
				}
			case WM_SYSCOMMAND:
				{
					MY_TRACE_FUNC("WM_SYSCOMMAND, 0x%08X, 0x%08X", wParam, lParam);

					if (wParam >= CommandID::SHUTTLE_BEGIN && wParam < CommandID::SHUTTLE_END)
					{
						HMENU menu = ::GetSystemMenu(hwnd, FALSE);
						UINT id = (UINT)wParam;

						// メニューアイテムのテキストを取得します。
						TCHAR text[MAX_PATH] = {};
						::GetMenuString(menu, id, text, std::size(text), MF_BYCOMMAND);
						MY_TRACE_TSTR(text);

						// テキストからシャトルを取得します。ドッキングできるかチェックもします。
						auto shuttle = shuttleManager.get(text);
						if (!shuttle) break;

						::SendMessage(*shuttle, WM_CLOSE, 0, 0);

						break;
					}

					switch (wParam)
					{
					case CommandID::SHOW_CONFIG_DIALOG:
						{
							// コンフィグダイアログを開きます。
							if (IDOK == showConfigDialog(hwnd))
							{
								// レイアウトを再計算します。
								calcAllLayout();

								// 設定が変更された可能性があるので、
								// メインウィンドウのメニューを更新します。
								updateMainMenu();
							}

							break;
						}
					case CommandID::IMPORT_LAYOUT:
						{
							// レイアウトファイルをインポートします。
							importLayout(hwnd);

							break;
						}
					case CommandID::EXPORT_LAYOUT:
						{
							// レイアウトファイルをエクスポートします。
							exportLayout(hwnd);

							break;
						}
					case CommandID::CREATE_SUB_WINDOW:
						{
							// サブウィンドウを新規作成します。
							createSubWindow(hwnd);

							break;
						}
					case SC_RESTORE:
					case SC_MINIMIZE: // 「patch.aul」用の処理です。
						{
							::SendMessage(hive.aviutlWindow, message, wParam, lParam);

							break;
						}
					}

					break;
				}
			case WM_INITMENUPOPUP:
				{
					initSystemMenuPopup();

					break;
				}
			case WM_CREATE:
				{
					MY_TRACE_FUNC("WM_CREATE");

					hive.theme = ::OpenThemeData(hwnd, VSCLASS_WINDOW);
					MY_TRACE_HEX(hive.theme);

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY");

					::CloseThemeData(hive.theme), hive.theme = 0;

					break;
				}
			case Hive::WindowMessage::WM_POST_INIT: // AviUtlの初期化処理が終わったあとに通知されます。
				{
					MY_TRACE_FUNC("0x%08X, WM_POST_INIT, 0x%08X, 0x%08X, Begin", hwnd, wParam, lParam);

					// システムメニューに独自の項目を追加する。
					initSystemMenu();

					// 設定をファイルから読み込みます。
					loadConfig();

					MY_TRACE_FUNC("0x%08X, WM_POST_INIT, 0x%08X, 0x%08X, End", hwnd, wParam, lParam);

					// メインウィンドウのタイトルを更新します。
					refreshTitle();

					break;
				}
			case Hive::WindowMessage::WM_LOAD_CONFIG: // 設定を読み込む必要があるときに通知されます。
				{
					// 設定をファイルから読み込みます。
					loadConfig();

					break;
				}
			case Hive::WindowMessage::WM_SAVE_CONFIG: // 設定を保存する必要があるときに通知されます。
				{
					// 設定をファイルに保存します。
					saveConfig();

					break;
				}
			case Hive::WindowMessage::WM_INIT_SHUTTLE: // シャトルを初期化するために通知されます。
				{
					auto hwnd = (HWND)wParam;
					auto orig = (Hive::OrigWindow*)lParam;

					// シャトルを初期化します。
					initShuttle(hwnd, orig->windowName);

					// メモリを開放します。
					delete orig;

					break;
				}
			case Hive::WindowMessage::WM_REFRESH_TITLE: // タイトルを更新するために通知されます。
				{
					refreshTitle();

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
			int doModal(HWND parent)
			{
				create(hive.instance, MAKEINTRESOURCE(IDD_CONFIG), parent);

				setUInt(IDC_FILL_COLOR, hive.fillColor);
				setUInt(IDC_BORDER_COLOR, hive.borderColor);
				setUInt(IDC_HOT_BORDER_COLOR, hive.hotBorderColor);
				setUInt(IDC_ACTIVE_CAPTION_COLOR, hive.activeCaptionColor);
				setUInt(IDC_ACTIVE_CAPTION_TEXT_COLOR, hive.activeCaptionTextColor);
				setUInt(IDC_INACTIVE_CAPTION_COLOR, hive.inactiveCaptionColor);
				setUInt(IDC_INACTIVE_CAPTION_TEXT_COLOR, hive.inactiveCaptionTextColor);
				setUInt(IDC_BORDER_WIDTH, Pane::borderWidth);
				setUInt(IDC_CAPTION_HEIGHT, Pane::captionHeight);
				setUInt(IDC_TAB_HEIGHT, Pane::tabHeight);
				setComboBox(IDC_TAB_MODE, Pane::tabMode, _T("タイトル"), _T("上"), _T("下"));
				setUInt(IDC_MENU_BREAK, hive.menuBreak);
				setCheck(IDC_USE_THEME, hive.useTheme);
				setCheck(IDC_FORCE_SCROLL, hive.forceScroll);
				setCheck(IDC_SHOW_PLAYER, hive.showPlayer);

				return doModal2(parent);
			}

			INT_PTR onDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
			{
				switch (message)
				{
				case WM_COMMAND:
					{
						UINT id = LOWORD(wParam);

						switch (id)
						{
						case IDC_FILL_COLOR:
						case IDC_BORDER_COLOR:
						case IDC_HOT_BORDER_COLOR:
						case IDC_ACTIVE_CAPTION_COLOR:
						case IDC_ACTIVE_CAPTION_TEXT_COLOR:
						case IDC_INACTIVE_CAPTION_COLOR:
						case IDC_INACTIVE_CAPTION_TEXT_COLOR:
							{
								HWND control = (HWND)lParam;

								COLORREF color = ::GetDlgItemInt(hwnd, id, 0, FALSE);

								static COLORREF customColors[16] = {};
								CHOOSECOLOR cc { sizeof(cc) };
								cc.hwndOwner = hwnd;
								cc.lpCustColors = customColors;
								cc.rgbResult = color;
								cc.Flags = CC_RGBINIT | CC_FULLOPEN;
								if (!::ChooseColor(&cc)) return TRUE;

								color = cc.rgbResult;

								::SetDlgItemInt(hwnd, id, color, FALSE);
								::InvalidateRect(control, 0, FALSE);

								return TRUE;
							}
						}

						break;
					}
				case WM_DRAWITEM:
					{
						UINT id = wParam;

						switch (id)
						{
						case IDC_FILL_COLOR:
						case IDC_BORDER_COLOR:
						case IDC_HOT_BORDER_COLOR:
						case IDC_ACTIVE_CAPTION_COLOR:
						case IDC_ACTIVE_CAPTION_TEXT_COLOR:
						case IDC_INACTIVE_CAPTION_COLOR:
						case IDC_INACTIVE_CAPTION_TEXT_COLOR:
							{
								DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lParam;

								COLORREF color = ::GetDlgItemInt(hwnd, id, 0, FALSE);

								HBRUSH brush = ::CreateSolidBrush(color);
								FillRect(dis->hDC, &dis->rcItem, brush);
								::DeleteObject(brush);

								return TRUE;
							}
						}

						break;
					}
				}

				return __super::onDlgProc(hwnd, message, wParam, lParam);
			}
		};
	}; inline auto mainWindow = std::make_shared<MainWindow>();
}
