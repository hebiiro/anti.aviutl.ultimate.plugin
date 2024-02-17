#pragma once
#include "Hive.h"
#include "Tab.h"
#include "Shuttle/Shuttle.h"

namespace fgo::nest
{
	struct Pane : Shuttle::Listener, std::enable_shared_from_this<Pane>
	{
		struct SplitMode {
			static const int None = 0;
			static const int Vert = 1;
			static const int Horz = 2;

			inline static const Tools::Label label[] = {
				{ None, L"None" },
				{ Vert, L"Vert" },
				{ Horz, L"Horz" },

				// 以下は古いバージョン用のラベルです。
				{ None, L"none" },
				{ Vert, L"vert" },
				{ Horz, L"horz" },
			};
		};

		struct Origin {
			static const int TopLeft = 0;
			static const int BottomRight = 1;

			inline static const Tools::Label label[] = {
				{ TopLeft, L"TopLeft" },
				{ BottomRight, L"BottomRight" },

				// 以下は古いバージョン用のラベルです。
				{ TopLeft, L"topLeft" },
				{ BottomRight, L"bottomRight" },
			};
		};

		struct CaptionMode {
			static const int Hide = 0;
			static const int Show = 1;

			inline static const Tools::Label label[] = {
				{ Hide, L"Hide" },
				{ Show, L"Show" },
			};
		};

		struct TabMode {
			static const int Caption = 0;
			static const int Top = 1;
			static const int Bottom = 2;

			inline static const Tools::Label label[] = {
				{ Caption, L"Caption" },
				{ Top, L"Top" },
				{ Bottom, L"Bottom" },

				// 以下は古いバージョン用のラベルです。
				{ Caption, L"title" },
				{ Top, L"top" },
				{ Bottom, L"bottom" },
			};
		};

		struct UpdateFlag {
			static constexpr DWORD Deep					= 0x00000001;
			static constexpr DWORD Invalidate			= 0x00000002;
			static constexpr DWORD SetFocus				= 0x00000004;
			static constexpr DWORD Normalize				= 0x00000008;
			static constexpr DWORD ShowCurrentShuttle	= 0x00000010;
			static constexpr DWORD ShowTab				= 0x00000020;
			static constexpr DWORD Default = Deep | Invalidate | SetFocus | Normalize | ShowCurrentShuttle | ShowTab;
		};

		inline static const LPCTSTR PropName = _T("Nest.Pane");
		inline static int borderWidth = 8;
		inline static int captionHeight = 24;
		inline static int tabHeight = 24;
		inline static int tabMode = TabMode::Bottom;

		HWND owner = 0;
		RECT position = {};
		int splitMode = SplitMode::None;
		int origin = Origin::BottomRight;
		int captionMode = CaptionMode::Show;
		BOOL isBorderLocked = FALSE;
		int border = 0; // ボーダーの位置です。
		int dragOffset = 0; // ドラッグ処理に使用します。

		Tab tab;
		std::shared_ptr<Pane> children[2];

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
			tab.setFont();

			setPane(tab, this);
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
		// 指定されたシャトルを持つタブを指定されたタブ名に変更します。
		//
		void setTabText(Shuttle* shuttle, LPCTSTR text)
		{
			int index = findTab(shuttle);
			if (index == -1) return;

			TCITEM item = {};
			item.mask = TCIF_TEXT;
			item.pszText = (LPTSTR)text;
			::SendMessage(tab, TCM_SETITEM, index, (LPARAM)&item);
		}

