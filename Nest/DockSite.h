#pragma once
#include "Hive.h"
#include "Pane/Pane.h"
#include "Shuttle/ShuttleManager.h"

namespace fgo::nest
{
	//
	// このクラスはシャトルをドッキングさせることができるウィンドウです。
	// 再帰分割可能なペインを保持し、このペインにシャトルをドッキングさせることができます。
	// 1つのペインに複数のシャトルをドッキングさせることもできます。(タブ化)
	//
	template<class T>
	struct DockSite : T
	{
		struct CommandID
		{
			static const UINT SPLIT_MODE_NONE = 1000;
			static const UINT SPLIT_MODE_VERT = 1001;
			static const UINT SPLIT_MODE_HORZ = 1002;
			static const UINT ORIGIN_TOP_LEFT = 1010;
			static const UINT ORIGIN_BOTTOM_RIGHT = 1011;
			static const UINT MOVE_TO_LEFT = 1012;
			static const UINT MOVE_TO_RIGHT = 1013;
			static const UINT IS_BORDER_LOCKED = 1014;
			static const UINT RENAME_SUB_WINDOW = 1015;
			static const UINT UNDOCK = 1016;
			static const UINT SHUTTLE = 2000;
		};

		inline static const LPCTSTR PropName = _T("RootPane");
		inline static std::shared_ptr<Pane> hotBorderPane;

		//
		// ルートペインを作成し、ウィンドウに関連付けます。
		//
		static void initRootPane(HWND hwnd)
		{
			auto p = new std::shared_ptr<Pane>(std::make_shared<Pane>(hwnd));
			::SetProp(hwnd, PropName, p);
		}

		//
		// ルートペインを削除し、ウィンドウとの関連付けを解除します。
		//
		static void exitRootPane(HWND hwnd)
		{
			// ルートペインを取得します。
			auto p = (std::shared_ptr<Pane>*)::GetProp(hwnd, PropName);

			// ルートペインをリセットします。
			// これにより、ドッキング中のすべてのシャトルがフローティング状態になり、
			// hwndが削除されたとしても、他のウィンドウが削除されずに済むようになります。
			(*p)->resetPane();

			// ルートペインを削除します。
			delete p;

			// ウィンドウとの関連付けを解除します。
			::RemoveProp(hwnd, PropName);
		}

		//
		// ルートペインを返します。
		//
		static std::shared_ptr<Pane> getRootPane(HWND hwnd)
		{
			auto p = (std::shared_ptr<Pane>*)::GetProp(hwnd, PropName);
			if (!p) return 0; // 取得できなかった場合は 0 を返します。
			return *p;
		}

		//
		// 指定されたウィンドウがこのウィンドウにドッキング可能な場合はTRUEを返します。
		//
		BOOL canDocking(HWND hwnd)
		{
			auto root = getRootPane(hwnd);
			if (root)
			{
				// hwndはペインホルダーなので、入れ子構造をチェックします。

				if (isAncestor(*this, hwnd))
				{
					// hwndはこのホルダーの祖先なのでドッキングできません。
					return FALSE;
				}
			}

			return TRUE;
		}

		//
		// ペインのレイアウトを再帰的に再計算します。
		//
		static void calcLayout(HWND hwnd)
		{
			MY_TRACE(_T("DockSite::calcLayout(0x%08X)\n"), hwnd);

			RECT rc; ::GetClientRect(hwnd, &rc);
			auto root = getRootPane(hwnd);
			if (root)
			{
				root->recalcLayout(&rc);
				::InvalidateRect(hwnd, 0, FALSE);
			}
		}

		//
		// メニューのコピーを作成します。
		//
		void copyMenu(HMENU dstMenu, HMENU srcMenu)
		{
			int c = ::GetMenuItemCount(srcMenu);

			for (int i = 0; i < c; i++)
			{
				TCHAR text[MAX_PATH] = {};
				MENUITEMINFO mii = { sizeof(mii) };
				mii.fMask = MIIM_BITMAP | MIIM_CHECKMARKS | MIIM_DATA | MIIM_FTYPE | MIIM_ID | MIIM_STATE | MIIM_STRING | MIIM_SUBMENU;
				mii.dwTypeData = text;
				mii.cch = MAX_PATH;
				::GetMenuItemInfo(srcMenu, i, TRUE, &mii);

				if (mii.hSubMenu)
				{
					HMENU subMenu = ::CreatePopupMenu();
					copyMenu(mii.hSubMenu, subMenu);
					mii.hSubMenu = subMenu;
				}

				::InsertMenuItem(dstMenu, i, TRUE, &mii);
			}
		}

