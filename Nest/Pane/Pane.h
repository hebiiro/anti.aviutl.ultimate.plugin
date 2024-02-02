﻿#pragma once
#include "Hive.h"
#include "Tab.h"
#include "Shuttle/Shuttle.h"

namespace fgo::nest
{
	struct Pane : Shuttle::Listener, std::enable_shared_from_this<Pane>
	{
		struct SplitMode
		{
			static const int none = 0;
			static const int vert = 1;
			static const int horz = 2;

			inline static const Hive::Label label[] = 
			{
				{ L"none", none },
				{ L"vert", vert },
				{ L"horz", horz },
			};
		};

		struct Origin
		{
			static const int topLeft = 0;
			static const int bottomRight = 1;

			inline static const Hive::Label label[] = 
			{
				{ L"topLeft", topLeft },
				{ L"bottomRight", bottomRight },
			};
		};

		struct TabMode
		{
			static const int title = 0;
			static const int top = 1;
			static const int bottom = 2;

			inline static const Hive::Label label[] =
			{
				{ L"title", title },
				{ L"top", top },
				{ L"bottom", bottom },
			};
		};

		inline static const LPCTSTR PropName = _T("Nest.Pane");
		inline static int borderWidth = 8;
		inline static int captionHeight = 24;
		inline static int tabHeight = 24;
		inline static int tabMode = TabMode::bottom;

		HWND owner = 0;
		RECT position = {};
		int splitMode = SplitMode::none;
		int origin = Origin::bottomRight;
		BOOL isBorderLocked = FALSE;
		int border = 0; // ボーダーの位置です。
		int dragOffset = 0; // ドラッグ処理に使用します。

		Tab tab;
		std::shared_ptr<Pane> children[2];

		int thisCaptionHeight = 0; // シャトルのタイトル表示状態に応じてキャッシュしておく高さ．

		//
		// コンストラクタです。タブを作成します。
		// ownerはこのペインを保有するウィンドウ(DockSiteの派生クラス)です。
		//
		Pane(HWND owner)
			: owner(owner)
		{
			tab.create(
				WS_EX_NOPARENTNOTIFY,
				WC_TABCONTROL,
				_T("Nest.Tab"),
				WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
				TCS_FOCUSNEVER,
				0, 0, 0, 0,
				owner, 0, hive.instance, 0);

			AviUtl::SysInfo si = {};
			magi.auin.get_sys_info(0, &si);
			::SendMessage(tab, WM_SETFONT, (WPARAM)si.hfont, TRUE);

			::SetProp(tab, PropName, this);
		}

		//
		// デストラクタです。タブを削除します。
		//
		virtual ~Pane()
		{
			tab.destroy();
		}

		//
		// 指定されたウィンドウハンドルに関連付けられているペインを返します。
		//
		static Pane* getPane(HWND hwnd)
		{
			return (Pane*)::GetProp(hwnd, PropName);
		}

		//
		// 指定されたウィンドウハンドルにペインを関連付けます。
		//
		static void setPane(HWND hwnd, Pane* pane)
		{
			::SetProp(hwnd, PropName, pane);
		}

		//
		// タブの数を返します。
		//
		int getTabCount()
		{
			return ::SendMessage(tab, TCM_GETITEMCOUNT, 0, 0);
		}

		//
		// 指定されたインデックスにあるタブのシャトルを返します。
		//
		Shuttle* getShuttle(int index)
		{
			if (index == -1) return 0;

			TCITEM item = {};
			item.mask = TCIF_PARAM;
			::SendMessage(tab, TCM_GETITEM, index, (LPARAM)&item);

			return (Shuttle*)item.lParam;
		}

		//
		// タブのカレントインデックスを返します。
		//
		int getCurrentIndex()
		{
			return ::SendMessage(tab, TCM_GETCURSEL, 0, 0);
		}

		//
		// タブのカレントインデックスを指定されたインデックスに変更します。
		//
		int setCurrentIndex(int index)
		{
			if (index < 0 || index >= getTabCount())
				index = 0;

			return ::SendMessage(tab, TCM_SETCURSEL, index, 0);
		}

