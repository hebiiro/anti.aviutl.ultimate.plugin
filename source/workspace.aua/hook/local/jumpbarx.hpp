#pragma once

namespace apn::workspace::hook
{
	//
	// このクラスはjumpbarx.aufをフックします。
	//
	inline struct : Entity {
		//
		// このクラスは::AdjustWindowRectEx()をフックします。
		//
		inline static struct {
			inline static BOOL WINAPI hook_proc(LPRECT rc, DWORD style, BOOL menu, DWORD ex_style)
			{
				MY_TRACE_FUNC("{/hex}, {/}, {/hex}", style, menu, ex_style);

				auto result = orig_proc(rc, style | WS_CAPTION, menu, ex_style);

				if (auto shuttle = shuttle_manager.get(L"ジャンプバー＋"))
				{
					// ここで一旦フローティングコンテナのサイズを大きくします。
					// こうしないと何故かフローティングコンテナのサイズが不正になります。
					::SetWindowPos(*shuttle->float_container, nullptr,
						0, 0, 800, 100, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
				}

				// 高さを0にします。
				rc->bottom = rc->top;

				return result;
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} AdjustWindowRectEx;

		//
		// 初期化処理を実行します。
		//
		virtual BOOL on_init() override
		{
			MY_TRACE_FUNC("");

			auto jumpbarx = ::GetModuleHandle(_T("jumpbarx.auf"));
			MY_TRACE_HEX(jumpbarx);
			if (!jumpbarx) return FALSE;

			// jumpbarx.auf内の::AdjustWindowRectEx()をフックします。
			my::hook::attach_import(AdjustWindowRectEx, jumpbarx, "AdjustWindowRectEx");
			MY_TRACE_HEX(AdjustWindowRectEx.orig_proc);

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL on_exit() override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}
	} jumpbarx;
}