		//
		// ターゲットのメニューを表示します。
		//
		BOOL showTargetMenu(HWND dockSite, POINT point)
		{
			MY_TRACE(_T("DockSite::showTargetMenu(%d, %d)\n"), point.x, point.y);

			// ルートペインを取得します。
			auto root = getRootPane(dockSite);
			if (!root) return FALSE;

			// 指定された座標からペインを取得します。
			auto pane = root->hitTestPane(point);
			if (!pane) return FALSE;

			// カレントシャトルを取得します。
			Shuttle* shuttle = pane->getCurrentShuttle();
			if (!shuttle) return FALSE;

			// 指定された座標がメニュー矩形の中にあるかチェックします。
			RECT rcMenu = pane->getMenuRect();
			if (!::PtInRect(&rcMenu, point)) return FALSE;

			// メニュー矩形からメニューを表示する座標を算出します。
			point.x = rcMenu.left;
			point.y = rcMenu.bottom;
			::ClientToScreen(dockSite, &point);

			// ターゲットを取得します。
			HWND hwnd = *shuttle;

			// ターゲットのメニューを取得します。
			HMENU srcMenu = ::GetMenu(hwnd);
			if (!srcMenu) return FALSE;

			// ポップアップメニューを作成します。
			HMENU dstMenu = ::CreatePopupMenu();
			copyMenu(dstMenu, srcMenu);

			// ポップアップメニューを表示します。
			::TrackPopupMenuEx(dstMenu, TPM_VERPOSANIMATION, point.x, point.y, hwnd, 0);

			// ポップアップメニューを削除します。
			::DestroyMenu(dstMenu);

			return TRUE;
		}

		struct Category
		{
			static const int None = 0;
			static const int Primary = 1;
			static const int Secondary = 2;
			static const int SubWindow = 3;
			static const int SubProcess = 4;
			static const int Ultimate = 5;
			static const int Filer = 6;
		};

		//
		// 指定されたシャトルのカテゴリを返します。
		//
		static int getCategory(Shuttle* shuttle)
		{
			if (_tcscmp(shuttle->name, _T("* AviUtl")) == 0) return Category::Primary;
			if (_tcscmp(shuttle->name, _T("* 拡張編集")) == 0) return Category::Primary;
			if (_tcscmp(shuttle->name, _T("* 設定ダイアログ")) == 0) return Category::Primary;
			if (_tcscmp(shuttle->name, _T("ぼかしフィルタ")) == 0) return Category::Primary;
			if (_tcscmp(shuttle->name, _T("クリッピング＆リサイズ")) == 0) return Category::Primary;
			if (_tcscmp(shuttle->name, _T("シャープフィルタ")) == 0) return Category::Primary;
			if (_tcscmp(shuttle->name, _T("ツールウィンドウ")) == 0) return Category::Primary;
			if (_tcscmp(shuttle->name, _T("ノイズ除去(時間軸)フィルタ")) == 0) return Category::Primary;
			if (_tcscmp(shuttle->name, _T("ノイズ除去フィルタ")) == 0) return Category::Primary;
			if (_tcscmp(shuttle->name, _T("ヒストグラム")) == 0) return Category::Primary;
			if (_tcscmp(shuttle->name, _T("偶数")) == 0) return Category::Primary;
			if (_tcscmp(shuttle->name, _T("再生ウィンドウ")) == 0) return Category::Primary;
			if (_tcscmp(shuttle->name, _T("奇数")) == 0) return Category::Primary;
			if (_tcscmp(shuttle->name, _T("拡張色調補正")) == 0) return Category::Primary;
			if (_tcscmp(shuttle->name, _T("縁塗りつぶし")) == 0) return Category::Primary;
			if (_tcscmp(shuttle->name, _T("自動24fps")) == 0) return Category::Primary;
			if (_tcscmp(shuttle->name, _T("色調補正フィルタ")) == 0) return Category::Primary;
			if (_tcscmp(shuttle->name, _T("音声の位置調整")) == 0) return Category::Primary;
			if (_tcscmp(shuttle->name, _T("音量の最大化")) == 0) return Category::Primary;
			if (_tcscmp(shuttle->name, _T("音量の調整")) == 0) return Category::Primary;
			if (_tcscmp(shuttle->name, _T("アルティメットプラグイン")) == 0) return Category::Ultimate;

			{
				TCHAR className[MAX_PATH] = {};
				::GetClassName(*shuttle, className, std::size(className));
				if (::StrCmpI(className, hive.SubWindowClassName) == 0) return Category::SubWindow;
				if (::StrCmpI(className, hive.SubProcessClassName) == 0) return Category::SubProcess;
			}

			{
				auto instance = (HINSTANCE)::GetWindowLongPtr(*shuttle, GWLP_HINSTANCE);
				MY_TRACE_HEX(instance);

				TCHAR fileName[MAX_PATH] = {};
				::GetModuleFileName(instance, fileName, std::size(fileName));
				MY_TRACE_TSTR(fileName);

				if (::StrCmpI(::PathFindExtension(fileName), _T(".aua")) == 0)
				{
					if (::StrCmpI(::PathFindFileName(fileName), _T("Filer.aua")) == 0)
					{
						if (!Share::Filer::HostWindow::getClientWindow(*shuttle))
							return Category::Filer;
					}

					return Category::Ultimate;
				}
			}

			return Category::Secondary;
		}