		//
		// 指定された座標にあるタブのインデックスを返します。
		// pointはタブの親ウィンドウの座標系で指定します。
		//
		int hitTest(POINT point)
		{
			TCHITTESTINFO hti = {};
			hti.pt = point;
			::MapWindowPoints(::GetParent(tab), tab, &hti.pt, 1);
			return ::SendMessage(tab, TCM_HITTEST, 0, (LPARAM)&hti);
		}

		//
		// 指定されたインデックスの位置にタブを追加します。
		// shuttleはタブに関連付けるシャトルです。
		// textはタブ名です。
		//
		int addTab(Shuttle* shuttle, LPCTSTR text, int index)
		{
			TCITEM item = {};
			item.mask = TCIF_PARAM | TCIF_TEXT;
			item.lParam = (LPARAM)shuttle;
			item.pszText = (LPTSTR)text;
			return ::SendMessage(tab, TCM_INSERTITEM, index, (LPARAM)&item);
		}

		//
		// 指定されたインデックスのタブを削除します。
		//
		void deleteTab(int index)
		{
			::SendMessage(tab, TCM_DELETEITEM, index, 0);
		}

		//
		// すべてのタブを削除します。
		//
		void deleteAllTabs()
		{
			::SendMessage(tab, TCM_DELETEALLITEMS, 0, 0);
		}

		//
		// 指定されたシャトルを持つタブのインデックスを返します。
		// 存在しない場合は-1を返します。
		//
		int findTab(Shuttle* shuttle)
		{
			int c = getTabCount();
			for (int i = 0; i < c; i++)
			{
				if (getShuttle(i) == shuttle)
					return i;
			}

			return -1;
		}

		//
		// fromで指定された位置にあるタブをtoで指定された位置へ移動します。
		// 失敗した場合は-1を返します。
		//
		int moveTab(int from, int to)
		{
			int c = getTabCount();

			if (from < 0 || from >= c) return -1;
			if (to < 0 || to >= c) return -1;

			int current = getCurrentIndex();
			TCHAR text[MAX_PATH] = {};
			TCITEM item = {};
			item.mask = TCIF_PARAM | TCIF_TEXT;
			item.pszText = text;
			item.cchTextMax = MAX_PATH;
			::SendMessage(tab, TCM_GETITEM, from, (LPARAM)&item);
			::SendMessage(tab, TCM_DELETEITEM, from, 0);
			::SendMessage(tab, TCM_INSERTITEM, to, (LPARAM)&item);
			if (from == current) setCurrentIndex(to);

			return to;
		}

		//
		// カレントのシャトルだけが表示されるようにします。
		//
		void refreshCurrent()
		{
			MY_TRACE_FUNC("");

			int current = getCurrentIndex();
			int c = getTabCount();
			for (int i = 0; i < c; i++)
			{
				Shuttle* shuttle = getShuttle(i);

				if (i == current)
				{
					// 空の設定ダイアログかどうかチェックします。
					if (*shuttle == hive.settingDialog &&
						magi.auin.GetCurrentObjectIndex() == -1)
					{
						MY_TRACE(_T("空の設定ダイアログなので表示できません\n"));
					}
					else
					{
						MY_TRACE(_T("「%ws」を表示します\n"), (BSTR)shuttle->name);

						::SetWindowPos(*shuttle->dockContainer, HWND_TOP, 0, 0, 0, 0,
							SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_ASYNCWINDOWPOS);
						::ShowWindowAsync(*shuttle, SW_SHOW);
					}
				}
				else
				{
					MY_TRACE(_T("「%ws」を非表示にします\n"), (BSTR)shuttle->name);

					::ShowWindowAsync(*shuttle->dockContainer, SW_HIDE);
					::ShowWindowAsync(*shuttle, SW_HIDE);
				}

//				MY_TRACE_INT(::IsWindowVisible(*shuttle));
//				MY_TRACE_INT(::IsWindowVisible(*shuttle->dockContainer));
//				MY_TRACE_INT(::IsWindowVisible(::GetParent(*shuttle->dockContainer)));
			}
		}