		//
		// このペインがタブを持つ(表示する)場合はTRUEを返します。
		//
		BOOL hasTab()
		{
			int c = getTabCount();
			if (hive.showTabForce)
				return c >= 1;
			else
				return c >= 2;
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
		// 指定されたシャトルをこのペインにドッキングします。
		//
		void dockShuttle(Shuttle* shuttle)
		{
			setPane(*shuttle, this);
			shuttle->addListener(this);
			shuttle->dockWindow(owner);
		}

		//
		// 指定されたシャトルをこのペインから切り離します。
		//
		void floatShuttle(Shuttle* shuttle)
		{
			setPane(*shuttle, 0);
			shuttle->removeListener(this);
			shuttle->floatWindow();
		}

		//
		// 指定されたタブインデックスの位置に指定されたシャトルを追加します。
		//
		int addShuttle(Shuttle* shuttle, int index = -1)
		{
			MY_TRACE_FUNC("0x%08X, %d", shuttle, index);

			// シャトルはすでにこのペインとドッキング済みなので何もしません。
			if (findTab(shuttle) != -1) return -1;

			// シャトルが他のペインとドッキング中かもしれないので、ドッキングを解除させます。
			shuttle->fireReleaseShuttle();

			// 追加位置が無効の場合は末尾に追加します。
			if (index == -1)
				index = getTabCount();

			// ウィンドウテキストを取得します。
			TCHAR text[MAX_PATH] = {};
			::GetWindowText(*shuttle, text, std::size(text));

			// タブを追加します。
			int result = addTab(shuttle, text, index);

			// シャトルをドッキング状態にします。
			dockShuttle(shuttle);

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

			// カレントシャトルが切り替わったので、ペインの表示状態を更新します。
			update();

			// シャトルをフローティング状態にします。
			floatShuttle(shuttle);
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
				auto shuttle = getShuttle(i);

				floatShuttle(shuttle);
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
			case SplitMode::None:
				{
					resetPane();

					break;
				}
			case SplitMode::Vert:
				{
					border = getWidth(position) / 2;

					if (!children[0]) children[0] = std::make_shared<Pane>(owner);
					if (!children[1]) children[1] = std::make_shared<Pane>(owner);

					break;
				}
			case SplitMode::Horz:
				{
					border = getHeight(position) / 2;

					if (!children[0]) children[0] = std::make_shared<Pane>(owner);
					if (!children[1]) children[1] = std::make_shared<Pane>(owner);

					break;
				}
			}

			removeAllShuttles();
		}

		//
		// ペインの原点を変更します。
		// そのとき、ボーダーの見た目の位置が変化しないように調整します。
		//
		void setOrigin(int newOrigin)
		{
			if (origin == newOrigin) return;
			origin = newOrigin;
			if (splitMode == Pane::SplitMode::None) return;

			border = -borderWidth - border;

			switch (splitMode)
			{
			case Pane::SplitMode::Horz: border += position.bottom - position.top; break;
			case Pane::SplitMode::Vert: border += position.right - position.left; break;
			}
		}

		//
		// キャプションモードを変更します。
		//
		void setCaptionMode(int newCaptionMode)
		{
			if (captionMode == newCaptionMode) return;
			captionMode = newCaptionMode;
		}

		//
		// このペインのキャプションの高さを返します。
		//
		int getCaptionHeight()
		{
			if (captionMode == CaptionMode::Hide) return 0;
			return captionHeight;
		}

		inline static int clamp(int x, int minValue, int maxValue)
		{
			if (x < minValue) return minValue;
			if (x > maxValue) return maxValue;
			return x;
		}

		int absoluteX(int x)
		{
			switch (origin)
			{
			case Origin::TopLeft: return position.left + x;
			case Origin::BottomRight: return position.right - x - borderWidth;
			}

			return 0;
		}

		int absoluteY(int y)
		{
			switch (origin)
			{
			case Origin::TopLeft: return position.top + y;
			case Origin::BottomRight: return position.bottom - y - borderWidth;
			}

			return 0;
		}

		int relativeX(int x)
		{
			switch (origin)
			{
			case Origin::TopLeft: return x - position.left;
			case Origin::BottomRight: return position.right - x + borderWidth;
			}

			return 0;
		}