		// ペイン操作用のメニューを表示します。
		void showPaneMenu(HWND dockSite)
		{
			// ルートペインを取得します。
			auto root = getRootPane(dockSite);
			if (!root) return;

			// カーソル座標を取得して、クライアント座標に変換します。
			POINT cursorPos; ::GetCursorPos(&cursorPos);
			POINT point = cursorPos;
			::ScreenToClient(dockSite, &point);

			// カーソル座標にヒットするペインを取得します。
			auto pane = root->hitTestPane(point);
			if (!pane) return;

			POINT point2 = point;
			::MapWindowPoints(dockSite, pane->owner, &point2, 1);
			BOOL inCaption = pane->hitTestCaption(point2);

			int c = pane->getTabCount();
			int ht = pane->hitTest(point);

			// シャトルを取得します。取得できない場合もあります。
			Shuttle* shuttle = 0;
			if (ht != -1)
				shuttle = pane->getShuttle(ht);
			else if (inCaption)
				shuttle = pane->getShuttle(pane->getCurrentIndex());

			// ターゲットがペインホルダーならそのウィンドウハンドルを取得します。
			HWND subWindow = 0;
			if (shuttle && getRootPane(*shuttle))
				subWindow = *shuttle;

			// メニューを作成します。
			HMENU menu = ::CreatePopupMenu();
			HMENU primaryMenu = ::CreatePopupMenu();
			HMENU secondaryMenu = ::CreatePopupMenu();
			HMENU subWindowMenu = ::CreatePopupMenu();
			HMENU subProcessMenu = ::CreatePopupMenu();
			HMENU ultimateMenu = ::CreatePopupMenu();
			HMENU filerMenu = ::CreatePopupMenu();

			::AppendMenu(menu, MF_POPUP, (UINT)primaryMenu, _T("プライマリ"));
			::AppendMenu(menu, MF_POPUP, (UINT)secondaryMenu, _T("セカンダリ"));
			::AppendMenu(menu, MF_POPUP, (UINT)subWindowMenu, _T("サブウィンドウ"));
			::AppendMenu(menu, MF_POPUP, (UINT)subProcessMenu, _T("サブプロセス"));
			::AppendMenu(menu, MF_POPUP, (UINT)ultimateMenu, _T("アルティメット"));
			::AppendMenu(menu, MF_POPUP, (UINT)filerMenu, _T("ファイラ"));
			::AppendMenu(menu, MF_SEPARATOR, -1, 0);
			::AppendMenu(menu, MF_STRING, CommandID::SPLIT_MODE_NONE, _T("分割なし"));
			::AppendMenu(menu, MF_STRING, CommandID::SPLIT_MODE_VERT, _T("垂直線で分割"));
			::AppendMenu(menu, MF_STRING, CommandID::SPLIT_MODE_HORZ, _T("水平線で分割"));
			::AppendMenu(menu, MF_SEPARATOR, -1, 0);
			::AppendMenu(menu, MF_STRING, CommandID::ORIGIN_TOP_LEFT, _T("左上を原点にする"));
			::AppendMenu(menu, MF_STRING, CommandID::ORIGIN_BOTTOM_RIGHT, _T("右下を原点にする"));
			::AppendMenu(menu, MF_SEPARATOR, -1, 0);
			::AppendMenu(menu, MF_STRING, CommandID::MOVE_TO_LEFT, _T("左に移動する"));
			if (ht == -1 || ht <= 0)
				::EnableMenuItem(menu, CommandID::MOVE_TO_LEFT, MF_GRAYED | MF_DISABLED);
			::AppendMenu(menu, MF_STRING, CommandID::MOVE_TO_RIGHT, _T("右に移動する"));
			if (ht == -1 || ht >= c - 1)
				::EnableMenuItem(menu, CommandID::MOVE_TO_RIGHT, MF_GRAYED | MF_DISABLED);
			::AppendMenu(menu, MF_STRING, CommandID::IS_BORDER_LOCKED, _T("ボーダーをロック"));
			if (pane->isBorderLocked)
				::CheckMenuItem(menu, CommandID::IS_BORDER_LOCKED, MF_CHECKED);
			::AppendMenu(menu, MF_SEPARATOR, -1, 0);
			::AppendMenu(menu, MF_STRING, CommandID::UNDOCK, _T("ドッキングを解除"));
			if (c == 0) // ドッキングしているシャトルが存在しない場合はこのメニューアイテムを無効化します。
				::EnableMenuItem(menu, CommandID::UNDOCK, MF_DISABLED | MF_GRAYED);
			::AppendMenu(menu, MF_STRING, CommandID::RENAME_SUB_WINDOW, _T("名前を変更"));
			if (!subWindow) // ペインのオーナーがサブウィンドウではない場合はこのメニューアイテムを無効化します。
				::EnableMenuItem(menu, CommandID::RENAME_SUB_WINDOW, MF_GRAYED | MF_DISABLED);

			{
				// シャトルをメニューに追加します。
				UINT id = CommandID::SHUTTLE;
				for (auto& pair : shuttleManager.shuttles)
				{
					auto& shuttle = pair.second;

					// 追加先メニューを判定します。
					HMENU menu = primaryMenu;
					switch (getCategory(shuttle.get()))
					{
					case Category::Primary: menu = primaryMenu; break;
					case Category::Secondary: menu = secondaryMenu; break;
					case Category::SubWindow: menu = subWindowMenu; break;
					case Category::SubProcess: menu = subProcessMenu; break;
					case Category::Ultimate: menu = ultimateMenu; break;
					case Category::Filer: menu = filerMenu; break;
					}

					// メニューアイテムを追加します。
					::AppendMenu(menu, MF_STRING, id, shuttle->name);

					// シャトルのタブが存在するなら
					if (pane->findTab(shuttle.get()) != -1)
					{
						// メニューアイテムにチェックを入れます。
						::CheckMenuItem(menu, id, MF_CHECKED);
					}

					// ドッキング不可能なら
					if (!canDocking(*shuttle))
					{
						// メニューアイテムを無効にします。
						::EnableMenuItem(menu, id, MF_DISABLED | MF_GRAYED);
					}

					// IDをインクリメントします。
					id++;
				}
			}

			switch (pane->splitMode)
			{
			case Pane::SplitMode::none: ::CheckMenuItem(menu, CommandID::SPLIT_MODE_NONE, MF_CHECKED); break;
			case Pane::SplitMode::vert: ::CheckMenuItem(menu, CommandID::SPLIT_MODE_VERT, MF_CHECKED); break;
			case Pane::SplitMode::horz: ::CheckMenuItem(menu, CommandID::SPLIT_MODE_HORZ, MF_CHECKED); break;
			}

			switch (pane->origin)
			{
			case Pane::Origin::topLeft: ::CheckMenuItem(menu, CommandID::ORIGIN_TOP_LEFT, MF_CHECKED); break;
			case Pane::Origin::bottomRight: ::CheckMenuItem(menu, CommandID::ORIGIN_BOTTOM_RIGHT, MF_CHECKED); break;
			}

			int id = ::TrackPopupMenu(menu, TPM_NONOTIFY | TPM_RETURNCMD, cursorPos.x, cursorPos.y, 0, dockSite, 0);

			if (id)
			{
				switch (id)
				{
				case CommandID::SPLIT_MODE_NONE: pane->setSplitMode(Pane::SplitMode::none); break;
				case CommandID::SPLIT_MODE_VERT: pane->setSplitMode(Pane::SplitMode::vert); break;
				case CommandID::SPLIT_MODE_HORZ: pane->setSplitMode(Pane::SplitMode::horz); break;

				case CommandID::ORIGIN_TOP_LEFT: pane->origin = Pane::Origin::topLeft; break;
				case CommandID::ORIGIN_BOTTOM_RIGHT: pane->origin = Pane::Origin::bottomRight; break;

				case CommandID::MOVE_TO_LEFT: pane->moveTab(ht, ht - 1); break;
				case CommandID::MOVE_TO_RIGHT: pane->moveTab(ht, ht + 1); break;

				case CommandID::IS_BORDER_LOCKED: pane->isBorderLocked = !pane->isBorderLocked; break;

				case CommandID::RENAME_SUB_WINDOW: shuttleManager.showRenameDialog(shuttle, dockSite); break;

				case CommandID::UNDOCK:
					{
						// ドッキングを解除します。

						// カーソルの位置にあるタブアイテムのインデックスです。
						int index = ht;

						// インデックスが無効の場合はカレントインデックスを使用します。
						if (index == -1)
							index = pane->getCurrentIndex();

						// インデックスが有効の場合は
						if (index != -1)
						{
							// ドッキングしているシャトルを取得します。
							Shuttle* shuttle = pane->getShuttle(index);
							if (shuttle) // ドッキングしているシャトルが存在する場合は
								pane->releaseShuttle(shuttle); // ドッキングを解除します。
						}

						break;
					}
				}

				if (id > CommandID::SHUTTLE)
				{
					// ユーザーが指定したシャトルをクリックされたペインにドッキングさせます。

					// メニューアイテムのテキストを取得します。
					TCHAR text[MAX_PATH] = {};
					::GetMenuString(menu, id, text, MAX_PATH, MF_BYCOMMAND);
					MY_TRACE_TSTR(text);

					// テキストからシャトルを取得します。ドッキングできるかチェックもします。
					auto shuttle = shuttleManager.get(text);
					if (shuttle && canDocking(*shuttle))
					{
						// ペインにシャトルを追加します。
						// これにより、シャトルがペインにドッキングされます。
						int index = pane->addShuttle(shuttle.get(), ht);
						if (index != -1) // ドッキングに成功した場合は-1以外になります。
						{
							// ドッキングしたシャトルをカレントにしてペインをリフレッシュします。
							pane->setCurrentIndex(index);
							pane->refresh(FALSE);
						}
					}
				}

				// ペインの状態が変更されたのでレイアウトを再計算します。
				pane->recalcLayout();

				// ペインのレイアウトが更新されたので再描画します。
				::InvalidateRect(dockSite, 0, FALSE);
			}

			// メニューを削除します。
			::DestroyMenu(menu); // 再帰的にサブメニューも削除されます。
		}