		//
		// 指定されたシャトルを持つタブを指定されたタブ名に変更します。
		//
		void refreshText(Shuttle* shuttle, LPCTSTR text)
		{
			int index = findTab(shuttle);
			if (index == -1) return;

			TCITEM item = {};
			item.mask = TCIF_TEXT;
			item.pszText = (LPTSTR)text;
			::SendMessage(tab, TCM_SETITEM, index, (LPARAM)&item);
		}

		//
		// カレントタブのシャトルを返します。
		//
		Shuttle* getCurrentShuttle()
		{
			int current = getCurrentIndex();
			return getShuttle(current);
		}

		//
		// 指定されたタブインデックスの位置に指定されたシャトルを追加します。
		//
		int addShuttle(Shuttle* shuttle, int index = -1)
		{
			MY_TRACE_FUNC("0x%08X, %d", shuttle, index);

			// シャトルはすでにドッキング済みなので何もしません。
			if (findTab(shuttle) != -1) return -1;

			// シャトルが他のペインとドッキング中なら
			auto listener = shuttle->getListener();
			if (listener)
			{
				// 他のペインとのドッキングを解除させます。
				listener->releaseShuttle(shuttle);
			}

			// 追加位置が無効の場合は末尾に追加します。
			if (index == -1)
				index = getTabCount();

			// ウィンドウテキストを取得します。
			TCHAR text[MAX_PATH] = {};
			::GetWindowText(*shuttle, text, std::size(text));

			// タブを追加します。
			int result = addTab(shuttle, text, index);

			// シャトルをドッキング状態にします。
			shuttle->setListener(this);
			shuttle->dockWindow(this->getDockSite());

			return result;
		}

		//
		// タブから指定されたシャトルを削除します。
		//
		void removeShuttle(Shuttle* shuttle)
		{
			MY_TRACE_FUNC("0x%08X", shuttle);

			// 指定されたシャトルを保持しているかチェックします。
			int index = findTab(shuttle);
			if (index == -1) return; // シャトルを保持していない場合は何もしません。

			// シャトルがカレントかどうかチェックしておきます。
			BOOL same = index == getCurrentIndex();

			// タブを削除します。
			deleteTab(index);

			// 削除されたタブとカレントタブが同じなら
			if (same)
			{
				// カレントタブを設定し直します。
				setCurrentIndex(index);
			}

			// カレントタブが無効なら
			if (getCurrentIndex() == -1)
			{
				// 末尾のタブをカレントにします。
				setCurrentIndex(getTabCount() - 1);
			}

			refresh(FALSE);

			// シャトルをフローティング状態にします。
			shuttle->setListener(0);
			shuttle->floatWindow();
		}

		//
		// タブからすべてのシャトルを削除します。
		//
		void removeAllShuttles()
		{
			MY_TRACE_FUNC("");

			// すべてのシャトルをフローティング状態にします。

			int c = getTabCount();
			for (int i = 0; i < c; i++)
			{
				Shuttle* shuttle = getShuttle(i);

				shuttle->setListener(0);
				shuttle->floatWindow();
			}

			// すべてのタブを削除します。
			deleteAllTabs();
		}

		//
		// ペインをリセットします。
		//
		virtual void resetPane()
		{
			removeAllShuttles();

			for (auto& child : children)
			{
				if (!child) continue;

				child->resetPane();
				child = 0;
			}
		}

		//
		// ペインの分割モードを変更します。
		//
		void setSplitMode(int newSplitMode)
		{
			splitMode = newSplitMode;

			switch (splitMode)
			{
			case SplitMode::none:
				{
					resetPane();

					break;
				}
			case SplitMode::vert:
				{
					border = getWidth(position) / 2;

					if (!children[0]) children[0] = std::make_shared<Pane>(owner);
					if (!children[1]) children[1] = std::make_shared<Pane>(owner);

					break;
				}
			case SplitMode::horz:
				{
					border = getHeight(position) / 2;

					if (!children[0]) children[0] = std::make_shared<Pane>(owner);
					if (!children[1]) children[1] = std::make_shared<Pane>(owner);

					break;
				}
			}

			removeAllShuttles();
		}

		inline int clamp(int x, int minValue, int maxValue)
		{
			if (x < minValue) return minValue;
			if (x > maxValue) return maxValue;
			return x;
		}

