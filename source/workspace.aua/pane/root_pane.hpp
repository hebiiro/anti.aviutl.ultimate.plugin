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

			// 最大化ペインが有効の場合は
			if (maximized_pane)
			{
				// 他のペインを非表示にします。
				hide_others(my::DeferWindowPos(100), shared_from_this());
			}
		}

		//
		// 他のペインを非表示にします。
		//
		void hide_others(HDWP dwp, const std::shared_ptr<Pane>& pane)
		{
			if (pane == maximized_pane) return;

			{
				// タブコントロールを非表示にします。
				pane->hide_tab(dwp);

				// カレントシャトルを非表示にします。
				if (auto shuttle = pane->get_current_shuttle())
					pane->hide_shuttle(dwp, shuttle);
			}

			for (auto& child : pane->children)
			{
				if (!child) continue;

				hide_others(dwp, child);
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
				update_children(dwp, c_update_flag.c_deep);

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
