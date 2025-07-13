#pragma once

namespace apn::filter_drag
{
	//
	// このクラスは設定ダイアログのレイアウトです。
	//
	struct Layout
	{
		HWND setting_dialog;
		std::vector<RECT> panes;

		//
		// コンストラクタです。
		//
		Layout(HWND setting_dialog)
			: setting_dialog(setting_dialog)
		{
			init();
		}

		//
		// ペインを構築します。
		//
		void init()
		{
			for (auto i = 0; i < ExEdit::Object::MAX_FILTER; i++)
			{
				auto arrow = ::GetDlgItem(setting_dialog, 4500 + i);
				auto name = ::GetDlgItem(setting_dialog, 4400 + i);

				if (!::IsWindowVisible(arrow)) break;

				auto arrow_rect = my::get_client_rect(arrow);
				my::map_window_points(arrow, setting_dialog, &arrow_rect);

				auto name_rect = my::get_client_rect(name);
				my::map_window_points(name, setting_dialog, &name_rect);

				auto pane = RECT {
					.left = arrow_rect.left,
					.top = arrow_rect.top,
					.right = name_rect.right,
					.bottom = arrow_rect.bottom,
				};
				panes.push_back(pane);

				if (i > 0)
				{
					auto& prev = panes[i - 1];
					if (prev.top < pane.top)
					{
						prev.bottom = pane.top;
					}
					else
					{
						prev.bottom = calc_bottom(prev);
					}
				}
			}

			if (!panes.empty())
			{
				auto& pane = panes.back();

				pane.bottom = calc_bottom(pane);
			}
		}

		//
		// paneのleftとrightの間にあるウィンドウ群の最大の下辺(ボトム)を返します。
		//
		int calc_bottom(const RECT& pane)
		{
			int result = 0;

			auto child = ::GetWindow(setting_dialog, GW_CHILD);
			while (child)
			{
				if (::IsWindowVisible(child))
				{
					auto rc = my::get_client_rect(child);
					my::map_window_points(child, setting_dialog, &rc);

					if (rc.left >= pane.left && rc.right <= pane.right)
						result = std::max<int>(result, rc.bottom);
				}

				child = ::GetWindow(child, GW_HWNDNEXT);
			}

			return result;
		}

		//
		// ダイアログのウィンドウハンドルを返します。
		//
		HWND get_setting_dialog() const { return setting_dialog; }

		//
		// ドラッグ元フィルタを返します。
		//
		FilterHolder get_src_filter(const POINT& pos, const ObjectHolder& object) const
		{
			for (size_t i = 0; i < panes.size(); i++)
			{
				if (!::PtInRect(&panes[i], pos)) continue;

				FilterHolder filter(object, (int32_t)i);
				if (filter.is_moveable()) return filter;

				break;
			}

			return FilterHolder(object, -1);
		}

		//
		// ドラッグ先フィルタを返します。
		//
		FilterHolder get_dst_filter(const POINT& pos, const ObjectHolder& object) const
		{
			for (size_t i = 0; i < panes.size(); i++)
			{
				if (!::PtInRect(&panes[i], pos)) continue;

				FilterHolder filter(object, (int32_t)i);
				if (filter.is_moveable()) return filter;

				// フィルタが先頭要素の場合はその次のフィルタを返します。
				if (i == 0) return FilterHolder(object, 1);

				break;
			}

			return FilterHolder(object, -1);
		}

		//
		// フィルタ矩形を返します。
		//
		RECT get_filter_rect(const FilterHolder& filter) const
		{
			size_t i = filter.get_filter_index();
			if (i < panes.size())
				return panes[i];
			else
				return my::get_client_rect(setting_dialog);
		}
	};
}
