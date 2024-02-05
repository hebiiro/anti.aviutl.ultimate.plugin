#pragma once
#include "Pane.h"

namespace fgo::nest
{
	struct RootPane : Pane
	{
		//
		// 最大化されたペインです。
		//
		std::shared_ptr<Pane> maximizedPane;

		//
		// コンストラクタです。
		//
		RootPane(HWND owner)
			: Pane(owner)
		{
		}

		//
		// デストラクタです。
		//
		virtual ~RootPane()
		{
		}

		//
		// 最大化されたペインを返します。
		//
		auto getMaximizedPane() const
		{
			return maximizedPane;
		}

		//
		// 最大化されたペインをセットします。
		//
		void setMaximizedPane(const std::shared_ptr<Pane>& newMaximizedPane)
		{
			if (newMaximizedPane.get() == this) return;
			if (maximizedPane == newMaximizedPane) return;

			maximizedPane = newMaximizedPane;

			showTab(shared_from_this(), !maximizedPane);
		}

		//
		// タブの表示状態を変更します。
		//
		void showTab(const std::shared_ptr<Pane>& pane, BOOL show)
		{
			if (pane == maximizedPane) return;

			::ShowWindow(tab, show ? SW_SHOW : SW_HIDE);

			for (auto& child : pane->children)
			{
				if (!child) continue;

				showTab(child, show);
			}
		}

		//
		// ペインをリセットします。
		//
		void resetPane() override
		{
			MY_TRACE_FUNC("");

			maximizedPane = 0;

			removeAllShuttles();

			for (auto& child : children)
			{
				if (!child) continue;

				child->resetPane();
				child = 0;
			}
		}

		//
		// このペインおよび子孫ペインの位置情報を再計算します。
		// rcはこのペインの新しい位置です。
		//
		void recalcLayout(LPCRECT rc) override
		{
			MY_TRACE_FUNC("");

			if (maximizedPane)
			{
				// maximizedPaneがルートペインとなるようにレイアウトを再計算します。
				maximizedPane->recalcLayout(rc);
			}
			else
			{
				// thisがルートペインとなるようにレイアウトを再計算します。
				__super::recalcLayout(rc);
			}
		}

		//
		// このペインの表示状態を最新の状態に更新します。
		// deepがTRUEなら子孫ペインも再帰的に更新します。
		//
		 void refresh(BOOL deep) override
		{
			MY_TRACE_FUNC("");

			if (maximizedPane)
				maximizedPane->refresh(deep);
			else
				__super::refresh(deep);
		}
	};
}