		int absoluteX(int x)
		{
			switch (origin)
			{
			case Origin::topLeft: return position.left + x;
			case Origin::bottomRight: return position.right - x - borderWidth;
			}

			return 0;
		}

		int absoluteY(int y)
		{
			switch (origin)
			{
			case Origin::topLeft: return position.top + y;
			case Origin::bottomRight: return position.bottom - y - borderWidth;
			}

			return 0;
		}

		int relativeX(int x)
		{
			switch (origin)
			{
			case Origin::topLeft: return x - position.left;
			case Origin::bottomRight: return position.right - x + borderWidth;
			}

			return 0;
		}

		int relativeY(int y)
		{
			switch (origin)
			{
			case Origin::topLeft: return y - position.top;
			case Origin::bottomRight: return position.bottom - y + borderWidth;
			}

			return 0;
		}

		//
		// ドッキング領域を返します。
		//
		RECT getDockRect()
		{
			// タブの数が 2 個以上なら
			if (getTabCount() >= 2)
			{
				// タブを考慮に入れてドッキング領域を返します。
				switch (tabMode)
				{
				case TabMode::title: // タブをタイトルに配置する場合
					{
						return RECT
						{
							position.left,
							position.top + std::max<int>(thisCaptionHeight, tabHeight),
							position.right,
							position.bottom,
						};
					}
				case TabMode::top: // タブを上に配置する場合
					{
						return RECT
						{
							position.left,
							position.top + thisCaptionHeight + tabHeight,
							position.right,
							position.bottom,
						};
					}
				case TabMode::bottom: // タブを下に配置する場合
					{
						return RECT
						{
							position.left,
							position.top + thisCaptionHeight,
							position.right,
							position.bottom - tabHeight,
						};
					}
				}
			}

			// タブを考慮に入れずにドッキング領域を返します。
			return RECT
			{
				position.left,
				position.top + thisCaptionHeight,
				position.right,
				position.bottom,
			};
		}

		//
		// キャプション領域を返します。
		//
		RECT getCaptionRect()
		{
			return RECT
			{
				position.left,
				position.top,
				position.right,
				position.top + thisCaptionHeight,
			};
		}

		//
		// メニューボタンの領域を返します。
		//
		RECT getMenuRect()
		{
			return RECT
			{
				position.left,
				position.top,
				position.left + thisCaptionHeight,
				position.top + thisCaptionHeight,
			};
		}

		void normalize()
		{
//			MY_TRACE_FUNC("");

			switch (splitMode)
			{
			case SplitMode::vert: border = clamp(border, 0, getWidth(position) - borderWidth); break;
			case SplitMode::horz: border = clamp(border, 0, getHeight(position) - borderWidth); break;
			}
		}

		//
		// このペインおよび子孫ペインの位置情報を再計算します。
		//
		void recalcLayout()
		{
			recalcLayout(&position, 0);
		}

