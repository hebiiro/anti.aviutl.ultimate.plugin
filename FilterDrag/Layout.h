#pragma once

namespace fgo::filter_drag
{
	struct Layout
	{
		HWND m_dialog;
		std::vector<RECT> panes;

		//
		// コンストラクタです。
		//
		Layout(HWND dialog)
			: m_dialog(dialog)
		{
			init();
		}

		//
		// panes を構築します。
		//
		void init()
		{
			for (int i = 0; i < ExEdit::Object::MAX_FILTER; i++)
			{
				HWND arrow = ::GetDlgItem(m_dialog, 4500 + i);
				HWND name = ::GetDlgItem(m_dialog, 4400 + i);

				if (!::IsWindowVisible(arrow)) break;

				RECT arrowRect;
				::GetClientRect(arrow, &arrowRect);
				::MapWindowPoints(arrow, m_dialog, (LPPOINT)&arrowRect, 2);

				RECT nameRect;
				::GetClientRect(name, &nameRect);
				::MapWindowPoints(name, m_dialog, (LPPOINT)&nameRect, 2);

				RECT pane;
				pane.left = arrowRect.left;
				pane.top = arrowRect.top;
				pane.right = nameRect.right;
				pane.bottom = arrowRect.bottom;
				panes.push_back(pane);

				if (i > 0)
				{
					RECT& prev = panes[i - 1];
					if (prev.top < pane.top)
					{
						prev.bottom = pane.top;
					}
					else
					{
						prev.bottom = calcBottom(prev);
					}
				}
			}

			if (!panes.empty())
			{
				RECT& pane = panes.back();

				pane.bottom = calcBottom(pane);
			}
		}

		//
		// pane の left と right の間にあるウィンドウ群の最大の下辺(ボトム)を返します。
		//
		int calcBottom(const RECT& pane)
		{
			LONG result = 0;

			HWND child = ::GetWindow(m_dialog, GW_CHILD);
			while (child)
			{
				if (::IsWindowVisible(child))
				{
					RECT rc; ::GetClientRect(child, &rc);
					::MapWindowPoints(child, m_dialog, (LPPOINT)&rc, 2);
					if (rc.left >= pane.left && rc.right <= pane.right)
					{
						result = std::max(result, rc.bottom);
					}
				}

				child = ::GetWindow(child, GW_HWNDNEXT);
			}

			return result;
		}

		//
		// ダイアログのウィンドウハンドルを返します。
		//
		HWND getDialog() const { return m_dialog; }

		//
		// ドラッグ元フィルタを返します。
		//
		FilterHolder getSrcFilter(POINT pos, ObjectHolder object) const
		{
			for (int i = 0; i < (int)panes.size(); i++)
			{
				if (!::PtInRect(&panes[i], pos)) continue;

				FilterHolder filter(object, i);
				if (filter.isMoveable()) return filter;

				break;
			}

			return FilterHolder(object, -1);
		}

		//
		// ドラッグ先フィルタを返します。
		//
		FilterHolder getDstFilter(POINT pos, ObjectHolder object) const
		{
			for (int i = 0; i < (int)panes.size(); i++)
			{
				if (!::PtInRect(&panes[i], pos)) continue;

				FilterHolder filter(object, i);
				if (filter.isMoveable()) return filter;

				// フィルタが先頭要素の場合はその次のフィルタを返す。
				if (i == 0) return FilterHolder(object, 1);

				break;
			}

			return FilterHolder(object, -1);
		}

		//
		// フィルタ矩形を返します。
		//
		void getFilterRect(FilterHolder filter, LPRECT rc) const
		{
			int i = filter.getFilterIndex();
			if (i >= 0 && i < (int)panes.size())
				*rc = panes[i];
			else
				::GetClientRect(m_dialog, rc);
		}
	};
}