		int relativeY(int y)
		{
			switch (origin)
			{
			case Origin::TopLeft: return y - position.top;
			case Origin::BottomRight: return position.bottom - y + borderWidth;
			}

			return 0;
		}

		//
		// ドッキング領域を返します。
		//
		RECT getDockRect()
		{
			// キャプションの高さを取得します。
			int captionHeight = getCaptionHeight();

			// このペインがタブを表示する場合は
			if (hasTab())
			{
				// タブを考慮に入れてドッキング領域を返します。
				switch (tabMode)
				{
				case TabMode::Caption: // タブをキャプションに配置する場合
					{
						return RECT
						{
							position.left,
							position.top + std::max<int>(captionHeight, tabHeight),
							position.right,
							position.bottom,
						};
					}
				case TabMode::Top: // タブを上に配置する場合
					{
						return RECT
						{
							position.left,
							position.top + captionHeight + tabHeight,
							position.right,
							position.bottom,
						};
					}
				case TabMode::Bottom: // タブを下に配置する場合
					{
						return RECT
						{
							position.left,
							position.top + captionHeight,
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
				position.top + captionHeight,
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
				position.top + captionHeight,
			};
		}

		//
		// メニューボタンの領域を返します。
		//
		RECT getMenuRect()
		{
			return RECT
			{
				position.right - captionHeight,
				position.top,
				position.right,
				position.top + captionHeight,
			};
		}

		void normalize()
		{
//			MY_TRACE_FUNC("");

			switch (splitMode)
			{
			case SplitMode::Vert: border = clamp(border, 0, getWidth(position) - borderWidth); break;
			case SplitMode::Horz: border = clamp(border, 0, getHeight(position) - borderWidth); break;
			}
		}

		//
		// このペインおよび子孫ペインの位置情報を更新します。
		//
		void update(DWORD flags = UpdateFlag::Default)
		{
			update(&position, flags);
		}

		//
		// このペインおよび子孫ペインの位置情報を更新します。
		// rcはこのペインの新しい位置です。
		//
		virtual void update(LPCRECT rc, DWORD flags)
		{
//			MY_TRACE_FUNC("");

			if (::IsIconic(owner))
				return;

			// このペインの位置を更新します。
			position = *rc;

			if (flags & UpdateFlag::Invalidate)
			{
				// ペイン矩形が変更されたので、
				// オーナーにペイン矩形の再描画処理を発注しておきます。
				invalidate();
			}

			// タブの数が最小数以上の場合は
			if (hasTab() && flags & UpdateFlag::ShowTab)
			{
				updateTabLayout();
			}
			// タブの数が最小数未満の場合は
			else
			{
				// タブコントロールを非表示にします。
				::ShowWindow(tab, SW_HIDE);
			}

			// シャトルを持っている場合は
			int c = getTabCount();
			if (c)
			{
				if (flags & UpdateFlag::ShowCurrentShuttle)
				{
					// カレントシャトルのみを表示し、フォーカスを与えます。
					showCurrentShuttle(flags);
				}

				return; // シャトルを持つペインはボーダーも子ペインも持たないので、ここで処理を終了します。
			}

			if (flags & UpdateFlag::Normalize)
			{
				// ボーダーが飛び出ないように調整します。
				normalize();
			}

			if (flags & UpdateFlag::Deep)
			{
				// 子ペインを再帰的に更新します。
				// すでに親ペインの再描画が発行されている場合は
				// Invalidateフラグは冗長になるので除外します。
				updateChildren(flags & ~UpdateFlag::Invalidate);
			}
		}

		//
		// タブのウィンドウ位置を更新します。
		// update()から呼び出されます。
		//
		void updateTabLayout()
		{
			// キャプションの高さを取得します。
			int captionHeight = getCaptionHeight();

			switch (tabMode)
			{
			case TabMode::Caption: // タブをキャプションに表示するなら
				{
					int cx = 0;
					int cy = std::max<int>(captionHeight, tabHeight);

					// 現在表示しているシャトルを取得します。
					auto shuttle = getCurrentShuttle();
					if (shuttle)
					{
						// シャトルがメニューを持っている場合は
						if (::GetMenu(*shuttle))
						{
							// メニューアイコンのスペースを確保します。
							cx = cy;
						}
					}

					int x = position.left;
					int y = position.top;
					int w = getWidth(position) - cx; // 右側にあるメニューアイコンの分だけ少し狭めます。
					int h = cy;

					modifyStyle(tab, TCS_BOTTOM, 0);

					// タブコントロールを表示します。
					::SetWindowPos(tab, HWND_TOP,
						x, y, w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);

					break;
				}
			case TabMode::Top: // タブを上に表示するなら
				{
					int x = position.left;
					int y = position.top + captionHeight;
					int w = getWidth(position);
					int h = tabHeight;

					modifyStyle(tab, TCS_BOTTOM, 0);

					// タブコントロールを表示します。
					::SetWindowPos(tab, HWND_TOP,
						x, y, w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);

					break;
				}
			case TabMode::Bottom: // タブを下に表示するなら
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
		}

		//
		// タブのウィンドウ位置を更新します。
		// update()から呼び出されます。
		//
		void showCurrentShuttle(DWORD flags)
		{
			// タブの数を取得します。
			int c = getTabCount();

			// カレントシャトルのインデックスを取得します。
			int currentIndex = getCurrentIndex();

			// ドッキング領域の矩形を取得します。
			RECT rcDock = getDockRect();

			// このペインにドッキングしているすべてのシャトルを順々に処理します。
			for (int i = 0; i < c; i++)
			{
				// インデックスからシャトルを取得します。
				auto shuttle = getShuttle(i);

				// シャトルをリサイズします。
				shuttle->resize(&rcDock);

				if (i == currentIndex)
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

						::SetWindowPos(*shuttle->dockContainer, HWND_TOP,
							0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
						::ShowWindow(*shuttle, SW_SHOW);
					}

					if (flags & UpdateFlag::SetFocus)
					{
						if (::IsWindowVisible(*shuttle))
							::SetFocus(*shuttle);
					}
				}
				else
				{
					MY_TRACE(_T("「%ws」を非表示にします\n"), (BSTR)shuttle->name);

					::ShowWindow(*shuttle->dockContainer, SW_HIDE);
					::ShowWindow(*shuttle, SW_HIDE);
				}

				MY_TRACE_INT(::IsWindowVisible(*shuttle));
				MY_TRACE_INT(::IsWindowVisible(*shuttle->dockContainer));
				MY_TRACE_INT(::IsWindowVisible(::GetParent(*shuttle->dockContainer)));
			}
		}

		//
		// 子ペインを更新します。
		// update()から呼び出されます。
		//
		void updateChildren(DWORD flags)
		{
			switch (splitMode)
			{
			case SplitMode::Vert:
				{
					int absBorder = absoluteX(border);

					if (children[0])
					{
						RECT rc = { position.left, position.top, absBorder, position.bottom };

						children[0]->update(&rc, flags);
					}

					if (children[1])
					{
						RECT rc = { absBorder + borderWidth, position.top, position.right, position.bottom };

						children[1]->update(&rc, flags);
					}

					break;
				}
			case SplitMode::Horz:
				{
					int absBorder = absoluteY(border);

					if (children[0])
					{
						RECT rc = { position.left, position.top, position.right, absBorder };

						children[0]->update(&rc, flags);
					}

					if (children[1])
					{
						RECT rc = { position.left, absBorder + borderWidth, position.right, position.bottom };

						children[1]->update(&rc, flags);
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
			case SplitMode::Vert:
			case SplitMode::Horz:
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
				case SplitMode::Vert:
					{
						int absBorder = absoluteX(border);

						// pointがボーダーの範囲内なら
						if (point.x >= absBorder && point.x < absBorder + borderWidth)
							return shared_from_this(); // ヒットしているのでこのペインを返します。

						break;
					}
				case SplitMode::Horz:
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
			case SplitMode::Vert: return border - relativeX(point.x);
			case SplitMode::Horz: return border - relativeY(point.y);
			}

			return 0;
		}

		void dragBorder(POINT point)
		{
			switch (splitMode)
			{
			case SplitMode::Vert: border = relativeX(point.x) + dragOffset; break;
			case SplitMode::Horz: border = relativeY(point.y) + dragOffset; break;
			}
		}

		BOOL getBorderRect(LPRECT rc)
		{
			// シャトルを持っている場合は
			if (getTabCount())
				return FALSE; // ボーダーを持たないのでFALSEを返します。

			switch (splitMode)
			{
			case SplitMode::Vert:
				{
					int absBorder = absoluteX(border);

					rc->left = absBorder;
					rc->top = position.top;
					rc->right = absBorder + borderWidth;
					rc->bottom = position.bottom;

					return TRUE;
				}
			case SplitMode::Horz:
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
			case SplitMode::Vert:
			case SplitMode::Horz:
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
			auto shuttle = getCurrentShuttle();
			if (shuttle)
			{
				// キャプションを描画します。

				RECT rc = getCaptionRect();

				if (rc.bottom > position.bottom)
					return; // キャプションが描画範囲からはみ出てしまう場合は何もしません。

				RECT rcMenu = getMenuRect();
				BOOL hasMenu = !!::GetMenu(*shuttle);
				BOOL hasFocus = ::GetFocus() == *shuttle;

				// シャトルのウィンドウテキストを取得します。
				WCHAR text[MAX_PATH] = {};
				::GetWindowTextW(*shuttle, text, std::size(text));

				constexpr LPCWSTR menuIcon = L"\xFE19";

				// テーマを使用するなら
				if (hive.useTheme)
				{
					// ウィンドウの状態からstateIdを取得します。
					int stateId = CS_ACTIVE;
					if (!hasFocus) stateId = CS_INACTIVE;
					if (!::IsWindowEnabled(*shuttle)) stateId = CS_DISABLED;

					// テーマAPIを使用してタイトルを描画します。
					::DrawThemeBackground(hive.theme, dc, WP_CAPTION, stateId, &rc, 0);

					{
						RECT rcText = rc;

						if (hasMenu)
						{
							rcText.right = rcMenu.left;

							::DrawThemeText(hive.theme, dc, WP_CAPTION, stateId,
								menuIcon, -1, DT_CENTER | DT_VCENTER | DT_SINGLELINE, 0, &rcMenu);
						}

						if (rcText.left < rcText.right)
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

					if (!hasFocus)
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
							rcText.right = rcMenu.left;

							::DrawTextW(dc, menuIcon, -1, &rcMenu, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						}

						if (rcText.left < rcText.right)
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
			case SplitMode::Vert:
			case SplitMode::Horz:
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
		// ドッキングサイトからシャトルを切り離すときに呼ばれます。
		//
		void releaseShuttle(Shuttle* shuttle) override
		{
			// このペインからシャトルを切り離します。
			removeShuttle(shuttle);

			// レイアウトを更新します。
			update();
		}

		//
		// ターゲットのテキストが変更されたとき、シャトルから通知されます。
		//
		void onChangeText(Shuttle* shuttle, LPCTSTR newText) override
		{
			// タブのテキストを変更します。
			setTabText(shuttle, newText);

			// ペインのタイトル部分を再描画します。
			invalidate();
		}

		//
		// ターゲットのフォーカスが変更されたとき、シャトルから通知されます。
		//
		void onChangeFocus(Shuttle* shuttle) override
		{
			// ペインのタイトル部分を再描画します。
			invalidate();
		}
	};
}
