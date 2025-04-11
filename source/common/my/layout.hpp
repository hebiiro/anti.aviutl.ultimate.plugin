#pragma once

namespace my::layout
{
	inline static constexpr struct Axis {
		inline static constexpr int c_horz = 0;
		inline static constexpr int c_vert = 1;
	} c_axis;

	inline static constexpr struct Align {
		inline static constexpr int c_near = 0;
		inline static constexpr int c_far = 1;
		inline static constexpr int c_left = c_near;
		inline static constexpr int c_top = c_near;
		inline static constexpr int c_right = c_far;
		inline static constexpr int c_bottom = c_far;
	} c_align;

	//
	// このクラスは位置です。
	//
	struct Pos
	{
		virtual void resize(RECT& rc, LONG RECT::* near_member, LONG RECT::* far_member, const RECT& base_rc) = 0;
	};

	//
	// このクラスは指定された位置にリサイズします。
	//
	struct RelativePos : Pos
	{
		int mul, div, add;

		//
		// コンストラクタです。
		//
		RelativePos(int add)
			: mul(0), div(1), add(add)
		{
		}

		//
		// コンストラクタです。
		//
		RelativePos(int mul, int div, int add = 0)
			: mul(mul), div(div), add(add)
		{
		}

		//
		// 与えられた矩形をリサイズします。
		//
		virtual void resize(RECT& rc, LONG RECT::* near_member, LONG RECT::* far_member, const RECT& base_rc) override
		{
			auto w = base_rc.*far_member - rc.*near_member;
			auto add = (base_rc.*far_member > base_rc.*near_member) ? this->add : -this->add;

			rc.*far_member = rc.*near_member + ::MulDiv(w, mul, div) + add;
		}
	};

	//
	// このクラスは指定された割合でリサイズします。
	//
	struct AbsolutePos : Pos
	{
		int mul, div, add;

		//
		// コンストラクタです。
		//
		AbsolutePos(int add)
			: mul(0), div(1), add(add)
		{
		}

		//
		// コンストラクタです。
		//
		AbsolutePos(int mul, int div, int add = 0)
			: mul(mul), div(div), add(add)
		{
		}

		//
		// 与えられた矩形をリサイズします。
		//
		virtual void resize(RECT& rc, LONG RECT::* near_member, LONG RECT::* far_member, const RECT& base_rc) override
		{
			auto w = base_rc.*far_member - base_rc.*near_member;
			auto add = (base_rc.*far_member > base_rc.*near_member) ? this->add : -this->add;

			rc.*far_member = base_rc.*near_member + ::MulDiv(w, mul, div) + add;
		}
	};

	//
	// このクラスはペインです。
	// レイアウトの一画を担当します。
	//
	struct Pane
	{
		//
		// リサイズの対象となる軸です。
		//
		int axis;

		//
		// リサイズの基準となる原点です。
		//
		int align;

		//
		// このペインのサイズを決定します。
		//
		std::shared_ptr<Pos> resizer;

		//
		// このペインの周りのマージンです。
		//
		RECT margin;

		//
		// リサイズの対象となるウィンドウです。
		//
		HWND hwnd;

		//
		// 子要素のコレクションです。
		//
		std::vector<std::shared_ptr<Pane>> children;

		//
		// コンストラクタです。
		//
		Pane(
			int axis = {},
			int align = {},
			const std::shared_ptr<Pos>& resizer = {},
			const RECT& margin = {},
			HWND hwnd = {})
			: axis(axis), align(align), resizer(resizer), margin(margin), hwnd(hwnd)
		{
		}

		//
		// 子要素を追加します。
		//
		auto add_pane(
			int axis,
			int align,
			const std::shared_ptr<Pos>& resizer,
			const RECT& margin = {},
			HWND hwnd = {})
		{
			return children.emplace_back(std::make_shared<Pane>(axis, align, resizer, margin, hwnd));
		}

		//
		// 指定された矩形をリサイズします。
		//
		void resize_rect(RECT& rc, LONG RECT::* near_member, LONG RECT::* far_member, const RECT& base_rc)
		{
			if (resizer)
				resizer->resize(rc, near_member, far_member, base_rc);
		}

		//
		// 指定された矩形を縮小します。
		//
		void deflate_rect(RECT& rc)
		{
			rc.left += margin.left;
			rc.top += margin.top;
			rc.right -= margin.right;
			rc.bottom -= margin.bottom;
		}

		//
		// 保有しているウィンドウをリサイズします。
		//
		void resize_window(my::DeferWindowPos& dwp, const RECT& rc)
		{
			if (!hwnd) return;

			auto x = rc.left;
			auto y = rc.top;
			auto w = get_width(rc);
			auto h = get_height(rc);

			dwp.set_window_pos(hwnd, nullptr, x, y, w, h, SWP_NOZORDER);
		}

		//
		// このペインのレイアウトを更新します。
		//
		BOOL update(my::DeferWindowPos& dwp, const RECT& base_rc)
		{
			// カレント矩形を取得します。
			RECT current_rc = base_rc;

			// すべての子要素を更新します。
			for (auto& child : children)
			{
				// リサイズする方向のポインタを取得します。
				auto near_member = (child->axis == c_axis.c_horz) ? &RECT::left : &RECT::top;
				auto far_member = (child->axis == c_axis.c_horz) ? &RECT::right : &RECT::bottom;

				// アラインが反対の場合はポインタをスワップします。
				if (child->align == c_align.c_far) std::swap(near_member, far_member);

				// 子要素の矩形です。
				auto rc = current_rc;

				// 子要素に矩形をリサイズさせます。
				child->resize_rect(rc, near_member, far_member, base_rc);

				// カレント矩形を更新します。
				current_rc.*near_member = rc.*far_member;

				// 子要素に矩形を縮小させます。
				child->deflate_rect(rc);

				// 子要素のウィンドウをリサイズします。
				child->resize_window(dwp, rc);

				// 子要素を再帰的に更新します。
				child->update(dwp, rc);
			}

			return TRUE;
		}
	};

	template <typename T>
	struct Window : T
	{
		//
		// ルートペインです。
		//
		std::shared_ptr<Pane> root;

		//
		// コンストラクタです。
		//
		Window()
			: root(std::make_shared<Pane>())
		{
		}

		//
		// 指定されたIDのコントロールを返します。
		//
		auto ctrl(UINT id)
		{
			return ::GetDlgItem(*this, id);
		}

		//
		// 指定された矩形でレイアウトを更新します。
		//
		virtual BOOL update_layout(const RECT& pane_rc, int init_count)
		{
			my::DeferWindowPos dwp(init_count);

			return root->update(dwp, pane_rc);
		}

		//
		// クライアント矩形でレイアウトを更新します。
		//
		BOOL update_layout(int init_count = 10)
		{
			return update_layout(my::get_client_rect(*this), init_count);
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_SIZE:
				{
					MY_TRACE_FUNC("WM_SIZE, {/hex}, {/hex}", wParam, lParam);

					update_layout();

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	};
}