		//
		// このペインおよび子孫ペインの位置情報を再計算します。
		// rcはこのペインの新しい位置です。
		//
		virtual void recalcLayout(LPCRECT rc, const std::shared_ptr<Pane>& limited)
		{
//			MY_TRACE_FUNC("");

			if (::IsIconic(owner))
				return;

			// このペインの位置を更新します。
			position = *rc;

			int c = getTabCount();

			// キャプションを表示するかどうかに応じて高さを再計算します．
			thisCaptionHeight = 0;
			if (limited && this != limited.get()) {
				// 最大化ペインがあるときは，そのキャプション部分やタブに覆い被さらないように大きめにとります．
				switch (tabMode) {
				case TabMode::title:
					thisCaptionHeight = std::max(captionHeight, tabHeight);
					break;
				case TabMode::top:
					thisCaptionHeight = captionHeight + tabHeight;
					break;
				case TabMode::bottom:
					thisCaptionHeight = captionHeight;
					break;
				}
			}
			else if (c > 0) {
				// タブ表示の場合は1つでも表示するキャプションがある場合，通常通りの高さにします．
				for (int i = 0; i < c; i++) {
					if (auto shuttle = getShuttle(i);
						!shuttle || shuttle->show_caption) {
						thisCaptionHeight = captionHeight;
						break;
					}
				}
			}

			// タブが2個以上あるなら
			if (c >= 2 && (!limited || this == limited.get()))
			{
				switch (tabMode)
				{
				case TabMode::title: // タブをタイトルに表示するなら
					{
						int x = position.left + thisCaptionHeight;
						int y = position.top;
						int w = getWidth(position) - thisCaptionHeight;
						int h = std::max(thisCaptionHeight, tabHeight);

						modifyStyle(tab, TCS_BOTTOM, 0);

						// タブコントロールを表示します。
						::SetWindowPos(tab, HWND_TOP,
							x, y, w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);

						break;
					}
				case TabMode::top: // タブを上に表示するなら
					{
						int x = position.left;
						int y = position.top + thisCaptionHeight;
						int w = getWidth(position);
						int h = tabHeight;

						modifyStyle(tab, TCS_BOTTOM, 0);

						// タブコントロールを表示します。
						::SetWindowPos(tab, HWND_TOP,
							x, y, w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);

						break;
					}
				case TabMode::bottom: // タブを下に表示するなら
					{
						int x = position.left;
						int y = position.bottom - tabHeight;
						int w = getWidth(position);
						int h = tabHeight;

						modifyStyle(tab, 0, TCS_BOTTOM);

						// タブコントロールを表示します。
						::SetWindowPos(tab, HWND_TOP,
							x, y, w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);

						break;
					}
				}
				if (limited)
					// タブコントロールのサイズが変わらないと再描画が行われないらしい．
					// 最大化モードで再描画を抑制中にレイアウトした後で移動が起こった場合，
					// アーティファクトが残っていたので対処．
					::InvalidateRect(tab, nullptr, FALSE);
			}
			// タブが1個以下なら
			else
			{
				// タブコントロールを非表示にします。
				::ShowWindow(tab, SW_HIDE);
			}

			if (c) // シャトルを持っている場合は
			{
				RECT rcDock = getDockRect();

				for (int i = 0; i < c; i++)
				{
					Shuttle* shuttle = getShuttle(i);

					shuttle->resize(&rcDock); // シャトルをリサイズする。
				}

				return; // シャトルを持つペインはボーダーも子ペインも持たないので、ここで処理を終了します。
			}

			// ボーダーが飛び出ないように調整します。
			normalize();

			switch (splitMode)
			{
			case SplitMode::vert:
				{
					int absBorder = absoluteX(border);

					if (children[0])
					{
						RECT rc = { position.left, position.top, absBorder, position.bottom };

						children[0]->recalcLayout(&rc, limited);
					}

					if (children[1])
					{
						RECT rc = { absBorder + borderWidth, position.top, position.right, position.bottom };

						children[1]->recalcLayout(&rc, limited);
					}

					break;
				}
			case SplitMode::horz:
				{
					int absBorder = absoluteY(border);

					if (children[0])
					{
						RECT rc = { position.left, position.top, position.right, absBorder };

						children[0]->recalcLayout(&rc, limited);
					}

					if (children[1])
					{
						RECT rc = { position.left, absBorder + borderWidth, position.right, position.bottom };

						children[1]->recalcLayout(&rc, limited);
					}

					break;
				}
			}
		}

		BOOL hitTestCaption(POINT point)
		{
			RECT rc = getCaptionRect();

			return ::PtInRect(&rc, point);
		}

		std::shared_ptr<Pane> hitTestPane(POINT point)
		{
			// pointがこのペインの範囲外なら
			if (!::PtInRect(&position, point))
				return 0; // ヒットしていないので0を返します。

			// このペインがシャトルを持っているなら
			if (getTabCount())
				return shared_from_this(); // ヒットしているのでこのペインを返します。

			switch (splitMode)
			{
			case SplitMode::vert:
			case SplitMode::horz:
				{
					// 子ペインに再帰的に処理させます。
					for (auto& child : children)
					{
						if (!child) continue;

						auto pane = child->hitTestPane(point);
						if (pane) return pane;
					}

					break;
				}
			}

			return shared_from_this(); // ボーダーなどにヒットしているのでこのペインを返します。
		}

