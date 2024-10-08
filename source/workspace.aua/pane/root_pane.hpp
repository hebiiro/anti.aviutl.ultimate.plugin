#pragma once

namespace apn::workspace
{
	struct RootPane : Pane
	{
		//
		// 最大化されたペインです。
		//
		std::shared_ptr<Pane> maximized_pane;

		//
		// レイアウトが固定かどうかのフラグです。
		// TRUEの場合はレイアウトの再読み込みが抑制されます。
		//
		BOOL is_solid = FALSE;

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
		virtual ~RootPane() override
		{
		}

		//
		// 最大化されたペインを返します。
		//
		auto get_maximized_pane() const
		{
			return maximized_pane;
		}

		//
		// 最大化されたペインをセットします。
		//
		void set_maximized_pane(const std::shared_ptr<Pane>& new_maximized_pane)
		{
			if (new_maximized_pane.get() == this) return;
			if (maximized_pane == new_maximized_pane) return;

			maximized_pane = new_maximized_pane;

			show_tab(shared_from_this(), !maximized_pane);
		}

		//
		// タブの表示状態を変更します。
		//
		void show_tab(const std::shared_ptr<Pane>& pane, BOOL show)
		{
			if (pane == maximized_pane) return;

			::ShowWindow(tab, show ? SW_SHOW : SW_HIDE);

			for (auto& child : pane->children)
			{
				if (!child) continue;

				show_tab(child, show);
			}
		}

		//
		// ペインをリセットします。
		//
		virtual void reset_pane() override
		{
			MY_TRACE_FUNC("");

			maximized_pane = nullptr;

			clear_all_shuttles();

			for (auto& child : children)
			{
				if (!child) continue;

				child->reset_pane();
				child = nullptr;
			}
		}

		//
		// ペインを更新します。
		//
		virtual void update(HDWP dwp, LPCRECT rc, uint32_t flags) override
		{
			MY_TRACE_FUNC("");

			if (maximized_pane)
			{
				// c_show_tabフラグなしで子ペインを更新します。
				// これにより、すべての子孫ペインのタブが非表示になります。
				update_children(dwp, UpdateFlag::c_deep);

				// maximized_paneがルートペインとなるようにレイアウトを更新します。
				maximized_pane->update(dwp, rc, flags);
			}
			else
			{
				// thisがルートペインとなるようにレイアウトを更新します。
				__super::update(dwp, rc, flags);
			}
		}
	};
}