		//
		// ウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch (message)
			{
			case WM_NOTIFY: // ペインのタブから通知されます。
				{
					NMHDR* header = (NMHDR*)lParam;

					switch (header->code)
					{
					case NM_RCLICK:
						{
							// タブ上で右クリックが発生した場合はペインメニューを表示します。
							showPaneMenu(hwnd);

							break;
						}
					case TCN_SELCHANGE:
						{
							// タブアイテムが切り替わった場合はペインをリフレッシュします。
							Pane* pane = Pane::getPane(header->hwndFrom);
							if (pane)
								pane->refresh(FALSE);

							break;
						}
					}

					break;
				}
			case WM_CREATE:
				{
					MY_TRACE(_T("DockSite::onWndProc(WM_CREATE, 0x%08X, 0x%08X)\n"), wParam, lParam);

					// ルートペインを作成します。
					initRootPane(hwnd);

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE(_T("DockSite::onWndProc(WM_DESTROY, 0x%08X, 0x%08X)\n"), wParam, lParam);

					// ルートペインを削除します。
					exitRootPane(hwnd);

					break;
				}
			case WM_SIZE:
				{
					calcLayout(hwnd);

					break;
				}
			case WM_SHOWWINDOW:
				{
					MY_TRACE(_T("DockSite::onWndProc(WM_SHOWWINDOW, 0x%08X, 0x%08X)\n"), wParam, lParam);

					if (wParam)
					{
						auto root = getRootPane(hwnd);

						root->refresh(TRUE);
					}

					break;
				}
			case WM_PAINT:
				{
					PaintDC pdc(hwnd);
					RECT rc = pdc.m_ps.rcPaint;
					UxDC dc(pdc, &rc);

					if (!dc.isValid())
						return 0;

					auto root = getRootPane(hwnd);

					{
						// 背景を塗りつぶします。

						hive.fillBackground(dc, &rc);
					}

					{
						// ボーダーを再帰的に描画します。

						HBRUSH brush = ::CreateSolidBrush(hive.borderColor);
						root->drawBorder(dc, brush);
						::DeleteObject(brush);
					}

					{
						// ホットボーダーの矩形を取得できる場合は
						RECT rcHotBorder;
						if (hotBorderPane &&
							hotBorderPane->owner == hwnd &&
							hotBorderPane->getBorderRect(&rcHotBorder))
						{
							// ホットボーダーを描画します。
							hive.fillBackground(dc, &rcHotBorder, hive.hotBorderColor, WP_BORDER, CS_ACTIVE);
						}
					}

					{
						// 各ウィンドウのキャプションを再帰的に描画します。

						LOGFONTW lf = {};
						::GetThemeSysFont(hive.theme, TMT_CAPTIONFONT, &lf);
						HFONT font = ::CreateFontIndirectW(&lf);
						HFONT oldFont = (HFONT)::SelectObject(dc, font);

						root->drawCaption(dc);

						::SelectObject(dc, oldFont);
						::DeleteObject(font);
					}

					return 0;
				}
			case WM_SETCURSOR:
				{
					// マウスカーソルがボーダーの上にある場合はマウスカーソルを変更します。

					if (hwnd != (HWND)wParam) break;

					auto root = getRootPane(hwnd);

					POINT point; ::GetCursorPos(&point);
					::ScreenToClient(hwnd, &point);

					auto borderPane = root->hitTestBorder(point);
					if (!borderPane) break;

					switch (borderPane->splitMode)
					{
					case Pane::SplitMode::vert:
						{
							::SetCursor(::LoadCursor(0, IDC_SIZEWE));

							return TRUE;
						}
					case Pane::SplitMode::horz:
						{
							::SetCursor(::LoadCursor(0, IDC_SIZENS));

							return TRUE;
						}
					}

					break;
				}
			case WM_LBUTTONDOWN:
				{
					MY_TRACE(_T("DockSite::onWndProc(WM_LBUTTONDOWN, 0x%08X, 0x%08X)\n"), wParam, lParam);

					// マウス座標を取得します。
					POINT point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

					// 必要であればターゲットウィンドウのメニューを表示します。
					if (showTargetMenu(hwnd, point))
						break; // メニューの表示が行われた場合はここで処理を終わります。

					// ルートペインを取得します。
					auto root = getRootPane(hwnd);

					// マウス座標にあるボーダーを取得します。
					hotBorderPane = root->hitTestBorder(point);

					// ボーダーが有効かチェックします。
					if (hotBorderPane)
					{
						// オフセットを取得します。
						hotBorderPane->dragOffset = hotBorderPane->getDragOffset(point);

						// マウスキャプチャを開始します。
						::SetCapture(hwnd);

						// 再描画します。
						hotBorderPane->invalidate();

						break;
					}

					{
						// マウス座標にあるペインを取得できたら
						auto pane = root->hitTestPane(point);
						if (pane)
						{
							// クリックされたペインがシャトルを持っているなら
							Shuttle* shuttle = pane->getCurrentShuttle();
							if (shuttle)
								::SetFocus(*shuttle); // そのシャトルにフォーカスを当てます。
						}
					}

					break;
				}
			case WM_LBUTTONUP:
				{
					MY_TRACE(_T("DockSite::onWndProc(WM_LBUTTONUP, 0x%08X, 0x%08X)\n"), wParam, lParam);

					// マウス座標を取得します。
					POINT point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

					// マウスをキャプチャ中かチェックします。
					if (::GetCapture() == hwnd)
					{
						// マウスキャプチャを終了します。
						::ReleaseCapture();

						if (hotBorderPane)
						{
							// ボーダーを動かします。
							hotBorderPane->dragBorder(point);

							// レイアウトを再計算します。
							hotBorderPane->recalcLayout();

							// 再描画します。
							hotBorderPane->invalidate();
						}
					}

					break;
				}
			case WM_RBUTTONDOWN:
				{
					MY_TRACE(_T("DockSite::onWndProc(WM_RBUTTONDOWN, 0x%08X, 0x%08X)\n"), wParam, lParam);

					// ペイン編集用メニューを表示します。
					showPaneMenu(hwnd);

					break;
				}
			case WM_MOUSEMOVE:
				{
					// マウス座標を取得します。
					POINT point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

					// マウスをキャプチャ中かチェックします。
					if (::GetCapture() == hwnd)
					{
						if (hotBorderPane)
						{
							// ボーダーを動かします。
							hotBorderPane->dragBorder(point);

							// レイアウトを再計算します。
							hotBorderPane->recalcLayout();

							// 再描画します。
							hotBorderPane->invalidate();
						}
					}
					else
					{
						auto root = getRootPane(hwnd);

						// マウス座標にあるボーダーを取得します。
						auto pane = root->hitTestBorder(point);

						// 現在のホットボーダーと違うなら
						if (hotBorderPane != pane)
						{
							// ホットボーダーを更新します。
							hotBorderPane = pane;

							// 再描画します。
							::InvalidateRect(hwnd, 0, FALSE);
						}

						// マウスリーブイベントをトラックします。
						TRACKMOUSEEVENT tme = { sizeof(tme) };
						tme.dwFlags = TME_LEAVE;
						tme.hwndTrack = hwnd;
						::TrackMouseEvent(&tme);
					}

					break;
				}
			case WM_MOUSELEAVE:
				{
					MY_TRACE(_T("DockSite::onWndProc(WM_MOUSELEAVE, 0x%08X, 0x%08X)\n"), wParam, lParam);

					// ホットボーダーが存在する場合は
					if (hotBorderPane)
					{
						// ホットボーダーを再描画します。
						hotBorderPane->invalidate();

						// ホットボーダーを無効にします。
						hotBorderPane = 0;
					}

					break;
				}
			case WM_MOUSEWHEEL:
				{
					MY_TRACE(_T("DockSite::onWndProc(WM_MOUSEWHEEL, 0x%08X, 0x%08X)\n"), wParam, lParam);

					// タブ上でマウスホイールが発生した場合はカレントタブを切り替えます。

					int delta = (short)HIWORD(wParam);
					POINT point = LP2PT(lParam);
					::MapWindowPoints(0, hwnd, &point, 1);
					HWND child = ::ChildWindowFromPointEx(hwnd, point,
						CWP_SKIPDISABLED | CWP_SKIPINVISIBLE | CWP_SKIPTRANSPARENT);

					Pane* pane = Pane::getPane(child);
					if (pane) // childがタブならペインを取得できます。
					{
						int c = pane->getTabCount();
						int current = pane->getCurrentIndex();

						if (delta > 0)
						{
							current--;
						}
						else
						{
							current++;
						}

						current = std::max<int>(current, 0);
						current = std::min<int>(current, c - 1);

						pane->setCurrentIndex(current);
						pane->refresh(FALSE);

						return 0;
					}

					break;
				}
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}
	};
}
