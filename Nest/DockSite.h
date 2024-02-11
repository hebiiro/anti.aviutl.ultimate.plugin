#pragma once
#include "Hive.h"
#include "Pane/RootPane.h"
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
		struct CommandID {
			struct SplitMode {
				static const UINT None = 1000;
				static const UINT Vert = 1001;
				static const UINT Horz = 1002;
			};
			struct Origin {
				static const UINT TopLeft = 1010;
				static const UINT BottomRight = 1011;
			};
			struct CaptionMode {
				static const UINT Hide = 1020;
				static const UINT Show = 1021;
			};
			static const UINT IsBorderLocked = 1100;
			static const UINT RenameSubWindow = 1101;
			static const UINT MoveToLeft = 1102;
			static const UINT MoveToRight = 1103;
			static const UINT Undock = 1104;
			struct Shuttle {
				static const UINT Begin = 2000;
				static const UINT End = 3000;
			};
		};

		inline static const LPCTSTR PropName = _T("RootPane");
		inline static std::shared_ptr<Pane> hotBorderPane;

		LPARAM mouseMovePos = -1; // WM_MOUSEMOVEのLPARAMです。

		//
		// ルートペインを作成し、ウィンドウに関連付けます。
		//
		static void initRootPane(HWND hwnd)
		{
			auto p = new std::shared_ptr<RootPane>(std::make_shared<RootPane>(hwnd));
			::SetProp(hwnd, PropName, p);
		}

		//
		// ルートペインを削除し、ウィンドウとの関連付けを解除します。
		//
		static void exitRootPane(HWND hwnd)
		{
			// ルートペインを取得します。
			auto p = (std::shared_ptr<RootPane>*)::GetProp(hwnd, PropName);

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
		static std::shared_ptr<RootPane> getRootPane(HWND hwnd)
		{
			auto p = (std::shared_ptr<RootPane>*)::GetProp(hwnd, PropName);
			if (!p) return 0; // 取得できなかった場合は0を返します。
			return *p;
		}

		//
		// 最初のペインを返します。
		// ルートペインではなく、最大化されたペインの場合もあります。
		//
		static std::shared_ptr<Pane> getFirstPane(HWND hwnd)
		{
			auto root = getRootPane(hwnd);
			if (!root) return 0; // 取得できなかった場合は0を返します。
			auto maximizedPane = root->getMaximizedPane();
			if (maximizedPane) return maximizedPane; // 最大化されたペインがあれば、それを返します。
			return root;
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
		void calcLayout(HWND hwnd)
		{
//			MY_TRACE_FUNC("0x%08X", hwnd);

			auto root = getRootPane(hwnd);
			if (root)
			{
				RECT rc; ::GetClientRect(hwnd, &rc);
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
					copyMenu(subMenu, mii.hSubMenu);
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
			MY_TRACE_FUNC("%d, %d", point.x, point.y);

			// 最初のペインを取得します。
			auto first = getFirstPane(dockSite);
			if (!first) return FALSE;

			// 指定された座標からペインを取得します。
			auto pane = first->hitTestPane(point);
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
			static constexpr int None = -1;
			static constexpr int Primary = 0;
			static constexpr int Secondary = 1;
			static constexpr int SubWindow = 2;
			static constexpr int SubProcess = 3;
			static constexpr int Ultimate = 4;
			static constexpr int Filer = 5;
			static constexpr int MaxSize = 6;
			inline static constexpr LPCTSTR labels[] = {
				_T("プライマリ"),
				_T("セカンダリ"),
				_T("サブウィンドウ"),
				_T("サブプロセス"),
				_T("アルティメット"),
				_T("ファイラ"),
			};
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

		//
		// ペイン操作用のメニューを表示します。
		//
		void showPaneMenu(HWND dockSite)
		{
			// ルートペインを取得します。
			auto root = getRootPane(dockSite);
			if (!root || root->getMaximizedPane()) return;

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
			BOOL isSubWindow = shuttle && getRootPane(*shuttle);

			// メニューを作成します。
			HMENU menu = ::CreatePopupMenu();

			// サブメニューを作成します。
			HMENU subMenu[Category::MaxSize] = {};
			for (size_t i = 0; i < std::size(subMenu); i++)
			{
				subMenu[i] = ::CreatePopupMenu();

				::AppendMenu(menu, MF_POPUP, (UINT)subMenu[i], Category::labels[i]);
			}

			::AppendMenu(menu, MF_SEPARATOR, -1, 0);
			::AppendMenu(menu, MF_STRING, CommandID::SplitMode::None, _T("分割なし"));
			::AppendMenu(menu, MF_STRING, CommandID::SplitMode::Vert, _T("垂直線で分割"));
			::AppendMenu(menu, MF_STRING, CommandID::SplitMode::Horz, _T("水平線で分割"));
			switch (pane->splitMode) {
			case Pane::SplitMode::None: ::CheckMenuItem(menu, CommandID::SplitMode::None, MF_CHECKED); break;
			case Pane::SplitMode::Vert: ::CheckMenuItem(menu, CommandID::SplitMode::Vert, MF_CHECKED); break;
			case Pane::SplitMode::Horz: ::CheckMenuItem(menu, CommandID::SplitMode::Horz, MF_CHECKED); break;
			}
			::AppendMenu(menu, MF_SEPARATOR, -1, 0);
			::AppendMenu(menu, MF_STRING, CommandID::Origin::TopLeft, _T("左上を原点にする"));
			::AppendMenu(menu, MF_STRING, CommandID::Origin::BottomRight, _T("右下を原点にする"));
			switch (pane->origin) {
			case Pane::Origin::TopLeft: ::CheckMenuItem(menu, CommandID::Origin::TopLeft, MF_CHECKED); break;
			case Pane::Origin::BottomRight: ::CheckMenuItem(menu, CommandID::Origin::BottomRight, MF_CHECKED); break;
			}
			::AppendMenu(menu, MF_SEPARATOR, -1, 0);
			::AppendMenu(menu, MF_STRING, CommandID::CaptionMode::Show, _T("キャプションを表示する"));
			::AppendMenu(menu, MF_STRING, CommandID::CaptionMode::Hide, _T("キャプションを表示しない"));
			switch (pane->captionMode) {
			case Pane::CaptionMode::Hide: ::CheckMenuItem(menu, CommandID::CaptionMode::Hide, MF_CHECKED); break;
			case Pane::CaptionMode::Show: ::CheckMenuItem(menu, CommandID::CaptionMode::Show, MF_CHECKED); break;
			}
			::AppendMenu(menu, MF_SEPARATOR, -1, 0);
			::AppendMenu(menu, MF_STRING, CommandID::MoveToLeft, _T("左に移動する"));
			if (ht == -1 || ht <= 0)
				::EnableMenuItem(menu, CommandID::MoveToLeft, MF_GRAYED | MF_DISABLED);
			::AppendMenu(menu, MF_STRING, CommandID::MoveToRight, _T("右に移動する"));
			if (ht == -1 || ht >= c - 1)
				::EnableMenuItem(menu, CommandID::MoveToRight, MF_GRAYED | MF_DISABLED);
			::AppendMenu(menu, MF_STRING, CommandID::IsBorderLocked, _T("ボーダーをロックする"));
			if (pane->isBorderLocked)
				::CheckMenuItem(menu, CommandID::IsBorderLocked, MF_CHECKED);
			::AppendMenu(menu, MF_STRING, CommandID::RenameSubWindow, _T("名前を変更"));
			if (!isSubWindow) // ペインのオーナーがサブウィンドウではない場合はこのメニューアイテムを無効化します。
				::EnableMenuItem(menu, CommandID::RenameSubWindow, MF_GRAYED | MF_DISABLED);
			::AppendMenu(menu, MF_SEPARATOR, -1, 0);
			::AppendMenu(menu, MF_STRING, CommandID::Undock, _T("ドッキングを解除"));
			if (c == 0) // ドッキングしているシャトルが存在しない場合はこのメニューアイテムを無効化します。
				::EnableMenuItem(menu, CommandID::Undock, MF_DISABLED | MF_GRAYED);

			{
				// シャトルをメニューに追加します。
				UINT id = CommandID::Shuttle::Begin;
				for (auto& pair : shuttleManager.shuttles)
				{
					auto& shuttle = pair.second;

					// シャトルのカテゴリを取得します。
					int category = getCategory(shuttle.get());
					if (category == Category::None) continue;

					// 追加先メニューを取得します。
					HMENU menu = subMenu[category];

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

			int id = ::TrackPopupMenu(menu, TPM_NONOTIFY | TPM_RETURNCMD, cursorPos.x, cursorPos.y, 0, dockSite, 0);

			if (id)
			{
				switch (id)
				{
				case CommandID::SplitMode::None: pane->setSplitMode(Pane::SplitMode::None); break;
				case CommandID::SplitMode::Vert: pane->setSplitMode(Pane::SplitMode::Vert); break;
				case CommandID::SplitMode::Horz: pane->setSplitMode(Pane::SplitMode::Horz); break;

				case CommandID::Origin::TopLeft: pane->setOrigin(Pane::Origin::TopLeft); break;
				case CommandID::Origin::BottomRight: pane->setOrigin(Pane::Origin::BottomRight); break;

				case CommandID::CaptionMode::Hide: pane->setCaptionMode(Pane::CaptionMode::Hide); break;
				case CommandID::CaptionMode::Show: pane->setCaptionMode(Pane::CaptionMode::Show); break;

				case CommandID::MoveToLeft: pane->moveTab(ht, ht - 1); break;
				case CommandID::MoveToRight: pane->moveTab(ht, ht + 1); break;

				case CommandID::IsBorderLocked: pane->isBorderLocked = !pane->isBorderLocked; break;

				case CommandID::RenameSubWindow: shuttleManager.showRenameDialog(shuttle, dockSite); break;

				case CommandID::Undock:
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

				if (id >= CommandID::Shuttle::Begin && id <= CommandID::Shuttle::End)
				{
					// ユーザーが指定したシャトルをクリックされたペインにドッキングさせます。

					// メニューアイテムのテキストを取得します。
					TCHAR text[MAX_PATH] = {};
					::GetMenuString(menu, id, text, std::size(text), MF_BYCOMMAND);
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

					MY_TRACE_FUNC("WM_NOTIFY, %d", header->code);

					switch (header->code)
					{
					case NM_DBLCLK:
						{
							auto pane = Pane::getPane(header->hwndFrom);
							if (!pane) break;

							// ルートペインを取得します。
							auto root = getRootPane(hwnd);

							// 最大化されたペインが存在するなら
							if (root->getMaximizedPane().get() == pane)
							{
								// 最大化されたペインをリセットします。
								root->setMaximizedPane(0);
							}
							else
							{
								// 最大化されたペインにセットします。
								root->setMaximizedPane(pane->shared_from_this());
							}

							// レイアウトを再計算します。
							calcLayout(hwnd);

							// ペインをリフレッシュします。
							root->refresh(TRUE);

							break;
						}
					case NM_RCLICK:
						{
							// タブ上で右クリックが発生した場合はペインメニューを表示します。
							showPaneMenu(hwnd);

							break;
						}
					case TCN_SELCHANGE:
						{
							// タブアイテムが切り替わった場合はペインをリフレッシュします。
							auto pane = Pane::getPane(header->hwndFrom);
							if (pane)
							{
								pane->refresh(FALSE);

								// ペインがシャトルを持っているなら
								Shuttle* shuttle = pane->getCurrentShuttle();
								if (shuttle)
									::SetFocus(*shuttle); // そのシャトルにフォーカスを当てます。
							}

							break;
						}
					}

					break;
				}
			case WM_CREATE:
				{
					MY_TRACE_FUNC("WM_CREATE, 0x%08X, 0x%08X", wParam, lParam);

					// ルートペインを作成します。
					initRootPane(hwnd);

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, 0x%08X, 0x%08X", wParam, lParam);

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
					MY_TRACE_FUNC("WM_SHOWWINDOW, 0x%08X, 0x%08X", wParam, lParam);

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

					// 最初のペインを取得します。
					auto first = getFirstPane(hwnd);
					if (!first) return 0;

					{
						// 背景を塗りつぶします。

						hive.fillBackground(dc, &rc);
					}

					{
						// ボーダーを再帰的に描画します。

						HBRUSH brush = ::CreateSolidBrush(hive.borderColor);
						first->drawBorder(dc, brush);
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

						first->drawCaption(dc);

						::SelectObject(dc, oldFont);
						::DeleteObject(font);
					}

					return 0;
				}
			case WM_SETCURSOR:
				{
					// マウスカーソルがボーダーの上にある場合はマウスカーソルを変更します。

					if (hwnd != (HWND)wParam) break;

					// ルートペインを取得します。
					auto root = getRootPane(hwnd);
					if (!root || root->getMaximizedPane()) break;

					POINT point; ::GetCursorPos(&point);
					::ScreenToClient(hwnd, &point);

					auto borderPane = root->hitTestBorder(point);
					if (!borderPane) break;

					switch (borderPane->splitMode)
					{
					case Pane::SplitMode::Vert:
						{
							::SetCursor(::LoadCursor(0, IDC_SIZEWE));

							return TRUE;
						}
					case Pane::SplitMode::Horz:
						{
							::SetCursor(::LoadCursor(0, IDC_SIZENS));

							return TRUE;
						}
					}

					break;
				}
			case WM_LBUTTONDOWN:
				{
					MY_TRACE_FUNC("WM_LBUTTONDOWN, 0x%08X, 0x%08X", wParam, lParam);

					// マウス座標を取得します。
					POINT point = LP2PT(lParam);

					// 必要であればターゲットウィンドウのメニューを表示します。
					if (showTargetMenu(hwnd, point))
						break; // メニューの表示が行われた場合はここで処理を終わります。

					// 最初のペインを取得します。
					auto first = getFirstPane(hwnd);
					if (!first) break;

					// マウス座標にあるボーダーを取得します。
					hotBorderPane = first->hitTestBorder(point);

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
						auto pane = first->hitTestPane(point);
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
					MY_TRACE_FUNC("WM_LBUTTONUP, 0x%08X, 0x%08X", wParam, lParam);

					// マウス座標を取得します。
					POINT point = LP2PT(lParam);

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
			case WM_LBUTTONDBLCLK:
				{
					MY_TRACE_FUNC("WM_LBUTTONDBLCLK, 0x%08X, 0x%08X", wParam, lParam);

					// ルートペインを取得します。
					auto root = getRootPane(hwnd);

					// 最大化されたペインが存在するなら
					if (root->getMaximizedPane())
					{
						// 最大化されたペインをリセットします。
						root->setMaximizedPane(0);
					}
					else
					{
						// マウス座標を取得します。
						POINT point = LP2PT(lParam);

						// マウス座標にあるキャプションのペインを取得します。
						auto pane = root->hitTestPane(point);
						if (!pane || !pane->hitTestCaption(point)) break;

						// キャプションをダブルクリックされたペインを
						// 最大化されたペインにセットします。
						root->setMaximizedPane(pane);
					}

					// レイアウトを再計算します。
					calcLayout(hwnd);

					// ペインをリフレッシュします。
					root->refresh(TRUE);

					break;
				}
			case WM_RBUTTONDOWN:
				{
					MY_TRACE_FUNC("WM_RBUTTONDOWN, 0x%08X, 0x%08X", wParam, lParam);

					// ペイン編集用メニューを表示します。
					showPaneMenu(hwnd);

					break;
				}
			case WM_MOUSEMOVE:
				{
					// 同じ座標のWM_MOUSEMOVEが大量に送られてくるので間引きます。
					if (mouseMovePos == lParam) break;
					mouseMovePos = lParam;

//					MY_TRACE_FUNC("WM_MOUSEMOVE, 0x%08X, 0x%08X", wParam, lParam);

					// マウス座標を取得します。
					POINT point = LP2PT(lParam);

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
						// 最初のペインを取得します。
						auto first = getFirstPane(hwnd);
						if (!first) break;

						// マウス座標にあるボーダーを取得します。
						auto pane = first->hitTestBorder(point);

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
//					MY_TRACE_FUNC("WM_MOUSELEAVE, 0x%08X, 0x%08X", wParam, lParam);

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
					MY_TRACE_FUNC("WM_MOUSEWHEEL, 0x%08X, 0x%08X", wParam, lParam);

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
