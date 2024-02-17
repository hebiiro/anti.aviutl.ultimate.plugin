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
		// ペインを更新します。
		//
		void update(LPCRECT rc, DWORD flags) override
		{
			MY_TRACE_FUNC("");

			if (maximizedPane)
			{
				// ShowTabフラグなしで子ペインを更新します。
				// これにより、すべての子孫ペインのタブが非表示になります。
				updateChildren(UpdateFlag::Deep);

				// maximizedPaneがルートペインとなるようにレイアウトを更新します。
				maximizedPane->update(rc, flags);
			}
			else
			{
				// thisがルートペインとなるようにレイアウトを更新します。
				__super::update(rc, flags);
			}
		}
	};
}
