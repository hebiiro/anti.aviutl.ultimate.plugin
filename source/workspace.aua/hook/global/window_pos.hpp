#pragma once

namespace apn::workspace::hook::global
{
	//
	// このクラスはウィンドウ位置関係の処理をフックします。
	//
	inline struct : Entity
	{
		//
		// 初期化処理を実行します。
		//
		virtual BOOL on_dll_init() override
		{
			MY_TRACE_FUNC("");

//			my::hook::attach(MoveWindow);
//			my::hook::attach(SetWindowPos);
			my::hook::attach(GetWindowPlacement);
			my::hook::attach(SetWindowPlacement);

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL on_dll_exit() override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// コンテナを返します。
		//
		inline static std::shared_ptr<Container> get_container(HWND hwnd)
		{
			// シャトルが取得できる場合は
			if (auto shuttle = Shuttle::get_pointer(hwnd))
			{
				// フローティングコンテナが有効の場合は
				if (shuttle->float_container)
				{
					// フローティングコンテナがロックされていない場合は
					if (!shuttle->float_container->is_locked())
					{
						// フローティングコンテナを返します。
						return shuttle->float_container;
					}
				}
			}

			return nullptr;
		}

		//
		// 代替ウィンドウを返します。
		//
		inline static HWND get_imposter(HWND hwnd)
		{
			if (auto container = get_container(hwnd))
				return *container;
			else
				return hwnd;
		}
#if 0
		//
		// このクラスは::MoveWindow()をフックします。
		//
		inline static struct {
			inline static BOOL WINAPI hook_proc(HWND hwnd, int x, int y, int cx, int cy, BOOL repaint)
			{
				MY_TRACE_FUNC("{/hex}, {/}, {/}, {/}, {/}, {/}", hwnd, x, y, cx, cy, repaint);

				// 代替ウィンドウを取得します。
				hwnd = get_imposter(hwnd);

				return orig_proc(hwnd, x, y, cx, cy, repaint);
			}
			inline static decltype(&hook_proc) orig_proc = ::MoveWindow;
		} MoveWindow;

		//
		// このクラスは::SetWindowPos()をフックします。
		//
		inline static struct {
			inline static BOOL WINAPI hook_proc(HWND hwnd, HWND insert_after, int x, int y, int cx, int cy, UINT flags)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/}, {/hex}", hwnd, insert_after, x, y, cx, cy, flags);

				// ウィンドウをリサイズする場合は
				if (!(flags & SWP_NOMOVE) || !(flags & SWP_NOSIZE))
				{
					// コンテナが取得できる場合は
					if (auto container = get_container(hwnd))
					{
						// 一旦構造体に格納します。
						auto wp = WINDOWPOS { hwnd, insert_after, x, y, cx, cy, flags };

						// コンテナにコンテンツ位置の変更を通知します。
						// コンテナがTRUEを返した場合は
						if (container->on_set_content_position(&wp))
						{
							// 構造体の値を使用します。
							insert_after = wp.hwndInsertAfter;
							x = wp.x;
							y = wp.y;
							cx = wp.cx;
							cy = wp.cy;
							flags = wp.flags;
						}

						// コンテンツの代わりにコンテナを移動対象にします。
						hwnd = *container;
					}
				}

				return orig_proc(hwnd, insert_after, x, y, cx, cy, flags);
			}
			inline static decltype(&hook_proc) orig_proc = ::SetWindowPos;
		} SetWindowPos;
#endif
		//
		// このクラスは::SetWindowPlacement()をフックします。
		//
		inline static struct {
			inline static BOOL WINAPI hook_proc(HWND hwnd, const WINDOWPLACEMENT* wp)
			{
				MY_TRACE_FUNC("{/hex}", hwnd);

				// 代替ウィンドウを取得します。
				hwnd = get_imposter(hwnd);
				MY_TRACE_HWND(hwnd);

				return orig_proc(hwnd, wp);
			}
			inline static decltype(&hook_proc) orig_proc = ::SetWindowPlacement;
		} SetWindowPlacement;

		//
		// このクラスは::GetWindowPlacement()をフックします。
		//
		inline static struct {
			inline static BOOL WINAPI hook_proc(HWND hwnd, WINDOWPLACEMENT* wp)
			{
				MY_TRACE_FUNC("{/hex}", hwnd);

				// 代替ウィンドウを取得します。
				hwnd = get_imposter(hwnd);
				MY_TRACE_HWND(hwnd);

				return orig_proc(hwnd, wp);
			}
			inline static decltype(&hook_proc) orig_proc = ::GetWindowPlacement;
		} GetWindowPlacement;
	} window_pos;
}