		std::shared_ptr<Pane> hitTestBorder(POINT point)
		{
			// pointがこのペインの範囲外なら
			if (!::PtInRect(&position, point))
				return 0; // ヒットしていないので0を返します。

			// このペインがウィンドウを持っているなら
			if (getTabCount())
				return 0; // ヒットしていないので0を返します。

			// このペインのボーダーがロックされていないなら
			if (!isBorderLocked || ::GetKeyState(VK_CONTROL) < 0)
			{
				switch (splitMode)
				{
				case SplitMode::vert:
					{
						int absBorder = absoluteX(border);

						// pointがボーダーの範囲内なら
						if (point.x >= absBorder && point.x < absBorder + borderWidth)
							return shared_from_this(); // ヒットしているのでこのペインを返します。

						break;
					}
				case SplitMode::horz:
					{
						int absBorder = absoluteY(border);

						// pointがボーダーの範囲内なら
						if (point.y >= absBorder && point.y < absBorder + borderWidth)
							return shared_from_this(); // ヒットしているのでこのペインを返します。

						break;
					}
				default:
					{
						return 0;
					}
				}
			}

			// 子ペインに再帰的に処理させます。
			for (auto& child : children)
			{
				if (!child) continue;

				auto pane = child->hitTestBorder(point);
				if (pane) return pane;
			}

			return 0;
		}

		int getDragOffset(POINT point)
		{
			switch (splitMode)
			{
			case SplitMode::vert: return border - relativeX(point.x);
			case SplitMode::horz: return border - relativeY(point.y);
			}

			return 0;
		}

		void dragBorder(POINT point)
		{
			switch (splitMode)
			{
			case SplitMode::vert: border = relativeX(point.x) + dragOffset; break;
			case SplitMode::horz: border = relativeY(point.y) + dragOffset; break;
			}
		}

		BOOL getBorderRect(LPRECT rc)
		{
			// シャトルを持っている場合は
			if (getTabCount())
				return FALSE; // ボーダーを持たないのでFALSEを返します。

			switch (splitMode)
			{
			case SplitMode::vert:
				{
					int absBorder = absoluteX(border);

					rc->left = absBorder;
					rc->top = position.top;
					rc->right = absBorder + borderWidth;
					rc->bottom = position.bottom;

					return TRUE;
				}
			case SplitMode::horz:
				{
					int absBorder = absoluteY(border);

					rc->left = position.left;
					rc->top = absBorder;
					rc->right = position.right;
					rc->bottom = absBorder + borderWidth;

					return TRUE;
				}
			}

			return FALSE;
		}

		void drawBorder(HDC dc, HBRUSH brush)
		{
			// シャトルを持っている場合は
			if (getTabCount())
				return; // ボーダーを持たないので何もしません。

			switch (splitMode)
			{
			case SplitMode::vert:
			case SplitMode::horz:
				{
					RECT rc;
					if (getBorderRect(&rc))
					{
						// テーマを使用するなら
						if (hive.useTheme)
						{
							int partId = WP_BORDER;
							int stateId = CS_INACTIVE;

							// テーマAPIを使用してボーダーを描画します。
							::DrawThemeBackground(hive.theme, dc, partId, stateId, &rc, 0);
						}
						// テーマを使用しないなら
						else
						{
							// ブラシで塗り潰します。
							::FillRect(dc, &rc, brush);
						}
					}

					for (auto& child : children)
					{
						if (!child) continue;

						child->drawBorder(dc, brush);
					}

					break;
				}
			}
		}

		void drawCaption(HDC dc)
		{
			// カレントシャトルが存在するなら
			Shuttle* shuttle = getCurrentShuttle();
			if (shuttle)
			{
				// キャプションが非表示の場合は即時 return.
				if (thisCaptionHeight <= 0) return;

				// キャプションを描画します。

				RECT rc = getCaptionRect();

				if (rc.bottom > position.bottom)
					return; // キャプションが描画範囲からはみ出てしまう場合は何もしません。

				RECT rcMenu = getMenuRect();
				BOOL hasMenu = !!::GetMenu(*shuttle);

				// シャトルのウィンドウテキストを取得します。
				WCHAR text[MAX_PATH] = {};
				::GetWindowTextW(*shuttle, text, std::size(text));

				// テーマを使用するなら
				if (hive.useTheme)
				{
					// ウィンドウの状態からstateIdを取得します。
					int stateId = CS_ACTIVE;
					if (::GetFocus() != *shuttle) stateId = CS_INACTIVE;
					if (!::IsWindowEnabled(*shuttle)) stateId = CS_DISABLED;

					// テーマAPIを使用してタイトルを描画します。
					::DrawThemeBackground(hive.theme, dc, WP_CAPTION, stateId, &rc, 0);

					{
						RECT rcText = rc;

						if (hasMenu)
						{
							rcText.left = rcMenu.right;

							::DrawThemeText(hive.theme, dc, WP_CAPTION, stateId,
								L"\ufe19", -1, DT_CENTER | DT_VCENTER | DT_SINGLELINE, 0, &rcMenu);
						}

						if (shuttle->show_caption && rcText.left < rcText.right)
							::DrawThemeText(hive.theme, dc, WP_CAPTION, stateId,
								text, ::lstrlenW(text), DT_CENTER | DT_VCENTER | DT_SINGLELINE, 0, &rcText);
					}
				}
				// テーマを使用しないなら
				else
				{
					// 直接GDIを使用して描画します。

					COLORREF captionColor = hive.activeCaptionColor;
					COLORREF captionTextColor = hive.activeCaptionTextColor;

					if (::GetFocus() != *shuttle)
					{
						captionColor = hive.inactiveCaptionColor;
						captionTextColor = hive.inactiveCaptionTextColor;
					}

					HBRUSH brush = ::CreateSolidBrush(captionColor);
					::FillRect(dc, &rc, brush);
					::DeleteObject(brush);

					int bkMode = ::SetBkMode(dc, TRANSPARENT);
					COLORREF textColor = ::SetTextColor(dc, captionTextColor);
					{
						RECT rcText = rc;

						if (hasMenu)
						{
							rcText.left = rcMenu.right;

							::DrawTextW(dc, L"\ufe19", -1, &rcMenu, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						}

						if (shuttle->show_caption && rcText.left < rcText.right)
							::DrawTextW(dc, text, ::lstrlenW(text), &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					}
					::SetTextColor(dc, textColor);
					::SetBkMode(dc, bkMode);
				}

				return;
			}

			// 子ペインも再帰的に処理させます。

			switch (splitMode)
			{
			case SplitMode::vert:
			case SplitMode::horz:
				{
					for (auto& child : children)
					{
						if (!child) continue;

						child->drawCaption(dc);
					}

					break;
				}
			}
		}

		//
		// このペインを再描画します。
		//
		void invalidate()
		{
			::InvalidateRect(owner, &position, FALSE);
		}

		//
		// このペインの表示状態を最新の状態に更新します。
		// deepがTRUEなら子孫ペインも再帰的に更新します。
		//
		virtual void refresh(BOOL deep)
		{
			refreshCurrent();
			invalidate();

			if (deep)
			{
				for (auto& child : children)
				{
					if (!child) continue;

					child->refresh(deep);
				}
			}
		}

		//
		// ドッキングサイトのウィンドウハンドルが必要なときに呼ばれます。
		//
		HWND getDockSite() override
		{
			return owner;
		}

		//
		// ドッキングサイトからシャトルを切り離すときに呼ばれます。
		//
		void releaseShuttle(Shuttle* shuttle) override
		{
			// このペインからシャトルを切り離します。
			removeShuttle(shuttle);
			recalcLayout();
			invalidate();
		}

		//
		// ターゲットのテキストが変更されたとき、シャトルから通知されます。
		//
		void onChangeText(Shuttle* shuttle, LPCTSTR newText) override
		{
			// タブのテキストを更新します。
			refreshText(shuttle, newText);

			// ペインのタイトル部分を再描画します。
			if (shuttle->show_caption) invalidate();
		}

		//
		// ターゲットのフォーカスが変更されたとき、シャトルから通知されます。
		//
		void onChangeFocus(Shuttle* shuttle) override
		{
			// ペインのタイトル部分を再描画します。
			if (thisCaptionHeight > 0) invalidate();
		}
	};
}
